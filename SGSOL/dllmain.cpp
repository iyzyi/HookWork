// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "DataLog.h"


#pragma warning(disable : 4996)

#define QWORD unsigned long long int
#define PQWORD QWORD*


HANDLE hPipe = NULL;

CDataLog* m_pDataLog;


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



int DllPrintf(PCHAR fmt, ...)
{
	//不定参数格式化
	va_list argptr;
	va_start(argptr, fmt);
	char buffer[512] = { 0 };
	int cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	//创建控制台窗口
	static HANDLE gHConsole = INVALID_HANDLE_VALUE;
	if (INVALID_HANDLE_VALUE == gHConsole) {
		AllocConsole();								// 为调用进程分配一个新的控制台。
		gHConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	//打印到指定的控制台窗口
	DWORD dw;
	WriteConsoleA(gHConsole, buffer, strlen(buffer), &dw, NULL);

	return(cnt);
}








// ***************************自定义HOOK函数***************************
int WINAPI My_Recv(LPVOID ssl, void* buf, int num)
{
	DllPrintf("recv ");
	int ret = TrueRecv(ssl, buf, num);
	if (ret > 0) {
		m_pDataLog->LogFormatString(64, "[PID:%d\tSSL:0x%llx] Recv Data (%d Bytes): \n", GetCurrentProcessId(), ssl, ret);
		m_pDataLog->LogHexData("", (PBYTE)buf, ret);
		m_pDataLog->LogString("\n\n");


		CHAR szBuffer[1024] = { 0 };
		DWORD dwReturn = 0;
		sprintf(szBuffer, "recv(0x%p, 0x%p, %d)", ssl, buf, num);
		DllPrintf(szBuffer);
		if (!WriteFile(hPipe, szBuffer, strlen(szBuffer), &dwReturn, NULL))
		{
			printf("Write Failed\n");
		}
	}
	return ret;
}

int WINAPI My_Send(LPVOID ssl, const void* buf, int num)
{
	DllPrintf("send ");
	m_pDataLog->LogFormatString(64, "[PID:%d\tSSL:0x%llx] Send Data (%d Bytes): \n", GetCurrentProcessId(), ssl, num);
	m_pDataLog->LogHexData("", (PBYTE)buf, num);
	m_pDataLog->LogString("\n\n");

	return TrueSend(ssl, buf, num);
}
// ***************************自定义HOOK函数***************************








#define COMMAND_PIPE_BUF_SIZE			4096
#define DATA_PIPE_BUF_SIZE				0xffffff
// CommandPipe命令管道：server写，本dll读
// DataPipe数据管道：本dll写，server读

#define PIPE							"\\\\.\\pipe\\Pipe"


DWORD WINAPI ThreadFunc(LPVOID lpParameter) {
	char  szBuffer[COMMAND_PIPE_BUF_SIZE] = { 0 };
	DWORD dwReturn = 0;

	// 判断是否有可以利用的命名管道  
	if (!WaitNamedPipeA(PIPE, NMPWAIT_USE_DEFAULT_WAIT))
	{
		DllPrintf("未找到CommandPipe管道\n");
		return 0;
	}

	// 打开可用的命名管道 , 并与服务器端进程进行通信  
	hPipe = CreateFileA(
		PIPE,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, 
		OPEN_EXISTING, 
		0, 
		NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		DllPrintf("CommandPipe管道打开失败\n");
		return 0;
	}

	m_pDataLog = new CDataLog("d:\\桌面\\sanguosha.log");			// 创建日志

	while (ReadFile(hPipe, szBuffer, COMMAND_PIPE_BUF_SIZE, &dwReturn, NULL)) {
		szBuffer[dwReturn] = '\0';
		//DllPrintf("收到命令: %s\n", szBuffer);

		if (strcmp(szBuffer, "InstallHook") == 0) {
			DllPrintf("InstallHook......\n");
			InstallHook((void**)&TrueRecv, My_Recv);
			InstallHook((void**)&TrueSend, My_Send);
		}
		else if (strcmp(szBuffer, "UninstallHook") == 0) {
			DllPrintf("UninstallHook......\n");
			UninstallHook((void**)&TrueRecv);
			UninstallHook((void**)&TrueSend);
		}
	}

	return 0;
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DllPrintf("远程注入DLL......\n");
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, 0, NULL);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}