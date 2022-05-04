#include "pch.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "CommunicationPipe.h"
#include "MyFunction.h"
#include "Misc.h"
#include "InstallAndUninstallHook.h"
#include <stdio.h>


HANDLE	hCommandPipe = NULL;
HANDLE	hDataPipe = NULL;


// 接收命令数据（Command）的线程 
DWORD WINAPI ThreadFunc_CommandPipeRecv() {
	PCHAR szBuffer = new CHAR[COMMAND_PIPE_BUF_SIZE];
	DWORD dwReturn = 0;

	while (ReadFile(hCommandPipe, szBuffer, COMMAND_PIPE_BUF_SIZE, &dwReturn, NULL)) {
		szBuffer[dwReturn] = '\0';

		DllPrintf("接收：\n");
		PrintData((PBYTE)szBuffer, dwReturn);

		if (strcmp(szBuffer, "AllInstallHook") == 0) {			// 准备用于API监测系统，HOOK全部函数
			AllInstallHook();
		}
		else if (strcmp(szBuffer, "AllUninstallHook") == 0) {	// 这个都可以用。mhook应该是就算是没hook过的，调用uninstallhook也不会出错，因为有uninstall前有额外的判断
			AllUninstallHook();
			StopWork();
		}
		else if (dwReturn % 15 == 0 && 0 == strncmp(szBuffer, "InstallHook", 11)) {	// 每条安装HOOK指令长15，InstallHook + DWORD(FuncID)。可能有粘包，故判断是否为15的倍数
			for (int i = 0; i < dwReturn / 15; i++){
				InstallOneHook((PBYTE)szBuffer + i * 15);
			}
		}
	}
	return 0;
}


// 连接服务端创建的管道
HANDLE ConnectPipe(LPSTR szPipeName) {
	// 判断是否有可以使用的命名管道实例，不成功就继续尝试 
	while (!WaitNamedPipeA(szPipeName, NMPWAIT_USE_DEFAULT_WAIT)) {
		DllPrintf("%s管道无可用实例，0.1s后再次尝试\n", szPipeName);
		Sleep(100);
	}
	DllPrintf("%s管道存在可用实例\n", szPipeName);

	// 打开可用的命名管道 , 并与服务器端进程进行通信  
	HANDLE hPipe = CreateFileA(szPipeName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		DllPrintf("%s管道打开失败\n", szPipeName);
		return NULL;
	}
	else 
		DllPrintf("%s管道打开成功\n", szPipeName);

	return hPipe;
}


DWORD WINAPI ThreadFunc_Main(LPVOID lpParameter) {
	hCommandPipe = ConnectPipe(COMMAND_PIPE);
	hDataPipe = ConnectPipe(DATA_PIPE);

	if (hCommandPipe == NULL || hDataPipe == NULL)
		return 0;

	// 接收数据的线程 
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc_CommandPipeRecv, NULL, 0, NULL);

	return 0;
}




DWORD SendData(PBYTE pBuffer, DWORD dwBufLen) {
	DWORD dwReturn = 0;

	if (hDataPipe == NULL) {
		DllPrintf("hDataPipe为NULL");
		return 0;
	}

	if (!WriteFile(hDataPipe, pBuffer, dwBufLen, &dwReturn, NULL))
	{
		DllPrintf("向DataPipe管道写入数据失败，即将卸载HOOK并销毁管道\n");
		AllUninstallHook();
		StopWork();
	}
	return dwReturn;
}


DWORD SendData(PCHAR szBuffer) {
	return SendData((PBYTE)szBuffer, strlen(szBuffer));
}

void StopWork() {
	if (hCommandPipe != NULL) {
		CloseHandle(hCommandPipe);
		hCommandPipe = NULL;
		DllPrintf("销毁hCommandPipe\n");
	}

	if (hDataPipe != NULL) {
		CloseHandle(hDataPipe);
		hDataPipe = NULL;
		DllPrintf("销毁hDataPipe\n");
	}
}