#include "pch.h"
#include "InstallAndUninstallHook.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "CommunicationPipe.h"
#include "MyFunction.h"
#include "Misc.h"


void AllInstallHook() {
	DllPrintf("AllInstallHook......\n");

	InstallHook((void**)&True_send, My_send);
	InstallHook((void**)&True_sendto, My_sendto);
	InstallHook((void**)&True_WSASend, My_WSASend);
	InstallHook((void**)&True_WSASendTo, My_WSASendTo);
	InstallHook((void**)&True_WSASendMsg, My_WSASendMsg);

	InstallHook((void**)&True_recv, My_recv);
	InstallHook((void**)&True_recvfrom, My_recvfrom);
	InstallHook((void**)&True_WSARecv, My_WSARecv);
	InstallHook((void**)&True_WSARecvFrom, My_WSARecvFrom);
}


void AllUninstallHook() {
	DllPrintf("AllUninstallHook......\n");

	UninstallHook((void**)&True_send);
	UninstallHook((void**)&True_sendto);
	UninstallHook((void**)&True_WSASend);
	UninstallHook((void**)&True_WSASendTo);
	UninstallHook((void**)&True_WSASendMsg);

	UninstallHook((void**)&True_recv);
	UninstallHook((void**)&True_recvfrom);
	UninstallHook((void**)&True_WSARecv);
	UninstallHook((void**)&True_WSARecvFrom);
}


void InstallOneHook(PBYTE szBuffer) {
	DWORD dwFuncId = GetDwordFromBuffer(szBuffer, 11);

	switch (dwFuncId) {

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

	default:
		break;
	}
}