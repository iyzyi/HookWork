#pragma once
#include <windows.h>


/// <summary>
/// 远程注入。
/// </summary>
/// <param name="dwPid"></param>
/// <param name="szDllPath">必须是绝对路径，因为是跨进程执行的，工作路径是目标程序的工作路径，而非本程序的工作路径</param>
/// <returns></returns>
extern BOOL RemoteInjectByProcessId(DWORD dwPid, PCHAR szDllPath);


/// <summary>
/// 远程注入。
/// 如果ProcessName对应多个ProcessId，则逐一对这些进程注入。
/// 返回值高16bit为符合ProcessName的进程数量，低16bit为成功注入的进程数量
/// </summary>
/// <param name="szProcessName">进程名称，如SGSOL.exe</param>
/// <param name="szDllPath">必须是绝对路径，因为是跨进程执行的，工作路径是目标程序的工作路径，而非本程序的工作路径</param>
/// <returns></returns>
extern DWORD RemoteInjectByProcessName(PCHAR szProcessName, PCHAR szDllPath);


/// <summary>
/// 远程卸载DLL
/// </summary>
/// <param name="dwPid"></param>
/// <param name="szDllName"></param>
extern BOOL RemoteUnInjectDllByProcessId(DWORD dwPID, LPCTSTR szDllName);



DWORD GetProcessIDByName(PCHAR szName, PDWORD ProcessIdList);
HMODULE GetHModuleIDByName(DWORD dwPid, PCHAR szName);
HMODULE GetModuleBaseAddress(DWORD dwPID, PCHAR szName);