#include "pch.h"
#include "Misc.h"
#include <stdio.h>

#pragma warning (disable:4996)


int DllPrintf(PCHAR fmt, ...)
{
	//不定参数格式化
	va_list argptr;
	va_start(argptr, fmt);
	char buffer[512] = { 0 };
	int cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	//创建控制台窗口
	static HANDLE gHConsole = INVALID_HANDLE_VALUE;
	if (INVALID_HANDLE_VALUE == gHConsole) {
		AllocConsole();								// 为调用进程分配一个新的控制台。
		gHConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	//打印到指定的控制台窗口
	DWORD dw;
	WriteConsoleA(gHConsole, buffer, strlen(buffer), &dw, NULL);

	return(cnt);
}