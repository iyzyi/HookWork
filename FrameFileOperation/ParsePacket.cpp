#include "pch.h"

#include "../MyMessagePack/MyMessagePack.h"
#include "../InjectDll/EnumFunction.h"
#include "Misc.h"
//#include "ListNetworkData.h"
#include "FrameFileOperationDlg.h"


VOID ParsePacket(CFrameFileOperationDlg* pMainDlg, PBYTE pBuffer, DWORD dwBufferSize) {
	if (pMainDlg == NULL)
		return;
	
	//CListNetworkData* pListData = pMainDlg->m_pListData;
	
	if (dwBufferSize <= 4) {
		printf("接收到长度为%d的无效数据\n", dwBufferSize);
		return;
	}

	DWORD dwFuncId = GetDwordFromBuffer(pBuffer, 0);
	PBYTE pMsgBuffer = pBuffer + 4;
	DWORD dwMsgBufferSize = dwBufferSize - 4;

	switch (dwFuncId)
	{

	case ID_CreateFileW:
	{
		_Data_CreateFileW data = MsgUnpack<_Data_CreateFileW>(pMsgBuffer, dwMsgBufferSize);
		
		WCHAR wszFileName[MAX_PATH] = { 0 };
		wcscpy_s(wszFileName, (PWCHAR)data.msgFilePath.ptr);

		//wprintf(L"CreateFile %s\n", wszFileName);

		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_ReadFile:
	{
		_Data_ReadFile data = MsgUnpack<_Data_ReadFile>(pMsgBuffer, dwMsgBufferSize);

		DWORD dwFileHandle = data.dwFileHandle;

		CHAR szFileName[MAX_PATH] = { 0 };
		strcpy_s(szFileName, MAX_PATH, (PCHAR)data.msgFilePath.ptr);

		printf("ReadFile: %s\n", szFileName);

		pMainDlg->m_dwIndex++;
		break;
	}

	default:
		break;
	}
}