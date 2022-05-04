#pragma once
#include <windows.h>


#define COMMAND_PIPE_BUF_SIZE		4096
#define COMMAND_PIPE				"\\\\.\\pipe\\CommandPipe"

#define DATA_PIPE_BUF_SIZE			0xffffff
#define DATA_PIPE					"\\\\.\\pipe\\DataPipe"


extern HANDLE	hCommandPipe;
extern HANDLE	hDataPipe;


DWORD WINAPI ThreadFunc_Main(LPVOID lpParameter);


// 向server发数据（DataPipe管道）
DWORD SendData(PBYTE szBuffer, DWORD dwBufLen);
DWORD SendData(PCHAR szBuffer);

void AllInstallHook();
void AllUninstallHook();

void StopWork();