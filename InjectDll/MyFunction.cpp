// �������ô�.cpp�ļ���Ӧ��ͷ�ļ�����

#include "pch.h"
#include "MyFunction.h"
#include "Misc.h"



int My_Send(SOCKET s, const char* buf, int len, int flags) {
	DllPrintf("My_Send");
	return True_Send(s, buf, len, flags);
}


int My_Recv(SOCKET s, char* buf, int len, int flags) {
	DllPrintf("My_Recv");
	return True_Recv(s, buf, len, flags);
}