// 最终引用此.cpp文件对应的头文件即可

#include "pch.h"
#include "MyFunction.h"
#include "CommunicationPipe.h"
#include "../MyMessagePack/MyMessagePack.h"
#include "Misc.h"




int WSAAPI My_send(SOCKET s, const char* buf, int len, int flags) {
	//DllPrintf("My_send ");
	SendData("Call My_Send");
	return True_send(s, buf, len, flags);
}


int WSAAPI My_sendto(SOCKET s, const char FAR* buf, int len, int flags, const struct sockaddr FAR* to, int tolen) {
	SendData("Call My_sendto");
	return True_sendto(s, buf, len, flags, to, tolen);
}


struct _Data_WSASend {
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(sbuffer)
};
int WSAAPI My_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	
	SendData("Call My_WSASendTo");

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
	SendData("Call My_WSASendTo");



	return True_WSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_WSASendMsg(SOCKET Handle, LPWSAMSG lpMsg, DWORD dwFlags, LPDWORD lpNumberOfBytesSent, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SendData("Call My_WSASendMsg");
	return True_WSASendMsg(Handle, lpMsg, dwFlags, lpNumberOfBytesSent, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_recv(SOCKET s, char* buf, int len, int flags) {
	//DllPrintf("My_recv ");
	SendData("Call My_recv");
	
	//return True_recv(s, buf, len, flags);

	int iRet = True_recv(s, buf, len, flags);
	if (iRet > 0) {
		CHAR szBuffer[32];
		sprintf_s(szBuffer, "My_recv len = %d", iRet);
		SendData(szBuffer);
	}
	return iRet;
}


int WSAAPI My_recvfrom(SOCKET s, char FAR* buf, int len, int flags, struct sockaddr FAR* from, int FAR* fromlen) {
	SendData("Call My_recvfrom");
	return True_recvfrom(s, buf, len, flags, from, fromlen);
}


int WSAAPI My_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SendData("Call My_WSARecv");
	return True_WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr FAR* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SendData("Call My_WSARecvFrom");
	return True_WSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
}