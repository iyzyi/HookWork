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
	case ID_RegCreateKeyExW: {
		_Data_RegCreateKeyExW data = MsgUnpack<_Data_RegCreateKeyExW>(pMsgBuffer, dwMsgBufferSize);
		HANDLE hFile = (HANDLE)data.upKeyHandle;
		CString csPath = CStringW((PWCHAR)(data.msgPath.ptr));
		DWORD dwResult = data.dwRet;

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegCreateKeyExW"), hFile, csPath, dwResult, L"");
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_RegOpenKeyExW: {
		_Data_RegOpenKeyExW data = MsgUnpack<_Data_RegOpenKeyExW>(pMsgBuffer, dwMsgBufferSize);
		HANDLE hFile = (HANDLE)data.upKeyHandle;
		CString csPath = CStringW((PWCHAR)(data.msgPath.ptr));
		DWORD dwResult = data.dwRet;

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegOpenKeyExW"), hFile, csPath, dwResult, L"");
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_RegDeleteKeyExW: {
		_Data_RegDeleteKeyExW data = MsgUnpack<_Data_RegDeleteKeyExW>(pMsgBuffer, dwMsgBufferSize);
		HANDLE hFile = (HANDLE)data.upKeyHandle;
		CString csPath = CStringW((PWCHAR)(data.msgPath.ptr));
		DWORD dwResult = data.dwRet;

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegDeleteKeyExW"), hFile, csPath, dwResult, L"");
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_RegCloseKey: {
		_Data_RegCloseKey data = MsgUnpack<_Data_RegCloseKey>(pMsgBuffer, dwMsgBufferSize);
		HANDLE hFile = (HANDLE)data.upKeyHandle;
		CString csPath = CString(data.msgPath.ptr);
		DWORD dwResult = data.dwRet;

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegCloseKey"), hFile, csPath, dwResult, L"");
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_RegSetValueExW: {
		_Data_RegSetValueExW data = MsgUnpack<_Data_RegSetValueExW>(pMsgBuffer, dwMsgBufferSize);
		HANDLE hFile = (HANDLE)data.upKeyHandle;
		CString csPath = CStringW((PWCHAR)data.msgPath.ptr);
		DWORD dwResult = data.dwRet;
		CString csValueName = CStringW((PWCHAR)data.msgValueName.ptr);
		CString csType = GetRegValueType(data.dwType);

		CString csDetailInfo;
		csDetailInfo.Format(_T("ValueName: %ls  Type: %ls "), (PWCHAR)csValueName.GetBuffer(), csType.GetBuffer());

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegSetValueExW"), hFile, csPath, dwResult, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_RegQueryValueExW: {
		_Data_RegQueryValueExW data = MsgUnpack<_Data_RegQueryValueExW>(pMsgBuffer, dwMsgBufferSize);
		HANDLE hFile = (HANDLE)data.upKeyHandle;
		CString csPath = CStringW((PWCHAR)data.msgPath.ptr);
		DWORD dwResult = data.dwRet;
		CString csValueName = CStringW((PWCHAR)data.msgValueName.ptr);
		CString csType = GetRegValueType(data.dwType);

		CString csDetailInfo;
		csDetailInfo.Format(_T("ValueName: %ls  Type: %ls "), (PWCHAR)csValueName.GetBuffer(), csType.GetBuffer());

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegQueryValueExW"), hFile, csPath, dwResult, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_RegGetValueW: {
		_Data_RegGetValueW data = MsgUnpack<_Data_RegGetValueW>(pMsgBuffer, dwMsgBufferSize);
		HANDLE hFile = (HANDLE)data.upKeyHandle;
		CString csPath = CStringW((PWCHAR)data.msgPath.ptr);
		DWORD dwResult = data.dwRet;
		CString csValueName = CStringW((PWCHAR)data.msgValueName.ptr);
		CString csType = GetRegValueType(data.dwType);

		CString csDetailInfo;
		csDetailInfo.Format(_T("ValueName: %ls  Type: %ls "), (PWCHAR)csValueName.GetBuffer(), csType.GetBuffer());

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegGetValueW"), hFile, csPath, dwResult, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}

	default:
		break;
	}
}