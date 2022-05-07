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


#pragma region 文件系统相关函数-自定义函数

// ************************************************ 文件系统相关函数-自定义函数 ************************************************

HANDLE WINAPI My_CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	HANDLE hFile = True_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (hFile == INVALID_HANDLE_VALUE)
		return hFile;

	_Data_CreateFileA data;
	data.dwFileHandle = (DWORD)hFile;

	data.msgFilePath.ptr = (char*)lpFileName;
	data.msgFilePath.size = strlen(lpFileName) + 1;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_CreateFileA>(data, pBuffer, ID_CreateFileA);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return hFile;
}


HANDLE WINAPI My_CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	HANDLE hFile = True_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (hFile == INVALID_HANDLE_VALUE)
		return hFile;

	_Data_CreateFileW data;
	data.dwFileHandle = (DWORD)hFile;

	data.msgFilePath.ptr = (char*)lpFileName;
	data.msgFilePath.size = wcslen(lpFileName) * 2 + 2;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_CreateFileW>(data, pBuffer, ID_CreateFileW);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return hFile;
}


BOOL WINAPI My_ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
	BOOL bRet = True_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

	if (!bRet)				// 如果函数失败或异步完成，则返回值为零 （FALSE） 
		return bRet;		// TODO 这里我没考虑异步

	CHAR szFilePath[MAX_PATH] = { 0 };
	GetFinalPathNameByHandleA(hFile, szFilePath, MAX_PATH, FILE_NAME_NORMALIZED);		// TODO 该函数不适用于xp系统。
	
	_Data_ReadFile data;
	data.dwFileHandle = (DWORD)hFile;

	data.msgFilePath.ptr = szFilePath;
	data.msgFilePath.size = strlen(szFilePath) + 1;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_ReadFile>(data, pBuffer, ID_ReadFile);		// TODO 这里只传输了文件路径，没传输文件具体读写内容

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return bRet;
}

BOOL WINAPI My_ReadFileEx(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	BOOL bRet = True_ReadFileEx(hFile, lpBuffer, nNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine);

	if (!bRet)			
		return bRet;

	CHAR szFilePath[MAX_PATH] = { 0 };
	GetFinalPathNameByHandleA(hFile, szFilePath, MAX_PATH, FILE_NAME_NORMALIZED);

	_Data_ReadFileEx data;
	data.dwFileHandle = (DWORD)hFile;

	data.msgFilePath.ptr = szFilePath;
	data.msgFilePath.size = strlen(szFilePath) + 1;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_ReadFileEx>(data, pBuffer, ID_ReadFileEx);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return bRet;
}

BOOL WINAPI My_WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) {
	BOOL bRet = True_WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	
	if (!bRet)				// 如果函数失败或异步完成，则返回值为零 （FALSE） 
		return bRet;		// TODO 这里我没考虑异步

	CHAR szFilePath[MAX_PATH] = { 0 };
	GetFinalPathNameByHandleA(hFile, szFilePath, MAX_PATH, FILE_NAME_NORMALIZED);

	_Data_WriteFile data;
	data.dwFileHandle = (DWORD)hFile;

	data.msgFilePath.ptr = szFilePath;
	data.msgFilePath.size = strlen(szFilePath) + 1;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_WriteFile>(data, pBuffer, ID_WriteFile);

	SendData(pBuffer, dwBufferSize);			// 此函数内部调用了WriteFile，必须改为True_WriteFile，否则会循环HOOK，导致程序崩溃。
	delete[] pBuffer;

	return bRet;
}

BOOL WINAPI My_WriteFileEx(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	BOOL bRet = True_WriteFileEx(hFile, lpBuffer, nNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine);

	if (!bRet)		
		return bRet;

	CHAR szFilePath[MAX_PATH] = { 0 };
	GetFinalPathNameByHandleA(hFile, szFilePath, MAX_PATH, FILE_NAME_NORMALIZED);

	_Data_WriteFileEx data;
	data.dwFileHandle = (DWORD)hFile;

	data.msgFilePath.ptr = szFilePath;
	data.msgFilePath.size = strlen(szFilePath) + 1;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_WriteFileEx>(data, pBuffer, ID_WriteFileEx);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return bRet;
}

BOOL WINAPI My_CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	BOOL bRet = True_CreateDirectoryA(lpPathName, lpSecurityAttributes);
	if (!bRet)
		return bRet;

	_Data_CreateDirectoryA data;
	data.msgPathName.ptr = lpPathName;
	data.msgPathName.size = strlen(lpPathName) + 1;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_CreateDirectoryA>(data, pBuffer, ID_CreateDirectoryA);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return bRet;
}

BOOL WINAPI My_CreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	BOOL bRet = True_CreateDirectoryW(lpPathName, lpSecurityAttributes);
	if (!bRet)
		return bRet;

	_Data_CreateDirectoryW data;
	data.msgPathName.ptr = (char*)lpPathName;
	data.msgPathName.size = wcslen(lpPathName) * 2 + 2;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_CreateDirectoryW>(data, pBuffer, ID_CreateDirectoryW);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return bRet;
}

#pragma endregion










LSTATUS APIENTRY My_RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition) {
	return True_RegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS APIENTRY My_RegCreateKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition) {
	// hKey不是创建后的hKey，而是打开的注册表项的句柄。lpSubKey是将要创建的 子 项的相对路径
	// 创建成功后的新hKey存入phkResult
	
	_Data_RegCreateKeyExW data;

	std::wstring wsRootPath = GetWstrKeyPathFromHKEY(hKey);
	PWCHAR pwszPath = NULL;
	DWORD dwStrLen = 0, dwBufLen = 0;
	if (lpSubKey != NULL) {								// 这里没判断是否为NULL，坑我近两个小时。。。。
		dwStrLen = wsRootPath.length() * 2 + wcslen(lpSubKey) * 2 + 2;
		dwBufLen = dwStrLen + 16;
		pwszPath = new WCHAR[dwBufLen];
		memset(pwszPath, 0, dwBufLen);
		swprintf(pwszPath, dwBufLen, L"%s\\%s", (LPCWSTR)wsRootPath.c_str(), lpSubKey);
		pwszPath[dwStrLen / 2] = 0;
	}
	else {
		dwStrLen = wsRootPath.length() * 2 + 2;
		dwBufLen = dwStrLen + 16;
		pwszPath = new WCHAR[dwBufLen];
		memset(pwszPath, 0, dwBufLen);
		wcscpy_s(pwszPath, dwBufLen, wsRootPath.c_str());
		pwszPath[dwStrLen / 2] = 0;
	}
	data.msgPath.ptr = (char*)pwszPath;
	data.msgPath.size = dwStrLen;

	LSTATUS dwRet = True_RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);

	data.upKeyHandle = (UINT_PTR)*phkResult;		
	data.dwRet = dwRet;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_RegCreateKeyExW>(data, pBuffer, ID_RegCreateKeyExW);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return dwRet;
}

LSTATUS APIENTRY My_RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
	return True_RegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}


LSTATUS APIENTRY My_RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
	_Data_RegOpenKeyExW data;

	std::wstring wsRootPath = GetWstrKeyPathFromHKEY(hKey);
	PWCHAR pwszPath = NULL;
	DWORD dwStrLen = 0, dwBufLen = 0;
	if (lpSubKey != NULL) {								// 这里没判断是否为NULL，坑我近两个小时。。。。
		dwStrLen = wsRootPath.length() * 2 + wcslen(lpSubKey) * 2 + 2;
		dwBufLen = dwStrLen + 16;
		pwszPath = new WCHAR[dwBufLen];
		memset(pwszPath, 0, dwBufLen);
		swprintf(pwszPath, dwBufLen, L"%s\\%s", (LPCWSTR)wsRootPath.c_str(), lpSubKey);
		pwszPath[dwStrLen / 2] = 0;
	}
	else {
		dwStrLen = wsRootPath.length() * 2 + 2;
		dwBufLen = dwStrLen + 16;
		pwszPath = new WCHAR[dwBufLen];
		memset(pwszPath, 0, dwBufLen);
		wcscpy_s(pwszPath, dwBufLen, wsRootPath.c_str());
		pwszPath[dwStrLen / 2] = 0;
	}
	data.msgPath.ptr = (char*)pwszPath;
	data.msgPath.size = dwStrLen;

	LSTATUS dwRet = True_RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);

	data.upKeyHandle = (UINT_PTR)*phkResult;
	data.dwRet = dwRet;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_RegOpenKeyExW>(data, pBuffer, ID_RegOpenKeyExW);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return dwRet;
}

LSTATUS APIENTRY My_RegDeleteKeyExA(HKEY hKey, LPCSTR lpSubKey, REGSAM samDesired, DWORD Reserved) {
	return True_RegDeleteKeyExA(hKey, lpSubKey, samDesired, Reserved);
}

LSTATUS APIENTRY My_RegDeleteKeyExW(HKEY hKey, LPCWSTR lpSubKey, REGSAM samDesired, DWORD Reserved) {
	_Data_RegDeleteKeyExW data;

	if (lpSubKey != NULL) {
		data.msgPath.ptr = (char*)lpSubKey;
		data.msgPath.size = wcslen(lpSubKey) * 2 + 2;
	}
	else {
		data.msgPath.size = 0;
	}

	LSTATUS dwRet = True_RegDeleteKeyExW(hKey, lpSubKey, samDesired, Reserved);

	data.upKeyHandle = (UINT_PTR)hKey;
	data.dwRet = dwRet;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_RegDeleteKeyExW>(data, pBuffer, ID_RegDeleteKeyExW);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return dwRet;
}


LSTATUS APIENTRY My_RegCloseKey(HKEY hKey){

	std::string sKeyPath = GetStrKeyPathFromHKEY(hKey);
		
	LSTATUS dwRet = True_RegCloseKey(hKey);					// 一定要先获取Path，在CloseKey。若先CloseKey，则必然获取Path失败

	_Data_RegCloseKey data;
	data.upKeyHandle = (UINT_PTR)hKey;
	data.msgPath.ptr = sKeyPath.c_str();
	data.msgPath.size = sKeyPath.length() + 1;
	data.dwRet = dwRet;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_RegCloseKey>(data, pBuffer, ID_RegCloseKey);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;

	return dwRet;
}

LSTATUS APIENTRY My_RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, DWORD cbData) {
	return True_RegSetValueExA(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

LSTATUS APIENTRY My_RegSetValueExW(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, DWORD cbData) {

	// TODO lpData没传给server

	_Data_RegSetValueExW data;

	std::wstring wsKeyPath = GetWstrKeyPathFromHKEY(hKey);
	data.msgPath.ptr = (char*)wsKeyPath.c_str();
	data.msgPath.size = wsKeyPath.length() * 2 + 2;

	if (lpValueName != NULL) {
		data.msgValueName.ptr = (char*)lpValueName;
		data.msgValueName.size = wcslen(lpValueName) * 2 + 2;
	}
	else {
		WCHAR wszTemp[] = L"NULL";
		data.msgValueName.ptr = (char*)wszTemp;
		data.msgValueName.size = wcslen(wszTemp) * 2 + 2;
	}

	LSTATUS dwRet = True_RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);

	data.upKeyHandle = (UINT_PTR)hKey;
	data.dwRet = dwRet;
	data.dwType = dwType;

	PBYTE pBuffer = NULL;
	DWORD dwBufferSize = MsgPackWithFuncId<_Data_RegSetValueExW>(data, pBuffer, ID_RegSetValueExW);

	SendData(pBuffer, dwBufferSize);
	delete[] pBuffer;
	
	return dwRet;
}

LSTATUS APIENTRY My_RegQueryValueA(HKEY hKey, LPCSTR lpSubKey, LPSTR lpData, PLONG lpcbData) {
	return True_RegQueryValueA(hKey, lpSubKey, lpData, lpcbData);
}

LSTATUS APIENTRY My_RegQueryValueW(HKEY hKey, LPCWSTR lpSubKey, LPWSTR lpData, PLONG lpcbData) {
	return True_RegQueryValueW(hKey, lpSubKey, lpData, lpcbData);
}

LSTATUS APIENTRY My_RegGetValueA(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
	return True_RegGetValueA(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

LSTATUS APIENTRY My_RegGetValueW(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
	return True_RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

LSTATUS APIENTRY My_RegEnumKeyExA(HKEY hKey, DWORD dwIndex, LPSTR lpName, LPDWORD lpcchName, LPDWORD lpReserved, LPSTR lpClass, LPDWORD lpcchClass, PFILETIME lpftLastWriteTime) {
	return True_RegEnumKeyExA(hKey, dwIndex, lpName, lpcchName, lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
}

LSTATUS APIENTRY My_RegEnumKeyExW(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName, LPDWORD lpReserved, LPWSTR lpClass, LPDWORD lpcchClass, PFILETIME lpftLastWriteTime) {
	return True_RegEnumKeyExW(hKey, dwIndex, lpName, lpcchName, lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
}

LSTATUS APIENTRY My_RegEnumValueA(HKEY hKey, DWORD dwIndex, LPSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
	return True_RegEnumValueA(hKey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData);
}

LSTATUS APIENTRY My_RegEnumValueW(HKEY hKey, DWORD dwIndex, LPWSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
	return True_RegEnumValueW(hKey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData);
}