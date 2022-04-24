// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "DataLog.h"


#pragma warning(disable : 4996)

#define QWORD unsigned long long int
#define PQWORD QWORD*





CDataLog* m_pDataLog;
LPVOID pWebSocketSSL = NULL;




// ***************************相关函数原型***************************

//等效于HOOK前的recv和send的函数的指针
typedef int (WINAPI* PFN_Recv)(LPVOID ssl, void* buf, int num);
typedef int (WINAPI* PFN_Send)(LPVOID ssl, const void* buf, int num);
// LPVOID ssl 原为ssl_st* ssl 或 SSL* ssl
// ssl_st与SSL是typedef关系
// 我应该用不到这个参数，所以直接转换成LPVOID
// ssl_st详见 https://docs.huihoo.com/doxygen/openssl/1.0.1c/structssl__st.html


PFN_Recv TrueRecv = (PFN_Recv)((QWORD)GetModuleHandleA("SGSOL.exe") + (QWORD)(0x14268E690 - 0x140000000));
PFN_Send TrueSend = (PFN_Send)((QWORD)GetModuleHandleA("SGSOL.exe") + (QWORD)(0x14268EAD0 - 0x140000000));

// ***************************相关函数原型***************************





// ***************************自定义HOOK函数***************************
//VOID SetWebSocketSSL(LPVOID ssl, void* buf, int len) {
//	if (len > 17) {
//		CHAR PartOfFirstRecvPacket[] = { 0x55, 0x70, 0x67, 0x72, 0x61, 0x64, 0x65, 0x3a, 0x77, 0x65, 0x62, 0x73, 0x6f, 0x63, 0x6b, 0x65, 0x74, 0x00 };		//Upgrade:websocket
//		PCHAR szData = new CHAR[len + 12];
//		memset(szData, 0, len + 12);
//		memcpy(szData, buf, len);			// buf中第num个字节未必是\x00，所以需要拷贝到szData中再调用strstr
//
//		if (strstr(szData, PartOfFirstRecvPacket)) {
//			pWebSocketSSL = ssl;
//			m_pDataLog->LogFormatString(64, "[INFO] Find WebSocket SSL: 0x%llx\n\n", ssl);
//		}
//	}
//}
//
//
//BOOL IsWebSocketSSL(LPVOID pCurrentSSL) {
//	if (pWebSocketSSL != NULL) {
//		return pWebSocketSSL == pCurrentSSL;
//	}
//	return FALSE;
//}


int WINAPI My_Recv(LPVOID ssl, void* buf, int num)
{
	//MessageBoxA(NULL, "弹窗2", "弹窗2", NULL);
	int ret = TrueRecv(ssl, buf, num);
	if (ret > 0) {
		//if (pWebSocketSSL == NULL) {
		//	SetWebSocketSSL(ssl, buf, ret);
		//}

		//if (IsWebSocketSSL(ssl)) {
			m_pDataLog->LogFormatString(64, "[PID:%d\tSSL:0x%llx] Recv Data (%d Bytes): \n", GetCurrentProcessId(), ssl, ret);
			m_pDataLog->LogHexData("", (PBYTE)buf, ret);
			m_pDataLog->LogString("\n\n");
		//}
	}
	return ret;
}

int WINAPI My_Send(LPVOID ssl, const void* buf, int num)
{
	//if (IsWebSocketSSL(ssl)) {
		m_pDataLog->LogFormatString(64, "[PID:%d\tSSL:0x%llx] Send Data (%d Bytes): \n", GetCurrentProcessId(), ssl, num);
		m_pDataLog->LogHexData("", (PBYTE)buf, num);
		m_pDataLog->LogString("\n\n");
	//}

	return TrueSend(ssl, buf, num);
}
// ***************************自定义HOOK函数***************************





BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		m_pDataLog = new CDataLog("d:\\桌面\\sanguosha.log");
		InstallHook((void**)&TrueRecv, My_Recv);
		InstallHook((void**)&TrueSend, My_Send);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}