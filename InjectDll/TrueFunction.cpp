#include "pch.h"
#include "TrueFunction.h"


#pragma region ����ͨ����غ���-����ָ���ʼ��

// ************************************************ ����ͨ����غ���-����ָ���ʼ�� ************************************************

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





PFN_CreateFileA True_CreateFileA = (PFN_CreateFileA)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateFileA");
PFN_CreateFileW True_CreateFileW = (PFN_CreateFileW)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateFileW");