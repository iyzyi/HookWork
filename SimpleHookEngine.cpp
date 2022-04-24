#include "stdafx.h"
#include "mhook/mhook.h"


typedef int
(WINAPI* PFN_MessageBoxA)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

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


int wmain(int argc, WCHAR* argv[]) {
	MessageBoxA(NULL, "测试文本1", "测试1", NULL);

	InstallHook((void**)&TrueMessageBoxA, MyMessageBoxA);

	MessageBoxA(NULL, "测试文本2", "测试2", NULL);

    UninstallHook((void**)&TrueMessageBoxA);

    MessageBoxA(NULL, "测试文本3", "测试3", NULL);
}
