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
	CListCtrl m_List;


public:
	BOOL ListProcess();
	afx_msg void OnEnChangeEdit1();
	CEdit m_EditTextBox;
	afx_msg void OnBnClickedButton1();
};
