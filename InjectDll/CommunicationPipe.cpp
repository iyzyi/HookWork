#include "pch.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "CommunicationPipe.h"
#include "MyFunction.h"
#include "Misc.h"
#include <stdio.h>


HANDLE	hCommandPipe = NULL;
HANDLE	hDataPipe = NULL;


// �����������ݣ�Command�����߳� 
DWORD WINAPI ThreadFunc_CommandPipeRecv() {
	PCHAR szBuffer = new CHAR[COMMAND_PIPE_BUF_SIZE];
	DWORD dwReturn = 0;

	while (ReadFile(hCommandPipe, szBuffer, COMMAND_PIPE_BUF_SIZE, &dwReturn, NULL)) {
		szBuffer[dwReturn] = '\0';
		//DllPrintf("����: % s\n", szBuffer);

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


// ���ӷ���˴����Ĺܵ�
HANDLE ConnectPipe(LPSTR szPipeName) {
	// �ж��Ƿ��п���ʹ�õ������ܵ�ʵ�������ɹ��ͼ������� 
	while (!WaitNamedPipeA(szPipeName, NMPWAIT_USE_DEFAULT_WAIT)) {
		printf("%s�ܵ��޿���ʵ��\n", szPipeName);
		Sleep(100);
	}

	// �򿪿��õ������ܵ� , ����������˽��̽���ͨ��  
	HANDLE hPipe = CreateFileA(szPipeName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("%s�ܵ���ʧ��\n", szPipeName);
		return NULL;
	}
	return hPipe;
}


DWORD WINAPI ThreadFunc_Main(LPVOID lpParameter) {
	hCommandPipe = ConnectPipe(COMMAND_PIPE);
	hDataPipe = ConnectPipe(DATA_PIPE);

	if (hCommandPipe == NULL || hDataPipe == NULL)
		return 0;

	// �������ݵ��߳� 
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc_CommandPipeRecv, NULL, 0, NULL);

	return 0;
}




DWORD SendData(PCHAR szBuffer, DWORD dwBufLen) {
	DWORD dwReturn = 0;

	if (hDataPipe == NULL) {
		DllPrintf("hDataPipeΪNULL");
		return 0;
	}

	if (!WriteFile(hDataPipe, szBuffer, dwBufLen, &dwReturn, NULL))
	{
		DllPrintf("��DataPipe�ܵ�д������ʧ��\n");
	}
	return dwReturn;
}


DWORD SendData(PCHAR szBuffer) {
	return SendData(szBuffer, strlen(szBuffer));
}