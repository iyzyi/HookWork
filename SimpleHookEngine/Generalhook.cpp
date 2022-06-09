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


// �洢��װһ��hook���е��������
struct MHOOKS_TRAMPOLINE {
	PBYTE	pSystemFunction;								// ԭ������ַ
	DWORD	cbOverwrittenCode;								// ��תָ��Ĵ�С
	PBYTE	pHookFunction;									// Ŀ�꺯��
	BYTE	codeJumpToHookFunction[MAX_CODE_BYTES];	// ��תָ��
	BYTE	codeTrampoline[MAX_CODE_BYTES];			// Trampoline�����д洢�Ĵ���
	BYTE	codeUntouched[MAX_CODE_BYTES];			// δ�޸ĵ�ԭʼ����
};


// �����ַ�ض�λ����ʱ��Ҫ�õ������ݽṹ
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


// ����
static VOID EnterCritSec() {
	if (!g_bVarsInitialized) {
		InitializeCriticalSection(&g_cs);
		ZeroMemory(g_pHooks, sizeof(g_pHooks));
		g_bVarsInitialized = TRUE;
	}
	EnterCriticalSection(&g_cs);
}


// �ͷ���
static VOID LeaveCritSec() {
	LeaveCriticalSection(&g_cs);
}


// �������ƫ��
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


// д����תָ��
static PBYTE EmitJump(PBYTE pbCode, PBYTE pbJumpTo) {
#ifdef _M_IX86_X64
	PBYTE pbJumpFrom = pbCode + 5;
	SIZE_T cbDiff = pbJumpFrom > pbJumpTo ? pbJumpFrom - pbJumpTo : pbJumpTo - pbJumpFrom;
	DebugPrint("GeneralHook::EmitJump: ��%p��ת��%p�����ƫ��Ϊ%p\n", pbJumpFrom, pbJumpTo, cbDiff);
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


// ����Trampoline����
static MHOOKS_TRAMPOLINE* TrampolineAlloc(PBYTE pSystemFunction, S64 nLimitUp, S64 nLimitDown) {

	MHOOKS_TRAMPOLINE* pTrampoline = NULL;

	if (g_nHooksInUse < MAX_SUPPORTED_HOOKS) {

		// ȷ������λ�õ��½���Ͻ�
		PBYTE pLower = pSystemFunction + nLimitUp;
		pLower = pLower < (PBYTE)(DWORD_PTR)0x0000000080000000 ? (PBYTE)(0x1) : (PBYTE)(pLower - (PBYTE)0x7fff0000);
		PBYTE pUpper = pSystemFunction + nLimitDown;
		pUpper = pUpper < (PBYTE)(DWORD_PTR)0xffffffff80000000 ? (PBYTE)(pUpper + (DWORD_PTR)0x7ff80000) : (PBYTE)(DWORD_PTR)0xfffffffffff80000;
		DebugPrint("GeneralHook::TrampolineAlloc: ��%p��ʼ����%p��%p֮�����\n", pSystemFunction, pLower, pUpper);

		SYSTEM_INFO sSysInfo =  {0};
		::GetSystemInfo(&sSysInfo);

		// �������õ��ڴ�飬�����Է���
		for (PBYTE pbAlloc = pLower; pbAlloc < pUpper;) {
			// ȷ����ǰ״̬
			MEMORY_BASIC_INFORMATION mbi;
			DebugPrint("GeneralHook::TrampolineAlloc: �жϵ�ַ%p�Ƿ�ɷ���\n", pbAlloc);
			if (!VirtualQuery(pbAlloc, &mbi, sizeof(mbi)))
				break;

			if (mbi.State == MEM_FREE && mbi.RegionSize >= sizeof(MHOOKS_TRAMPOLINE) && mbi.RegionSize >= sSysInfo.dwAllocationGranularity) {
				pbAlloc = (PBYTE)(ULONG_PTR((ULONG_PTR(pbAlloc) + (sSysInfo.dwAllocationGranularity-1)) / sSysInfo.dwAllocationGranularity) * sSysInfo.dwAllocationGranularity);
				pTrampoline = (MHOOKS_TRAMPOLINE*)VirtualAlloc(pbAlloc, sizeof(MHOOKS_TRAMPOLINE), MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READ);
				if (pTrampoline) {
					DebugPrint("GeneralHook::TrampolineAlloc: ��%p����ռ�����Trampoline����\n", pTrampoline);
					break;
				}
			}

			// ��������
			pbAlloc = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
		}

		if (pTrampoline) {
			// ��Trampoline��ַ�ŵ�ȫ�ֱ��� 
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


// ��ȡĿ�꺯����Ӧ��Trampoline�����ĵ�ַ
static MHOOKS_TRAMPOLINE* TrampolineGet(PBYTE pHookedFunction) {
	for (DWORD i=0; i<MAX_SUPPORTED_HOOKS; i++) {
		if (g_pHooks[i]) {
			if (g_pHooks[i]->codeTrampoline == pHookedFunction)
				return g_pHooks[i];
		}
	}
	return NULL;
}


// �ͷ�Trampoline�Ŀռ�
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


// ��ͣһ���������߳�
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
						DebugPrint("GeneralHook::SuspendOneThread: �����߳�%dʧ�ܣ�IP=%p�����ٴγ���\n", dwThreadId, pIp);
						ResumeThread(hThread);
						Sleep(100);
						SuspendThread(hThread);
						nTries++;
					} else {
						DebugPrint("GeneralHook::SuspendOneThread: �����߳�%dʧ�ܣ�IP=%p����ֹ����\n", dwThreadId, pIp);
						ResumeThread(hThread);
						CloseHandle(hThread);
						hThread = NULL;
						break;
					}
				} else {
					DebugPrint("GeneralHook::SuspendOneThread: �ɹ������߳�%d��IP=%p\n", dwThreadId, pIp);
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


// �ָ���ǰ��������ǰ����������̡߳�
static VOID ResumeOtherThreads() {
	// �����߳����ȼ�
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


// ��������е������߳�
static BOOL SuspendOtherThreads(PBYTE pbCode, DWORD cbBytes) {
	BOOL bRet = FALSE;
	
	// ֻ��������װHook������߳�
	INT nOriginalPriority = GetThreadPriority(GetCurrentThread());
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	// �ӿ����л�ȡ�����̵߳ľ��
	HANDLE hSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
	if (GOOD_HANDLE(hSnap)) {
		THREADENTRY32 te;
		te.dwSize = sizeof(te);

		// ���������߳�����
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
		DebugPrint("GeneralHook::SuspendOtherThreads: ����%d�������߳�\n", nThreadsInProcess);

		if (nThreadsInProcess) {
			// ���仺��������Ҫ�����̵߳ľ�� 
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
								// ���Թ����߳�
								g_hThreadHandles[nCurrentThread] = SuspendOneThread(te.th32ThreadID, pbCode, cbBytes);
								if (GOOD_HANDLE(g_hThreadHandles[nCurrentThread])) {
									DebugPrint("GeneralHook::SuspendOtherThreads: �ɹ������߳�%d\n", te.th32ThreadID);
									nCurrentThread++;
								} else {
									DebugPrint("GeneralHook::SuspendOtherThreads: �����߳�%dʧ�ܣ�������Ϊ%d\n", te.th32ThreadID, GetLastError());
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
		DebugPrint("GeneralHook::SuspendOtherThreads: �޷��������̿���: %d\n", GetLastError());
	}

	SetThreadPriority(GetCurrentThread(), nOriginalPriority);

	if (!bRet) {
		DebugPrint("GeneralHook::SuspendOtherThreads: δ֪�������ڽ��ָ�ȫ���߳�\n");
		ResumeOtherThreads();
	}

	return bRet;
}


// ��ַ�ض�λ�����ƫ��
static void FixupIPRelativeAddressing(PBYTE pbNew, PBYTE pbOriginal, MHOOKS_PATCHDATA* pdata)
{
#if defined _M_X64
	S64 diff = pbNew - pbOriginal;
	for (DWORD i = 0; i < pdata->nRipCnt; i++) {
		DWORD dwNewDisplacement = (DWORD)(pdata->rips[i].nDisplacement - diff);
		DebugPrint("GeneralHook: �޸�RIPָ�������Ϊ0x%p�Ĵ���: \n"
			"�ɵ����ƫ��: 0x%8.8x, �µ����ƫ��: 0x%8.8x\n", 
			pbNew + pdata->rips[i].dwOffset, 
			(DWORD)pdata->rips[i].nDisplacement, 
			dwNewDisplacement);
		*(PDWORD)(pbNew + pdata->rips[i].dwOffset) = dwNewDisplacement;
	}
#endif
}


// �����Ŀ�꺯����ͷ�ļ������ָ����������ƫ������
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

		DebugPrint("GeneralHook::DisassembleAndSkip() ���ڷ���࣬��ַ=%p\n", pLoc);
		while ( (dwRet < dwMinLen) && (pins = GetInstruction(&dis, (ULONG_PTR)pLoc, pLoc, dwFlags)) ) {
			DebugPrint("GeneralHook::DisassembleAndSkip() %p:(0x%2.2x) %s\n", pLoc, pins->Length, pins->String);
			if (pins->Type == ITYPE_RET		) break;
			if (pins->Type == ITYPE_BRANCH	) break;
			if (pins->Type == ITYPE_BRANCHCC) break;
			if (pins->Type == ITYPE_CALL	) break;
			if (pins->Type == ITYPE_CALLCC	) break;

			#if defined _M_X64
				BOOL bProcessRip = FALSE;

				// mov��lea����rip+imm32���Ĵ����С�
				if ((pins->Type == ITYPE_MOV || pins->Type == ITYPE_LEA) && (pins->X86.Relative) && 
					(pins->X86.OperandSize == 8) && (pins->OperandCount == 2) &&
					(pins->Operands[1].Flags & OP_IPREL) && (pins->Operands[1].Register == AMD64_REG_RIP))
				{
					// rip-addressing "mov reg, [rip+imm32]"
					DebugPrint("GeneralHook::DisassembleAndSkip: �ڲ�����%d���ҵ�OP_IPREL�����ƫ��Ϊ0x%x���ڴ���Ϊ0x%x\n", 1, pins->X86.Displacement, *(PDWORD)(pLoc+3));
					bProcessRip = TRUE;
				}

				// mov or lea���ӼĴ�����rip+imm32
				else if ((pins->Type == ITYPE_MOV || pins->Type == ITYPE_LEA) && (pins->X86.Relative) && 
					(pins->X86.OperandSize == 8) && (pins->OperandCount == 2) &&
					(pins->Operands[0].Flags & OP_IPREL) && (pins->Operands[0].Register == AMD64_REG_RIP))
				{
					// rip-addressing "mov [rip+imm32], reg"
					DebugPrint("GeneralHook::DisassembleAndSkip: �ڲ�����%d���ҵ�OP_IPREL�����ƫ��Ϊ0x%x���ڴ���Ϊ0x%x\n", 0, pins->X86.Displacement, *(PDWORD)(pLoc+3));
					bProcessRip = TRUE;
				}

				else if ( (pins->OperandCount >= 1) && (pins->Operands[0].Flags & OP_IPREL) )
				{
					// ��֧��rip-addressing
					DebugPrint("GeneralHook::DisassembleAndSkip: ���ֲ�����%d�ϲ�֧��OP_IPREL\n", 0);
					
					// dumpָ�DebugPrint
					for (DWORD i=0; i<pins->Length; i++) {
						DebugPrint("GeneralHook::DisassembleAndSkip: ָ��byte %2.2d: 0x%2.2x\n", i, pLoc[i]);
					}
					break;
				}

				else if ( (pins->OperandCount >= 2) && (pins->Operands[1].Flags & OP_IPREL) )
				{
					// ��֧��rip-addressing
					DebugPrint("GeneralHook::DisassembleAndSkip: ���ֲ�����%d�ϲ�֧��OP_IPREL\n", 1);

					// dumpָ�DebugPrint
					for (DWORD i=0; i<pins->Length; i++) {
						DebugPrint("GeneralHook::DisassembleAndSkip: ָ��byte %2.2d: 0x%2.2x\n", i, pLoc[i]);
					}
					break;
				}

				else if ( (pins->OperandCount >= 3) && (pins->Operands[2].Flags & OP_IPREL) )
				{
					DebugPrint("GeneralHook::DisassembleAndSkip: ���ֲ�����%d�ϲ�֧��OP_IPREL\n", 2);

					// dumpָ�DebugPrint
					for (DWORD i=0; i<pins->Length; i++) {
						DebugPrint("GeneralHook::DisassembleAndSkip: ָ��byte %2.2d: 0x%2.2x\n", i, pLoc[i]);
					}
					break;
				}

				if (bProcessRip) {
					// ��������ں�����ʼ��ƫ�� 
					S64 nAdjustedDisplacement = pins->X86.Displacement + (pLoc - (U8*)pFunction);

					// �洢������Զ�����ƫ��(�����ɸ�) 
					if (nAdjustedDisplacement < pdata->nLimitDown)
						pdata->nLimitDown = nAdjustedDisplacement;
					if (nAdjustedDisplacement > pdata->nLimitUp)
						pdata->nLimitUp = nAdjustedDisplacement;

					// �洢patch����
					if (pdata->nRipCnt < MAX_RIPS) {
						pdata->rips[pdata->nRipCnt].dwOffset = dwRet + 3;
						pdata->rips[pdata->nRipCnt].nDisplacement = pins->X86.Displacement;
						pdata->nRipCnt++;
					} else {
						// �ռ䲻��, ֹͣ�����
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


// ��װHook
BOOL SetHook(PVOID *ppSystemFunction, PVOID pHookFunction) {
	MHOOKS_TRAMPOLINE* pTrampoline = NULL;
	PVOID pSystemFunction = *ppSystemFunction;

	// ����
	EnterCritSec();
	DebugPrint("GeneralHook::SetHook: ��ʼHook����ַ=%p / %p\n", pSystemFunction, pHookFunction);

	// �ҵ�Ŀ�꺯����ʵ��ַ (����jmp func��Ҫ�ҵ����յ�ַ)
	pSystemFunction = SkipJumps((PBYTE)pSystemFunction);
	pHookFunction   = SkipJumps((PBYTE)pHookFunction);
	DebugPrint("GeneralHook::SetHook: ��ʼHook����ʵ������ַ=%p / %p\n", pSystemFunction, pHookFunction);

	// ���㹹�����תָ���
	MHOOKS_PATCHDATA patchdata = {0};
	DWORD dwInstructionLength = DisassembleAndSkip(pSystemFunction, MHOOK_JMPSIZE, &patchdata);

	if (dwInstructionLength >= MHOOK_JMPSIZE) {
		DebugPrint("GeneralHook::SetHook: �����%d�ֽ�\n", dwInstructionLength);
		
		// ������������е������߳�
		SuspendOtherThreads((PBYTE)pSystemFunction, dwInstructionLength);

		// ����Trampoline����
		pTrampoline = TrampolineAlloc((PBYTE)pSystemFunction, patchdata.nLimitUp, patchdata.nLimitDown);
		if (pTrampoline) {
			DebugPrint("GeneralHook::SetHook: ����Trampoline�ṹ����ַ=%p\n", pTrampoline);
			DWORD dwOldProtectSystemFunction = 0;
			DWORD dwOldProtectTrampolineFunction = 0;

			// �޸��´������ڴ��Ȩ��Ϊ��ִ��
			if (VirtualProtect(pSystemFunction, dwInstructionLength, PAGE_EXECUTE_READWRITE, &dwOldProtectSystemFunction)) {
				DebugPrint("GeneralHook::SetHook: ���ô洢Trampoline������ڴ��Ȩ��Ϊ�ɶ���д��ִ��\n");
				if (VirtualProtect(pTrampoline, sizeof(MHOOKS_TRAMPOLINE), PAGE_EXECUTE_READWRITE, &dwOldProtectTrampolineFunction)) {

					// ����Trampoline����
					PBYTE pbCode = pTrampoline->codeTrampoline;

					// ����ԭ����ڴ���
					for (DWORD i = 0; i<dwInstructionLength; i++) {
						pTrampoline->codeUntouched[i] = pbCode[i] = ((PBYTE)pSystemFunction)[i];
					}
					pbCode += dwInstructionLength;

					// ������һ����תָ��
					pbCode = EmitJump(pbCode, ((PBYTE)pSystemFunction) + dwInstructionLength);
					DebugPrint("GeneralHook::SetHook: ΪTrampoline�������һ����תָ��\n");

					// ��ַ�ض�λ����
					FixupIPRelativeAddressing(pTrampoline->codeTrampoline, (PBYTE)pSystemFunction, &patchdata);

					DWORD_PTR dwDistance = (PBYTE)pHookFunction < (PBYTE)pSystemFunction ? 
						(PBYTE)pSystemFunction - (PBYTE)pHookFunction : (PBYTE)pHookFunction - (PBYTE)pSystemFunction;
					if (dwDistance > 0x7fff0000) {

						pbCode = pTrampoline->codeJumpToHookFunction;
						pbCode = EmitJump(pbCode, (PBYTE)pHookFunction);
						FlushInstructionCache(GetCurrentProcess(), pTrampoline->codeJumpToHookFunction, 
							pbCode - pTrampoline->codeJumpToHookFunction);

						// ��������IAT
						pbCode = (PBYTE)pSystemFunction;
						pbCode = EmitJump(pbCode, pTrampoline->codeJumpToHookFunction);
					} else {
						pbCode = (PBYTE)pSystemFunction;
						pbCode = EmitJump(pbCode, (PBYTE)pHookFunction);
					}

					pTrampoline->cbOverwrittenCode = dwInstructionLength;
					pTrampoline->pSystemFunction = (PBYTE)pSystemFunction;
					pTrampoline->pHookFunction = (PBYTE)pHookFunction;

					// ˢ��ָ��沢�ָ��ڴ��ԭ����Ȩ�� 
					FlushInstructionCache(GetCurrentProcess(), pTrampoline->codeTrampoline, dwInstructionLength);
					VirtualProtect(pTrampoline, sizeof(MHOOKS_TRAMPOLINE), dwOldProtectTrampolineFunction, &dwOldProtectTrampolineFunction);
				} else {
					DebugPrint("GeneralHook::SetHook: VirtualProtectʧ�ܣ�������Ϊ%d\n", GetLastError());
				}

				// ˢ��ָ��沢�ָ��ڴ��ԭ����Ȩ�� 
				FlushInstructionCache(GetCurrentProcess(), pSystemFunction, dwInstructionLength);
				VirtualProtect(pSystemFunction, dwInstructionLength, dwOldProtectSystemFunction, &dwOldProtectSystemFunction);
			} else {
				DebugPrint("GeneralHook::SetHook: VirtualProtectʧ�ܣ�������Ϊ: %d\n", GetLastError());
			}
			if (pTrampoline->pSystemFunction) {
				
				// ԭĿ�꺯���µ���ڵ�
				*ppSystemFunction = pTrampoline->codeTrampoline;
				DebugPrint("GeneralHook::SetHook: �ɹ�HookĿ�꺯��\n");
			} else {
				TrampolineFree(pTrampoline, TRUE);
				pTrampoline = NULL;
			}
		}

		// �ָ������߳�
		ResumeOtherThreads();
	} else {
		DebugPrint("GeneralHook::�����%d�ֽ�\n", dwInstructionLength);
	}

	// ����
	LeaveCritSec();
	return (pTrampoline != NULL);
}


// ж��Hook
BOOL UnHook(PVOID *ppHookedFunction) {
	DebugPrint("GeneralHook::Unhook: ж��Hook����ַ=%p\n", *ppHookedFunction);

	BOOL bRet = FALSE;

	// ����
	EnterCritSec();

	// �����Ĺ������л�ȡ��Ӧ��Trampoline������ַ
	MHOOKS_TRAMPOLINE* pTrampoline = TrampolineGet((PBYTE)*ppHookedFunction);
	if (pTrampoline) {

		// �̰߳�ȫ����
		SuspendOtherThreads(pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode);
		DebugPrint("GeneralHook::Unhook: Trampoline�ṹ��ַ=%p\n", pTrampoline);
		DWORD dwOldProtectSystemFunction = 0;
		
		// ȷ���ڴ��д
		if (VirtualProtect(pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode, PAGE_EXECUTE_READWRITE, &dwOldProtectSystemFunction)) {
			DebugPrint("GeneralHook::Unhook: ����ϵͳ�Ķ�д����\n");
			PBYTE pbCode = (PBYTE)pTrampoline->pSystemFunction;
			for (DWORD i = 0; i<pTrampoline->cbOverwrittenCode; i++) {
				pbCode[i] = pTrampoline->codeUntouched[i];
			}

			// ˢ��ָ��棬ʹ�ڴ治��д 
			FlushInstructionCache(GetCurrentProcess(), pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode);
			VirtualProtect(pTrampoline->pSystemFunction, pTrampoline->cbOverwrittenCode, dwOldProtectSystemFunction, &dwOldProtectSystemFunction);

			// ����ԭĿ�꺯���ĵ�ַ
			*ppHookedFunction = pTrampoline->pSystemFunction;
			bRet = TRUE;
			DebugPrint("GeneralHook::Unhook: ԭĿ�꺯����ַ=%p\n", *ppHookedFunction);

			// �ͷ�Trampoline����
			TrampolineFree(pTrampoline, FALSE);
			DebugPrint("GeneralHook::Unhook: ж�سɹ�\n");
		} else {
			DebugPrint("GeneralHook::Unhook: VirtualProtectʧ�ܣ�������Ϊ%d\n", GetLastError());
		}

		// �ָ������߳�
		ResumeOtherThreads();
	}

	// ����
	LeaveCritSec();
	return bRet;
}