#include "pch.h"
#include "ListRegOperationData.h"


_ListRegOperationRowData::_ListRegOperationRowData(DWORD dwIndex, CString csFuncName, HANDLE hFile, CString csPath, DWORD dwResult, CString csDetailInfo) {
	this->dwIndex = dwIndex;
	this->csFuncName = csFuncName;
	this->hFile = hFile;
	this->csPath = csPath;
	this->dwResult = dwResult;
	this->csDetailInfo = csDetailInfo;

	this->csIndex.Format(_T("%d"), dwIndex);
	if (dwResult == ERROR_SUCCESS)
		this->csFileHandle.Format(_T("0x%.8X"), hFile);
	else
		this->csFileHandle = CString(_T(""));
	
	_com_error error(dwResult);
	LPCTSTR errorText = error.ErrorMessage();
	csResult = CString(errorText);

	pNext = NULL;
}



CListRegOperationData::CListRegOperationData(CFrameRegOperationDlg* pMainDlg) {
	m_pMainDlg = pMainDlg;
	m_pListCtrl = &(m_pMainDlg->m_List);

	//m_pRowDataIndexTable = new _ListNetworkRowData * [MAX_PACKET_NUM];
}


CListRegOperationData::~CListRegOperationData() {
	if (m_pMainDlg != NULL) {
		_ListRegOperationRowData* pData = pFirst;
		_ListRegOperationRowData* pTemp = NULL;
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

	//if (m_pRowDataIndexTable != NULL)
	//	delete m_pRowDataIndexTable;
}


void CListRegOperationData::AddRow(DWORD dwIndex, CString csFuncName, HANDLE hFile, CString csPath, DWORD dwResult, CString csDetailInfo) {
	_ListRegOperationRowData* pListRegOperationRowData = new _ListRegOperationRowData(dwIndex, csFuncName, hFile, csPath, dwResult, csDetailInfo);

	if (pFirst == NULL) {
		pFirst = pListRegOperationRowData;
		pLast = pFirst;
	}
	else {
		pLast->pNext = pListRegOperationRowData;
		pLast = pListRegOperationRowData;
	}

	//m_pRowDataIndexTable[dwIndex] = pListNetworkRowData;		// 将pListNetworkRowData放入索引表，提高查找效率

	// 如果没筛选关键字，则直接绘制此行数据
	if (!m_pMainDlg->m_bFiltering) {
		DisplayRow(pListRegOperationRowData);
	}
	else {
		;
	}
}


// 在ListCtrl中绘制一行数据
void CListRegOperationData::DisplayRow(_ListRegOperationRowData* pListRegOperationRowData) {
	DWORD dwInsertIndex = m_pListCtrl->GetItemCount();

	LV_ITEM   lvitemData = { 0 };
	lvitemData.mask = LVIF_PARAM;
	lvitemData.iItem = dwInsertIndex;
	lvitemData.lParam = (LPARAM)(NULL);	// 这里的指针可以用于保存额外的信息

	m_pListCtrl->InsertItem(&lvitemData);

	m_pListCtrl->SetItemText(dwInsertIndex, 0, pListRegOperationRowData->csIndex);
	m_pListCtrl->SetItemText(dwInsertIndex, 1, pListRegOperationRowData->csFuncName);
	m_pListCtrl->SetItemText(dwInsertIndex, 2, pListRegOperationRowData->csFileHandle);
	m_pListCtrl->SetItemText(dwInsertIndex, 3, pListRegOperationRowData->csPath);
	m_pListCtrl->SetItemText(dwInsertIndex, 4, pListRegOperationRowData->csResult);
	m_pListCtrl->SetItemText(dwInsertIndex, 5, pListRegOperationRowData->csDetailInfo);
}