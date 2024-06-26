﻿#include "pch.h"

#include "../MyMessagePack/MyMessagePack.h"
#include "../InjectDll/EnumFunction.h"
#include "Misc.h"
#include "ListProcessThreadData.h"
#include "FrameProcessThreadDlg.h"


VOID ParsePacket(CFrameProcessThreadDlg* pMainDlg, PBYTE pBuffer, DWORD dwBufferSize) {
	if (pMainDlg == NULL)
		return;

	CListProcessThreadData* pListData = pMainDlg->m_pListData;
	
	if (dwBufferSize <= 4) {
		printf("接收到长度为%d的无效数据\n", dwBufferSize);
		return;
	}

	DWORD dwFuncId = GetDwordFromBuffer(pBuffer, 0);
	PBYTE pMsgBuffer = pBuffer + 4;
	DWORD dwMsgBufferSize = dwBufferSize - 4;

	switch (dwFuncId)
	{
	case ID_CreateProcessW: {
		_Data_CreateProcessW data = MsgUnpack<_Data_CreateProcessW>(pMsgBuffer, dwMsgBufferSize);
		CString csDetailInfo;
		csDetailInfo.Format(_T("模块名称:   %ls\r\n命令行:     %ls\r\n当前目录:   %ls"), (PWCHAR)data.msgAppName.ptr, (PWCHAR)data.msgCmdLine.ptr, (PWCHAR)data.msgCurrentDir.ptr);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateProcessW"), data.dwErrorCode, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}


	case ID_CreateThread: {
		_Data_CreateThread data = MsgUnpack<_Data_CreateThread>(pMsgBuffer, dwMsgBufferSize);

		CString csDetailInfo;
		csDetailInfo.Format(_T("线程句柄:            0x%p\r\n线程标识符:          0x%p\r\n线程函数地址:        0x%p\r\n线程函数参数地址:    0x%p\r\n"), data.upThreadHandle, data.upThreadId, data.upStartAddress, data.upParameterAddress);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateThread"), data.dwErrorCode, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_CreateRemoteThread: {
		_Data_CreateRemoteThread data = MsgUnpack<_Data_CreateRemoteThread>(pMsgBuffer, dwMsgBufferSize);

		CString csDetailInfo;
		csDetailInfo.Format(_T("进程句柄:            0x%p\r\n线程句柄:            0x%p\r\n线程标识符:          0x%p\r\n线程函数地址:        0x%p\r\n线程函数参数地址:    0x%p\r\n"), data.upProcessHandle, data.upThreadHandle, data.upThreadId, data.upStartAddress, data.upParameterAddress);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateRemoteThread"), data.dwErrorCode, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_CreateRemoteThreadEx: {
		_Data_CreateRemoteThreadEx data = MsgUnpack<_Data_CreateRemoteThreadEx>(pMsgBuffer, dwMsgBufferSize);

		CString csDetailInfo;
		csDetailInfo.Format(_T("进程句柄:            0x%p\r\n线程句柄:            0x%p\r\n线程标识符:          0x%p\r\n线程函数地址:        0x%p\r\n线程函数参数地址:    0x%p\r\n"), data.upProcessHandle, data.upThreadHandle, data.upThreadId, data.upStartAddress, data.upParameterAddress);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("CreateRemoteThreadEx"), data.dwErrorCode, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}



	case ID_ExitProcess: {
		_Data_ExitProcess data = MsgUnpack<_Data_ExitProcess>(pMsgBuffer, dwMsgBufferSize);

		CString csDetailInfo;
		csDetailInfo.Format(_T("进程退出代码: 0x%p\r\n"), data.upExitCode);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("ExitProcess"), NULL, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_ExitThread: {
		_Data_ExitThread data = MsgUnpack<_Data_ExitThread>(pMsgBuffer, dwMsgBufferSize);

		CString csDetailInfo;
		csDetailInfo.Format(_T("进程退出代码: 0x%p\r\n"), data.upExitCode);

		pListData->AddRow(pMainDlg->m_dwIndex, _T("ExitThread"), NULL, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}

	default:
		break;
	}
}