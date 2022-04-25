#pragma once
#include <windows.h>


/// <summary>
/// Զ��ע�롣
/// </summary>
/// <param name="dwPid"></param>
/// <param name="szDllPath">�����Ǿ���·������Ϊ�ǿ����ִ�еģ�����·����Ŀ�����Ĺ���·�������Ǳ�����Ĺ���·��</param>
/// <returns></returns>
extern BOOL RemoteInjectByProcessId(DWORD dwPid, PCHAR szDllPath);


/// <summary>
/// Զ��ע�롣
/// ���ProcessName��Ӧ���ProcessId������һ����Щ����ע�롣
/// ����ֵ��16bitΪ����ProcessName�Ľ�����������16bitΪ�ɹ�ע��Ľ�������
/// </summary>
/// <param name="szProcessName">�������ƣ���SGSOL.exe</param>
/// <param name="szDllPath">�����Ǿ���·������Ϊ�ǿ����ִ�еģ�����·����Ŀ�����Ĺ���·�������Ǳ�����Ĺ���·��</param>
/// <returns></returns>
extern DWORD RemoteInjectByProcessName(PCHAR szProcessName, PCHAR szDllPath);




DWORD GetProcessIDByName(PCHAR szName, PDWORD ProcessIdList);
HMODULE GetHModuleIDByName(DWORD dwPid, PCHAR szName);
HMODULE GetModuleBaseAddress(DWORD dwPID, PCHAR szName);