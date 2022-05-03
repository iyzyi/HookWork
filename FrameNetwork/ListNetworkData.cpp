#include "pch.h"
#include "ListNetworkData.h"


_ListNetworkRowData::_ListNetworkRowData(DWORD dwIndex, CString csFuncName, SOCKET SocketId, DWORD dwIP, WORD wPort, DWORD dwLen, PBYTE pbData) {
	this->dwIndex = dwIndex;
	this->socket = SocketId;
	this->dwIP = dwIP;
	this->wPort = wPort;
	this->dwLen = dwLen;
	this->dwDataBufLen = dwDataBufLen;

	csIndex.Format(_T("%d"), dwIndex);
	this->csFuncName = csFuncName;
	csSocket.Format(_T("%lld"), SocketId);
	csPort.Format(_T("%d"), wPort);
	csLen.Format(_T("%d"), dwLen);

	SOCKADDR_IN mys;
	mys.sin_addr.S_un.S_addr = dwIP;

	char* pszIP = inet_ntoa(mys.sin_addr);
	csIP = CString(pszIP);

	this->pbData = new BYTE[dwLen + 16];
	memcpy(this->pbData, pbData, dwLen);

	csData = ByteArray2HexCString(pbData, dwLen);

	pNext = NULL;
}





CListNetworkData::CListNetworkData(CFrameNetworkDlg* pMainDlg) {
	m_pMainDlg = pMainDlg;
	m_pListCtrl = &(m_pMainDlg->m_List);
}

CListNetworkData::~CListNetworkData() {
	if (m_pMainDlg != NULL) {
		_ListNetworkRowData* pData = pFirst;
		_ListNetworkRowData* pTemp = NULL;
		if (pFirst != NULL) {
			while (true) {
				pTemp = pData->pNext;

				if (pData->pbData != NULL) {
					delete pData->pbData;
				}

				delete pData;

				if (pTemp == NULL)
					break;
				else
					pData = pTemp;
			}
		}
	}
}


void CListNetworkData::AddRow(DWORD dwIndex, CString csFuncName, SOCKET SocketId, DWORD dwIP, WORD wPort, DWORD dwLen, PBYTE pbData) {
	_ListNetworkRowData* pListNetworkRowData = new _ListNetworkRowData(dwIndex, csFuncName, SocketId, dwIP, wPort, dwLen, pbData);

	if (pFirst == NULL) {
		pFirst = pListNetworkRowData;
		pLast = pFirst;
	}
	else {
		pLast->pNext = pListNetworkRowData;
		pLast = pListNetworkRowData;
	}

	if (!m_pMainDlg->m_bFiltering) {
		DisplayRow(pListNetworkRowData);
	}
	else {
		;
	}
}


// ��ListCtrl�л���һ������
void CListNetworkData::DisplayRow(_ListNetworkRowData* pListNetworkRowData) {
	DWORD dwInsertIndex = m_pListCtrl->GetItemCount();

	LV_ITEM   lvitemData = { 0 };
	lvitemData.mask = LVIF_PARAM;
	lvitemData.iItem = dwInsertIndex;
	lvitemData.lParam = (LPARAM)(NULL);	// �����ָ��������ڱ���������Ϣ

	m_pListCtrl->InsertItem(&lvitemData);

	m_pListCtrl->SetItemText(dwInsertIndex, 0, pListNetworkRowData->csIndex);
	m_pListCtrl->SetItemText(dwInsertIndex, 1, pListNetworkRowData->csFuncName);
	m_pListCtrl->SetItemText(dwInsertIndex, 2, pListNetworkRowData->csSocket);
	m_pListCtrl->SetItemText(dwInsertIndex, 3, pListNetworkRowData->csIP);
	m_pListCtrl->SetItemText(dwInsertIndex, 4, pListNetworkRowData->csPort);
	m_pListCtrl->SetItemText(dwInsertIndex, 5, pListNetworkRowData->csLen);
	m_pListCtrl->SetItemText(dwInsertIndex, 6, pListNetworkRowData->csData);
}

//
//void CListData::DisplayData() {
//	//������̽�ֹˢ�½���
//	pListCtrl->LockWindowUpdate();
//	pListCtrl->SetRedraw(FALSE);//����ʱ��������˱�ѡ��״̬�ͻ������ػ�
//
//	// ɾ��֮ǰ��ListCtrl�е�����
//	pListCtrl->DeleteAllItems();
//
//	if (pFirst != NULL) {
//		_ListRowData* pData = pFirst;
//		while (true) {
//
//			DWORD dwInsertIndex = pListCtrl->GetItemCount();
//
//			LV_ITEM   lvitemData = { 0 };
//			lvitemData.mask = LVIF_PARAM;
//			lvitemData.iItem = dwInsertIndex;
//			lvitemData.lParam = (LPARAM)(NULL);	// �����ָ��������ڱ���������Ϣ
//
//			pListCtrl->InsertItem(&lvitemData);
//
//			pListCtrl->SetItemText(dwInsertIndex, 0, pData->csPID);
//			pListCtrl->SetItemText(dwInsertIndex, 1, pData->csProcName);
//			pListCtrl->SetItemText(dwInsertIndex, 2, pData->csProcWinText);
//			pListCtrl->SetItemText(dwInsertIndex, 3, pData->csProcPath);
//			pListCtrl->SetItemText(dwInsertIndex, 4, pData->csProcCmdLine);
//
//			if (pData->pNext == NULL)
//				break;
//			else
//				pData = pData->pNext;
//
//		}
//	}
//
//	// �����ǰ���ù�����ʽ������֮ǰ�����ý�������
//	int iCol = CheckSortColNum();
//	if (iCol >= 0) {
//		SortDataByCol(iCol);
//	}
//
//	// �������������ݺ�Ϳ���ˢ�½�����
//	pListCtrl->SetRedraw(TRUE);
//	pListCtrl->UnlockWindowUpdate();
//}