#pragma once
#include "Misc.h"
#include "FrameNetworkDlg.h"
#pragma warning(disable:4996)


#define MAX_PACKET_NUM		0xfffff			// Ϊ����߲���Ч�ʣ��Զ�����������������ݰ������ܵ��̶�����������Ĵ�С������


class CFrameNetworkDlg;		// ��ӵ㣬CFrameNetworkDlg��CListNetworkData���������������ǰ����


struct _ListNetworkRowData {
	CString					csIndex;
	CString					csFuncName;
	CString					csSocket;
	CString					csIP;
	CString					csPort;
	CString					csLen;
	CString					csData;
	
	DWORD					dwIndex;
	SOCKET					socket;
	DWORD					dwIP;
	DWORD					wPort;
	DWORD					dwLen;
	PBYTE					pbData = NULL;
	DWORD					dwDataBufLen;

	_ListNetworkRowData*	pNext = NULL;

	_ListNetworkRowData(DWORD dwIndex, CString csFuncName, SOCKET SocketId, DWORD dwIP, WORD wPort, DWORD dwLen, PBYTE pbData);
};



// ���ݶ��ڱ����ListCtrlֻ������ơ�
// ֻ������CListData�ͻ���CListCtrl���룬����ʵ��ListCtrl��ɸѡ�����ٻ��ơ�
class CListNetworkData {
private:
	CFrameNetworkDlg* m_pMainDlg = NULL;
	CListCtrl* m_pListCtrl = NULL;

	_ListNetworkRowData* pFirst = NULL;
	_ListNetworkRowData* pLast = NULL;
public:
	_ListNetworkRowData** m_pRowDataIndexTable = NULL;

public:
	CListNetworkData(CFrameNetworkDlg* pMainDlg);
	~CListNetworkData();

	// ���һ�����ݣ�������ListCtrl����ӣ������������Զ����CListData������ӣ����û�й�����������ͬʱ���ƴ��е�ListCtrl��
	void AddRow(DWORD dwIndex, CString csFuncName, SOCKET SocketId, DWORD dwIP, WORD wPort, DWORD dwLen, PBYTE pbData);

	// ��ListCtrl�л���һ������
	void DisplayRow(_ListNetworkRowData* pListNetworkRowData);

	//// ��ListCtrl�л��������Զ����CListData���е������е����ݣ�����������������
	//void DisplayData();

	//// ���Ӵ�Сд�ж��ַ�������
	//BOOL ContainsTextNoCase(CString csText, CString csKeyword);

	//// ��ListCtrl�л��������Զ����CListData���е����ݣ������˹ؼ��ʵĹ��ˣ�������������������
	//void DisplayFilterData(CString csFilterKeyword);

	//// ��������
	//void SortDataByCol(DWORD dwCol);

	//// ����ǰ���õ��ǰ��ڼ������򣨸��ݱ�ͷ�Ƿ��С�or��)
	//int CheckSortColNum();

	//// �ı�ĳһ��Ĭ�ϵ�����ʽ�����������򣨡����򨋽���ת��
	//void ChangeSortType(DWORD dwCol);
};

// ����ListCtrl����Ļص�����
int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lpSortData);

