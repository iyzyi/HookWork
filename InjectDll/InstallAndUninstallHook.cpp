#include "pch.h"
#include "InstallAndUninstallHook.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "CommunicationPipe.h"
#include "MyFunction.h"
#include "Misc.h"


void AllInstallHook() {
	DllPrintf("All Install Hook......\n");

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


	UninstallHook((void**)&True_RegCreateKeyExA);
	UninstallHook((void**)&True_RegCreateKeyExW);
	UninstallHook((void**)&True_RegOpenKeyExA);
	UninstallHook((void**)&True_RegOpenKeyExW);
	UninstallHook((void**)&True_RegDeleteKeyExA);
	UninstallHook((void**)&True_RegDeleteKeyExW);
	UninstallHook((void**)&True_RegCloseKey);
	UninstallHook((void**)&True_RegSetValueExA);
	UninstallHook((void**)&True_RegSetValueExW);
	//UninstallHook((void**)&True_RegQueryValueA);
	//UninstallHook((void**)&True_RegQueryValueW);
	UninstallHook((void**)&True_RegQueryValueExA);
	UninstallHook((void**)&True_RegQueryValueExW);
	UninstallHook((void**)&True_RegGetValueA);
	UninstallHook((void**)&True_RegGetValueW);
	UninstallHook((void**)&True_RegEnumKeyExA);
	UninstallHook((void**)&True_RegEnumKeyExW);
	//UninstallHook((void**)&True_RegEnumValueA);
	//UninstallHook((void**)&True_RegEnumValueW);
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

	case ID_RegCreateKeyExA: {
		DllPrintf("Install Hook RegCreateKeyExA\n");
		InstallHook((void**)&True_RegCreateKeyExA, My_RegCreateKeyExA);
		break;
	}

	case ID_RegCreateKeyExW: {
		DllPrintf("Install Hook RegCreateKeyExW\n");
		InstallHook((void**)&True_RegCreateKeyExW, My_RegCreateKeyExW);
		break;
	}

	case ID_RegOpenKeyExA: {
		DllPrintf("Install Hook RegOpenKeyExA\n");
		InstallHook((void**)&True_RegOpenKeyExA, My_RegOpenKeyExA);
		break;
	}

	case ID_RegOpenKeyExW: {
		DllPrintf("Install Hook RegOpenKeyExW\n");
		InstallHook((void**)&True_RegOpenKeyExW, My_RegOpenKeyExW);
		break;
	}

	case ID_RegDeleteKeyExA: {
		DllPrintf("Install Hook RegDeleteKeyExA\n");
		InstallHook((void**)&True_RegDeleteKeyExA, My_RegDeleteKeyExA);
		break;
	}

	case ID_RegDeleteKeyExW: {
		DllPrintf("Install Hook RegDeleteKeyExW\n");
		InstallHook((void**)&True_RegDeleteKeyExW, My_RegDeleteKeyExW);
		break;
	}

	case ID_RegCloseKey: {
		DllPrintf("Install Hook RegCloseKey\n");
		InstallHook((void**)&True_RegCloseKey, My_RegCloseKey);
		break;
	}

	case ID_RegSetValueExA: {
		DllPrintf("Install Hook RegSetValueExA\n");
		InstallHook((void**)&True_RegSetValueExA, My_RegSetValueExA);
		break;
	}

	case ID_RegSetValueExW: {
		DllPrintf("Install Hook RegSetValueExW\n");
		InstallHook((void**)&True_RegSetValueExW, My_RegSetValueExW);
		break;
	}

	//case ID_RegQueryValueA: {
	//	DllPrintf("Install Hook RegQueryValueA\n");
	//	InstallHook((void**)&True_RegQueryValueA, My_RegQueryValueA);
	//	break;
	//}

	//case ID_RegQueryValueW: {
	//	DllPrintf("Install Hook RegQueryValueW\n");
	//	InstallHook((void**)&True_RegQueryValueW, My_RegQueryValueW);
	//	break;
	//}

	case ID_RegGetValueA: {
		DllPrintf("Install Hook RegGetValueA\n");
		InstallHook((void**)&True_RegGetValueA, My_RegGetValueA);
		break;
	}

	case ID_RegGetValueW: {
		DllPrintf("Install Hook RegGetValueW\n");
		InstallHook((void**)&True_RegGetValueW, My_RegGetValueW);
		break;
	}

	case ID_RegEnumKeyExA: {
		DllPrintf("Install Hook RegEnumKeyExA\n");
		InstallHook((void**)&True_RegEnumKeyExA, My_RegEnumKeyExA);
		break;
	}

	case ID_RegEnumKeyExW: {
		DllPrintf("Install Hook RegEnumKeyExW\n");
		InstallHook((void**)&True_RegEnumKeyExW, My_RegEnumKeyExW);
		break;
	}

	//case ID_RegEnumValueA: {
	//	DllPrintf("Install Hook RegEnumValueA\n");
	//	InstallHook((void**)&True_RegEnumValueA, My_RegEnumValueA);
	//	break;
	//}

	//case ID_RegEnumValueW: {
	//	DllPrintf("Install Hook RegEnumValueW\n");
	//	InstallHook((void**)&True_RegEnumValueW, My_RegEnumValueW);
	//	break;
	//}

	default:
		break;
	}
}