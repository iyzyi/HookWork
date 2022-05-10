#include "BasicHook.h"
#include "LDE.h"
#include <stdio.h>


bool BasicHook::InitHook(void* pHookPointAddr, void* pfnMyFunc, int Architecture) {

	pHookPoint = (ULONG_PTR)pHookPointAddr;

	dwHookCodeLen = LengthDisasm((void*)pHookPoint, Architecture, 12);		// 12是x64的
	printf("HookCodeLen = %d\n", dwHookCodeLen);
	if (dwHookCodeLen == 0)
		MessageBoxA(NULL, "反汇编长度为0", "错误", NULL);

	memset(byOldEntry, 0, sizeof(byOldEntry));
	memset(byNewEntry, 0, sizeof(byNewEntry));

	pfnTrampolineFun = (ULONG_PTR)VirtualAlloc(NULL, 128, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	pfnDetourFun = (ULONG_PTR)pfnMyFunc;

	return true;
}


bool BasicHook::InstallHook() {
	SIZE_T dwBytesReturned = 0;
	HANDLE hProcess = GetCurrentProcess();

	BOOL bResult = FALSE;
	if (pHookPoint == 0 || pfnDetourFun == NULL || pfnTrampolineFun == NULL)
	{
		return FALSE;
	}

	pJmpBackAddr = pHookPoint + dwHookCodeLen;

	LPVOID pOriginalAddr = (LPVOID)pHookPoint;

	InitHookEntry();				//填充Inline Hook代码
	InitTrampoline();				//构造Trampoline
	
	if (ReadProcessMemory(hProcess, pOriginalAddr, byOldEntry, dwHookCodeLen, &dwBytesReturned))	//读取并保存原有入口点的几条指令
	{
		if (WriteProcessMemory(hProcess, pOriginalAddr, byNewEntry, dwHookCodeLen, &dwBytesReturned))
		{
			bResult = TRUE;
		}
	}

	pfnOriginalFunc = (void*)pfnTrampolineFun;			//相当于HOOK前的函数

	return bResult;
}


bool BasicHook::UninstallHook() {
	return false;
}


//填充入口点指令
//使用的是mov rax xxxxxxxx; jmp rax，长度12
//为了清除指令碎屑，后面要跟着M个nop
void BasicHook::InitHookEntry()
{
	byNewEntry[0] = 0x48;
	byNewEntry[1] = 0xb8;
	*(ULONG_PTR*)(byNewEntry + 2) = pfnDetourFun;
	byNewEntry[10] = 0xff;
	byNewEntry[11] = 0xe0;

	// NOP
	for (int i = 12; i < dwHookCodeLen; i++) {
		byNewEntry[i] = 0x90;
	}
}


//构造从hook后的函数中回到原有函数的指令
//由原来函数的入口点指令加上一个jmp构成
void BasicHook::InitTrampoline()
{
	//保存前dwHookCodeLen字节
	PBYTE pFun = (PBYTE)pfnTrampolineFun;
	memcpy(pFun, (void*)pHookPoint, dwHookCodeLen);

	//在后面添加一个跳转指令
	pFun += dwHookCodeLen; //跳过前N行指令
	pFun[0] = 0xFF;
	pFun[1] = 0x25;
	*(ULONG_PTR*)(pFun + 6) = pJmpBackAddr;
}


////获取指定模块中指定API的地址
//void* BasicHook::GetAddress(char* dllname, char* funname)
//{
//	HMODULE hMod = 0;
//	if (hMod = GetModuleHandleA(dllname))
//	{
//		printf("%s早已加载\n", dllname);
//		return GetProcAddress(hMod, funname);
//	}
//	else
//	{
//		printf("成功加载%s\n", dllname);
//		hMod = LoadLibraryA(dllname);
//		return GetProcAddress(hMod, funname);
//	}
//}