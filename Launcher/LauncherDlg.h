
// LauncherDlg.h: 头文件
//

#pragma once


// CLauncherDlg 对话框
class CLauncherDlg : public CDialogEx
{
// 构造
public:
	CLauncherDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAUNCHER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CHAR m_szCurrentDir[MAX_PATH];



	afx_msg void OnBnClickedButton1();


	void ShowInfo(PCHAR fmt, ...);
	void ShowInfo(PWCHAR fmt, ...);
};
