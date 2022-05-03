
// FrameNetworkDlg.h: 头文件
//

#pragma once
#include "ListNetworkData.h"

class CListNetworkData;				// 大坑点，CFrameNetworkDlg和CListNetworkData互相包含，必须提前声明

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
	afx_msg LRESULT OnGetChooseProcessId(WPARAM w, LPARAM l);

public:
	DWORD m_CurrentChooseProcId = NULL;

	HANDLE	m_hCommandPipe = NULL;
	HANDLE	m_hDataPipe = NULL;

	BOOL m_bInjectSuccess = FALSE;

	BOOL m_bFiltering = FALSE;

	CListNetworkData* m_pListData = NULL;

	DWORD m_dwIndex = 0;

public:
	afx_msg void OnBeginWorkCommand();

	void ShowInfo(PCHAR fmt, ...);
	void ShowInfo(PWCHAR fmt, ...);

	BOOL RemoteInject();
	BOOL InstallHook();
	BOOL UninstallHook();


	afx_msg void OnEndWorkCommand();
};


#define COMMAND_PIPE_BUF_SIZE		4096
#define COMMAND_PIPE				"\\\\.\\pipe\\CommandPipe"

#define DATA_PIPE_BUF_SIZE			0xffffff
#define DATA_PIPE					"\\\\.\\pipe\\DataPipe"