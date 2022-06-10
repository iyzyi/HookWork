// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "BypassUAC.h"
#include <windows.h>
#include <atlconv.h>


// 经过配置，不管是Win32还是x64配置下，本项目都是生成x86架构Release模式的DLL。因为本DLL通过rundll32调用
// 但是，如果是x64的话，本DLL生成的目录是Win32下的。因为宏定义中只有项目的平台名，没有整个解决方案的平台名
// 而且Debug编译的dll无法被rundll32找到。
// 所以不管是在win32还是x64下对解决方案进行编译，都设置一下生成后命令，使得win32 release文件夹内的本DLL复制到x64\release x64\debug win32\debug中
// 设置生成后事件，使得生成本DLL后自动调用CopyDllBypassUAC.bat


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