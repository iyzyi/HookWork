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
    printf("pid=%d\n", ProcessIdList[i]);
    RemoteInjectByProcessId(ProcessIdList[i], szDllPath);

    system("pause");
}




// ***************************************************************************************************




int main() {
    test_remote_inject();
}