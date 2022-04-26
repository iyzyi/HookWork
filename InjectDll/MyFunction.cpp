// 最终引用此.cpp文件对应的头文件即可

#include "pch.h"
#include "MyFunction.h"
#include "CommunicationPipe.h"
#include "Misc.h"



int My_send(SOCKET s, const char* buf, int len, int flags) {
	//DllPrintf("My_send ");
	SendData("Call My_Send");
	return True_send(s, buf, len, flags);
}


int My_sendto(SOCKET s, const char FAR* buf, int len, int flags, const struct sockaddr FAR* to, int tolen) {
	SendData("Call My_sendto");
	return True_sendto(s, buf, len, flags, to, tolen);
}


int My_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SendData("Call My_WSASend");
	return True_WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}


int My_WSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr FAR* lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SendData("Call My_WSASendTo");
	return True_WSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine);
}


int My_WSASendMsg(SOCKET Handle, LPWSAMSG lpMsg, DWORD dwFlags, LPDWORD lpNumberOfBytesSent, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SendData("Call My_WSASendMsg");
	return True_WSASendMsg(Handle, lpMsg, dwFlags, lpNumberOfBytesSent, lpOverlapped, lpCompletionRoutine);
}


int My_recv(SOCKET s, char* buf, int len, int flags) {
	//DllPrintf("My_recv ");
	SendData("Call My_recv");
	return True_recv(s, buf, len, flags);
}


int My_recvfrom(SOCKET s, char FAR* buf, int len, int flags, struct sockaddr FAR* from, int FAR* fromlen) {
	SendData("Call My_recvfrom");
	return True_recvfrom(s, buf, len, flags, from, fromlen);
}


int My_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SendData("Call My_WSARecv");
	return True_WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
}


int My_WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr FAR* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SendData("Call My_WSARecvFrom");
	return True_WSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
}