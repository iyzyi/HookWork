#include <stdio.h>
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "../RemoteInjectTool/RemoteInjectTool.h"



// **********************************Hook MessageBoxA*********************************************

typedef int (WINAPI* PFN_MessageBoxA)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

PFN_MessageBoxA TrueMessageBoxA = (PFN_MessageBoxA)GetProcAddress(GetModuleHandleA("user32"), "MessageBoxA");

int WINAPI MyMessageBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    int ret;
    char newText[1024] = { 0 };
    char newCaption[] = "iyzyi";
    strcpy_s(newText, (char*)lpText);
    strcat_s(newText, "\nMessageBoxA Hacked by iyzyi!");//�۸���Ϣ������
    uType |= MB_ICONERROR;
    ret = TrueMessageBoxA(hWnd, (LPCTSTR)newText, (LPCTSTR)newCaption, uType);
    return ret;
}

void test_hook_messagebox() {
    MessageBoxA(NULL, "�����ı�1", "����1", NULL);

    InstallHook((void**)&TrueMessageBoxA, MyMessageBoxA);

    MessageBoxA(NULL, "�����ı�2", "����2", NULL);

    UninstallHook((void**)&TrueMessageBoxA);

    MessageBoxA(NULL, "�����ı�3", "����3", NULL);
}




// *******************************Զ��ע��*****************************************

void test_remote_inject() {
    CHAR szDllPath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, szDllPath, MAX_PATH);		// ��ȡ����������·��
    (strrchr(szDllPath, '\\'))[1] = 0;					// ·����ȥ������������
    strcat_s(szDllPath, "SGSOL.dll");			        // ƴ����DLL������

    //RemoteInjectByProcessName("SGSOL.exe", szDllPath);

    // SGSOL.exe��6�����̣�ֻ�еڶ��������Ǻ�������ͨ�ŵġ�
    DWORD ProcessIdList[32];
    CHAR szProcessName[] = "SGSOL.exe";
    DWORD dwProcessIdNumbers = GetProcessIDByName(szProcessName, ProcessIdList);
    int i = 2;
    RemoteInjectByProcessId(ProcessIdList[i], szDllPath);

    system("pause");
}




// ***************************************************************************************************

#define BUF_SIZE        4096
#define EXAMP_PIPE      "\\\\.\\pipe\\ReadPipe"   

BOOL test_pipe_thread_func() {
    printf("iyzyi");
    // ���������ܵ�
    HANDLE hPipe = NULL;
    hPipe = CreateNamedPipeA(
        EXAMP_PIPE,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE |
        PIPE_READMODE_MESSAGE |
        PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUF_SIZE,
        BUF_SIZE,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        printf("Create Read Pipe Error\n");
        return FALSE;
    }

    // �ȴ��ͻ��˵����ӣ��������ģ�

    if (!ConnectNamedPipe(hPipe, NULL))
    {
        printf("Connect Failed\n");
    }
    else {
        printf("Connect Success!\n");
    }

    
    

    DWORD dwReturn = 0;
    char szBuffer[BUF_SIZE] = { 0 };

    // ��ͻ��˷�������
    szBuffer[0] = 'Y';
    if (!WriteFile(hPipe, szBuffer, strlen(szBuffer), &dwReturn, NULL))
    {
        printf("Write Failed\n");
    }

    //DisconnectNamedPipe(hPipe);
    //CloseHandle(hPipe);

}





int main() {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)test_pipe_thread_func, NULL, 0, NULL);
    Sleep(4000);
    test_remote_inject();
}