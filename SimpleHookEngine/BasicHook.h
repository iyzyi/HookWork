#pragma once
#include <windows.h>


class BasicHook {

private:
	ULONG_PTR	pHookPoint;						// 待HOOK的位置

	int			dwHookCodeLen;					// Hook长度（含NOP指令）

	BYTE		byOldEntry[256];				// 保存Hook位置的原始指令
	BYTE		byNewEntry[256];				// 保存要写入Hook位置的新指令

	ULONG_PTR	pJmpBackAddr;					// 回跳到原函数中的位置

	ULONG_PTR	pfnTrampolineFun;				// 用以调用原始函数
	ULONG_PTR	pfnDetourFun;					// 自定义HOOK函数


public:
	void* pfnOriginalFunc;						// 相当于原始函数


private:
	void InitHookEntry();
	void InitTrampoline();


public:
	bool InitHook(void* pHookPointAddr, void* pfnMyFunc, int Architecture);
	bool InstallHook();
	bool UninstallHook();
};