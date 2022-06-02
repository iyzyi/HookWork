#include "pch.h"
#include "ListProcessThreadData.h"


_ListProcessThreadRowData::_ListProcessThreadRowData(DWORD dwIndex, CString csFuncName, DWORD dwErrorCode, CString csDetailInfo) {
	this->dwIndex = dwIndex;
	this->csFuncName = csFuncName;
	this->csDetailInfo = csDetailInfo;
	this->dwErrorCode = dwErrorCode;

	this->csIndex.Format(_T("%d"), dwIndex);
	
	if (csFuncName == _T("ExitProcess") || csFuncName == _T("ExitThread")) {
		csResult = _T("");
	}
	else {
		_com_error error(dwErrorCode);
		LPCTSTR errorText = error.ErrorMessage();
		csResult = CString(errorText);
	}

	pNext = NULL;
}



CListProcessThreadData::CListProcessThreadData(CFrameProcessThreadDlg* pMainDlg) {
	m_pMainDlg = pMainDlg;
	m_pListCtrl = &(m_pMainDlg->m_List);

	m_pRowDataIndexTable = new _ListProcessThreadRowData * [MAX_PACKET_NUM];
}


CListProcessThreadData::~CListProcessThreadData() {
	if (m_pMainDlg != NULL) {
		_ListProcessThreadRowData* pData = pFirst;
		_ListProcessThreadRowData* pTemp = NULL;
		if (pFirst != NULL) {
			while (true) {
				pTemp = pData->pNext;

				//if (pData->pbData != NULL) {
				//	delete pData->pbData;
				//}

				delete pData;

				if (pTemp == NULL)
					break;
				else
					pData = pTemp;
			}
		}
	}

	if (m_pRowDataIndexTable != NULL)
		delete m_pRowDataIndexTable;
}


void CListProcessThreadData::AddRow(DWORD dwIndex, CString csFuncName, DWORD dwErrorCode, CString csDetailInfo) {
	_ListProcessThreadRowData* pListProcessThreadRowData = new _ListProcessThreadRowData(dwIndex, csFuncName, dwErrorCode, csDetailInfo);

	if (pFirst == NULL) {
		pFirst = pListProcessThreadRowData;
		pLast = pFirst;
	}
	else {
		pLast->pNext = pListProcessThreadRowData;
		pLast = pListProcessThreadRowData;
	}

	m_pRowDataIndexTable[dwIndex] = pListProcessThreadRowData;		// 将pListNetworkRowData放入索引表，提高查找效率

	// 如果没筛选关键字，则直接绘制此行数据
	if (!m_pMainDlg->m_bFiltering) {
		DisplayRow(pListProcessThreadRowData);
	}
	else {
		;
	}
}


// 在ListCtrl中绘制一行数据
void CListProcessThreadData::DisplayRow(_ListProcessThreadRowData* pListProcessThreadRowData) {
	DWORD dwInsertIndex = m_pListCtrl->GetItemCount();

	LV_ITEM   lvitemData = { 0 };
	lvitemData.mask = LVIF_PARAM;
	lvitemData.iItem = dwInsertIndex;
	lvitemData.lParam = (LPARAM)(NULL);// 这里的指针可以用于保存额外的信息

	m_pListCtrl->InsertItem(&lvitemData);

	m_pListCtrl->SetItemText(dwInsertIndex, 0, pListProcessThreadRowData->csIndex);
	m_pListCtrl->SetItemText(dwInsertIndex, 1, pListProcessThreadRowData->csFuncName);
	m_pListCtrl->SetItemText(dwInsertIndex, 2, pListProcessThreadRowData->csResult);
}