#include "pch.h"
#include "ListFileOperationData.h"


_ListFileOperationRowData::_ListFileOperationRowData(DWORD dwIndex, CString csFuncName, HANDLE hFile, CString csName, CString csPath) {
	this->dwIndex = dwIndex;
	this->csFuncName = csFuncName;
	this->hFile = hFile;
	this->csName = csName;
	this->csPath = csPath;

	this->csIndex.Format(_T("%d"), dwIndex);
	this->csFileHandle.Format(_T("%d"), hFile);

	pNext = NULL;
}



CListFileOperationData::CListFileOperationData(CFrameFileOperationDlg* pMainDlg) {
	m_pMainDlg = pMainDlg;
	m_pListCtrl = &(m_pMainDlg->m_List);

	//m_pRowDataIndexTable = new _ListNetworkRowData * [MAX_PACKET_NUM];
}


CListFileOperationData::~CListFileOperationData() {
	if (m_pMainDlg != NULL) {
		_ListFileOperationRowData* pData = pFirst;
		_ListFileOperationRowData* pTemp = NULL;
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


void CListFileOperationData::AddRow(DWORD dwIndex, CString csFuncName, HANDLE hFile, CString csName, CString csPath) {
	_ListFileOperationRowData* pListFileOperationRowData = new _ListFileOperationRowData(dwIndex, csFuncName, hFile, csName, csPath);

	if (pFirst == NULL) {
		pFirst = pListFileOperationRowData;
		pLast = pFirst;
	}
	else {
		pLast->pNext = pListFileOperationRowData;
		pLast = pListFileOperationRowData;
	}

	//m_pRowDataIndexTable[dwIndex] = pListNetworkRowData;		// 将pListNetworkRowData放入索引表，提高查找效率

	// 如果没筛选关键字，则直接绘制此行数据
	if (!m_pMainDlg->m_bFiltering) {
		DisplayRow(pListFileOperationRowData);
	}
	else {
		;
	}
}


// 在ListCtrl中绘制一行数据
void CListFileOperationData::DisplayRow(_ListFileOperationRowData* pListFileOperationRowData) {
	DWORD dwInsertIndex = m_pListCtrl->GetItemCount();

	LV_ITEM   lvitemData = { 0 };
	lvitemData.mask = LVIF_PARAM;
	lvitemData.iItem = dwInsertIndex;
	lvitemData.lParam = (LPARAM)(NULL);	// 这里的指针可以用于保存额外的信息

	m_pListCtrl->InsertItem(&lvitemData);

	m_pListCtrl->SetItemText(dwInsertIndex, 0, pListFileOperationRowData->csIndex);
	m_pListCtrl->SetItemText(dwInsertIndex, 1, pListFileOperationRowData->csFuncName);
	m_pListCtrl->SetItemText(dwInsertIndex, 2, pListFileOperationRowData->csFileHandle);
	m_pListCtrl->SetItemText(dwInsertIndex, 3, pListFileOperationRowData->csName);
	m_pListCtrl->SetItemText(dwInsertIndex, 4, pListFileOperationRowData->csPath);
}