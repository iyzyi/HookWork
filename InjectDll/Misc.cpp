#include "pch.h"
#include "Misc.h"
#include <stdio.h>

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