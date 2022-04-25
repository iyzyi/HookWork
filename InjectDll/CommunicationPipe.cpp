#include "pch.h"
#include "CommunicationPipe.h"
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
			//InstallHook((void**)&TrueRecv, My_Recv);
			//InstallHook((void**)&TrueSend, My_Send);
		}
		else if (strcmp(szBuffer, "UninstallHook") == 0) {
			DllPrintf("UninstallHook......\n");
			//UninstallHook((void**)&TrueRecv);
			//UninstallHook((void**)&TrueSend);
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