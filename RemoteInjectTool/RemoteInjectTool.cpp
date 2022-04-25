#include <stdio.h>
#include <tchar.h>
#include "stdlib.h"
#include "windows.h"
#include <Tlhelp32.h>
#include <Shlwapi.h>  
#pragma comment(lib, "shlwapi.lib")
#include "atlconv.h"
#include <assert.h>
#include <direct.h>
#pragma warning(disable:4996) 


#define PROCESS_ID_LIST_NUMBER 256						// ͨ���������������ҽ��̿��ܻ��ҵ����PID


DWORD GetProcessIDByName(PCHAR szName, PDWORD ProcessIdList)
{
	int dwWideSize = MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), NULL, 0);
	PWCHAR pwszName = (wchar_t*)malloc(dwWideSize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), pwszName, dwWideSize);

	DWORD dwProcessIdNumbers = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe))
	{
		if (wcscmp(pe.szExeFile, pwszName) == 0)
		{
			assert(dwProcessIdNumbers + 1 <= PROCESS_ID_LIST_NUMBER);
			ProcessIdList[dwProcessIdNumbers] = pe.th32ProcessID;
			dwProcessIdNumbers++;
		}
	}
	CloseHandle(hSnapshot);
	return dwProcessIdNumbers;
}


HMODULE GetHModuleIDByName(DWORD dwPid, PCHAR szName)
{
	int dwWideSize = MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), NULL, 0);
	PWCHAR pwszName = (wchar_t*)malloc(dwWideSize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), pwszName, dwWideSize);

	DWORD dwProcessIdNumbers = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	MODULEENTRY32 me;
	me.dwSize = sizeof(me);
	for (BOOL ret = Module32First(hSnapshot, &me); ret; ret = Module32Next(hSnapshot, &me))
	{
		if (wcscmp(me.szModule, pwszName) == 0)
		{
			CloseHandle(hSnapshot);
			return me.hModule;
		}
	}
	CloseHandle(hSnapshot);
	return NULL;
}


HMODULE GetModuleBaseAddress(DWORD dwPID, PCHAR szName) {
	int dwWideSize = MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), NULL, 0);
	PWCHAR pwszName = (wchar_t*)malloc(dwWideSize * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), pwszName, dwWideSize);

	HANDLE hSnapShot;
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	MODULEENTRY32 ModuleEntry32;
	ModuleEntry32.dwSize = sizeof(ModuleEntry32);
	if (Module32First(hSnapShot, &ModuleEntry32))
	{
		do
		{
			LPTSTR ModuleName = PathFindFileName(ModuleEntry32.szExePath);
			if (wcscmp(ModuleName, pwszName) == 0) {
				CloseHandle(hSnapShot);
				return (HMODULE)ModuleEntry32.modBaseAddr;
			}
		} while (Module32Next(hSnapShot, &ModuleEntry32));
	}
	CloseHandle(hSnapShot);
	return NULL;
}

bool isFileExists(char* name) {
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}


BOOL RemoteInjectByProcessId(DWORD dwPid, PCHAR szDllPath) {
	HANDLE			hThread;								// Զ���̣߳��̺߳���ΪLoadLibrary
	HANDLE			hProcess;								// Զ�̽��̾��;
	void*			pDllPathRemote;							// szDllPath������Զ�̽��̵Ŀռ���
	DWORD			hDllRemoteBaseAddr;						// Զ��ע���DLL��Զ�̽����еĻ���ַ��HMODULE��;
	HMODULE			hKernel32;								// Kernel32.dll�Ļ�ַ
	CHAR			szLoadLibrary[] = "LoadLibraryA";		// A����W��ȡ����szDllPath��PCHAR����PWCHAR
	HMODULE			hModule;								// ע�뵽Զ�̽����е�DLL��HModule

	// �򿪾��
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL) {
		printf("�򿪾��ʧ��\n");
		return FALSE;
	}

	// �����ڴ�
	pDllPathRemote = VirtualAllocEx(hProcess, NULL, sizeof(szDllPath), MEM_COMMIT, PAGE_READWRITE);

	// DLL·��д��������ڴ���
	WriteProcessMemory(hProcess, pDllPathRemote, (void*)szDllPath, strlen(szDllPath) + 1, NULL);

	hKernel32 = GetModuleHandleA("Kernel32");
	FARPROC pLoadLibraryAddress = GetProcAddress(hKernel32, szLoadLibrary);

	// ����Զ���̣߳��̺߳���ΪLoadLibrary(pDllPathRemote);
	hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pDllPathRemote, 0, NULL);
	if (hThread == NULL) {
		printf("����Զ���߳�ʧ��\n");
		return FALSE;
	}

	// �ȴ�����
	WaitForSingleObject(hThread, INFINITE);

	// ȡ��DLL�Ļ���ַ
	GetExitCodeThread(hThread, &hDllRemoteBaseAddr);

	// �رվ��
	CloseHandle(hThread);

	// �ͷ��ڴ�
	VirtualFreeEx(hProcess, pDllPathRemote, sizeof(szDllPath), MEM_RELEASE);

	// ��·������ȡDLL������
	CHAR szDllName[MAX_PATH], * p;
	strcpy(szDllName, (p = strrchr(szDllPath, '\\')) ? p + 1 : szDllPath);

	// ��Զ�̽����в���ע���DLL Module�����Ҳ���˵��ע��ʧ��
	hModule = GetHModuleIDByName(dwPid, szDllName);
	if (hModule == NULL)
	{
		printf("[PID=0x%.8x] %sע��ʧ��\n\n", dwPid, szDllName);
		return FALSE;
	}
	else {
		printf("[PID=0x%.8x] %sע��ɹ�\n\n", dwPid, szDllName);
		return TRUE;
	}
}


DWORD RemoteInjectByProcessName(PCHAR szProcessName, PCHAR szDllPath) {
	DWORD ProcessIdList[PROCESS_ID_LIST_NUMBER];
	DWORD dwProcessIdNumbers = GetProcessIDByName(szProcessName, ProcessIdList);
	printf("%s��%d������\n", szProcessName, dwProcessIdNumbers);
	DWORD dwSuccessNumbers = 0;

	if (!isFileExists(szDllPath)) {
		printf("%s ������\n", szDllPath);
		return FALSE;
	}
	else {
		for (DWORD i = 0; i < dwProcessIdNumbers; i++) {
			printf("pid=%d\n", ProcessIdList[i]);
			if (RemoteInjectByProcessId(ProcessIdList[i], szDllPath))
				dwSuccessNumbers++;
		}
	}
	return ((dwProcessIdNumbers & 0xffff) << 16) | (dwSuccessNumbers & 0xffff);
}



//int main()
//{
//	CHAR szDllPath[MAX_PATH + 1] = { 0 };
//	GetModuleFileNameA(NULL, szDllPath, MAX_PATH);		// ��ȡ����������·��
//	(strrchr(szDllPath, '\\'))[1] = 0;					// ·����ȥ������������
//	strcat(szDllPath, "SGSOL.dll");						// ƴ����DLL������
//
//	RemoteInjectByProcessName("SGSOL.exe", szDllPath);
//
//	system("pause");
//	return 0;
//}