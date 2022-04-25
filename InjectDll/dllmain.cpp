// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "CommunicationPipe.h"
#include "Misc.h"



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DllPrintf("远程注入DLL......\n");
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc_Main, NULL, 0, NULL);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

