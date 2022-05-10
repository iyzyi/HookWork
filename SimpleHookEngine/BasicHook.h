#pragma once
#include <windows.h>


class BasicHook {

private:
	ULONG_PTR	pHookPoint;						// ��HOOK��λ��

	int			dwHookCodeLen;					// Hook���ȣ���NOPָ�

	BYTE		byOldEntry[256];				// ����Hookλ�õ�ԭʼָ��
	BYTE		byNewEntry[256];				// ����Ҫд��Hookλ�õ���ָ��

	ULONG_PTR	pJmpBackAddr;					// ������ԭ�����е�λ��

	ULONG_PTR	pfnTrampolineFun;				// ���Ե���ԭʼ����
	ULONG_PTR	pfnDetourFun;					// �Զ���HOOK����


public:
	void* pfnOriginalFunc;						// �൱��ԭʼ����


private:
	void InitHookEntry();
	void InitTrampoline();


public:
	bool InitHook(void* pHookPointAddr, void* pfnMyFunc, int Architecture);
	bool InstallHook();
	bool UninstallHook();
};