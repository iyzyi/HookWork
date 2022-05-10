// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "BypassUAC.h"
#include <windows.h>
#include <atlconv.h>


// 经过配置，不管是Win32还是x64配置下，本项目都是生成x86架构的DLL。因为本DLL通过rundll32调用


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}





// 提权执行
extern "C" __declspec(dllexport) 
void ElevatePrivilegesWinExec(HWND hwnd,              // 父窗口的句柄
                              HINSTANCE hinst,        // DLL的实例句柄
                              LPSTR lpCmdLine,        // DLL将要解析的字符串
                              int nCmdShow            // 显示状态
){
    //MessageBoxA(0, lpCmdLine, "", 0);

    USES_CONVERSION;
    CMLuaUtilBypassUAC(A2W(lpCmdLine));
}