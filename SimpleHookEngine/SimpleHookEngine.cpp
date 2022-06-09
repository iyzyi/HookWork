#include "stdafx.h"
#include "GeneralHook.h"
#include "SimpleHookEngine.h"


//typedef int (WINAPI* PFN_MessageBoxA)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
//
//PFN_MessageBoxA TrueMessageBoxA = (PFN_MessageBoxA)GetProcAddress(GetModuleHandleA("user32"), "MessageBoxA");
//
//int WINAPI MyMessageBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
//{
//    int ret;
//    char newText[1024] = { 0 };
//    char newCaption[] = "iyzyi";
//    strcpy_s(newText, (char*)lpText);
//    strcat_s(newText, "\nMessageBoxA Hacked by iyzyi!");//篡改消息框内容
//    uType |= MB_ICONERROR;							
//    ret = TrueMessageBoxA(hWnd, (LPCTSTR)newText, (LPCTSTR)newCaption, uType);
//    return ret;
//}
//
//
//int wmain(int argc, WCHAR* argv[]) {
//	MessageBoxA(NULL, "测试文本1", "测试1", NULL);
//
//	InstallHook((void**)&TrueMessageBoxA, MyMessageBoxA);
//
//	MessageBoxA(NULL, "测试文本2", "测试2", NULL);
//
//    UninstallHook((void**)&TrueMessageBoxA);
//
//    MessageBoxA(NULL, "测试文本3", "测试3", NULL);
//}


//BasicHook Hook;

//typedef int
//(WINAPI* PFN_MessageBoxA)(
//    HWND hWnd,
//    LPCTSTR lpText,
//    LPCTSTR lpCaption,
//    UINT uType
//    );
//
//
//int WINAPI My_MessageBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
//{
//    //在这里，你可以对原始参数进行任意操作
//    int ret;
//    char newText[1024] = { 0 };
//    char newCaption[256] = "pediy.com";
//    PFN_MessageBoxA OriginalMessageBox = (PFN_MessageBoxA)Hook.pfnOriginalFunc;
//    printf("有人调用MessageBox!\n");
//    //在调用原函数之前，可以对IN(输入类)参数进行干涉
//    strcpy_s(newText, (char*)lpText);//为防止原函数提供的缓冲区不够，这里复制到我们自己的一个缓冲区中再进行操作
//    strcat_s(newText, "\n\tMessageBox Hacked by pediy.com!");//篡改消息框内容
//    uType |= MB_ICONERROR;//增加一个错误图标
//    ret = OriginalMessageBox(hWnd, newText, newCaption, uType);//调用原MessageBox，并保存返回值
//    //调用原函数之后，可以继续对OUT(输出类)参数进行干涉,比如网络函数的recv，可以干涉返回的内容
//    return ret;//这里你还可以干涉原始函数的返回值
//}

//void* GetAddress(char* dllname, char* funname)
//{
//	HMODULE hMod = 0;
//	if (hMod = GetModuleHandleA(dllname))
//	{
//		printf("%s早已加载\n", dllname);
//		return GetProcAddress(hMod, funname);
//	}
//	else
//	{
//		printf("成功加载%s\n", dllname);
//		hMod = LoadLibraryA(dllname);
//		return GetProcAddress(hMod, funname);
//	}
//}

//int main()
//{
//    std::cout << "Hello World1!\n";
//
//    //MessageBoxA(NULL, "测试文本", "测试", NULL);
//
//    //char szApiName[] = "MessageBoxA";
//    //char szModuleName[] = "user32.dll";
//    //ULONG_PTR pHookPoint = (ULONG_PTR)GetAddress(szModuleName, szApiName);
//    //Hook.InitHook((void*)pHookPoint, (void*)My_MessageBoxA, 64);
//
//    //MessageBoxA(NULL, "测试文本2", "测试2", NULL);
//
//
//
//    ULONG_PTR pHookPoint = (ULONG_PTR)GetAddress((char*)"kernel32", (char*)"HeapAlloc");
//    Hook.InitHook((void*)pHookPoint, (void*)My_HeapAlloc, 64);
//    Hook.InstallHook();
//
//    HeapAlloc(GetProcessHeap(), 0x00000008, 16);
//}





BOOL InstallHook(PVOID* ppSystemFunction, PVOID pHookFunction) {
    return SetHook(ppSystemFunction, pHookFunction);
}

BOOL UninstallHook(PVOID* ppHookedFunction) {
    return UnHook(ppHookedFunction);
}