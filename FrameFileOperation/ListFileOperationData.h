#pragma once
#include "Misc.h"
#include "FrameFileOperationDlg.h"
#include <afxstr.h>
#pragma warning(disable:4996)


#define MAX_PACKET_NUM		0xfffff			// Ϊ����߲���Ч�ʣ��Զ�����������������ݰ������ܵ��̶�����������Ĵ�С������


class CFrameFileOperationDlg;		// ��ӵ㣬CFrameNetworkDlg��CListNetworkData���������������ǰ����


struct _ListFileOperationRowData {
	CString			csIndex;
	CString			csFuncName;
	CString			csFileHandle;
	CString			csName;
	CString			csPath;

	DWORD			dwIndex;
	DWORD			dwFileHandle;

	_ListFileOperationRowData* pNext = NULL;

	_ListFileOperationRowData(DWORD dwIndex, CString csFuncName, DWORD dwFileHandle, CString csPath);
};



// ���ݶ��ڱ����ListCtrlֻ������ơ�
// ֻ������CListData�ͻ���CListCtrl���룬����ʵ��ListCtrl��ɸѡ�����ٻ��ơ�
class CListFileOperationData {
private:
	CFrameFileOperationDlg* m_pMainDlg = NULL;
	CListCtrl* m_pListCtrl = NULL;

	_ListFileOperationRowData* pFirst = NULL;
	_ListFileOperationRowData* pLast = NULL;
//public:
//	_ListFileOperationRowData** m_pRowDataIndexTable = NULL;

public:
	CListFileOperationData(CFrameFileOperationDlg* pMainDlg);
	~CListFileOperationData();

	// ���һ�����ݣ�������ListCtrl����ӣ������������Զ����CListData������ӣ����û�й�����������ͬʱ���ƴ��е�ListCtrl��
	void AddRow(DWORD dwIndex, CString csFuncName, DWORD dwFileHandle, CString csPath);

	// ��ListCtrl�л���һ������
	void DisplayRow(_ListFileOperationRowData* pListFileOperationRowData);

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

//// ����ListCtrl����Ļص�����
//int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lpSortData);
//
