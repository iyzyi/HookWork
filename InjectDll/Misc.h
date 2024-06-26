#pragma once
#include <WinSock2.h>
#include <windows.h>



int DllPrintf(PCHAR fmt, ...);



#include <random>
#include <windows.h>


#ifndef QWORD
#define QWORD int64_t
#endif


VOID PrintData(LPBYTE pbPrintData, DWORD dwDataLen);

VOID WriteDwordToBuffer(PBYTE pbData, DWORD dwNum, DWORD dwPos = 0);

DWORD GetDwordFromBuffer(PBYTE pbData, DWORD dwPos = 0);

// 获取socket连接的对端IP(即远程IP)
BOOL GetSocketIpPort(SOCKET s, PDWORD pdwIP, PWORD pwPort);


// 通过句柄获取对应注册表路径
std::wstring GetWstrKeyPathFromHKEY(HKEY key);
std::string GetStrKeyPathFromHKEY(HKEY key);
