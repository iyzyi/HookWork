#include "pch.h"

#include "../MyMessagePack/MyMessagePack.h"
#include "../InjectDll/EnumFunction.h"
#include "Misc.h"
#include "ListFileOperationData.h"
#include "FrameFileOperationDlg.h"


// CreateFileA, ReadFileEx, WriteFileEx, CreateDirectoryA未经测试


VOID ParsePacket(CFrameFileOperationDlg* pMainDlg, PBYTE pBuffer, DWORD dwBufferSize) {
	if (pMainDlg == NULL)
		return;
	
	CListFileOperationData* pListData = pMainDlg->m_pListData;
	
	if (dwBufferSize <= 4) {
		printf("接收到长度为%d的无效数据\n", dwBufferSize);
		return;
	}

	DWORD dwFuncId = GetDwordFromBuffer(pBuffer, 0);
	PBYTE pMsgBuffer = pBuffer + 4;
	DWORD dwMsgBufferSize = dwBufferSize - 4;

	switch (dwFuncId)
	{
	case ID_CreateFileA: 
	{
		_Data_CreateFileA data = MsgUnpack<_Data_CreateFileA>(pMsgBuffer, dwMsgBufferSize);
		DWORD dwFileHandle = data.dwFileHandle;
		CString csFilePath = CString(data.msgFilePath.ptr);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateFileA"), dwFileHandle, csFilePath);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_CreateFileW:
	{
		_Data_CreateFileW data = MsgUnpack<_Data_CreateFileW>(pMsgBuffer, dwMsgBufferSize);		
		DWORD dwFileHandle = data.dwFileHandle;
		CString csFilePath = CStringW((PWCHAR)(data.msgFilePath.ptr));

		pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateFileW"), dwFileHandle, csFilePath);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_ReadFile:
	{
		_Data_ReadFile data = MsgUnpack<_Data_ReadFile>(pMsgBuffer, dwMsgBufferSize);
		DWORD dwFileHandle = data.dwFileHandle;
		CString csFilePath = CString(data.msgFilePath.ptr);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("ReadFile"), dwFileHandle, csFilePath);
		pMainDlg->m_dwIndex++;
		break;
	}
	
	case ID_ReadFileEx: 
	{
		_Data_ReadFileEx data = MsgUnpack<_Data_ReadFileEx>(pMsgBuffer, dwMsgBufferSize);
		DWORD dwFileHandle = data.dwFileHandle;
		CString csFilePath = CString(data.msgFilePath.ptr);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("ReadFileEx"), dwFileHandle, csFilePath);
		pMainDlg->m_dwIndex++;
		break;
	}


	case ID_WriteFile: 
	{
		_Data_WriteFile data = MsgUnpack<_Data_WriteFile>(pMsgBuffer, dwMsgBufferSize);
		DWORD dwFileHandle = data.dwFileHandle;
		CString csFilePath = CString(data.msgFilePath.ptr);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("WriteFile"), dwFileHandle, csFilePath);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_WriteFileEx:
	{
		_Data_WriteFileEx data = MsgUnpack<_Data_WriteFileEx>(pMsgBuffer, dwMsgBufferSize);
		DWORD dwFileHandle = data.dwFileHandle;
		CString csFilePath = CString(data.msgFilePath.ptr);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("WriteFileEx"), dwFileHandle, csFilePath);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_CreateDirectoryA: 
	{
		_Data_CreateDirectoryA data = MsgUnpack<_Data_CreateDirectoryA>(pMsgBuffer, dwMsgBufferSize);

		CString csPathName = CStringW((PWCHAR)(data.msgPathName.ptr));

		pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateDirectoryA"), NULL, csPathName);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_CreateDirectoryW:
	{
		_Data_CreateDirectoryW data = MsgUnpack<_Data_CreateDirectoryW>(pMsgBuffer, dwMsgBufferSize);

		CString csPathName = CStringW((PWCHAR)(data.msgPathName.ptr));

		pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateDirectoryW"), NULL, csPathName);
		pMainDlg->m_dwIndex++;
		break;
	}

	default:
		break;
	}
}