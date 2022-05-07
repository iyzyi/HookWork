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

		CString csType;
		switch (data.dwType) {
		case REG_BINARY: {
			csType = _T("REG_BINARY");
			break;
		} 
		case REG_DWORD: {
			csType = _T("REG_DWORD");
			break;
		}
		//case REG_DWORD_LITTLE_ENDIAN: {
		//	csType = _T("REG_DWORD_LITTLE_ENDIAN");
		//	break;
		//}
		case REG_DWORD_BIG_ENDIAN: {
			csType = _T("REG_DWORD_BIG_ENDIAN");
			break;
		}
		case REG_EXPAND_SZ: {
			csType = _T("REG_EXPAND_SZ");
			break;
		}
		case REG_LINK: {
			csType = _T("REG_LINK");
			break;
		}
		case REG_MULTI_SZ: {
			csType = _T("REG_MULTI_SZ");
			break;
		}
		case REG_NONE: {
			csType = _T("REG_NONE");
			break;
		}
		case REG_QWORD: {
			csType = _T("REG_QWORD");
			break;
		}
		//case REG_QWORD_LITTLE_ENDIAN: {
		//	csType = _T("");
		//	break;
		//}
		case REG_SZ: {
			csType = _T("REG_SZ");
			break;
		}
		default:
			csType = _T("???");
			break;
		}

		CString csDetailInfo;
		csDetailInfo.Format(_T("ValueName: %ls  Type: %ls "), (PWCHAR)csValueName.GetBuffer(), csType.GetBuffer());

		pListData->AddRow(pMainDlg->m_dwIndex, _T("RegSetValueExW"), hFile, csPath, dwResult, csDetailInfo);
		pMainDlg->m_dwIndex++;
		break;
	}

	default:
		break;
	}
}