#pragma once
#include <WinSock2.h>


#pragma region 网络通信相关函数-函数指针宏定义

// ************************************************ 网络通信相关函数-函数指针宏定义 ************************************************

typedef int (WSAAPI* PFN_send)(SOCKET s, const char FAR* buf, int len, int flags);

typedef int (WSAAPI* PFN_sendto)(SOCKET s, const char FAR* buf, int len, int flags, const struct sockaddr FAR* to, int tolen);

typedef int (WSAAPI* PFN_WSASend)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef int (WSAAPI* PFN_WSASendTo)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr FAR* lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef int (WSAAPI* PFN_WSASendMsg)(SOCKET Handle, LPWSAMSG lpMsg, DWORD dwFlags, LPDWORD lpNumberOfBytesSent, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef int (WSAAPI* PFN_recv)(SOCKET s, char FAR* buf, int len, int flags);

typedef int (WSAAPI* PFN_recvfrom)(SOCKET s, char FAR* buf, int len, int flags, struct sockaddr FAR* from, int FAR* fromlen);

typedef int (WSAAPI* PFN_WSARecv)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef int (WSAAPI* PFN_WSARecvFrom)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr FAR * lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#pragma endregion


#pragma region 文件系统相关函数-函数指针宏定义

// ************************************************ 文件系统相关函数-函数指针宏定义 ************************************************

typedef HANDLE (WINAPI* PFN_CreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

typedef HANDLE (WINAPI* PFN_CreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

typedef BOOL (WINAPI* PFN_ReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

typedef BOOL (WINAPI* PFN_ReadFileEx)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef BOOL (WINAPI* PFN_WriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

typedef BOOL (WINAPI* PFN_WriteFileEx)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef BOOL (WINAPI* PFN_CreateDirectoryA)(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

typedef BOOL (WINAPI* PFN_CreateDirectoryW)(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

#pragma endregion


#pragma region 注册表相关函数-函数指针宏定义

// ************************************************ 注册表相关函数-函数指针宏定义 ************************************************

typedef LSTATUS (APIENTRY* PFN_RegCreateKeyExA)(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);

typedef LSTATUS (APIENTRY* PFN_RegCreateKeyExW)(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);

typedef LSTATUS (APIENTRY* PFN_RegOpenKeyExA)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);

typedef LSTATUS (APIENTRY* PFN_RegOpenKeyExW)(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);

typedef LSTATUS (APIENTRY* PFN_RegDeleteKeyExA)(HKEY hKey, LPCSTR lpSubKey, REGSAM samDesired, DWORD Reserved);

typedef LSTATUS (APIENTRY* PFN_RegDeleteKeyExW)(HKEY hKey, LPCWSTR lpSubKey, REGSAM samDesired, DWORD Reserved);

typedef LSTATUS (APIENTRY* PFN_RegCloseKey)(HKEY hKey);

typedef LSTATUS (APIENTRY* PFN_RegSetValueExA)(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, DWORD cbData);

typedef LSTATUS (APIENTRY* PFN_RegSetValueExW)(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, DWORD cbData);

//typedef LSTATUS (APIENTRY* PFN_RegQueryValueA)(HKEY hKey, LPCSTR lpSubKey, LPSTR lpData, PLONG lpcbData);
//
//typedef LSTATUS (APIENTRY* PFN_RegQueryValueW)(HKEY hKey, LPCWSTR lpSubKey, LPWSTR lpData, PLONG lpcbData);

typedef LSTATUS (APIENTRY* PFN_RegQueryValueExA)(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

typedef LSTATUS (APIENTRY* PFN_RegQueryValueExW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE  lpData, LPDWORD lpcbData);

typedef LSTATUS (APIENTRY* PFN_RegGetValueA)(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);

typedef LSTATUS (APIENTRY* PFN_RegGetValueW)(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);

typedef LSTATUS (APIENTRY* PFN_RegEnumKeyExA)(HKEY hKey, DWORD dwIndex, LPSTR lpName, LPDWORD lpcchName, LPDWORD lpReserved, LPSTR lpClass, LPDWORD lpcchClass, PFILETIME lpftLastWriteTime);

typedef LSTATUS (APIENTRY* PFN_RegEnumKeyExW)(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName, LPDWORD lpReserved, LPWSTR lpClass, LPDWORD lpcchClass, PFILETIME lpftLastWriteTime);

//typedef LSTATUS (APIENTRY* PFN_RegEnumValueA)(HKEY hKey, DWORD dwIndex, LPSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
//
//typedef LSTATUS (APIENTRY* PFN_RegEnumValueW)(HKEY hKey, DWORD dwIndex, LPWSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

#pragma endregion





// **************************************************************************************************************************************************************************************


// 网络通信相关函数
extern PFN_send						True_send;
extern PFN_sendto					True_sendto;
extern PFN_WSASend					True_WSASend;
extern PFN_WSASendTo				True_WSASendTo;
extern PFN_WSASendMsg				True_WSASendMsg;
extern PFN_recv						True_recv;
extern PFN_recvfrom					True_recvfrom;
extern PFN_WSARecv					True_WSARecv;
extern PFN_WSARecvFrom				True_WSARecvFrom;


// 文件系统相关函数
extern PFN_CreateFileA				True_CreateFileA;
extern PFN_CreateFileW				True_CreateFileW;
extern PFN_ReadFile					True_ReadFile;
extern PFN_ReadFileEx				True_ReadFileEx;
extern PFN_WriteFile				True_WriteFile;
extern PFN_WriteFileEx				True_WriteFileEx;
extern PFN_CreateDirectoryA			True_CreateDirectoryA;
extern PFN_CreateDirectoryW			True_CreateDirectoryW;


// 注册表相关函数
extern PFN_RegCreateKeyExA			True_RegCreateKeyExA;
extern PFN_RegCreateKeyExW			True_RegCreateKeyExW;
extern PFN_RegOpenKeyExA			True_RegOpenKeyExA;
extern PFN_RegOpenKeyExW			True_RegOpenKeyExW;
extern PFN_RegDeleteKeyExA			True_RegDeleteKeyExA;
extern PFN_RegDeleteKeyExW			True_RegDeleteKeyExW;
extern PFN_RegCloseKey				True_RegCloseKey;
extern PFN_RegSetValueExA			True_RegSetValueExA;
extern PFN_RegSetValueExW			True_RegSetValueExW;
//extern PFN_RegQueryValueA			True_RegQueryValueA;
//extern PFN_RegQueryValueW			True_RegQueryValueW;
extern PFN_RegQueryValueExA			True_RegQueryValueExA;
extern PFN_RegQueryValueExW			True_RegQueryValueExW;
extern PFN_RegGetValueA				True_RegGetValueA;
extern PFN_RegGetValueW				True_RegGetValueW;
extern PFN_RegEnumKeyExA			True_RegEnumKeyExA;
extern PFN_RegEnumKeyExW			True_RegEnumKeyExW;
//extern PFN_RegEnumValueA			True_RegEnumValueA;
//extern PFN_RegEnumValueW			True_RegEnumValueW;