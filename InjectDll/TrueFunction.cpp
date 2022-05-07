#include "pch.h"
#include "TrueFunction.h"


#pragma region 网络通信相关函数-函数指针初始化

// ************************************************ 网络通信相关函数-函数指针初始化 ************************************************

PFN_send True_send = (PFN_send)GetProcAddress(GetModuleHandleA("ws2_32"), "send");

PFN_sendto True_sendto = (PFN_sendto)GetProcAddress(GetModuleHandleA("ws2_32"), "sendto");

PFN_WSASend True_WSASend = (PFN_WSASend)GetProcAddress(GetModuleHandleA("ws2_32"), "WSASend");

PFN_WSASendTo True_WSASendTo = (PFN_WSASendTo)GetProcAddress(GetModuleHandleA("ws2_32"), "WSASendTo");

PFN_WSASendMsg True_WSASendMsg = (PFN_WSASendMsg)GetProcAddress(GetModuleHandleA("ws2_32"), "WSASendMsg");

PFN_recv True_recv = (PFN_recv)GetProcAddress(GetModuleHandleA("ws2_32"), "recv");

PFN_recvfrom True_recvfrom = (PFN_recvfrom)GetProcAddress(GetModuleHandleA("ws2_32"), "recvfrom");

PFN_WSARecv True_WSARecv = (PFN_WSARecv)GetProcAddress(GetModuleHandleA("ws2_32"), "WSARecv");

PFN_WSARecvFrom True_WSARecvFrom = (PFN_WSARecvFrom)GetProcAddress(GetModuleHandleA("ws2_32"), "WSARecvFrom");

#pragma endregion




#pragma region 文件系统相关函数-函数指针初始化

// ************************************************ 文件系统相关函数-函数指针初始化 ************************************************

PFN_CreateFileA True_CreateFileA = (PFN_CreateFileA)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateFileA");

PFN_CreateFileW True_CreateFileW = (PFN_CreateFileW)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateFileW");

PFN_ReadFile True_ReadFile = (PFN_ReadFile)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "ReadFile");

PFN_ReadFileEx True_ReadFileEx = (PFN_ReadFileEx)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "ReadFileEx");

PFN_WriteFile True_WriteFile = (PFN_WriteFile)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "WriteFile");

PFN_WriteFileEx True_WriteFileEx = (PFN_WriteFileEx)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "WriteFileEx");

PFN_CreateDirectoryA True_CreateDirectoryA = (PFN_CreateDirectoryA)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateDirectoryA");

PFN_CreateDirectoryW True_CreateDirectoryW = (PFN_CreateDirectoryW)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateDirectoryW");

#pragma endregion




#pragma region 注册表相关函数-函数指针初始化

// ************************************************ 注册表相关函数-函数指针初始化 ************************************************

PFN_RegCreateKeyExA True_RegCreateKeyExA = (PFN_RegCreateKeyExA)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegCreateKeyExA");

PFN_RegCreateKeyExW True_RegCreateKeyExW = (PFN_RegCreateKeyExW)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegCreateKeyExW");

PFN_RegOpenKeyExA True_RegOpenKeyExA = (PFN_RegOpenKeyExA)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegOpenKeyExA");

PFN_RegOpenKeyExW True_RegOpenKeyExW = (PFN_RegOpenKeyExW)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegOpenKeyExW");

PFN_RegDeleteKeyExA True_RegDeleteKeyExA = (PFN_RegDeleteKeyExA)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegDeleteKeyExA");

PFN_RegDeleteKeyExW True_RegDeleteKeyExW = (PFN_RegDeleteKeyExW)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegDeleteKeyExW");

PFN_RegCloseKey True_RegCloseKey = (PFN_RegCloseKey)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegCloseKey");

PFN_RegSetValueExA True_RegSetValueExA = (PFN_RegSetValueExA)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegSetValueExA");

PFN_RegSetValueExW True_RegSetValueExW = (PFN_RegSetValueExW)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegSetValueExW");

PFN_RegQueryValueA True_RegQueryValueA = (PFN_RegQueryValueA)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegQueryValueA");

PFN_RegQueryValueW True_RegQueryValueW = (PFN_RegQueryValueW)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegQueryValueW");

PFN_RegGetValueA True_RegGetValueA = (PFN_RegGetValueA)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegGetValueA");

PFN_RegGetValueW True_RegGetValueW = (PFN_RegGetValueW)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegGetValueW");

PFN_RegEnumKeyExA True_RegEnumKeyExA = (PFN_RegEnumKeyExA)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegEnumKeyExA");

PFN_RegEnumKeyExW True_RegEnumKeyExW = (PFN_RegEnumKeyExW)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegEnumKeyExW");

PFN_RegEnumValueA True_RegEnumValueA = (PFN_RegEnumValueA)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegEnumValueA");

PFN_RegEnumValueW True_RegEnumValueW = (PFN_RegEnumValueW)GetProcAddress(GetModuleHandleA("Advapi32.dll"), "RegEnumValueW");

#pragma endregion