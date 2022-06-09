#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "GeneralHook.h"
#include "./disasm/disasm.h"


#ifndef GOOD_HANDLE
#define GOOD_HANDLE(a) ((a!=INVALID_HANDLE_VALUE)&&(a!=NULL))
#endif


#ifdef _DEBUG
	#define DebugPrint(...) printf(__VA_ARGS__)
#else
	#define DebugPrint(...) 
#endif


#define MAX_CODE_BYTES	32
#define MAX_RIPS		4


// 存储安装一个hook所有的相关数据
struct MHOOKS_TRAMPOLINE {
	PBYTE	pSystemFunction;								// 原函数地址
	DWORD	cbOverwrittenCode;								// 跳转指令的大小
	PBYTE	pHookFunction;									// 目标函数
	BYTE	codeJumpToHookFunction[MAX_CODE_BYTES];	// 跳转指令
	BYTE	codeTrampoline[MAX_CODE_BYTES];			// Trampoline函数中存储的代码
	BYTE	codeUntouched[MAX_CODE_BYTES];			// 未修改的原始代码
};


// 处理地址重定位问题时需要用到此数据结构
struct MHOOKS_RIPINFO
{
	DWORD	dwOffset;
	S64		nDisplacement;
};


struct MHOOKS_PATCHDATA
{
	S64				nLimitUp;
	S64				nLimitDown;
	DWORD			nRipCnt;
	MHOOKS_RIPINFO	rips[MAX_RIPS];
};


static BOOL g_bVarsInitialized = FALSE;
static CRITICAL_SECTION g_cs;
static MHOOKS_TRAMPOLINE* g_pHooks[MAX_SUPPORTED_HOOKS];
static DWORD g_nHooksInUse = 0;
static HANDLE* g_hThreadHandles = NULL;
static DWORD g_nThreadHandles = 0;
#define MHOOK_JMPSIZE 5


typedef HANDLE (WINAPI * _CreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL (WINAPI * _Thread32First)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
typedef BOOL (WINAPI * _Thread32Next)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
_CreateToolhelp32Snapshot fnCreateToolhelp32Snapshot = (_CreateToolhelp32Snapshot) GetProcAddress(GetModuleHandle(L"kernel32"), "CreateToolhelp32Snapshot");
_Thread32First fnThread32First = (_Thread32First) GetProcAddress(GetModuleHandle(L"kernel32"), "Thread32First");
_Thread32Next fnThread32Next = (_Thread32Next) GetProcAddress(GetModuleHandle(L"kernel32"), "Thread32Next");


// 加锁
static VOID EnterCritSec() {
	if (!g_bVarsInitialized) {
		InitializeCriticalSection(&g_cs);
		ZeroMemory(g_pHooks, sizeof(g_pHooks));
		g_bVarsInitialized = TRUE;
	}
	EnterCriticalSection(&g_cs);
}


// 释放锁
static VOID LeaveCritSec() {
	LeaveCriticalSection(&g_cs);
}


// 处理相对偏移
static PBYTE SkipJumps(PBYTE pbCode) {
	PBYTE pbOrgCode = pbCode;
#ifdef _M_IX86_X64

#ifdef _M_IX86
	if (pbCode[0] == 0x8b && pbCode[1] == 0xff)
		pbCode += 2;

	if (pbCode[0] == 0x55 && pbCode[1] == 0x8b && pbCode[2] == 0xec && pbCode[3] == 0x5d)
		pbCode += 4;
#endif	

	if (pbCode[0] == 0xff && pbCode[1] == 0x25) {
#ifdef _M_IX86
		PBYTE pbTarget = *(PBYTE *)&pbCode[2];
		return SkipJumps(*(PBYTE *)pbTarget);

#elif defined _M_X64
		INT32 lOffset = *(INT32 *)&pbCode[2];
		return SkipJumps(*(PBYTE*)(pbCode + 6 + lOffset));

	} else if (pbCode[0] == 0x48 && pbCode[1] == 0xff && pbCode[2] == 0x25) {
		INT32 lOffset = *(INT32 *)&pbCode[3];
		return SkipJumps(*(PBYTE*)(pbCode + 7 + lOffset));
#endif

	} else if (pbCode[0] == 0xe9) {
		return SkipJumps(pbCode + 5 + *(INT32 *)&pbCode[1]);

	} else if (pbCode[0] == 0xeb) {
		return SkipJumps(pbCode + 2 + *(CHAR *)&pbCode[1]);
	}

#else
#error unsupported platform
#endif
	return pbOrgCode;
}


// 写入跳转指令
static PBYTE EmitJump(PBYTE pbCode, PBYTE pbJumpTo) {
#ifdef _M_IX86_X64
	PBYTE pbJumpFrom = pbCode + 5;
	SIZE_T cbDiff = pbJumpFrom > pbJumpTo ? pbJumpFrom - pbJumpTo : pbJumpTo - pbJumpFrom;
	DebugPrint("GeneralHook::EmitJump: 从%p跳转到%p，相对偏移为%p\n", pbJumpFrom, pbJumpTo, cbDiff);
	if (cbDiff <= 0x7fff0000) {
		pbCode[0] = 0xe9;
		pbCode += 1;
		*((PDWORD)pbCode) = (DWORD)(DWORD_PTR)(pbJumpTo - pbJumpFrom);
		pbCode += sizeof(DWORD);

	} else {
		pbCode[0] = 0xff;
		pbCode[1] = 0x25;
		pbCode += 2;
#ifdef _M_IX86
		*((PDWORD)pbCode) = (DWORD)(DWORD_PTR)(pbCode + sizeof(DWORD));

#elif defined _M_X64
		*((PDWORD)pbCode) = (DWORD)0;

#endif
		pbCode += sizeof(DWORD);
		*((PDWORD_PTR)pbCode) = (DWORD_PTR)(pbJumpTo);
		pbCode += sizeof(DWORD_PTR);
	}
#else 
#error unsupported platform
#endif
	return pbCode;
}


// 生成Trampoline函数
static MHOOKS_TRAMPOLINE* TrampolineAlloc(PBYTE pSystemFunction, S64 nLimitUp, S64 nLimitDown) {

	MHOOKS_TRAMPOLINE* pTrampoline = NULL;

	if (g_nHooksInUse < MAX_SUPPORTED_HOOKS) {

		// 确定分配位置的下界和上界
		PBYTE pLower = pSystemFunction + nLimitUp;
		pLower = pLower < (PBYTE)(DWORD_PTR)0x0000000080000000 ? (PBYTE)(0x1) : (PBYTE)(pLower - (PBYTE)0x7fff0000);
		PBYTE pUpper = pSystemFunction + nLimitDown;
		pUpper = pUpper < (PBYTE)(DWORD_PTR)0xffffffff80000000 ? (PBYTE)(pUpper + (DWORD_PTR)0x7ff80000) : (PBYTE)(DWORD_PTR)0xfffffffffff80000;
		DebugPrint("GeneralHook::TrampolineAlloc: 从%p开始，在%p和%p之间分配\n", pSystemFunction, pLower, pUpper);

		SYSTEM_INFO sSysInfo =  {0};
		::GetSystemInfo(&sSysInfo);

		// 遍历可用的内存块，并尝试分配
		for (PBYTE pbAlloc = pLower; pbAlloc < pUpper;) {
			// 确定当前状态
			MEMORY_BASIC_INFORMATION mbi;
			DebugPrint("GeneralHook::TrampolineAlloc: 判断地址%p是否可分配\n", pbAlloc);
			if (!VirtualQuery(pbAlloc, &mbi, sizeof(mbi)))
				break;

			if (mbi.State == MEM_FREE && mbi.RegionSize >= sizeof(MHOOKS_TRAMPOLINE) && mbi.RegionSize >= sSysInfo.dwAllocationGranularity) {
				pbAlloc = (PBYTE)(ULONG_PTR((ULONG_PTR(pbAlloc) + (sSysInfo.dwAllocationGranularity-1)) / sSysInfo.dwAllocationGranularity) * sSysInfo.dwAllocationGranularity);
				pTrampoline = (MHOOKS_TRAMPOLINE*)VirtualAlloc(pbAlloc, sizeof(MHOOKS_TRAMPOLINE), MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READ);
				if (pTrampoline) {
					DebugPrint("GeneralHook::TrampolineAlloc: 在%p分配空间用于Trampoline函数\n", pTrampoline);
					break;
				}
			}

			// 继续搜索
			pbAlloc = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
		}

		if (pTrampoline) {
			// 把Trampoline地址放到全局表中 
			for (DWORD i=0; i<MAX_SUPPORTED_HOOKS; i++) {
				if (g_pHooks[i] == NULL) {
					g_pHooks[i] = pTrampoline;
					g_nHooksInUse++;
					break;
				}
			}
		}
	}

	return pTrampoline;
}


// 获取目标函数对应的Trampoline函数的地址
static MHOOKS_TRAMPOLINE* TrampolineGet(PBYTE pHookedFunction) {
	for (DWORD i=0; i<MAX_SUPPORTED_HOOKS; i++) {
		if (g_pHooks[i]) {
			if (g_pHooks[i]->codeTrampoline == pHookedFunction)
				return g_pHooks[i];
		}
	}
	return NULL;
}


// 释放Trampoline的空间
static VOID TrampolineFree(MHOOKS_TRAMPOLINE* pTrampoline, BOOL bNeverUsed) {
	for (DWORD i=0; i<MAX_SUPPORTED_HOOKS; i++) {
		if (g_pHooks[i] == pTrampoline) {
			g_pHooks[i] = NULL;

			if (bNeverUsed)
				VirtualFree(pTrampoline, 0, MEM_RELEASE);
			g_nHooksInUse--;
			break;
		}
	}
}


// 暂停一个给定的线程
static HANDLE SuspendOneThread(DWORD dwThreadId, PBYTE pbCode, DWORD cbBytes) {
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
	if (GOOD_HANDLE(hThread)) {
		DWORD dwSuspendCount = SuspendThread(hThread);
		if (dwSuspendCount != -1) {
			CONTEXT ctx;
			ctx.ContextFlags = CONTEXT_CONTROL;
			int nTries = 0;
			while (GetThreadContext(hThread, &ctx)) {
#ifdef _M_IX86
				PBYTE pIp = (PBYTE)(DWORD_PTR)ctx.Eip;
#elif defined _M_X64
				PBYTE pIp = (PBYTE)(DWORD_PTR)ctx.Rip;
#endif
				if (pIp >= pbCode && pIp < (pbCode + cbBytes)) {
					if (nTries < 3) {
						DebugPrint("GeneralHook::SuspendOneThread: 挂起线程%d失败，IP=%p，将再次尝试\n", dwThreadId, pIp);
						ResumeThread(hThread);
						Sleep(100);
						SuspendThread(hThread);
						nTries++;
					} else {
						DebugPrint("GeneralHook::SuspendOneThread: 挂起线程%d失败，IP=%p，中止尝试\n", dwThreadId, pIp);
						ResumeThread(hThread);
						CloseHandle(hThread);
						hThread = NULL;
						break;
					}
				} else {
					DebugPrint("GeneralHook::SuspendOneThread: 成功挂起线程%d，IP=%p\n", dwThreadId, pIp);
					break;
				}
			}
		} else {
			CloseHandle(hThread);
			hThread = NULL;
		}
	}
	return hThread;
}


// 恢复当前进程中先前挂起的所有线程。
static VOID ResumeOtherThreads() {
	// 设置线程优先级
	INT nOriginalPriority = GetThreadPriority(GetCurrentThread());
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	for (DWORD i=0; i<g_nThreadHandles; i++) {
		ResumeThread(g_hThreadHandles[i]);
		CloseHandle(g_hThreadHandles[i]);
	}

	free(g_hThreadHandles);
	g_hThreadHandles = NULL;
	g_nThreadHandles = 0;
	SetThreadPriority(GetCurrentThread(), nOriginalPriority);
}


// 挂起进程中的所有线程
static BOOL SuspendOtherThreads(PBYTE pbCode, DWORD cbBytes) {
	BOOL bRet = FALSE;
	
	// 只保留负责安装Hook的这个线程
	INT nOriginalPriority = GetThreadPriority(GetCurrentThread());
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	// 从快照中获取所有线程的句柄
	HANDLE hSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
	if (GOOD_HANDLE(hSnap)) {
		THREADENTRY32 te;
		te.dwSize = sizeof(te);

		// 计算其他线程总数
		DWORD nThreadsInProcess = 0;
		if (fnThread32First(hSnap, &te)) {
			do {
				if (te.th32OwnerProcessID == GetCurrentProcessId()) {
					if (te.th32ThreadID != GetCurrentThreadId()) {
						nThreadsInProcess++;
					}
				}
				te.dwSize = sizeof(te);
			} while(fnThread32Next(hSnap, &te));
		}
		DebugPrint("GeneralHook::SuspendOtherThreads: 共有%d个其他线程\n", nThreadsInProcess);

		if (nThreadsInProcess) {
			// 分配缓冲区给需要挂起线程的句柄 
			g_hThreadHandles = (HANDLE*)malloc(nThreadsInProcess*sizeof(HANDLE));
			if (g_hThreadHandles) {
				ZeroMemory(g_hThreadHandles, nThreadsInProcess*sizeof(HANDLE));
				DWORD nCurrentThread = 0;
				BOOL bFailed = FALSE;
				te.dwSize = sizeof(te);
				
				if (fnThread32First(hSnap, &te)) {
					do {
						if (te.th32OwnerProcessID == GetCurrentProcessId()) {
							if (te.th32ThreadID != GetCurrentThreadId()) {
								// 尝试挂起线程
								g_hThreadHandles[nCurrentThread] = SuspendOneThread(te.th32ThreadID, pbCode, cbBytes);
								if (GOOD_HANDLE(g_hThreadHandles[nCurrentThread])) {
									DebugPrint("GeneralHook::SuspendOtherThreads: 成功挂起线程%d\n", te.th32ThreadID);
									nCurrentThread++;
								} else {
									DebugPrint("GeneralHook::SuspendOtherThreads: 挂起线程%d失败，错误码为%d\n", te.th32ThreadID, GetLastError());
								}
							}
						}
						te.dwSize = sizeof(te);
					} while(fnThread32Next(hSnap, &te) && !bFailed);
				}
				g_nThreadHandles = nCurrentThread;
				bRet = !bFailed;
			}
		}
		CloseHandle(hSnap);
	} else {
		DebugPrint("GeneralHook::SuspendOtherThreads: 无法创建进程快照: %d\n", GetLastError());
	}

	SetThreadPriority(GetCurrentThread(), nOriginalPriority);

	if (!bRet) {
		DebugPrint("GeneralHook::SuspendOtherThreads: 未知错误，现在将恢复全部线程\n");
		ResumeOtherThreads();
	}

	return bRet;
}


// 地址重定位，相对偏移
static void FixupIPRelativeAddressing(PBYTE pbNew, PBYTE pbOriginal, MHOOKS_PATCHDATA* pdata)
{
#if defined _M_X64
	S64 diff = pbNew - pbOriginal;
	for (DWORD i = 0; i < pdata->nRipCnt; i++) {
		DWORD dwNewDisplacement = (DWORD)(pdata->rips[i].nDisplacement - diff);
		DebugPrint("GeneralHook: 修复RIP指令操作数为0x%p的代码: \n"
			"旧的相对偏移: 0x%8.8x, 新的相对偏移: 0x%8.8x\n", 
			pbNew + pdata->rips[i].dwOffset, 
			(DWORD)pdata->rips[i].nDisplacement, 
			dwNewDisplacement);
		*(PDWORD)(pbNew + pdata->rips[i].dwOffset) = dwNewDisplacement;
	}
#endif
}


// 反汇编目标函数开头的几条汇编指令，并处理相对偏移问题
static DWORD DisassembleAndSkip(PVOID pFunction, DWORD dwMinLen, MHOOKS_PATCHDATA* pdata) {
	DWORD dwRet = 0;
	pdata->nLimitDown = 0;
	pdata->nLimitUp = 0;
	pdata->nRipCnt = 0;
#ifdef _M_IX86
	ARCHITECTURE_TYPE arch = ARCH_X86;
#elif defined _M_X64
	ARCHITECTURE_TYPE arch = ARCH_X64;
#else
	#error unsupported platform
#endif
	DISASSEMBLER dis;
	if (InitDisassembler(&dis, arch)) {
		INSTRUCTION* pins = NULL;
		U8* pLoc = (U8*)pFunction;
		DWORD dwFlags = DISASM_DECODE | DISASM_DISASSEMBLE | DISASM_ALIGNOUTPUT;

		DebugPrint("GeneralHook::DisassembleAndSkip() 正在反汇编，地址=%p\n", pLoc);
		while ( (dwRet < dwMinLen) && (pins = GetInstruction(&dis, (ULONG_PTR)pLoc, pLoc, dwFlags)) ) {
			DebugPrint("GeneralHook::DisassembleAndSkip() %p:(0x%2.2x) %s\n", pLoc, pins->Length, pins->String);
			if (pins->Type == ITYPE_RET		) break;
			if (pins->Type == ITYPE_BRANCH	) break;
			if (pins->Type == ITYPE_BRANCHCC) break;
			if (pins->Type == ITYPE_CALL	) break;
			if (pins->Type == ITYPE_CALLCC	) break;

			#if defined _M_X64
				BOOL bProcessRip = FALSE;

				// mov或lea，从rip+imm32到寄存器中。
				if ((pins->Type == ITYPE_MOV || pins->Type == ITYPE_LEA) && (pins->X86.Relative) && 
					(pins->X86.OperandSize == 8) && (pins->OperandCount == 2) &&
					(pins->Operands[1].Flags & OP_IPREL) && (pins->Operands[1].Register == AMD64_REG_RIP))
				{
					// rip-addressing "mov reg, [rip+imm32]"
					DebugPrint("GeneralHook::DisassembleAndSkip: 在操作数%d上找到OP_IPREL，相对偏移为0x%x，内存中为0x%x\n", 1, pins->X86.Displacement, *(PDWORD)(pLoc+3));
					bProcessRip = TRUE;
				}

				// mov or lea，从寄存器到rip+imm32
				else if ((pins->Type == ITYPE_MOV || pins->Type == ITYPE_LEA) && (pins->X86.Relative) && 
					(pins->X86.OperandSize == 8) && (pins->OperandCount == 2) &&
					(pins->Operands[0].Flags & OP_IPREL) && (pins->Operands[0].Register == AMD64_REG_RIP))
				{
					// rip-addressing "mov [rip+imm32], reg"
					DebugPrint("GeneralHook::DisassembleAndSkip: 在操作数%d上找到OP_IPREL，相对偏移为0x%x，内存中为0x%x\n", 0, pins->X86.Displacement, *(PDWORD)(pLoc+3));
					bProcessRip = TRUE;
				}

				else if ( (pins->OperandCount >= 1) && (pins->Operands[0].Flags & OP_IPREL) )
				{
					// 不支持rip-addressing
					DebugPrint("GeneralHook::DisassembleAndSkip: 发现操作数%d上不支持OP_IPREL\n", 0);
					
					// dump指令并DebugPrint
					for (DWORD i=0; i<pins->Length; i++) {
						DebugPrint("GeneralHook::DisassembleAndSkip: 指令byte %2.2d: 0x%2.2x\n", i, pLoc[i]);
					}
					break;
				}

				else if ( (pins->OperandCount >= 2) && (pins->Operands[1].Flags & OP_IPREL) )
				{
					// 不支持rip-addressing
					DebugPrint("GeneralHook::DisassembleAndSkip: 发现操作数%d上不支持OP_IPREL\n", 1);

					// dump指令并DebugPrint
					for (DWORD i=0; i<pins->Length; i++) {
						DebugPrint("GeneralHook::DisassembleAndSkip: 指令byte %2.2d: 0x%2.2x\n", i, pLoc[i]);
					}
					break;
				}

				else if ( (pins->OperandCount >= 3) && (pins->Operands[2].Flags & OP_IPREL) )
				{
					DebugPrint("GeneralHook::DisassembleAndSkip: 发现操作数%d上不支持OP_IPREL\n", 2);

					// dump指令并DebugPrint
					for (DWORD i=0; i<pins->Length; i++) {
						DebugPrint("GeneralHook::DisassembleAndSkip: 指令byte %2.2d: 0x%2.2x\n", i, pLoc[i]);
					}
					break;
				}

				if (bProcessRip) {
					// 计算相对于函数开始的偏移 
					S64 nAdjustedDisplacement = pins->X86.Displacement + (pLoc - (U8*)pFunction);

					// 存储离零最远的相对偏移(可正可负) 
					if (nAdjustedDisplacement < pdata->nLimitDown)
						pdata->nLimitDown = nAdjustedDisplacement;
					if (nAdjustedDisplacement > pdata->nLimitUp)
						pdata->nLimitUp = nAdjustedDisplacement;

					// 存储patch数据
					if (pdata->nRipCnt < MAX_RIPS) {
						pdata->rips[pdata->nRipCnt].dwOffset = dwRet + 3;
						pdata->rips[pdata->nRipCnt].nDisplacement = pins->X86.Displacement;
						pdata->nRipCnt++;
					} else {
						// 空间不足, 停止反汇编
						break;
					}
				}
			#endif

			dwRet += pins->Length;
			pLoc  += pins->Length;
		}

		CloseDisassembler(&dis);
	}

	return dwRet;
}


// 安装Hook
BOOL SetHook(PVOID *ppSystemFunction, PVOID pHookFunction) {
	MHOOKS_TRAMPOLINE* pTrampoline = NULL;
	PVOID pSystemFunction = *ppSystemFunction;

	// 上锁
	EnterCritSec();
	DebugPrint("GeneralHook::SetHook: 开始Hook，地址=%p / %p\n", pSystemFunction, pHookFunction);

	// 找到目标函数真实地址 (比如jmp func的要找到最终地址)
	pSystemFunction = SkipJumps((PBYTE)pSystemFunction);
	pHookFunction   = SkipJumps((PBYTE)pHookFunction);
	DebugPrint("GeneralHook::SetHook: 开始Hook，真实函数地址=%p / %p\n", pSystemFunction, pHookFunction);

	// 计算构造的跳转指令长度
	MHOOKS_PATCHDATA patchdata = {0};
	DWORD dwInstructionLength = DisassembleAndSkip(pSystemFunction, MHOOK_JMPSIZE, &patchdata);

	if (dwInstructionLength >= MHOOK_JMPSIZE) {
		DebugPrint("GeneralHook::SetHook: 反汇编%d字节\n", dwInstructionLength);
		
		// 挂起这个进程中的其他线程
		SuspendOtherThreads((PBYTE)pSystemFunction, dwInstructionLength);

		// 生成Trampoline函数
		pTrampoline = TrampolineAlloc((PBYTE)pSystemFunction, patchdata.nLimitUp, patchdata.nLimitDown);
		if (pTrampoline) {
			DebugPrint("GeneralHook::SetHook: 分配Trampoline结构，地址=%p\n", pTrampoline);
			DWORD dwOldProtectSystemFunction = 0;
			DWORD dwOldProtectTrampolineFunction = 0;

			// 修改新创建的内存段权限为可执行
			if (VirtualProtect(pSystemFunction, dwInstructionLength, PAGE_EXECUTE_READWRITE, &dwOldProtectSystemFunction)) {
				DebugPrint("GeneralHook::SetHook: 设置存储Trampoline代码的内存段权限为可读可写可执行\n");
				if (VirtualProtect(pTrampoline, sizeof(MHOOKS_TRAMPOLINE), PAGE_EXECUTE_READWRITE, &dwOldProtectTrampolineFunction)) {

					// 创建Trampoline函数
					PBYTE pbCode = pTrampoline->codeTrampoline;

					// 保存原有入口代码
					for (DWORD i = 0; i<dwInstructionLength; i++) {
						pTrampoline->codeUntouched[i] = pbCode[i] = ((PBYTE)pSystemFunction)[i];
					}
					pbCode += dwInstructionLength;

					// 最后添加一个跳转指令
					pbCode = EmitJump(pbCode, ((PBYTE)pSystemFunction) + dwInstructionLength);
					DebugPrint("GeneralHook::SetHook: 为Trampoline函数添加一个跳转指令\n");

					// 地址重定位问题
					FixupIPRelativeAddressing(pTrampoline->codeTrampoline, (PBYTE)pSystemFunction, &patchdata);

					DWORD_PTR dwDistance = (PBYTE)pHookFunction < (PBYTE)pSystemFunction ? 
						(PBYTE)pSystemFunction - (PBYTE)pHookFunction : (PBYTE)pHookFunction - (PBYTE)pSystemFunction;
					if (dwDistance > 0x7fff0000) {

						pbCode = pTrampoline->codeJumpToHookFunction;
						pbCode = EmitJump(pbCode, (PBYTE)pHookFunction);
						FlushInstructionCache(GetCurrentProcess(), pTrampoline->codeJumpToHookFunction, 
							pbCode - pTrampoline->codeJumpToHookFunction);

						// 更新自身IAT
						pbCode = (PBYTE)pSystemFunction;
						pbCode = EmitJump(pbCode, pTrampoline->codeJumpToHookFunction);
					} else {
						pbCode = (PBYTE)pSystemFunction;
						pbCode = EmitJump(pbCode, (PBYTE)pHookFunction);
					}

					pTrampoline->cbOverwrittenCode = dwInstructionLength;
					pTrampoline->pSystemFunction = (PBYTE)pSystemFunction;
					pTrampoline->pHookFunction = (PBYTE)pHookFunction;

					// 刷新指令缓存并恢复内存段原来的权限 
					FlushInstructionCache(GetCurrentProcess(), pTrampoline->codeTrampoline, dwInstructionLength);
					VirtualProtect(pTrampoline, sizeof(MHOOKS_TRAMPOLINE), dwOldProtectTrampolineFunction, &dwOldProtectTrampolineFunction);
				} else {
					DebugPrint("GeneralHook::SetHook: VirtualProtect失败，错误码为%d\n", GetLastError());
				}

				// 刷新指令缓存并恢复内存段原来的权限 
				FlushInstructionCache(GetCurrentProcess(), pSystemFunction, dwInstructionLength);
				VirtualProtect(pSystemFunction, dwInstructionLength, dwOldProtectSystemFunction, &dwOldProtectSystemFunction);
			} else {
				DebugPrint("GeneralHook::SetHook: VirtualProtect失败，错误码为: %d\n", GetLastError());
			}
			if (pTrampoline->pSystemFunction) {
				
				// 原目标函数新的入口点
				*ppSystemFunction = pTrampoline->codeTrampoline;
				DebugPrint("GeneralHook::SetHook: 成功Hook目标函数\n");
			} else {
				TrampolineFree(pTrampoline, TRUE);
				pTrampoline = NULL;
			}
		}

		// 恢复其他线程
		ResumeOtherThreads();
	} else {
		DebugPrint("GeneralHook::反汇编%d字节\n", dwInstructionLength);
	}

	// 解锁
	LeaveCritSec();
	return (pTrampoline != NULL);
}


// 卸载Hook
BOOL UnHook(PVOID *ppHookedFunction) {
	DebugPrint("GeneralHook::Unhook: 卸载Hook，地址=%p\n", *ppHookedFunction);

	BOOL bRet = FALSE;

	// 上锁
	EnterCritSec();

	// 从中心管理器中获取对应的Trampoline函数地址
	MHOOKS_TRAMPOLINE* pTrampoline = TrampolineGet((PBYTE)*ppHookedFunction);
	if (pTrampoline) {

		// 线程安全问题
		SuspendOtherThreads(pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode);
		DebugPrint("GeneralHook::Unhook: Trampoline结构地址=%p\n", pTrampoline);
		DWORD dwOldProtectSystemFunction = 0;
		
		// 确保内存可写
		if (VirtualProtect(pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode, PAGE_EXECUTE_READWRITE, &dwOldProtectSystemFunction)) {
			DebugPrint("GeneralHook::Unhook: 设置系统的读写功能\n");
			PBYTE pbCode = (PBYTE)pTrampoline->pSystemFunction;
			for (DWORD i = 0; i<pTrampoline->cbOverwrittenCode; i++) {
				pbCode[i] = pTrampoline->codeUntouched[i];
			}

			// 刷新指令缓存，使内存不可写 
			FlushInstructionCache(GetCurrentProcess(), pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode);
			VirtualProtect(pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode, dwOldProtectSystemFunction, &dwOldProtectSystemFunction);

			// 返回原目标函数的地址
			*ppHookedFunction = pTrampoline->pSystemFunction;
			bRet = TRUE;
			DebugPrint("GeneralHook::Unhook: 原目标函数地址=%p\n", *ppHookedFunction);

			// 释放Trampoline函数
			TrampolineFree(pTrampoline, FALSE);
			DebugPrint("GeneralHook::Unhook: 卸载成功\n");
		} else {
			DebugPrint("GeneralHook::Unhook: VirtualProtect失败，错误码为%d\n", GetLastError());
		}

		// 恢复其他线程
		ResumeOtherThreads();
	}

	// 解锁
	LeaveCritSec();
	return bRet;
}