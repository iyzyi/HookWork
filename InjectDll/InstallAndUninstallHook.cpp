#include "pch.h"
#include "InstallAndUninstallHook.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "CommunicationPipe.h"
#include "MyFunction.h"
#include "Misc.h"


void AllInstallHook() {
	//DllPrintf("All Install Hook......\n");

	//InstallHook((void**)&True_send, My_send);
	//InstallHook((void**)&True_sendto, My_sendto);
	//InstallHook((void**)&True_WSASend, My_WSASend);
	//InstallHook((void**)&True_WSASendTo, My_WSASendTo);
	//InstallHook((void**)&True_WSASendMsg, My_WSASendMsg);
	//InstallHook((void**)&True_recv, My_recv);
	//InstallHook((void**)&True_recvfrom, My_recvfrom);
	//InstallHook((void**)&True_WSARecv, My_WSARecv);
	//InstallHook((void**)&True_WSARecvFrom, My_WSARecvFrom);
}


void AllUninstallHook() {
	DllPrintf("All Uninstall Hook......\n");

	UninstallHook((void**)&True_send);
	UninstallHook((void**)&True_sendto);
	UninstallHook((void**)&True_WSASend);
	UninstallHook((void**)&True_WSASendTo);
	UninstallHook((void**)&True_WSASendMsg);
	UninstallHook((void**)&True_recv);
	UninstallHook((void**)&True_recvfrom);
	UninstallHook((void**)&True_WSARecv);
	UninstallHook((void**)&True_WSARecvFrom);


	UninstallHook((void**)&True_CreateFileA);
	UninstallHook((void**)&True_CreateFileW);
	UninstallHook((void**)&True_ReadFile);
	UninstallHook((void**)&True_ReadFileEx);
	UninstallHook((void**)&True_WriteFile);
	UninstallHook((void**)&True_WriteFileEx);
	UninstallHook((void**)&True_CreateDirectoryA);
	UninstallHook((void**)&True_CreateDirectoryW);

	UninstallHook((void**)&True_RegOpenKeyExW);
}


void InstallOneHook(PBYTE szBuffer) {
	DWORD dwFuncId = GetDwordFromBuffer(szBuffer, 11);

	switch (dwFuncId) {

#pragma region 网络通信相关函数
	case ID_send: {
		DllPrintf("Install Hook send\n");
		InstallHook((void**)&True_send, My_send);
		break;
	}
	
	case ID_recv: {
		DllPrintf("Install Hook recv\n");
		InstallHook((void**)&True_recv, My_recv);
		break;
	}

	case ID_sendto:{
		DllPrintf("Install Hook sendto\n");
		InstallHook((void**)&True_sendto, My_sendto);
		break;
	}
	
	case ID_recvfrom: {
		DllPrintf("Install Hook recvfrom\n");
		InstallHook((void**)&True_recvfrom, My_recvfrom);
		break;
	}

	case ID_WSASend:{
		DllPrintf("Install Hook WSASend\n");
		InstallHook((void**)&True_WSASend, My_WSASend);
		break;
	}

	case ID_WSARecv: {
		DllPrintf("Install Hook WSARecv\n");
		InstallHook((void**)&True_WSARecv, My_WSARecv);
		break;
	}
#pragma endregion

#pragma region 文件系统相关函数
	case ID_CreateFileA: {
		DllPrintf("Install Hook CreateFileA\n");
		InstallHook((void**)&True_CreateFileA, My_CreateFileA);
		break;
	}

	case ID_CreateFileW: {
		DllPrintf("Install Hook CreateFileW\n");
		InstallHook((void**)&True_CreateFileW, My_CreateFileW);
		break;
	}
	
	case ID_ReadFile: {
		DllPrintf("Install Hook ReadFile\n");
		InstallHook((void**)&True_ReadFile, My_ReadFile);
		break;
	}

	case ID_ReadFileEx:
	{
		DllPrintf("Install Hook ReadFileEx\n");
		InstallHook((void**)&True_ReadFileEx, My_ReadFileEx);
		break;
	}


	case ID_WriteFile:
	{
		DllPrintf("Install Hook WriteFile\n");
		InstallHook((void**)&True_WriteFile, My_WriteFile);
		break;
	}

	case ID_WriteFileEx:
	{
		DllPrintf("Install Hook WriteFileEx\n");
		InstallHook((void**)&True_WriteFileEx, My_WriteFileEx);
		break;
	}

	case ID_CreateDirectoryA:
	{
		DllPrintf("Install Hook CreateDirectoryA\n");
		InstallHook((void**)&True_CreateDirectoryA, My_CreateDirectoryA);
		break;
	}

	case ID_CreateDirectoryW:
	{
		DllPrintf("Install Hook CreateDirectoryW\n");
		InstallHook((void**)&True_CreateDirectoryW, My_CreateDirectoryW);
		break;
	}
#pragma endregion

	case ID_RegOpenKeyExW:
	{
		DllPrintf("Install Hook RegOpenKeyExW\n");
		InstallHook((void**)&True_RegOpenKeyExW, My_RegOpenKeyExW);
		break;
	}

	default:
		break;
	}
}