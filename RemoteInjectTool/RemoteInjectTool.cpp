#include <stdio.h>
#include <tchar.h>
#include "stdlib.h"
#include "windows.h"
#include <Tlhelp32.h>
#include <Shlwapi.h>  
#pragma comment(lib, "shlwapi.lib")
#include "atlconv.h"
#include <assert.h>


#define PROCESS_ID_LIST_NUMBER 24						// 通过进程名称来查找进程可能会找到多个PID

DWORD GetProcessIDByName(PWCHAR pwszName, PDWORD ProcessIdList)
{
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
		//USES_CONVERSION;
		//printf("%d\t%s\n", pe.th32ProcessID, W2A(pe.szExeFile));
	}
	CloseHandle(hSnapshot);
	return dwProcessIdNumbers;
}


HMODULE GetHModuleIDByName(DWORD dwPid, PWCHAR pwszName)
{
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
		//USES_CONVERSION;
		//printf("%p\t%s\n", me.hModule, W2A(me.szModule));
	}
	CloseHandle(hSnapshot);
	return NULL;
}


HMODULE GetModuleBaseAddress(DWORD dwPID, PWCHAR pwszName) {
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
			//printf("0x%x\t%s\n", ModuleEntry32.modBaseAddr, PathFindFileName(ModuleEntry32.szExePath));
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


BOOL RemoteInject(DWORD dwPid, PCHAR szDllPath) {
	HANDLE	hThread;								// 远程线程，线程函数为LoadLibrary
	HANDLE	hProcess;								// 远程进程句柄;
	void* pDllPathRemote;							// szDllPath拷贝到远程进程的空间中
	DWORD hDllRemoteBaseAddr;						// 远程注入的DLL在远程进程中的基地址（HMODULE）;
	HMODULE hKernel32;								// Kernel32.dll的基址
	CHAR	szLoadLibrary[] = "LoadLibraryA";		// A还是W，取决于szDllPath是PCHAR还是PWCHAR
	HMODULE hModule;								// 注入到远程进程中的DLL的HModule

	// 打开句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL) {
		printf("打开句柄失败\n");
		return FALSE;
	}

	// 分配内存
	pDllPathRemote = VirtualAllocEx(hProcess, NULL, sizeof(szDllPath), MEM_COMMIT, PAGE_READWRITE);

	// DLL路径写进分配的内存中
	WriteProcessMemory(hProcess, pDllPathRemote, (void*)szDllPath, strlen(szDllPath) + 1, NULL);

	hKernel32 = GetModuleHandleA("Kernel32");
	FARPROC pLoadLibraryAddress = GetProcAddress(hKernel32, szLoadLibrary);

	// 创建远程线程，线程函数为LoadLibrary(pDllPathRemote);
	hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pDllPathRemote, 0, NULL);
	if (hThread == NULL) {
		printf("创建远程线程失败\n");
		return FALSE;
	}

	// 等待返回
	WaitForSingleObject(hThread, INFINITE);

	// 取得DLL的基地址
	GetExitCodeThread(hThread, &hDllRemoteBaseAddr);

	// 关闭句柄
	CloseHandle(hThread);

	// 释放内存
	VirtualFreeEx(hProcess, pDllPathRemote, sizeof(szDllPath), MEM_RELEASE);

	// 在远程进程中查找注入的DLL Module，若找不到说明注入失败
	hModule = GetHModuleIDByName(dwPid, (PWCHAR)L"RemoteInjectDll.dll");
	if (hModule == NULL)
	{
		printf("DLL注入失败");
		return FALSE;
	}

	return TRUE;
}



int main()
{
	WCHAR ProcessName[] = L"SGSOL.exe";
	DWORD ProcessIdList[PROCESS_ID_LIST_NUMBER];
	DWORD dwProcessIdNumbers = GetProcessIDByName(ProcessName, ProcessIdList);
	printf("共%d个进程\n", dwProcessIdNumbers);

	for (DWORD i = 0; i < dwProcessIdNumbers; i++) {
		printf("pid=%d\n", ProcessIdList[i]);
		CHAR szDllPath[] = "RemoteInjectDll.dll";
		RemoteInject(ProcessIdList[i], szDllPath);
	}

	system("pause");
	return 0;
}