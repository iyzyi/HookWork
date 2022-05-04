#include "pch.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "CommunicationPipe.h"
#include "MyFunction.h"
#include "Misc.h"
#include "InstallAndUninstallHook.h"
#include <stdio.h>


HANDLE	hCommandPipe = NULL;
HANDLE	hDataPipe = NULL;


// �����������ݣ�Command�����߳� 
DWORD WINAPI ThreadFunc_CommandPipeRecv() {
	PCHAR szBuffer = new CHAR[COMMAND_PIPE_BUF_SIZE];
	DWORD dwReturn = 0;

	while (ReadFile(hCommandPipe, szBuffer, COMMAND_PIPE_BUF_SIZE, &dwReturn, NULL)) {
		szBuffer[dwReturn] = '\0';

		DllPrintf("���գ�\n");
		PrintData((PBYTE)szBuffer, dwReturn);

		if (strcmp(szBuffer, "AllInstallHook") == 0) {			// ׼������API���ϵͳ��HOOKȫ������
			AllInstallHook();
		}
		else if (strcmp(szBuffer, "AllUninstallHook") == 0) {	// ����������á�mhookӦ���Ǿ�����ûhook���ģ�����uninstallhookҲ���������Ϊ��uninstallǰ�ж�����ж�
			AllUninstallHook();
			StopWork();
		}
		else if (dwReturn % 15 == 0 && 0 == strncmp(szBuffer, "InstallHook", 11)) {	// ÿ����װHOOKָ�15��InstallHook + DWORD(FuncID)��������ճ�������ж��Ƿ�Ϊ15�ı���
			for (int i = 0; i < dwReturn / 15; i++){
				InstallOneHook((PBYTE)szBuffer + i * 15);
			}
		}
	}
	return 0;
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
		DllPrintf("����hCommandPipe\n");
	}

	if (hDataPipe != NULL) {
		CloseHandle(hDataPipe);
		hDataPipe = NULL;
		DllPrintf("����hDataPipe\n");
	}
}