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
    strcat_s(newText, "\nMessageBoxA Hacked by iyzyi!");//篡改消息框内容
    uType |= MB_ICONERROR;
    ret = TrueMessageBoxA(hWnd, (LPCTSTR)newText, (LPCTSTR)newCaption, uType);
    return ret;
}

void test_hook_messagebox() {
    MessageBoxA(NULL, "测试文本1", "测试1", NULL);

    InstallHook((void**)&TrueMessageBoxA, MyMessageBoxA);

    MessageBoxA(NULL, "测试文本2", "测试2", NULL);

    UninstallHook((void**)&TrueMessageBoxA);

    MessageBoxA(NULL, "测试文本3", "测试3", NULL);
}




// *******************************远程注入*****************************************

void test_remote_inject() {
    CHAR szDllPath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, szDllPath, MAX_PATH);		// 获取本程序所在路径
    (strrchr(szDllPath, '\\'))[1] = 0;					// 路径中去掉本程序名称
    strcat_s(szDllPath, "SGSOL.dll");			        // 拼接上DLL的名称

    //RemoteInjectByProcessName("SGSOL.exe", szDllPath);

    // SGSOL.exe共6个进程，只有第二个进程是含有网络通信的。
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
    // 创建命名管道
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

    // 等待客户端的连接（是阻塞的）

    if (!ConnectNamedPipe(hPipe, NULL))
    {
        printf("Connect Failed\n");
    }
    else {
        printf("Connect Success!\n");
    }

    
    

    DWORD dwReturn = 0;
    char szBuffer[BUF_SIZE] = { 0 };

    // 向客户端发送数据
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