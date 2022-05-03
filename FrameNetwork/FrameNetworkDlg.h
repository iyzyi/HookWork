﻿
// FrameNetworkDlg.h: 头文件
//

#pragma once


// CFrameNetworkDlg 对话框
class CFrameNetworkDlg : public CDialogEx
{
// 构造
public:
	CFrameNetworkDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FRAMENETWORK_DIALOG };
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

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void ChangeWidget(int cx, int cy);

	CListCtrl m_List;
	afx_msg void OnChooseProcessCommand();
};