#pragma once

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


public:
	BOOL ListProcess();
	afx_msg void OnEnChangeEdit1();
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLvnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
};


