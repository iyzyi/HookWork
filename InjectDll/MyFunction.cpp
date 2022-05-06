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


#pragma region 网络通信相关函数-自定义函数

// ************************************************ 网络通信相关函数-自定义函数 ************************************************

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
	// 一些相关内容：https://blog.csdn.net/windcsn/article/details/420958	

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

	// TODO 此处未考虑重叠套接字，未考虑WSAGetOverlappedResult

	// 返回值为0表示未发生错误，并且接收操作已立即完成（即这肯定是非重叠套接字）
	if (dwRet == 0) {										
		for (int i = 0; i < dwBufferCount; i++) {
			_Data_WSARecv data;
			data.socket = s;
			data.sbuffer.ptr = lpBuffers[i].buf;
			data.sbuffer.size = *lpNumberOfBytesRecvd;		// lpNumberOfBytesRecvd： 如果接收操作立即完成，这里会返回函数调用所接收到的字节数

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