#include "pch.h"

#include "../MyMessagePack/MyMessagePack.h"
#include "../InjectDll/EnumFunction.h"
#include "Misc.h"
#include "ListRegOperationData.h"
#include "FrameRegOperationDlg.h"


VOID ParsePacket(CFrameRegOperationDlg* pMainDlg, PBYTE pBuffer, DWORD dwBufferSize) {
	if (pMainDlg == NULL)
		return;
	
	CListRegOperationData* pListData = pMainDlg->m_pListData;
	
	if (dwBufferSize <= 4) {
		printf("接收到长度为%d的无效数据\n", dwBufferSize);
		return;
	}

	DWORD dwFuncId = GetDwordFromBuffer(pBuffer, 0);
	PBYTE pMsgBuffer = pBuffer + 4;
	DWORD dwMsgBufferSize = dwBufferSize - 4;

	switch (dwFuncId)
	{
	//case ID_CreateFileA: 
	//{
	//	_Data_CreateFileA data = MsgUnpack<_Data_CreateFileA>(pMsgBuffer, dwMsgBufferSize);
	//	DWORD dwFileHandle = data.dwFileHandle;
	//	CString csFilePath = CString(data.msgFilePath.ptr);

	//	pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateFileA"), dwFileHandle, csFilePath);
	//	pMainDlg->m_dwIndex++;
	//	break;
	//}

	case ID_RegOpenKeyExW: {
		_Data_RegOpenKeyExW data = MsgUnpack<_Data_RegOpenKeyExW>(pMsgBuffer, dwMsgBufferSize);
		HANDLE hFile = (HANDLE)data.upFileHandle;
		CString csPath = CStringW((PWCHAR)(data.msgPath.ptr));
		DWORD dwResult = data.dwRet;

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegOpenKeyExW"), hFile, csPath, dwResult, L"");
		pMainDlg->m_dwIndex++;
		break;
	}

	default:
		break;
	}
}