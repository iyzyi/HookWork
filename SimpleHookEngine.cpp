#include "stdafx.h"
#include "mhook/mhook.h"


typedef int
(WINAPI* PFN_MessageBoxA)(
    HWND hWnd,
    LPCTSTR lpText,
    LPCTSTR lpCaption,
    UINT uType
    );

PFN_MessageBoxA TrueMessageBoxA = (PFN_MessageBoxA)GetProcAddress(GetModuleHandle(L"user32"), "MessageBoxA");

int WINAPI MyMessageBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    int ret;
    char newText[1024] = { 0 };
    char newCaption[256] = "iyzyi";
    strcpy_s(newText, (char*)lpText);
    strcat_s(newText, "\nMessageBox Hacked by iyzyi!");//篡改消息框内容
    uType |= MB_ICONERROR;							
    ret = TrueMessageBoxA(hWnd, (LPCTSTR)newText, (LPCTSTR)newCaption, uType);
    return ret;
}


int wmain(int argc, WCHAR* argv[]) {
	MessageBoxA(NULL, "测试文本1", "测试1", NULL);

	Mhook_SetHook((PVOID*)&TrueMessageBoxA, MyMessageBoxA);

	MessageBoxA(NULL, "测试文本2", "测试2", NULL);

    Mhook_Unhook((PVOID*)&TrueMessageBoxA);

    MessageBoxA(NULL, "测试文本3", "测试3", NULL);
}






























//
//typedef struct _CLIENT_ID {
//	DWORD_PTR UniqueProcess;
//	DWORD_PTR UniqueThread;
//} CLIENT_ID, *PCLIENT_ID;
//
//typedef ULONG (WINAPI* _NtOpenProcess)(OUT PHANDLE ProcessHandle, 
//	     IN ACCESS_MASK AccessMask, IN PVOID ObjectAttributes, 
//		 IN PCLIENT_ID ClientId ); 
//
//typedef HGDIOBJ (WINAPI* _SelectObject)(HDC hdc, HGDIOBJ hgdiobj); 
//
//typedef int (WSAAPI* _getaddrinfo)(const char* nodename, const char* servname, const struct addrinfo* hints, struct addrinfo** res);
//
//typedef LPVOID (WINAPI *_HeapAlloc)(HANDLE, DWORD, SIZE_T);
//
//typedef ULONG (WINAPI* _NtClose)(IN HANDLE Handle);
//
//
//_NtOpenProcess TrueNtOpenProcess = (_NtOpenProcess)
//	GetProcAddress(GetModuleHandle(L"ntdll"), "NtOpenProcess");
//
//_SelectObject TrueSelectObject = (_SelectObject)
//	GetProcAddress(GetModuleHandle(L"gdi32"), "SelectObject");
//
//_getaddrinfo Truegetaddrinfo = (_getaddrinfo)GetProcAddress(GetModuleHandle(L"ws2_32"), "getaddrinfo");
//
//_HeapAlloc TrueHeapAlloc = (_HeapAlloc)GetProcAddress(GetModuleHandle(L"kernel32"), "HeapAlloc");
//
//_NtClose TrueNtClose = (_NtClose)GetProcAddress(GetModuleHandle(L"ntdll"), "NtClose");
//
//ULONG WINAPI HookNtOpenProcess(OUT PHANDLE ProcessHandle, 
//							   IN ACCESS_MASK AccessMask, 
//							   IN PVOID ObjectAttributes, 
//							   IN PCLIENT_ID ClientId)
//{
//	printf("***** Call to open process %d\n", ClientId->UniqueProcess);
//	return TrueNtOpenProcess(ProcessHandle, AccessMask, 
//		ObjectAttributes, ClientId);
//}
//
//
//HGDIOBJ WINAPI HookSelectobject(HDC hdc, HGDIOBJ hgdiobj)
//{
//	printf("***** Call to SelectObject(0x%p, 0x%p)\n", hdc, hgdiobj);
//	return TrueSelectObject(hdc, hgdiobj);
//}
//
//
//int WSAAPI Hookgetaddrinfo(const char* nodename, const char* servname, const struct addrinfo* hints, struct addrinfo** res)
//{
//	printf("***** Call to getaddrinfo(0x%p, 0x%p, 0x%p, 0x%p)\n", nodename, servname, hints, res);
//	return Truegetaddrinfo(nodename, servname, hints, res);
//}
//
//
//LPVOID WINAPI HookHeapAlloc(HANDLE a_Handle, DWORD a_Bla, SIZE_T a_Bla2) {
//	printf("***** Call to HeapAlloc(0x%p, %u, 0x%p)\n", a_Handle, a_Bla, a_Bla2);
//	return TrueHeapAlloc(a_Handle, a_Bla, a_Bla2);
//}
//
//
//ULONG WINAPI HookNtClose(HANDLE hHandle) {
//	printf("***** Call to NtClose(0x%p)\n", hHandle);
//	return TrueNtClose(hHandle);
//}


//int wmain(int argc, WCHAR* argv[])
//{
//	HANDLE hProc = NULL;
//
//	if (Mhook_SetHook((PVOID*)&TrueNtOpenProcess, HookNtOpenProcess)) {
//		hProc = OpenProcess(PROCESS_ALL_ACCESS, 
//			FALSE, GetCurrentProcessId());
//		if (hProc) {
//			printf("Successfully opened self: %p\n", hProc);
//			CloseHandle(hProc);
//		} else {
//			printf("Could not open self: %d\n", GetLastError());
//		}
//		Mhook_Unhook((PVOID*)&TrueNtOpenProcess);
//	}
//
//	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
//	if (hProc) {
//		printf("Successfully opened self: %p\n", hProc);
//		CloseHandle(hProc);
//	} else {
//		printf("Could not open self: %d\n", GetLastError());
//	}
//
//	printf("Testing SelectObject.\n");
//	if (Mhook_SetHook((PVOID*)&TrueSelectObject, HookSelectobject)) {
//		HDC hdc = GetDC(NULL);
//		HDC hdcMem = CreateCompatibleDC(hdc);
//		HBITMAP hbm = CreateCompatibleBitmap(hdc, 32, 32);
//		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
//		SelectObject(hdcMem, hbmOld);
//		DeleteObject(hbm);
//		DeleteDC(hdcMem);
//		ReleaseDC(NULL, hdc);
//		Mhook_Unhook((PVOID*)&TrueSelectObject);
//	}
//
//	printf("Testing getaddrinfo.\n");
//	if (Mhook_SetHook((PVOID*)&Truegetaddrinfo, Hookgetaddrinfo)) {
//		WSADATA wd = {0};
//		WSAStartup(MAKEWORD(2, 2), &wd);
//		char* ip = "localhost";
//		struct addrinfo aiHints;
//		struct addrinfo *res = NULL;
//		memset(&aiHints, 0, sizeof(aiHints));
//		aiHints.ai_family = PF_UNSPEC;
//		aiHints.ai_socktype = SOCK_STREAM;
//		if (getaddrinfo(ip, NULL, &aiHints, &res)) {
//			printf("getaddrinfo failed\n");
//		} else {
//			int n = 0;
//			while(res) {
//				res = res->ai_next;
//				n++;
//			}
//			printf("got %d addresses\n", n);
//		}
//		WSACleanup();
//		Mhook_Unhook((PVOID*)&Truegetaddrinfo);
//	}
//
//	printf("Testing HeapAlloc.\n");
//	if (Mhook_SetHook((PVOID*)&TrueHeapAlloc, HookHeapAlloc))
//	{
//		free(malloc(10));
//		Mhook_Unhook((PVOID*)&TrueHeapAlloc);
//	}
//
//	printf("Testing NtClose.\n");
//	if (Mhook_SetHook((PVOID*)&TrueNtClose, HookNtClose))
//	{
//		CloseHandle(NULL);
//		Mhook_Unhook((PVOID*)&TrueNtClose);
//	}
//
//	return 0;
//}
//
