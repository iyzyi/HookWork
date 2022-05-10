
// FrameKeyboardMouseDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FrameKeyboardMouse.h"
#include "FrameKeyboardMouseDlg.h"
#include "afxdialogex.h"
#include "../RemoteInjectTool/RemoteInjectTool.h"

#include "Function.h"
#include <csignal>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable: 4996)

// CFrameKeyboardMouseDlg 对话框



CFrameKeyboardMouseDlg::CFrameKeyboardMouseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FRAMEKEYBOARDMOUSE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFrameKeyboardMouseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}

BEGIN_MESSAGE_MAP(CFrameKeyboardMouseDlg, CDialogEx)
	ON_WM_PAINT() 
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFrameKeyboardMouseDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFrameKeyboardMouseDlg::OnBnClickedButton2)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// 解决回车键 ESC 默认关闭窗口
BOOL CFrameKeyboardMouseDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);

}


// CFrameKeyboardMouseDlg 消息处理程序

BOOL CFrameKeyboardMouseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	CString csOldTitle;
	GetWindowText(csOldTitle);
	CString csNewTitle;
#ifdef _WIN64
	if (IsAdministrator()) {
		csNewTitle = _T("[管理员|x64] ");
		csNewTitle += csOldTitle;
	}
	else {
		csNewTitle = _T("[x64] ");
		csNewTitle += csOldTitle;
	}
#else
	if (IsAdministrator()) {
		csNewTitle = _T("[管理员|x86] ");
		csNewTitle += csOldTitle;
	}
	else {
		csNewTitle = _T("[x86] ");
		csNewTitle += csOldTitle;
	}
#endif
	SetWindowText(csNewTitle);
	

	// 以下是List Control
	//标题所需字段
	CString head[] = { TEXT("时间"), TEXT("消息")};

	//插入列标题
	m_List.InsertColumn(0, head[0], LVCFMT_LEFT, 200);			// 仅用于创建本行，长度设为0，不在图像界面的列表中显示
	m_List.InsertColumn(1, head[1], LVCFMT_LEFT, 600);

	//设置风格样式
	//LVS_EX_GRIDLINES 网格
	//LVS_EX_FULLROWSELECT 选中整行
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()
		| LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 以上是List Control


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFrameKeyboardMouseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFrameKeyboardMouseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFrameKeyboardMouseDlg::AddRow(DWORD dwTime, CString csInfo) {
	DWORD dwInsertIndex = m_List.GetItemCount();

	LV_ITEM   lvitemData = { 0 };
	lvitemData.mask = LVIF_PARAM;
	lvitemData.iItem = dwInsertIndex;
	lvitemData.lParam = (LPARAM)(NULL);	// 这里的指针可以用于保存额外的信息

	m_List.InsertItem(&lvitemData);

	CString csTime;
	csTime.Format(_T("%u"), dwTime);

	m_List.SetItemText(dwInsertIndex, 0, csTime);
	m_List.SetItemText(dwInsertIndex, 1, csInfo);
}


// 修改最后一行
void CFrameKeyboardMouseDlg::ChangeLastRow(CString csInfo) {
	DWORD dwLastIndex = m_List.GetItemCount() - 1;

	m_List.SetItemText(dwLastIndex, 1, csInfo);
}



// 在消息栏显示信息
void CFrameKeyboardMouseDlg::ShowInfo(PCHAR fmt, ...)
{
	//不定参数格式化
	va_list argptr;
	va_start(argptr, fmt);
	char buffer[4096] = { 0 };
	int cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	SetDlgItemTextA(theApp.m_pMainWnd->m_hWnd, IDC_STATIC_TEXT_INFO, buffer);

	return;
}


// 在消息栏显示信息
void CFrameKeyboardMouseDlg::ShowInfo(PWCHAR fmt, ...)
{
	//不定参数格式化
	va_list argptr;
	va_start(argptr, fmt);
	WCHAR buffer[4096] = { 0 };
	int cnt = vswprintf(buffer, fmt, argptr);
	va_end(argptr);

	SetDlgItemTextW(IDC_STATIC_TEXT_INFO, buffer);

	return;
}



void CFrameKeyboardMouseDlg::OnBnClickedButton1()
{
	CreateHookThread();
}


void CFrameKeyboardMouseDlg::OnBnClickedButton2()
{
	RemoveHookThread(SIGINT);
}


void CFrameKeyboardMouseDlg::OnClose()
{
	RemoveHookThread(SIGINT);

	CDialogEx::OnClose();
}
