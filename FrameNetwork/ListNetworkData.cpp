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
#ifdef _WIN64
	csSocket.Format(_T("%lld"), SocketId);
#else
	csSocket.Format(_T("%d"), SocketId);
#endif
	csPort.Format(_T("%d"), (DWORD)wPort);
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

	m_pRowDataIndexTable = new _ListNetworkRowData*[MAX_PACKET_NUM];
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

	if (m_pRowDataIndexTable != NULL)
		delete m_pRowDataIndexTable;
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

	m_pRowDataIndexTable[dwIndex] = pListNetworkRowData;		// 将pListNetworkRowData放入索引表，提高查找效率

	// 如果没筛选关键字，则直接绘制此行数据
	if (!m_pMainDlg->m_bFiltering) {
		DisplayRow(pListNetworkRowData);
	}
	else {
		;
	}
}


// 在ListCtrl中绘制一行数据
void CListNetworkData::DisplayRow(_ListNetworkRowData* pListNetworkRowData) {
	DWORD dwInsertIndex = m_pListCtrl->GetItemCount();

	LV_ITEM   lvitemData = { 0 };
	lvitemData.mask = LVIF_PARAM;
	lvitemData.iItem = dwInsertIndex;
	lvitemData.lParam = (LPARAM)(NULL);	// 这里的指针可以用于保存额外的信息

	m_pListCtrl->InsertItem(&lvitemData);

	m_pListCtrl->SetItemText(dwInsertIndex, 0, pListNetworkRowData->csIndex);
	m_pListCtrl->SetItemText(dwInsertIndex, 1, pListNetworkRowData->csFuncName);
	m_pListCtrl->SetItemText(dwInsertIndex, 2, pListNetworkRowData->csSocket);
	m_pListCtrl->SetItemText(dwInsertIndex, 3, pListNetworkRowData->csIP);
	m_pListCtrl->SetItemText(dwInsertIndex, 4, pListNetworkRowData->csPort);
	m_pListCtrl->SetItemText(dwInsertIndex, 5, pListNetworkRowData->csLen);
	m_pListCtrl->SetItemText(dwInsertIndex, 6, pListNetworkRowData->csData);
}