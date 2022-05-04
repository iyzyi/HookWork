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

		if (strcmp(szBuffer, "InstallHook") == 0) {
			AllInstallHook();
		}
		else if (strcmp(szBuffer, "UninstallHook") == 0) {
			AllUninstallHook();
			StopWork();
		}
	}
	return 0;
}


void AllInstallHook() {
	DllPrintf("InstallHook......\n");

	InstallHook((void**)&True_send, My_send);
	InstallHook((void**)&True_sendto, My_sendto);
	InstallHook((void**)&True_WSASend, My_WSASend);
	InstallHook((void**)&True_WSASendTo, My_WSASendTo);
	InstallHook((void**)&True_WSASendMsg, My_WSASendMsg);

	InstallHook((void**)&True_recv, My_recv);
	InstallHook((void**)&True_recvfrom, My_recvfrom);
	InstallHook((void**)&True_WSARecv, My_WSARecv);
	InstallHook((void**)&True_WSARecvFrom, My_WSARecvFrom);
}


void AllUninstallHook() {
	DllPrintf("UninstallHook......\n");

	UninstallHook((void**)&True_send);
	UninstallHook((void**)&True_sendto);
	UninstallHook((void**)&True_WSASend);
	UninstallHook((void**)&True_WSASendTo);
	UninstallHook((void**)&True_WSASendMsg);

	UninstallHook((void**)&True_recv);
	UninstallHook((void**)&True_recvfrom);
	UninstallHook((void**)&True_WSARecv);
	UninstallHook((void**)&True_WSARecvFrom);
}


// ���ӷ���˴����Ĺܵ�
HANDLE ConnectPipe(LPSTR szPipeName) {
	// �ж��Ƿ��п���ʹ�õ������ܵ�ʵ�������ɹ��ͼ������� 
	while (!WaitNamedPipeA(szPipeName, NMPWAIT_USE_DEFAULT_WAIT)) {
		DllPrintf("%s�ܵ��޿���ʵ����0.1s���ٴγ���\n", szPipeName);
		Sleep(100);
	}
	DllPrintf("%s�ܵ����ڿ���ʵ��\n", szPipeName);

	// �򿪿��õ������ܵ� , ����������˽��̽���ͨ��  
	HANDLE hPipe = CreateFileA(szPipeName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		DllPrintf("%s�ܵ���ʧ��\n", szPipeName);
		return NULL;
	}
	else 
		DllPrintf("%s�ܵ��򿪳ɹ�\n", szPipeName);

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




DWORD SendData(PBYTE pBuffer, DWORD dwBufLen) {
	DWORD dwReturn = 0;

	if (hDataPipe == NULL) {
		DllPrintf("hDataPipeΪNULL");
		return 0;
	}

	if (!WriteFile(hDataPipe, pBuffer, dwBufLen, &dwReturn, NULL))
	{
		DllPrintf("��DataPipe�ܵ�д������ʧ�ܣ�����ж��HOOK�����ٹܵ�\n");
		AllUninstallHook();

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
		DllPrintf("����hCommandPipe\n");
	}

	if (hDataPipe != NULL) {
		CloseHandle(hDataPipe);
		hDataPipe = NULL;
		DllPrintf("����hDataPipe\n");
	}
}