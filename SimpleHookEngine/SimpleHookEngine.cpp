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
//    strcat_s(newText, "\nMessageBoxA Hacked by iyzyi!");//�۸���Ϣ������
//    uType |= MB_ICONERROR;							
//    ret = TrueMessageBoxA(hWnd, (LPCTSTR)newText, (LPCTSTR)newCaption, uType);
//    return ret;
//}
//
//
//int wmain(int argc, WCHAR* argv[]) {
//	MessageBoxA(NULL, "�����ı�1", "����1", NULL);
//
//	InstallHook((void**)&TrueMessageBoxA, MyMessageBoxA);
//
//	MessageBoxA(NULL, "�����ı�2", "����2", NULL);
//
//    UninstallHook((void**)&TrueMessageBoxA);
//
//    MessageBoxA(NULL, "�����ı�3", "����3", NULL);
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
//    //���������Զ�ԭʼ���������������
//    int ret;
//    char newText[1024] = { 0 };
//    char newCaption[256] = "pediy.com";
//    PFN_MessageBoxA OriginalMessageBox = (PFN_MessageBoxA)Hook.pfnOriginalFunc;
//    printf("���˵���MessageBox!\n");
//    //�ڵ���ԭ����֮ǰ�����Զ�IN(������)�������и���
//    strcpy_s(newText, (char*)lpText);//Ϊ��ֹԭ�����ṩ�Ļ��������������︴�Ƶ������Լ���һ�����������ٽ��в���
//    strcat_s(newText, "\n\tMessageBox Hacked by pediy.com!");//�۸���Ϣ������
//    uType |= MB_ICONERROR;//����һ������ͼ��
//    ret = OriginalMessageBox(hWnd, newText, newCaption, uType);//����ԭMessageBox�������淵��ֵ
//    //����ԭ����֮�󣬿��Լ�����OUT(�����)�������и���,�������纯����recv�����Ը��淵�ص�����
//    return ret;//�����㻹���Ը���ԭʼ�����ķ���ֵ
//}

//void* GetAddress(char* dllname, char* funname)
//{
//	HMODULE hMod = 0;
//	if (hMod = GetModuleHandleA(dllname))
//	{
//		printf("%s���Ѽ���\n", dllname);
//		return GetProcAddress(hMod, funname);
//	}
//	else
//	{
//		printf("�ɹ�����%s\n", dllname);
//		hMod = LoadLibraryA(dllname);
//		return GetProcAddress(hMod, funname);
//	}
//}

//int main()
//{
//    std::cout << "Hello World1!\n";
//
//    //MessageBoxA(NULL, "�����ı�", "����", NULL);
//
//    //char szApiName[] = "MessageBoxA";
//    //char szModuleName[] = "user32.dll";
//    //ULONG_PTR pHookPoint = (ULONG_PTR)GetAddress(szModuleName, szApiName);
//    //Hook.InitHook((void*)pHookPoint, (void*)My_MessageBoxA, 64);
//
//    //MessageBoxA(NULL, "�����ı�2", "����2", NULL);
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