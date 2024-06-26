﻿
// TestUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "TestUI.h"
#include "TestUIDlg.h"
#include "afxdialogex.h"

#include "ParsePacket.h"

#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "../RemoteInjectTool/RemoteInjectTool.h"

#include "CChooseProcess.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestUIDlg 对话框



CTestUIDlg::CTestUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTUI_DIALOG, pParent)
{
	// 开启控制台窗口
	AllocConsole();
	FILE* stream = nullptr;
	freopen_s(&stream, "CONOUT$", "w", stdout);


	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// ******************************创建管道线程**********************************
	
}

void CTestUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// 解决回车键 ESC 默认关闭窗口
BOOL CTestUIDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);

}

BEGIN_MESSAGE_MAP(CTestUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CTestUIDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CTestUIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestUIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestUIDlg::OnBnClickedButton4)

	// 关联消息处理函数

	// 选择了进程ID之后，CChooseProcess窗口发送消息WM_GET_CHOOSE_PROCESS_ID，收到此消息后，执行OnGetChooseProcessId
	ON_MESSAGE(WM_GET_CHOOSE_PROCESS_ID, OnGetChooseProcessId)			
END_MESSAGE_MAP()


// CTestUIDlg 消息处理程序

BOOL CTestUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestUIDlg::OnPaint()
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
HCURSOR CTestUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// 接收数据的线程函数
DWORD WINAPI ThreadFunc_DataPipeRecv() {
	PBYTE pBuffer = new BYTE[DATA_PIPE_BUF_SIZE];
	DWORD dwReturn = 0;

	HANDLE hDataPipe = ((CTestUIDlg*)(theApp.m_pMainWnd))->m_hDataPipe;
	while (ReadFile(hDataPipe, pBuffer, DATA_PIPE_BUF_SIZE, &dwReturn, NULL)) {
		pBuffer[dwReturn] = '\0';
		
		ParsePacket(pBuffer, dwReturn);
	}
	return 0;
}


HANDLE CreatePipeAndWaitConnect(LPSTR szPipeName, DWORD dwBufLen) {
	// 创建命名管道
	HANDLE hPipe = CreateNamedPipeA(
		szPipeName,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE |
		PIPE_READMODE_MESSAGE |
		PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		dwBufLen,
		dwBufLen,
		0,
		NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("%s管道创建失败\n", szPipeName);
		return NULL;
	}

	// 等待客户端的连接(是阻塞的)
	if (!ConnectNamedPipe(hPipe, NULL))
	{
		printf("%s管道等待连接失败\n", szPipeName);
		return NULL;
	}

	printf("%s管道等待连接成功\n", szPipeName);
	return hPipe;
}



// 远程注入(需要保证InjectDll.dll和本程序在同一路径下)
void CTestUIDlg::OnBnClickedButton1()
{
	CHAR szDllPath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szDllPath, MAX_PATH);		// 获取本程序所在路径
	(strrchr(szDllPath, '\\'))[1] = 0;					// 路径中去掉本程序名称
	strcat_s(szDllPath, "InjectDll.dll");			    // 拼接上DLL的名称

	m_bInjectSuccess = RemoteInjectByProcessId(m_CurrentChooseProcId, szDllPath);
	if (!m_bInjectSuccess) {
		printf("向进程[PID=%.8X]注入失败\n", m_CurrentChooseProcId);
		return;
	}

	// 创建两个命令管道。
	// CommandPipe命令管道：本程序写，DLL读
	// DataPipe数据管道：DLL写，本程序读
	m_hCommandPipe = CreatePipeAndWaitConnect(COMMAND_PIPE, COMMAND_PIPE_BUF_SIZE);
	m_hDataPipe = CreatePipeAndWaitConnect(DATA_PIPE, DATA_PIPE_BUF_SIZE);

	if (m_hCommandPipe == NULL || m_hDataPipe == NULL)
		return ;

	// 接收数据的线程
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc_DataPipeRecv, NULL, 0, NULL);
}


// 安装HOOK
void CTestUIDlg::OnBnClickedButton2()
{
	DWORD dwReturn = 0;
	char szBuffer[] = "InstallHook";

	if (!m_bInjectSuccess) {
		printf("DLL未成功注入，无法安装HOOK\n");
		return;
	}

	// 向客户端发送数据
	if (!WriteFile(m_hCommandPipe, szBuffer, strlen(szBuffer), &dwReturn, NULL))
	{
		printf("向CommandPipe管道写入数据失败\n");
	}
}


// 卸载HOOK
void CTestUIDlg::OnBnClickedButton3()
{
	DWORD dwReturn = 0;
	char szBuffer[] = "UninstallHook";

	if (!m_bInjectSuccess) {
		printf("DLL未成功注入，无法卸载HOOK\n");
		return;
	}

	// 向客户端发送数据
	if (!WriteFile(m_hCommandPipe, szBuffer, strlen(szBuffer), &dwReturn, NULL))
	{
		printf("向CommandPipe管道写入数据失败\n");
	}
}

void CTestUIDlg::OnBnClickedButton4()
{
	CChooseProcess dlgChooseProcess;
	dlgChooseProcess.DoModal();
}


// 自定义消息处理函数，在CChooseProcess窗口中选择了某个进程之后自动执行
LRESULT CTestUIDlg::OnGetChooseProcessId(WPARAM w, LPARAM l)
{
	DWORD dwPID = (DWORD)w;
	CString* pcsProcName = (CString*)l;

	m_CurrentChooseProcId = dwPID;
	m_bInjectSuccess = FALSE;
	m_hCommandPipe = NULL;
	m_hDataPipe = NULL;	

	CString csText;
	csText.Format(_T("当前选择进程为[PID=%.8X] %s"), dwPID, *pcsProcName);

	SetDlgItemText(IDC_STATIC, csText);

	delete pcsProcName;
	return 0;
}
