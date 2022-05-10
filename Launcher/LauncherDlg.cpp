
// LauncherDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Launcher.h"
#include "LauncherDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning (disable: 4996)

// CLauncherDlg 对话框



CLauncherDlg::CLauncherDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LAUNCHER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLauncherDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CLauncherDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CLauncherDlg 消息处理程序

BOOL CLauncherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

		
	GetModuleFileNameA(NULL, m_szCurrentDir, MAX_PATH);		// 获取本程序所在路径
	(strrchr(m_szCurrentDir, '\\'))[1] = 0;					// 路径中去掉本程序名称


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLauncherDlg::OnPaint()
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
HCURSOR CLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



bool isFileExists(char* name) {
	//struct stat buffer;
	//return (stat(name, &buffer) == 0);
	return PathFileExistsA(name);
}



// 在消息栏显示信息
void CLauncherDlg::ShowInfo(PCHAR fmt, ...)
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
void CLauncherDlg::ShowInfo(PWCHAR fmt, ...)
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


// 网络通信分析框架
void CLauncherDlg::OnBnClickedButton1()
{
	CHAR szRunDllPath[MAX_PATH];
	sprintf_s(szRunDllPath, "%s%s", m_szCurrentDir, "BypassUAC.dll");

	if (!isFileExists(szRunDllPath)) {
		ShowInfo("[ERROR] %s 不存在\n", szRunDllPath);
		return ;
	}

	CHAR szRundll32Path[MAX_PATH] = "C:\\Windows\\System32\\rundll32.exe";

	CHAR szCmdLine[1024];
	sprintf_s(szCmdLine, "%s \"%s\" %s \"%s%s\"", szRundll32Path, szRunDllPath, "ElevatePrivilegesWinExec", m_szCurrentDir, "FrameNetwork.exe");
	
	WinExec(szCmdLine, SW_HIDE);
}
