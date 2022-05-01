// CChooseProcess.cpp: 实现文件
//

#include "pch.h"
#include "TestUI.h"
#include "CChooseProcess.h"
#include "afxdialogex.h"


#include <TlHelp32.h>
#include <tchar.h>
#include <Psapi.h>
#pragma comment(lib,"psapi.lib")


// CChooseProcess 对话框

IMPLEMENT_DYNAMIC(CChooseProcess, CDialogEx)

CChooseProcess::CChooseProcess(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHOOSE_PROCESS, pParent)
{
}

CChooseProcess::~CChooseProcess()
{
}

void CChooseProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_List);
	DDX_Control(pDX, IDC_EDIT1, m_EditTextBox);
}



BOOL bIs64BitsOperateSystem;
BOOL Is64BitsOperateSystem();

BOOL CChooseProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

#ifdef _WIN64
	SetWindowText(L"选择需要注入的64位进程");
#elif _WIN32
	SetWindowText(L"选择需要注入的32位进程");
#endif

	bIs64BitsOperateSystem = Is64BitsOperateSystem();

	// 以下是List Control
	//标题所需字段
	CString head[] = { TEXT("PID"), TEXT("名称"), TEXT("标题"), TEXT("路径"), TEXT("命令行参数") };

	//插入列标题
	m_List.InsertColumn(0, head[0], LVCFMT_LEFT, 62);			// 仅用于创建本行，长度设为0，不在图像界面的列表中显示
	m_List.InsertColumn(1, head[1], LVCFMT_LEFT, 180);
	m_List.InsertColumn(2, head[2], LVCFMT_LEFT, 150);
	m_List.InsertColumn(3, head[3], LVCFMT_LEFT, 300);
	m_List.InsertColumn(4, head[4], LVCFMT_LEFT, 350);

	//设置风格样式
	//LVS_EX_GRIDLINES 网格
	//LVS_EX_FULLROWSELECT 选中整行
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()
		| LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 以上是List Control

	ListProcess();
	SortDataByCol(1);		// 默认按第二列排序

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



BEGIN_MESSAGE_MAP(CChooseProcess, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CChooseProcess::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CChooseProcess::OnBnClickedButton1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST2, &CChooseProcess::OnLvnColumnclickList)
	ON_BN_CLICKED(IDC_BUTTON2, &CChooseProcess::OnBnClickedButton2)
END_MESSAGE_MAP()


// CChooseProcess 消息处理程序



BOOL Is64BitsOperateSystem()
{
	typedef VOID(WINAPI* LPFN_GetNativeSystemInfo)(__out LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandleW(L"kernel32"), "GetNativeSystemInfo");
	if (fnGetNativeSystemInfo)
	{
		SYSTEM_INFO stInfo = { 0 };
		fnGetNativeSystemInfo(&stInfo);
		if (stInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
			|| stInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		{
			return TRUE;
		}
	}
	return FALSE;
}


// 判断指定进程的位数
// 返回值-1表示判断失败（比如有的进程是SYSTEM权限的），0表示32位，1表示64位
int Is64BitsProcess(DWORD dwProcessId)
{
	// TODO 这里没测试32位windows系统。
	// 如果是32位windows系统，这个函数的代码可能会因为IsWow64Process而结果出错。
	
	// 先判断计算机操作系统位数, 32位系统只能执行32位进程
	if (FALSE == bIs64BitsOperateSystem)
	{
		return FALSE;
	}

	BOOL bWow64Process = FALSE;
	HANDLE hProcess = NULL;

	// 打开进程, 获取进程句柄
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (NULL == hProcess)
	{
		return -1;
	}

	// 判断进程是否处于WOW64仿真环境中(WOW64是指64位系统下模拟运行32位程序的环境)
	::IsWow64Process(hProcess, &bWow64Process);

	return bWow64Process ? 0: 1;
}


// 获取进程对应程序的所在路径
CString GetProcessExePath(DWORD dwPID) {

	MODULEENTRY32 me;
	me.dwSize = sizeof(me);

	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	Module32First(hModule, &me);
	
	return CString(me.szExePath);
}


BOOL CChooseProcess::ListProcess() {

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return FALSE;
	}
	
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);

	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe))
	{
		int iRet = Is64BitsProcess(pe.th32ProcessID);
#ifdef _WIN64
		// 64位程序只能选择64位进程
		if (iRet == -1 || iRet == 0)
			continue;
#elif _WIN32
		// 32位程序只能选择32位进程
		if (iRet == -1 || iRet == 1) 
			continue;
#endif

		DWORD dwInsertIndex = m_List.GetItemCount();

		LV_ITEM   lvitemData = { 0 };
		lvitemData.mask = LVIF_PARAM;
		lvitemData.iItem = dwInsertIndex;
		lvitemData.lParam = (LPARAM)(NULL);	// 这里的指针可以用于保存额外的信息

		m_List.InsertItem(&lvitemData);

		DWORD dwPID = pe.th32ProcessID;
		CString csPID;
		csPID.Format(L"%.8X", dwPID);

		m_List.SetItemText(dwInsertIndex, 0, csPID);
		m_List.SetItemText(dwInsertIndex, 1, pe.szExeFile);
		m_List.SetItemText(dwInsertIndex, 3, GetProcessExePath(dwPID));
	}
	CloseHandle(hSnapshot);
	return TRUE;
}

void CChooseProcess::OnEnChangeEdit1()
{
	//if (!bHaveListTemp) {
	//	m_ListTemp =  CListCtrl(m_List);
	//}

	CString strKeyword;
	m_EditTextBox.GetWindowTextW(strKeyword);

	DWORD dwInsertIndex = m_List.GetItemCount();
	DWORD dwCurrentRow = 0;
	printf("dwInsertIndex = %d\n", dwInsertIndex);
	for (int row = 0; row < dwInsertIndex; row++) {
		BOOL bRet = FALSE;
		for (int col = 1; col <2; col++) {				// 5列，如果增删表头的列数的话，这里得改成对应多少列。
			CString strText = m_List.GetItemText(dwCurrentRow, col);
			if (wcsstr(strKeyword, strText)) {
				bRet = TRUE;
				break;
			}
		}
		
		if (!bRet) {
			m_List.DeleteItem(dwCurrentRow);
		}
		else {
			dwCurrentRow++;
		}
	}
}


// 刷新进程列表
void CChooseProcess::OnBnClickedButton1()
{
	m_List.DeleteAllItems();
	ListProcess();
}



struct _SortData {
	CListCtrl*		m_lpList;
	DWORD			dwCol;
}SortData;

// 回调函数：排序函数的主体(排序无视大小写)
int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lpSortData)
{
	CListCtrl* pListCtrl = ((_SortData*)lpSortData)->m_lpList;
	DWORD dwCol = ((_SortData*)lpSortData)->dwCol;

	// 按第dwCol列排序
	CString strItem1 = pListCtrl->GetItemText(lParam1, dwCol);		
	CString strItem2 = pListCtrl->GetItemText(lParam2, dwCol);

	LVCOLUMN Vol;
	CString csStr("");
	TCHAR szCol[MAX_PATH];
	Vol.pszText = szCol;
	Vol.mask = LVCF_TEXT;
	Vol.cchTextMax = sizeof(szCol);
	pListCtrl->GetColumn(0, &Vol);
	csStr = CString(Vol.pszText);

	if (csStr.Right(1) == CString("▼"))
	{
		return _tcscmp(strItem2.MakeUpper().GetBuffer(MAX_PATH), strItem1.MakeUpper().GetBuffer(MAX_PATH));
	}
	else if (csStr.Right(1) == CString("▲"))
	{
		return _tcscmp(strItem1.MakeUpper().GetBuffer(MAX_PATH), strItem2.MakeUpper().GetBuffer(MAX_PATH));
	}
	else
	{
		return _tcscmp(strItem1.MakeUpper().GetBuffer(MAX_PATH), strItem2.MakeUpper().GetBuffer(MAX_PATH));
	}
}

// 按列排序
void CChooseProcess::SortDataByCol(DWORD dwCol) {
	// 设置索引，不然没法排序
	for (int i = 0; i < m_List.GetItemCount(); i++) {
		m_List.SetItemData(i, i);
	}

	//// 设置▲or▼
	//CHeaderCtrl* pHead = m_List.GetHeaderCtrl();


	_SortData SortData;
	SortData.m_lpList = &m_List;
	SortData.dwCol = dwCol;

	// 排序
	m_List.SortItems(ListCompare, (LPARAM)&SortData);
}


// 单击表头的某一列自动触发
void CChooseProcess::OnLvnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);	

	// 获取点击的是第几列
	int dwCol = pNMLV->iSubItem;

	SortDataByCol(dwCol);

	*pResult = 0;
}


// 确定选择某个进程
void CChooseProcess::OnBnClickedButton2()
{
	int nIndex = m_List.GetSelectionMark();				//获取选中行的行号
	CString csPID = m_List.GetItemText(nIndex, 0);		//获取PID
	CString csProcName = m_List.GetItemText(nIndex, 1);	//获取进程名

	PWCHAR wszPID = (LPWSTR)(LPCTSTR)csPID;
	DWORD dwPID = wcstol(wszPID, NULL, 16);				// 16表示16进制

	CString* pcsProcName = new CString(csProcName);
	
	::PostMessage(AfxGetMainWnd()->m_hWnd, WM_GET_CHOOSE_PROCESS_ID, dwPID, (LPARAM)pcsProcName);		// 第一个参数为主窗口句柄

	EndDialog(0);
}