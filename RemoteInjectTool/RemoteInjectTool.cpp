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

#include "RemoteInjectTool.h"


#define PROCESS_ID_LIST_NUMBER 256						// 通过进程名称来查找进程可能会找到多个PID


DWORD GetProcessIDByName(PCHAR szName, PDWORD ProcessIdList)
{
	int dwWideSize = MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), NULL, 0);
	WCHAR pwszName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), (LPWSTR)pwszName, dwWideSize);

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
			//assert(dwProcessIdNumbers + 1 <= PROCESS_ID_LIST_NUMBER);
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
	WCHAR pwszName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), (LPWSTR)pwszName, dwWideSize);

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
	WCHAR pwszName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), (LPWSTR)pwszName, dwWideSize);

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
	//struct stat buffer;
	//return (stat(name, &buffer) == 0);
	return PathFileExistsA(name);
}


BOOL RemoteInjectByProcessId(DWORD dwPid, PCHAR szDllPath) {
	HANDLE			hThread;								// 远程线程，线程函数为LoadLibrary
	HANDLE			hProcess;								// 远程进程句柄;
	void*			pDllPathRemote;							// szDllPath拷贝到远程进程的空间中
	DWORD			hDllRemoteBaseAddr;						// 远程注入的DLL在远程进程中的基地址（HMODULE）;
	HMODULE			hKernel32;								// Kernel32.dll的基址
	CHAR			szLoadLibrary[] = "LoadLibraryA";		// A还是W，取决于szDllPath是PCHAR还是PWCHAR
	HMODULE			hModule;								// 注入到远程进程中的DLL的HModule

	if (!isFileExists(szDllPath)) {
		printf("%s 不存在\n", szDllPath);
		return FALSE;
	}

	// 从路径中提取DLL的名称
	CHAR szDllName[MAX_PATH], * p;
	strcpy(szDllName, (p = strrchr(szDllPath, '\\')) ? p + 1 : szDllPath);

	// 注入之前先检查目标进程的内存内是否存在szDllName
	// 若存在，说明之前注入过一次了，先远程卸载这个DLL
	hModule = GetHModuleIDByName(dwPid, szDllName);
	if (hModule != NULL)
	{
		printf("[PID=0x%.8x] %s早已存在于目标进程内，正在卸载该DLL\n", dwPid, szDllName);
		RemoteUnInjectDllByProcessId(dwPid, L"InjectDll.dll");
	}

	// 打开句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL) {
		printf("打开[PID=0x%.8x]进程句柄失败\n", dwPid);
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
	hModule = GetHModuleIDByName(dwPid, szDllName);
	if (hModule == NULL)
	{
		printf("[PID=0x%.8x] %s注入失败\n", dwPid, szDllName);
		return FALSE;
	}
	else {
		printf("[PID=0x%.8x] %s注入成功\n", dwPid, szDllName);
		return TRUE;
	}
}


DWORD RemoteInjectByProcessName(PCHAR szProcessName, PCHAR szDllPath) {
	DWORD ProcessIdList[PROCESS_ID_LIST_NUMBER];
	DWORD dwProcessIdNumbers = GetProcessIDByName(szProcessName, ProcessIdList);
	printf("%s共%d个进程\n", szProcessName, dwProcessIdNumbers);
	DWORD dwSuccessNumbers = 0;

	for (DWORD i = 0; i < dwProcessIdNumbers; i++) {
		if (RemoteInjectByProcessId(ProcessIdList[i], szDllPath))
			dwSuccessNumbers++;
	}

	return ((dwProcessIdNumbers & 0xffff) << 16) | (dwSuccessNumbers & 0xffff);
}


//// 远程卸载DLL
//void RemoteUnInjectDllByProcessId(DWORD dwPid, char* szDllName)
//{
//	// 使目标进程调用GetModuleHandle，获得DLL在目标进程中的句柄  
//	DWORD dwHandle;
//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
//	LPVOID pFunc = GetModuleHandleA;
//	char lpBuf[MAXBYTE];
//	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
//		(LPTHREAD_START_ROUTINE)pFunc, lpBuf, 0, &dwPid);
//
//	// 等待GetModuleHandle运行完毕  
//	WaitForSingleObject(hThread, INFINITE);
//
//	// 获得GetModuleHandle的返回值  
//	GetExitCodeThread(hThread, &dwHandle);
//
//	// 释放目标进程中申请的空间  
//	int dwSize = strlen(szDllName) + sizeof(char);
//	VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
//	CloseHandle(hThread);
//
//	// 使目标进程调用FreeLibrary，卸载DLL  
//	pFunc = FreeLibrary;
//	hThread = CreateRemoteThread(hProcess, NULL, 0,
//		(LPTHREAD_START_ROUTINE)pFunc, (LPVOID)dwHandle, 0, &dwPid);
//
//	// 等待FreeLibrary卸载完毕  
//	WaitForSingleObject(hThread, INFINITE);
//	CloseHandle(hThread);
//	CloseHandle(hProcess);
//}

BOOL RemoteUnInjectDllByProcessId(DWORD dwPID, LPCTSTR szDllName)
{
	BOOL bMore = FALSE, bFound = FALSE;
	HANDLE hSnapshot, hProcess, hThread;
	HMODULE hModule = NULL;
	MODULEENTRY32 me = { sizeof(me) }; //定义一个用于储存模块快照的结构体
	LPTHREAD_START_ROUTINE pThreadProc;

	//1.
	//dwPID=notepad进程ID
	//使用TH32CS_SNAPMODULE参数
	//获取加载到notepad进程的DLL名称
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	//此函数检索与进程相关联的第一个模块的信息
	bMore = Module32First(hSnapshot, &me);

	for (; bMore; bMore = Module32Next(hSnapshot, &me))   //bMore用于判断该进程的模块快照是否还有，bFound用于判断是否找到了我们想要卸载的dll模块
	{
		if (!_tcsicmp((LPCTSTR)me.szModule, szDllName) || !_tcsicmp((LPCTSTR)me.szExePath, szDllName))
		{
			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}
	//2. 通过进程PID获取进程句柄
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		_tprintf(L"OpenProcess(%d) failed!!![%d]\n", dwPID, GetLastError);
		return FALSE;
	}
	//3. 获取FreeLibrary函数的地址
	hModule = GetModuleHandle(L"kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
	//4.创建线程来执行FreeLibrary(modBaseAddr要卸载的dll模块基址)
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);

	return TRUE;
}




//int main()
//{
//	CHAR szDllPath[MAX_PATH + 1] = { 0 };
//	GetModuleFileNameA(NULL, szDllPath, MAX_PATH);		// 获取本程序所在路径
//	(strrchr(szDllPath, '\\'))[1] = 0;					// 路径中去掉本程序名称
//	strcat(szDllPath, "SGSOL.dll");						// 拼接上DLL的名称
//
//	RemoteInjectByProcessName("SGSOL.exe", szDllPath);
//
//	system("pause");
//	return 0;
//}