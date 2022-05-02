﻿#pragma once

struct _ListRowData {
	CString				csPID;
	CString				csProcName;
	CString				csProcWinText;
	CString				csProcPath;
	CString				csProcCmdLine;
	_ListRowData*		pNext;

	_ListRowData(CString a, CString b, CString c, CString d, CString e) {
		csPID = a;
		csProcName = b;
		csProcWinText = c;
		csProcPath = d;
		csProcCmdLine = e;
		pNext = NULL;
	}
};

class CListData {
private:
	CListCtrl* pListCtrl = NULL;

	_ListRowData* pFirst = NULL;
	_ListRowData* pLast = NULL;

public:
	CListData(CListCtrl* pListCtrl);

	~CListData();


	// 添加一行数据（不是向ListCtrl中添加，而是向我们自定义的CListData类中添加）
	void AddRow(CString* pcsPID, CString* pcsProcName, CString* pcsProcWinText, CString* pcsProcPath, CString* pcsProcCmdLine);


	// 向ListCtrl中绘制我们自定义的CListData类中的所有行的数据（本函数不负责排序）
	void DisplayData();

	// 无视大小写判断字符串包含
	BOOL ContainsTextNoCase(CString csText, CString csKeyword);

	// 向ListCtrl中绘制我们自定义的CListData类中的数据（经过了关键词的过滤）（本函数不负责排序）
	void DisplayFilterData(CString csFilterKeyword);

	void SortDataByCol(DWORD dwCol);
	int CheckSortColNum();
	void ChangeSortType(DWORD dwCol);
};

int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lpSortData);



// CChooseProcess 对话框
class CChooseProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CChooseProcess)

public:
	CChooseProcess(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CChooseProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHOOSE_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	CListCtrl		m_List;

	CEdit			m_EditTextBox;

	CListData*		m_pListData = NULL;


public:
	BOOL ListProcess();
	afx_msg void OnEnChangeEdit1();
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLvnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButton2();
};


