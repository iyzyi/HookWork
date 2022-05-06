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



// ************************************************ 文件系统相关函数-函数指针宏定义 ************************************************

typedef HANDLE (WINAPI* PFN_CreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

typedef HANDLE (WINAPI* PFN_CreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);



typedef BOOL (WINAPI* PFN_ReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);











// *******************************************************************************************


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