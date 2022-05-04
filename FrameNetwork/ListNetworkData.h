#pragma once
#include "Misc.h"
#include "FrameNetworkDlg.h"
#pragma warning(disable:4996)


#define MAX_PACKET_NUM		0xfffff			// 为了提高查找效率（自定义索引表），最大数据包数量受到固定的索引数组的大小的限制


class CFrameNetworkDlg;		// 大坑点，CFrameNetworkDlg和CListNetworkData互相包含，必须提前声明


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



// 数据都在本类里，ListCtrl只负责绘制。
// 只有数据CListData和绘制CListCtrl分离，才能实现ListCtrl的筛选数据再绘制。
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

	// 添加一行数据（不是向ListCtrl中添加，而是向我们自定义的CListData类中添加）如果没有过滤条件，则同时绘制此行到ListCtrl中
	void AddRow(DWORD dwIndex, CString csFuncName, SOCKET SocketId, DWORD dwIP, WORD wPort, DWORD dwLen, PBYTE pbData);

	// 在ListCtrl中绘制一行数据
	void DisplayRow(_ListNetworkRowData* pListNetworkRowData);

	//// 向ListCtrl中绘制我们自定义的CListData类中的所有行的数据（本函数不负责排序）
	//void DisplayData();

	//// 无视大小写判断字符串包含
	//BOOL ContainsTextNoCase(CString csText, CString csKeyword);

	//// 向ListCtrl中绘制我们自定义的CListData类中的数据（经过了关键词的过滤）（本函数不负责排序）
	//void DisplayFilterData(CString csFilterKeyword);

	//// 按列排序
	//void SortDataByCol(DWORD dwCol);

	//// 检查此前设置的是按第几列排序（根据表头是否有▲or▼)
	//int CheckSortColNum();

	//// 改变某一列默认的排序方式，并进行排序（▲升序▼降序互转）
	//void ChangeSortType(DWORD dwCol);
};

// 进行ListCtrl排序的回调函数
int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lpSortData);

