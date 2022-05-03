
// FrameNetworkDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FrameNetwork.h"
#include "FrameNetworkDlg.h"
#include "afxdialogex.h"

#include "ChooseProcessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFrameNetworkDlg 对话框



CFrameNetworkDlg::CFrameNetworkDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FRAMENETWORK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFrameNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}

// 解决回车键 ESC 默认关闭窗口
BOOL CFrameNetworkDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);

}

BEGIN_MESSAGE_MAP(CFrameNetworkDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_COMMAND(ID_32773, &CFrameNetworkDlg::OnChooseProcessCommand)
END_MESSAGE_MAP()


// CFrameNetworkDlg 消息处理程序

BOOL CFrameNetworkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	// 以下是List Control
	//标题所需字段
	CString head[] = { TEXT("序号"), TEXT("函数"), TEXT("SOCKET"), TEXT("IP"), TEXT("端口"), TEXT("长度"), TEXT("数据")};

	//插入列标题
	m_List.InsertColumn(0, head[0], LVCFMT_LEFT, 60);			// 仅用于创建本行，长度设为0，不在图像界面的列表中显示
	m_List.InsertColumn(1, head[1], LVCFMT_LEFT, 100);
	m_List.InsertColumn(2, head[2], LVCFMT_LEFT, 60);
	m_List.InsertColumn(3, head[3], LVCFMT_LEFT, 150);
	m_List.InsertColumn(4, head[4], LVCFMT_LEFT, 60);
	m_List.InsertColumn(5, head[5], LVCFMT_LEFT, 60);
	m_List.InsertColumn(6, head[6], LVCFMT_LEFT, 665);

	//设置风格样式
	//LVS_EX_GRIDLINES 网格
	//LVS_EX_FULLROWSELECT 选中整行
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()
		| LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 以上是List Control


	//// 调整初始化窗口时的控件相对位置
	//CRect rt0;
	//GetClientRect(rt0);
	//ChangeWidget(rt0.right, rt0.bottom);

	// 设置窗口大小。并居中显示
	MoveWindow(0, 0, 1200, 800, FALSE);
	CenterWindow(GetDesktopWindow());


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFrameNetworkDlg::OnPaint()
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


void CFrameNetworkDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	//设置对话框最小宽度与高度
	lpMMI->ptMinTrackSize.x = 1200;
	lpMMI->ptMinTrackSize.y = 800;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


// 动态调整控件大小
void CFrameNetworkDlg::ChangeWidget(int cx, int cy) {
	CRect rt1;
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST1);
	if (pListCtrl == NULL)
		return;
	pListCtrl->GetWindowRect(rt1);
	ScreenToClient(rt1);
	rt1.right = cx - 10;
	rt1.bottom = cy / 1.66667 - 5;
	pListCtrl->MoveWindow(rt1);

	// 动态调整第7列（数据一列）的长度
	pListCtrl->SetColumnWidth(6, rt1.right - 60 - 100 - 60 - 150 - 60 - 60 - 16);


	CRect rt2;
	CListCtrl* pEdit = (CListCtrl*)GetDlgItem(IDC_EDIT1);
	if (pListCtrl == NULL)
		return;
	pEdit->GetWindowRect(rt2);
	ScreenToClient(rt2);
	rt2.right = cx - 10;
	rt2.top = cy / 1.66667 + 5;
	rt2.bottom = cy - 10;
	pEdit->MoveWindow(rt2);
}


// 调整窗口大小时触发
void CFrameNetworkDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	ChangeWidget(cx, cy);
}


void CFrameNetworkDlg::OnChooseProcessCommand()
{
	CChooseProcessDlg dlgChooseProcess;
	dlgChooseProcess.DoModal();
}
