#include "BasicHook.h"
#include "LDE.h"
#include <stdio.h>


bool BasicHook::InitHook(void* pHookPointAddr, void* pfnMyFunc, int Architecture) {

	pHookPoint = (ULONG_PTR)pHookPointAddr;

	dwHookCodeLen = LengthDisasm((void*)pHookPoint, Architecture, 12);		// 12��x64��
	printf("HookCodeLen = %d\n", dwHookCodeLen);
	if (dwHookCodeLen == 0)
		MessageBoxA(NULL, "����೤��Ϊ0", "����", NULL);

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

	InitHookEntry();				//���Inline Hook����
	InitTrampoline();				//����Trampoline
	
	if (ReadProcessMemory(hProcess, pOriginalAddr, byOldEntry, dwHookCodeLen, &dwBytesReturned))	//��ȡ������ԭ����ڵ�ļ���ָ��
	{
		if (WriteProcessMemory(hProcess, pOriginalAddr, byNewEntry, dwHookCodeLen, &dwBytesReturned))
		{
			bResult = TRUE;
		}
	}

	pfnOriginalFunc = (void*)pfnTrampolineFun;			//�൱��HOOKǰ�ĺ���

	return bResult;
}


bool BasicHook::UninstallHook() {
	return false;
}


//�����ڵ�ָ��
//ʹ�õ���mov rax xxxxxxxx; jmp rax������12
//Ϊ�����ָ����м������Ҫ����M��nop
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


//�����hook��ĺ����лص�ԭ�к�����ָ��
//��ԭ����������ڵ�ָ�����һ��jmp����
void BasicHook::InitTrampoline()
{
	//����ǰdwHookCodeLen�ֽ�
	PBYTE pFun = (PBYTE)pfnTrampolineFun;
	memcpy(pFun, (void*)pHookPoint, dwHookCodeLen);

	//�ں������һ����תָ��
	pFun += dwHookCodeLen; //����ǰN��ָ��
	pFun[0] = 0xFF;
	pFun[1] = 0x25;
	*(ULONG_PTR*)(pFun + 6) = pJmpBackAddr;
}


////��ȡָ��ģ����ָ��API�ĵ�ַ
//void* BasicHook::GetAddress(char* dllname, char* funname)
//{
//	HMODULE hMod = 0;
//	if (hMod = GetModuleHandleA(dllname))
//	{
//		printf("%s���Ѽ���\n", dllname);
//		return GetProcAddress(hMod, funname);
//	}
//	else
//	{
//		printf("�ɹ�����%s\n", dllname);
//		hMod = LoadLibraryA(dllname);
//		return GetProcAddress(hMod, funname);
//	}
//}