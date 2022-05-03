#include "pch.h"
#include "Misc.h"
#include <stdio.h>
#include <iostream>

#pragma warning (disable:4996)


int DllPrintf(PCHAR fmt, ...)
{
	//����������ʽ��
	va_list argptr;
	va_start(argptr, fmt);
	char buffer[512] = { 0 };
	int cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	//��������̨����
	static HANDLE gHConsole = INVALID_HANDLE_VALUE;
	if (INVALID_HANDLE_VALUE == gHConsole) {
		AllocConsole();								// Ϊ���ý��̷���һ���µĿ���̨��
		gHConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	//��ӡ��ָ���Ŀ���̨����
	DWORD dw;
	WriteConsoleA(gHConsole, buffer, strlen(buffer), &dw, NULL);

	return(cnt);
}


#define DebugPrint DllPrintf
#define printf DllPrintf

//#define DebugPrint printf


VOID PrintData(LPBYTE pbPrintData, DWORD dwDataLen)
{
	if (dwDataLen > 512) {
		DebugPrint("��%d�ֽڣ�����Ч�ʿ��ǣ�����ӡǰ512�ֽ�\n", dwDataLen);
		dwDataLen = 512;
	}

	DWORD dwRow = 0, dwColumn = 0;
	for (dwRow = 0; dwRow < dwDataLen / 16 + 1; dwRow++) {
		for (dwColumn = 0; (dwRow * 16 + dwColumn < dwDataLen) && (dwColumn < 16); dwColumn++) {
			DebugPrint("0x%02x ", pbPrintData[dwRow * 16 + dwColumn]);
		}

		if (dwColumn != 16) {
			while (dwColumn < 16) {
				DebugPrint("     ");
				dwColumn++;
			}
		}
		DebugPrint("\t");

		for (dwColumn = 0; (dwRow * 16 + dwColumn < dwDataLen) && (dwColumn < 16); dwColumn++) {
			DWORD dwIndex = dwRow * 16 + dwColumn;
			if (pbPrintData[dwIndex] >= 32 && pbPrintData[dwIndex] <= 126) {
				DebugPrint("%c", pbPrintData[dwIndex]);
			}
			else {
				DebugPrint(".");
			}
		}
		DebugPrint("\n");
	}
	DebugPrint("\n");
}


VOID WriteDwordToBuffer(PBYTE pbData, DWORD dwNum, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	pbData2[0] = dwNum & 0xff;
	pbData2[1] = (dwNum >> 8) & 0xff;
	pbData2[2] = (dwNum >> 16) & 0xff;
	pbData2[3] = (dwNum >> 24) & 0xff;
}


// ��ȡsocket���ӵĶԶ�IP(��Զ��IP)
BOOL GetSocketIpPort(SOCKET s, PDWORD pdwIP, PWORD pwPort) {
	struct sockaddr addr;
	struct sockaddr_in* addr_v4;
	int addr_len = sizeof(addr);

	ZeroMemory(&addr, sizeof(addr));
	if (0 == getpeername(s, &addr, &addr_len))		// ���Ӧ�ģ���ȡ����IP��getsockname,������ʱû�л�ȡ����IP����Ҫ���Ͳ�������������
	{
		if (addr.sa_family == AF_INET)
		{
			addr_v4 = (sockaddr_in*)&addr;
			*pdwIP = addr_v4->sin_addr.S_un.S_addr;
			*pwPort = (int)ntohs(addr_v4->sin_port);		
			return TRUE;
		}
	}
	return FALSE;
}