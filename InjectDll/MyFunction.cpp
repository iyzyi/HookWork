// �������ô�.cpp�ļ���Ӧ��ͷ�ļ�����

#include "pch.h"
#include "MyFunction.h"
#include "CommunicationPipe.h"
#include "../MyMessagePack/MyMessagePack.h"
#include "Misc.h"




// ��Ϊע���DLL������ʱһ��Ҫ���Ż����ص�
// ���� -> �������� -> C/C++ -> �Ż����ѽ���
// ��Ȼ�������⣬�ҿ�������2��Сʱ
// ���纯������ûд�������룬ֱ��return True_Func()ʱ
// ��������Ż�ѡ���ɾȥ����������ֱ࣬����һ��jmp��ת��True_Func()
// ������ʹ��ע��ĳ��������


#pragma region ����ͨ����غ���-�Զ��庯��

// ************************************************ ����ͨ����غ���-�Զ��庯�� ************************************************

int WSAAPI My_send(SOCKET s, const char* buf, int len, int flags) {
	_Data_send data;
	data.socket = s;
	data.sbuffer.ptr = (char*)buf;
	data.sbuffer.size = len;

	DWORD dwIP = 0;
	WORD wPort = 0;
	GetSocketIpPort(s, &dwIP, &wPort);
	data.dwIP = dwIP;
	data.wPort = wPort;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_send>(data, pBuffer, ID_send);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return True_send(s, buf, len, flags);
}


int WSAAPI My_sendto(SOCKET s, const char FAR* buf, int len, int flags, const struct sockaddr FAR* to, int tolen) {
	_Data_sendto data;
	data.socket = s;
	data.sbuffer.ptr = (char*)buf;
	data.sbuffer.size = len;

	data.dwIP = ((sockaddr_in*)to)->sin_addr.S_un.S_addr;
	data.wPort = (int)ntohs(((sockaddr_in*)to)->sin_port);

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_sendto>(data, pBuffer, ID_sendto);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return True_sendto(s, buf, len, flags, to, tolen);
}


int WSAAPI My_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	// һЩ������ݣ�https://blog.csdn.net/windcsn/article/details/420958	

	for (int i = 0; i < dwBufferCount; i++) {
		_Data_WSASend data;
		data.socket = s;
		data.sbuffer.ptr = lpBuffers[i].buf;
		data.sbuffer.size = lpBuffers[i].len;

		DWORD dwIP = 0;
		WORD wPort = 0;
		GetSocketIpPort(s, &dwIP, &wPort);
		data.dwIP = dwIP;
		data.wPort = wPort;

		PBYTE pBuffer = NULL;
		DWORD dwBufferSize = MsgPackWithFuncId<_Data_WSASend>(data, pBuffer, ID_WSASend);

		SendData(pBuffer, dwBufferSize);
		delete[] pBuffer;
	}

	return True_WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_WSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr FAR* lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	return True_WSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_WSASendMsg(SOCKET Handle, LPWSAMSG lpMsg, DWORD dwFlags, LPDWORD lpNumberOfBytesSent, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	return True_WSASendMsg(Handle, lpMsg, dwFlags, lpNumberOfBytesSent, lpOverlapped, lpCompletionRoutine);
}


int WSAAPI My_recv(SOCKET s, char* buf, int len, int flags) {
	int iRet = True_recv(s, buf, len, flags);
	if (iRet > 0) {
		_Data_recv data;
		data.socket = s;
		data.sbuffer.ptr = (char*)buf;
		data.sbuffer.size = iRet;

		DWORD dwIP = 0;
		WORD wPort = 0;
		GetSocketIpPort(s, &dwIP, &wPort);
		data.dwIP = dwIP;
		data.wPort = wPort;

		PBYTE pBuffer = NULL;
		DWORD dwBufferSize = MsgPackWithFuncId<_Data_recv>(data, pBuffer, ID_recv);

		SendData(pBuffer, dwBufferSize);
		delete[] pBuffer;
	}
	return iRet;
}


int WSAAPI My_recvfrom(SOCKET s, char FAR* buf, int len, int flags, struct sockaddr FAR* from, int FAR* fromlen) {
	int iRet = True_recvfrom(s, buf, len, flags, from, fromlen);
	if (iRet > 0) {
		_Data_recvfrom data;
		data.socket = s;
		data.sbuffer.ptr = (char*)buf;
		data.sbuffer.size = iRet;

		data.dwIP = ((sockaddr_in*)from)->sin_addr.S_un.S_addr;
		data.wPort = (int)ntohs(((sockaddr_in*)from)->sin_port);

		PBYTE pBuffer = NULL;
		DWORD dwBufferSize = MsgPackWithFuncId<_Data_recvfrom>(data, pBuffer, ID_recvfrom);

		SendData(pBuffer, dwBufferSize);
		delete[] pBuffer;
	}
	return iRet;
}


int WSAAPI My_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	DWORD dwRet = True_WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);

	// TODO �˴�δ�����ص��׽��֣�δ����WSAGetOverlappedResult

	// ����ֵΪ0��ʾδ�������󣬲��ҽ��ղ�����������ɣ�����϶��Ƿ��ص��׽��֣�
	if (dwRet == 0) {										
		for (int i = 0; i < dwBufferCount; i++) {
			_Data_WSARecv data;
			data.socket = s;
			data.sbuffer.ptr = lpBuffers[i].buf;
			data.sbuffer.size = *lpNumberOfBytesRecvd;		// lpNumberOfBytesRecvd�� ������ղ���������ɣ�����᷵�غ������������յ����ֽ���

			DWORD dwIP = 0;
			WORD wPort = 0;
			GetSocketIpPort(s, &dwIP, &wPort);
			data.dwIP = dwIP;
			data.wPort = wPort;

			PBYTE pBuffer = NULL;
			DWORD dwBufferSize = MsgPackWithFuncId<_Data_WSARecv>(data, pBuffer, ID_WSARecv);

			SendData(pBuffer, dwBufferSize);
			delete[] pBuffer;
		}
	}

	return dwRet;
}


int WSAAPI My_WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr FAR* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	return True_WSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
}

#pragma endregion




HANDLE WINAPI My_CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	DllPrintf("call My_CreateFileA\n");

	return True_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


HANDLE WINAPI My_CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	DllPrintf("call My_CreateFileW\n");

	_Data_CreateFileW data;
	data.msgFilePath.ptr = (char*)lpFileName;
	data.msgFilePath.size = wcslen(lpFileName) * 2 + 2;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_CreateFileW>(data, pBuffer, ID_CreateFileW);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return True_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


BOOL WINAPI My_ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {

	CHAR szFilePath[MAX_PATH] = { 0 };
	GetFinalPathNameByHandleA(hFile, szFilePath, MAX_PATH, FILE_NAME_NORMALIZED);
	
	_Data_ReadFile data;
	data.dwFileHandle = (DWORD)hFile;

	data.msgFilePath.ptr = szFilePath;
	data.msgFilePath.size = strlen(szFilePath) + 1;

	//DllPrintf("")

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_ReadFile>(data, pBuffer, ID_ReadFile);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return True_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}