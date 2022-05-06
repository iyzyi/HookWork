#include "pch.h"
#include "ListFileOperationData.h"


_ListFileOperationRowData::_ListFileOperationRowData(DWORD dwIndex, CString csFuncName, DWORD dwFileHandle, CString csPath) {
	this->dwIndex = dwIndex;
	this->csFuncName = csFuncName;
	this->dwFileHandle = dwFileHandle;
	this->csPath = csPath;

	this->csIndex.Format(_T("%d"), dwIndex);
	
	// ����Ŀ¼��صĺ���������ʾ�����һ��
	if (this->csFuncName == _T("CreateDirectoryW") || this->csFuncName == _T("CreateDirectoryA")) {
		this->csFileHandle = _T("");
	}
	else {
		this->csFileHandle.Format(_T("%d"), dwFileHandle);
	}

	// ·����ͷ�� \\?\�Ļ���ɾȥ\\?\ 
	if (this->csPath.GetLength() > 4 && this->csPath.Mid(0, 4).Compare(_T("\\\\?\\")) != -1) {		
		this->csPath.Delete(0, 4);
	}

	int nPos = csPath.ReverseFind('\\');
	this->csName = csPath.Right(csPath.GetLength() - nPos - 1);

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


void CListFileOperationData::AddRow(DWORD dwIndex, CString csFuncName, DWORD dwFileHandle, CString csPath) {
	_ListFileOperationRowData* pListFileOperationRowData = new _ListFileOperationRowData(dwIndex, csFuncName, dwFileHandle, csPath);

	if (pFirst == NULL) {
		pFirst = pListFileOperationRowData;
		pLast = pFirst;
	}
	else {
		pLast->pNext = pListFileOperationRowData;
		pLast = pListFileOperationRowData;
	}

	//m_pRowDataIndexTable[dwIndex] = pListNetworkRowData;		// ��pListNetworkRowData������������߲���Ч��

	// ���ûɸѡ�ؼ��֣���ֱ�ӻ��ƴ�������
	if (!m_pMainDlg->m_bFiltering) {

		if (pListFileOperationRowData->csFuncName == _T("CreateFileW") && pListFileOperationRowData->csPath == _T("\\\\.\\MountPointManager")) {
			return;			// CreateFileW \\.\MountPointManager �������̫Ƶ���ˣ�Ӱ�����Ĳ鿴�����������ε��ɡ�
		}
		else {
			DisplayRow(pListFileOperationRowData);
		}
		
	}
	else {
		;
	}
}


// ��ListCtrl�л���һ������
void CListFileOperationData::DisplayRow(_ListFileOperationRowData* pListFileOperationRowData) {
	DWORD dwInsertIndex = m_pListCtrl->GetItemCount();

	LV_ITEM   lvitemData = { 0 };
	lvitemData.mask = LVIF_PARAM;
	lvitemData.iItem = dwInsertIndex;
	lvitemData.lParam = (LPARAM)(NULL);	// �����ָ��������ڱ���������Ϣ

	m_pListCtrl->InsertItem(&lvitemData);

	m_pListCtrl->SetItemText(dwInsertIndex, 0, pListFileOperationRowData->csIndex);
	m_pListCtrl->SetItemText(dwInsertIndex, 1, pListFileOperationRowData->csFuncName);
	m_pListCtrl->SetItemText(dwInsertIndex, 2, pListFileOperationRowData->csFileHandle);
	m_pListCtrl->SetItemText(dwInsertIndex, 3, pListFileOperationRowData->csName);
	m_pListCtrl->SetItemText(dwInsertIndex, 4, pListFileOperationRowData->csPath);
}