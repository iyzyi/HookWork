
// FrameKeyboardMouseDlg.h: 头文件
//

#pragma once


// CFrameKeyboardMouseDlg 对话框
class CFrameKeyboardMouseDlg : public CDialogEx
{
// 构造
public:
	CFrameKeyboardMouseDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FRAMEKEYBOARDMOUSE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List;

	void AddRow(DWORD dwTime, CString csInfo);
	void ChangeLastRow(CString csInfo);

	void ShowInfo(PCHAR fmt, ...);
	void ShowInfo(PWCHAR fmt, ...);

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnClose();
};