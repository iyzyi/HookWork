// 最终引用此.cpp文件对应的头文件即可

#include "pch.h"
#include "MyFunction.h"
#include "CommunicationPipe.h"
#include "../MyMessagePack/MyMessagePack.h"
#include "Misc.h"



// 作为注入的DLL，编译时一定要把优化给关掉
// 属性 -> 配置属性 -> C/C++ -> 优化：已禁用
// 不然出大问题，我卡在这里2个小时
// 比如函数体内没写其他代码，直接return True_Func()时
// 如果开了优化选项，会删去整个函数汇编，直接用一个jmp跳转到True_Func()
// 这样会使得注入的程序崩溃。


int WSAAPI My_send(SOCKET s, const char* buf, int len, int flags) {
	_Data_send data;
	data.socket = s;
	data.sbuffer.ptr = (char*)buf;
	data.sbuffer.size = len;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_send>(data, pBuffer, ID_send);
	DllPrintf("send socket = 0x%p\n", s);
	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return True_send(s, buf, len, flags);
}


int WSAAPI My_sendto(SOCKET s, const char FAR* buf, int len, int flags, const struct sockaddr FAR* to, int tolen) {
	//SendData("Call My_sendto");
	return True_sendto(s, buf, len, flags, to, tolen);
}



int WSAAPI My_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	
	//SendData("Call My_WSASendTo");

	//PrintData((LPBYTE)lpBuffers, dwBufferCount);
	//
	//_Data_WSASend data;
	//data.sbuffer.ptr = (char*)lpBuffers;
	//data.sbuffer.size = dwBufferCount;

	//PBYTE pBuffer = NULL;
	//DWORD dwBufferSize = MsgPackWithFuncId<_Data_WSASend>(data, pBuffer, ID_WSASend);

	//SendData(pBuffer, dwBufferSize);

	return True_WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_WSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr FAR* lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	//SendData("Call My_WSASendTo");



	return True_WSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_WSASendMsg(SOCKET Handle, LPWSAMSG lpMsg, DWORD dwFlags, LPDWORD lpNumberOfBytesSent, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	//SendData("Call My_WSASendMsg");
	return True_WSASendMsg(Handle, lpMsg, dwFlags, lpNumberOfBytesSent, lpOverlapped, lpCompletionRoutine);
}



int WSAAPI My_recv(SOCKET s, char* buf, int len, int flags) {
	int iRet = True_recv(s, buf, len, flags);
	if (iRet > 0) {
		_Data_recv data;
		data.socket = s;
		data.sbuffer.ptr = (char*)buf;
		data.sbuffer.size = iRet;

		PBYTE pBuffer = NULL;
		DWORD dwBufferSize = MsgPackWithFuncId<_Data_recv>(data, pBuffer, ID_recv);
		DllPrintf("recv socket = 0x%p\n", s);
		SendData(pBuffer, dwBufferSize);
		delete[] pBuffer;
	}
	return iRet;
}


int WSAAPI My_recvfrom(SOCKET s, char FAR* buf, int len, int flags, struct sockaddr FAR* from, int FAR* fromlen) {
	//SendData("Call My_recvfrom");
	return True_recvfrom(s, buf, len, flags, from, fromlen);
}


int WSAAPI My_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	//SendData("Call My_WSARecv");
	return True_WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr FAR* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	//SendData("Call My_WSARecvFrom");
	return True_WSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
}