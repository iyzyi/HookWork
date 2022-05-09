
// FrameProcessThreadDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FrameProcessThread.h"
#include "FrameProcessThreadDlg.h"
#include "afxdialogex.h"

#include "../SimpleHookEngine/SimpleHookEngine.h"
#include "../RemoteInjectTool/RemoteInjectTool.h"
#include "../InjectDll/EnumFunction.h"
#include "ParsePacket.h"
#include "ChooseProcessDlg.h"
#include "Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4996)



// 只HOOK以下函数
DWORD FuncList[] = {
	ID_CreateProcessA,
	ID_CreateProcessW,
	ID_CreateProcessAsUserA,
	ID_CreateProcessAsUserW,
	ID_CreateThread,
	ID_CreateRemoteThread,
	ID_CreateRemoteThreadEx,
	ID_ExitProcess,
	ID_ExitThread,
};

DWORD dwFuncNum = sizeof(FuncList) / sizeof(FuncList[0]);


// CFrameProcessThreadDlg 对话框



CFrameProcessThreadDlg::CFrameProcessThreadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FRAMEProcessThread_DIALOG, pParent)
{

#ifdef _DEBUG
	// 开启控制台窗口
	AllocConsole();
	FILE* stream = nullptr;
	freopen_s(&stream, "CONOUT$", "w", stdout);
#endif

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFrameProcessThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TEXT_INFO, m_StaticText);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}

// 解决回车键 ESC 默认关闭窗口
BOOL CFrameProcessThreadDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);

}

BEGIN_MESSAGE_MAP(CFrameProcessThreadDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	// 选择了进程ID之后，CChooseProcess窗口发送消息WM_GET_CHOOSE_PROCESS_ID，收到此消息后，执行OnGetChooseProcessId
	ON_MESSAGE(WM_GET_CHOOSE_PROCESS_ID, &CFrameProcessThreadDlg::OnGetChooseProcessId)
	ON_COMMAND(ID_32773, &CFrameProcessThreadDlg::OnChooseProcessCommand)
	ON_COMMAND(ID_32775, &CFrameProcessThreadDlg::OnBeginWorkCommand)
	ON_COMMAND(ID_32776, &CFrameProcessThreadDlg::OnEndWorkCommand)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CFrameProcessThreadDlg::OnLvnItemchangedList)
END_MESSAGE_MAP()


// CFrameProcessThreadDlg 消息处理程序

BOOL CFrameProcessThreadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 设置菜单栏
	m_Menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_Menu);

	// 菜单子项进行变灰操作(开始 和 结束 两个都变灰)
	InvalidMenuItem(ID_32775);
	InvalidMenuItem(ID_32776);

	// 修改CEdit CListCtrl的字体，使得等宽
	static CFont font;
	font.DeleteObject();
	font.CreatePointFont(100, _T("新宋体"));
	m_List.SetFont(&font);
	m_Edit.SetFont(&font);

	// 以下是List Control
	//标题所需字段
	CString head[] = { TEXT("序号"), TEXT("函数"), TEXT("结果") };

	//插入列标题
	m_List.InsertColumn(0, head[0], LVCFMT_LEFT, 60);			// 仅用于创建本行，长度设为0，不在图像界面的列表中显示
	m_List.InsertColumn(1, head[1], LVCFMT_LEFT, 140);
	m_List.InsertColumn(2, head[2], LVCFMT_LEFT, 200);

	//设置风格样式
	//LVS_EX_GRIDLINES 网格
	//LVS_EX_FULLROWSELECT 选中整行
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()
		| LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 以上是List Control


	// 设置窗口大小。并居中显示
	MoveWindow(0, 0, 900, 600, FALSE);
	CenterWindow(GetDesktopWindow());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFrameProcessThreadDlg::OnPaint()
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
HCURSOR CFrameProcessThreadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//设置对话框最小宽度与高度
void CFrameProcessThreadDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 900;
	lpMMI->ptMinTrackSize.y = 600;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


// 动态调整控件大小
void CFrameProcessThreadDlg::ChangeWidget(int cx, int cy) {
	// CListCtrl
	CRect rt1;
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST1);
	if (pListCtrl == NULL)
		return;
	pListCtrl->GetWindowRect(rt1);
	ScreenToClient(rt1);
	rt1.right = cx - 10;
	rt1.bottom = cy / 1.66667 - 5;
	pListCtrl->MoveWindow(rt1);

	//// 动态调整第7列（数据一列）的长度
	//pListCtrl->SetColumnWidth(6, rt1.right - 60 - 100 - 60 - 150 - 60 - 60 - 33);


	// CEdit
	CRect rt2;
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	if (pListCtrl == NULL)
		return;
	pEdit->GetWindowRect(rt2);
	ScreenToClient(rt2);
	rt2.right = cx - 10;
	rt2.top = cy / 1.66667 + 5;
	rt2.bottom = cy - 30;
	pEdit->MoveWindow(rt2);

	// CStatic
	CRect rt3;
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_STATIC_TEXT_INFO);
	if (pStatic == NULL)
		return;
	pStatic->GetWindowRect(rt2);
	ScreenToClient(rt3);
	rt3.left = 12;
	rt3.right = cx - 10;
	rt3.top = cy - 24;
	rt3.bottom = cy - 10;
	pStatic->MoveWindow(rt3);
}


// 调整窗口大小时触发
void CFrameProcessThreadDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	ChangeWidget(cx, cy);
}


// 选择进程对话框
void CFrameProcessThreadDlg::OnChooseProcessCommand()
{
	CChooseProcessDlg dlgChooseProcess;
	dlgChooseProcess.DoModal();
}


// 自定义消息处理函数，在CChooseProcess窗口中选择了某个进程之后自动执行
LRESULT CFrameProcessThreadDlg::OnGetChooseProcessId(WPARAM w, LPARAM l)
{
	StopCurrentWork();

	// 选择新进程后，把之前UI中绘制的数据清空
	m_List.DeleteAllItems();
	m_Edit.SetWindowText(_T(""));

	DWORD dwPID = (DWORD)w;
	CString* pcsProcName = (CString*)l;

	m_CurrentChooseProcId = dwPID;
	m_bInjectSuccess = FALSE;
	m_hCommandPipe = NULL;
	m_hDataPipe = NULL;

	ShowInfo(_T("当前选择进程为[PID=%.8X] %s"), dwPID, *pcsProcName);

	// 选择了新进程后销毁之前的m_pListData，创建新的m_pListData
	if (m_pListData != NULL) {
		m_pListData->~CListProcessThreadData();
	}
	m_pListData = new CListProcessThreadData(((CFrameProcessThreadDlg*)(theApp.m_pMainWnd)));

	// 菜单栏 开始这一项可用
	ValidMenuItem(ID_32775);

	delete pcsProcName;
	return 0;
}


// 接收数据的线程函数
DWORD WINAPI ThreadFunc_DataPipeRecv() {
	PBYTE pBuffer = new BYTE[DATA_PIPE_BUF_SIZE];
	DWORD dwReturn = 0;
	
	CFrameProcessThreadDlg* pMainDlg = ((CFrameProcessThreadDlg*)(theApp.m_pMainWnd));
	HANDLE hDataPipe = ((CFrameProcessThreadDlg*)(theApp.m_pMainWnd))->m_hDataPipe;

	while (true) {

		if (!CheckProcessAlive(pMainDlg->m_CurrentChooseProcId)) {
			pMainDlg->StopCurrentWork();
			pMainDlg->ShowInfo("目标进程已关闭，中断捕获数据");
			return 0;
		}

		BOOL bRet = ReadFile(hDataPipe, pBuffer, DATA_PIPE_BUF_SIZE, &dwReturn, NULL);
		if (bRet) {
			pBuffer[dwReturn] = '\0';
			ParsePacket(((CFrameProcessThreadDlg*)(theApp.m_pMainWnd)), pBuffer, dwReturn);
		}
	}
	return 0;
}


HANDLE CreatePipeAndWaitConnect(LPSTR szPipeName, DWORD dwBufLen) {
	// 设置安全描述符为任意用户均可访问
	// 不然，这个程序如果用管理员权限启动，目标程序是普通权限，
	// 那么目标程序无法连接上由这个程序以管理员权限创建的管道
	SECURITY_ATTRIBUTES sa = { 0 };
	SECURITY_DESCRIPTOR sd = { 0 };
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.bInheritHandle = false;
	sa.lpSecurityDescriptor = &sd;
	sa.nLength = sizeof(sa);

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
		&sa);

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
BOOL CFrameProcessThreadDlg::RemoteInject()
{
	CHAR szDllPath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szDllPath, MAX_PATH);		// 获取本程序所在路径
	(strrchr(szDllPath, '\\'))[1] = 0;					// 路径中去掉本程序名称
	strcat_s(szDllPath, "InjectDll.dll");			    // 拼接上DLL的名称

	m_bInjectSuccess = RemoteInjectByProcessId(m_CurrentChooseProcId, szDllPath);
	if (!m_bInjectSuccess) {
		ShowInfo("向进程[PID=%.8X]注入失败\n", m_CurrentChooseProcId);
		return FALSE;
	}

	// 创建两个命令管道。
	// CommandPipe命令管道：本程序写，DLL读
	// DataPipe数据管道：DLL写，本程序读
	m_hCommandPipe = CreatePipeAndWaitConnect(COMMAND_PIPE, COMMAND_PIPE_BUF_SIZE);
	m_hDataPipe = CreatePipeAndWaitConnect(DATA_PIPE, DATA_PIPE_BUF_SIZE);

	if (m_hCommandPipe == NULL || m_hDataPipe == NULL) {
		ShowInfo("创建并等待管道连接失败");
		return FALSE;
	}

	// 接收数据的线程
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc_DataPipeRecv, NULL, 0, NULL);
	
	return TRUE;
}


// 安装HOOK
BOOL CFrameProcessThreadDlg::InstallHook()
{
	DWORD dwReturn = 0;
	char szBuffer[32] = "InstallHook";

	if (!m_bInjectSuccess) {
		ShowInfo("DLL未成功注入，无法安装HOOK\n");
		return FALSE;
	}

	// 发送数据的格式为：InstallHook + DWORD(函数ID)
	for (int i = 0; i < dwFuncNum; i++) {
		WriteDwordToBuffer((PBYTE)szBuffer, FuncList[i], 11);

		// 向客户端发送数据
		if (!WriteFile(m_hCommandPipe, szBuffer, 15, &dwReturn, NULL))
		{
			if (CheckProcessAlive(m_CurrentChooseProcId)) {
				StopCurrentWork();
				ShowInfo("目标进程已关闭，中断捕获数据");
			}
			return FALSE;
		}
		//Sleep(100);		// 实测，命名管道也是会有粘包的。
		// 算了，还是乖乖在另一端处理粘包吧
	}

	

	return TRUE;
}


// 卸载HOOK
BOOL CFrameProcessThreadDlg::UninstallHook()
{
	DWORD dwReturn = 0;
	char szBuffer[] = "AllUninstallHook";

	if (!m_bInjectSuccess) {
		ShowInfo("DLL未成功注入，无需卸载HOOK\n");
		return FALSE;
	}

	// 向客户端发送数据
	if (!WriteFile(m_hCommandPipe, szBuffer, strlen(szBuffer), &dwReturn, NULL))
	{
		if (CheckProcessAlive(m_CurrentChooseProcId)) {
			StopCurrentWork();
			ShowInfo("目标进程已关闭，中断捕获数据");
		}
		return FALSE;
	}

	StopCurrentWork();
	ShowInfo("中断捕获数据");
	return TRUE;
}


// 开始，即安装HOOK
void CFrameProcessThreadDlg::OnBeginWorkCommand()
{
	if (RemoteInject()) {
		if (InstallHook()) {
			// 菜单栏 停止 这一项可用
			ValidMenuItem(ID_32776);
			// 选择进程 和 开始 这两项都不可用
			InvalidMenuItem(ID_32773);
			InvalidMenuItem(ID_32775);
		}
	}
}

// 结束，即卸载HOOK
void CFrameProcessThreadDlg::OnEndWorkCommand()
{
	// 菜单子项进行变灰操作(开始 和 结束 两个都变灰)
	InvalidMenuItem(ID_32775);
	InvalidMenuItem(ID_32776);

	UninstallHook();

	// 选择进程 菜单子项 可用
	ValidMenuItem(ID_32773);
}



// 判断进程是否存活
BOOL CheckProcessAlive(DWORD dwPID) {
	// 僵尸进程具有PID，即使它们已终止。
	// 使用超时为 0 的 WaitForSingleObject 查看进程是否已终止

	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL)
		return FALSE;

	DWORD dwRet = WaitForSingleObject(hProcess, 0);
	CloseHandle(hProcess);
	return (dwRet == 0) ? 0 : 1;
}


// 中止对当前进程的后续跟踪
void CFrameProcessThreadDlg::StopCurrentWork() {
	if (m_hCommandPipe != NULL) {
		CloseHandle(m_hCommandPipe);
		m_hCommandPipe = NULL;
	}
		
	if (m_hDataPipe != NULL) {
		CloseHandle(m_hDataPipe);
		m_hDataPipe = NULL;
	}

	m_bInjectSuccess = FALSE;
	m_CurrentChooseProcId = 0;
	m_dwIndex = 0;

	// 菜单子项进行变灰操作(开始 和 结束 两个都变灰)
	InvalidMenuItem(ID_32775);
	InvalidMenuItem(ID_32776); 

	// 选择进程 菜单子项
	ValidMenuItem(ID_32773);
}




// 在消息栏显示信息
void CFrameProcessThreadDlg::ShowInfo(PCHAR fmt, ...)
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
void CFrameProcessThreadDlg::ShowInfo(PWCHAR fmt, ...)
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


// 点击行时触发，获取此时行号，显示详细的数据信息
// LVN_ITEMCHANGED 消息会响应三次，若函数中包含数据库操作，请添加条件判断排除其中的两次响应。
void CFrameProcessThreadDlg::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (!(pNMLV->uChanged == LVIF_STATE && (pNMLV->uNewState & LVIS_SELECTED)))		// 只有行号改变了才会继续
		return;

	int iItem = pNMLV->iItem;	// 选择的行号

	_ListProcessThreadRowData* pRowData = m_pListData->m_pRowDataIndexTable[iItem];

	m_Edit.SetWindowText(pRowData->csDetailInfo);

	*pResult = 0;
}


// 使菜单栏的某一项可用
void CFrameProcessThreadDlg::ValidMenuItem(UINT nIDEnableItem) {
	m_Menu.EnableMenuItem(nIDEnableItem, MF_BYCOMMAND | MF_ENABLED);
}


// 使菜单栏的某一项变灰不可用
void CFrameProcessThreadDlg::InvalidMenuItem(UINT nIDEnableItem) {
	m_Menu.EnableMenuItem(nIDEnableItem, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
}