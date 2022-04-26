#include "pch.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "CommunicationPipe.h"
#include "MyFunction.h"
#include "Misc.h"
#include <stdio.h>


HANDLE	hCommandPipe = NULL;
HANDLE	hDataPipe = NULL;


// 接收命令数据（Command）的线程 
DWORD WINAPI ThreadFunc_CommandPipeRecv() {
	PCHAR szBuffer = new CHAR[COMMAND_PIPE_BUF_SIZE];
	DWORD dwReturn = 0;

	while (ReadFile(hCommandPipe, szBuffer, COMMAND_PIPE_BUF_SIZE, &dwReturn, NULL)) {
		szBuffer[dwReturn] = '\0';
		//DllPrintf("接收: % s\n", szBuffer);

		if (strcmp(szBuffer, "InstallHook") == 0) {
			DllPrintf("InstallHook......\n");
			InstallHook((void**)&True_Recv, My_Recv);
			InstallHook((void**)&True_Send, My_Send);
		}
		else if (strcmp(szBuffer, "UninstallHook") == 0) {
			DllPrintf("UninstallHook......\n");
			UninstallHook((void**)&True_Recv);
			UninstallHook((void**)&True_Send);
		}
	}
	return 0;
}


// 连接服务端创建的管道
HANDLE ConnectPipe(LPSTR szPipeName) {
	// 判断是否有可以使用的命名管道实例，不成功就继续尝试 
	while (!WaitNamedPipeA(szPipeName, NMPWAIT_USE_DEFAULT_WAIT)) {
		printf("%s管道无可用实例\n", szPipeName);
		Sleep(100);
	}

	// 打开可用的命名管道 , 并与服务器端进程进行通信  
	HANDLE hPipe = CreateFileA(szPipeName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("%s管道打开失败\n", szPipeName);
		return NULL;
	}
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




DWORD SendData(PCHAR szBuffer, DWORD dwBufLen) {
	DWORD dwReturn = 0;

	if (hDataPipe == NULL) {
		DllPrintf("hDataPipe为NULL");
		return 0;
	}

	if (!WriteFile(hDataPipe, szBuffer, dwBufLen, &dwReturn, NULL))
	{
		DllPrintf("向DataPipe管道写入数据失败\n");
	}
	return dwReturn;
}


DWORD SendData(PCHAR szBuffer) {
	return SendData(szBuffer, strlen(szBuffer));
}