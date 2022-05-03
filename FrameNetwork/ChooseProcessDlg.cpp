// CChooseProcess.cpp: 实现文件
//

#include "pch.h"
#include "ChooseProcessDlg.h"
#include "Resource.h"
#include "afxdialogex.h"

#include <TlHelp32.h>
#include <tchar.h>
#include <Psapi.h>
#pragma comment(lib,"psapi.lib")
#include <Winternl.h>





// CChooseProcess 对话框

IMPLEMENT_DYNAMIC(CChooseProcessDlg, CDialogEx)

CChooseProcessDlg::CChooseProcessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHOOSE_PROCESS, pParent)
{
}

CChooseProcessDlg::~CChooseProcessDlg()
{
}

void CChooseProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_List);
	DDX_Control(pDX, IDC_EDIT_PROCESS_FILTER_KEYWORD, m_EditTextBox);
}



BOOL bIs64BitsOperateSystem;
BOOL Is64BitsOperateSystem();

BOOL CChooseProcessDlg::OnInitDialog()
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
	// ▲表示默认按《名称》一列升序排序。
	CString head[] = { TEXT("PID"), TEXT("名称▲"), TEXT("标题"), TEXT("路径"), TEXT("命令行参数") };

	//插入列标题
	m_List.InsertColumn(0, head[0], LVCFMT_LEFT, 62);			// 仅用于创建本行，长度设为0，不在图像界面的列表中显示
	m_List.InsertColumn(1, head[1], LVCFMT_LEFT, 180);
	m_List.InsertColumn(2, head[2], LVCFMT_LEFT, 150);
	m_List.InsertColumn(3, head[3], LVCFMT_LEFT, 300);
	m_List.InsertColumn(4, head[4], LVCFMT_LEFT, 600);

	//设置风格样式
	//LVS_EX_GRIDLINES 网格
	//LVS_EX_FULLROWSELECT 选中整行
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()
		| LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 以上是List Control

	ListProcess();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



BEGIN_MESSAGE_MAP(CChooseProcessDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_PROCESS_FILTER_KEYWORD, &CChooseProcessDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON_FLUSH_PROCESS, &CChooseProcessDlg::OnBnClickedButton1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PROCESS, &CChooseProcessDlg::OnLvnColumnclickList)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS_CHOOSE, &CChooseProcessDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CChooseProcess 消息处理程序


// 解决回车键 ESC 默认关闭窗口
BOOL CChooseProcessDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);

}


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


// ***************************************************************************
HANDLE mProcHandle;
HANDLE mForegroundHandle;

BOOL IsMainWindow(HWND handle)
{
	// using only OWNER condition allows getting titles of hidden "main windows"
	return !GetWindow(handle, GW_OWNER) && IsWindowVisible(handle);
}

BOOL CALLBACK chkWindowPidCallback(HWND hWnd, LPARAM lParam)
{
	DWORD procId = (DWORD)lParam;
	DWORD hwndPid = 0;
	GetWindowThreadProcessId(hWnd, &hwndPid);
	if (hwndPid == procId)
	{
		if (!mForegroundHandle)  // get the foreground if no owner visible
			mForegroundHandle = hWnd;

		if (IsMainWindow(hWnd))
		{
			mProcHandle = hWnd;
			return FALSE;
		}
	}

	return TRUE;
}

// 获取进程对应程序的标题，如果获取失败，则获取其class name
BOOL GetWinText(DWORD dwProcessId, CString* pcsWinText, DWORD dwStrBufLen)
{
	mProcHandle = NULL;
	mForegroundHandle = NULL;

	EnumWindows(chkWindowPidCallback, dwProcessId);
	if (!mProcHandle && !mForegroundHandle)
		return false;

	if (mProcHandle)  // get info from the "main window" (GW_OWNER + visible)
	{
		if (!GetWindowTextW((HWND)mProcHandle, pcsWinText->GetBuffer(), dwStrBufLen))
			GetClassNameW((HWND)mProcHandle, pcsWinText->GetBuffer(), dwStrBufLen); // go for the class name if none of the above
	}
	else if (mForegroundHandle)  // get info from the foreground window
	{
		if (!GetWindowTextW((HWND)mForegroundHandle, pcsWinText->GetBuffer(), dwStrBufLen))
			GetClassNameW((HWND)mForegroundHandle, pcsWinText->GetBuffer(), dwStrBufLen); // go for the class name if none of the above
	}

	return true;
}
// ***************************************************************************


// 获取进程对应程序的所在路径
CString GetProcessExePath(DWORD dwPID) {

	MODULEENTRY32 me;
	me.dwSize = sizeof(me);

	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	Module32First(hModule, &me);
	
	return CString(me.szExePath);
}

// ***************************************************************************

typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
	HANDLE ProcessHandle,
	DWORD ProcessInformationClass,
	PVOID ProcessInformation,
	DWORD ProcessInformationLength,
	PDWORD ReturnLength
	);

// 获取某个进程的命令行参数
PTCHAR GetProcessCommandLine(DWORD dwPID)
{
	HANDLE hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION |		/* required for NtQueryInformationProcess */
		PROCESS_VM_READ,				/* required for ReadProcessMemory */
		FALSE, dwPID					/* ProcessID */);
	if (hProcess == NULL)
		return NULL;

	UNICODE_STRING commandLine;
	TCHAR* commandLineContents = NULL;
	_NtQueryInformationProcess NtQuery = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
	if (NtQuery)
	{
		PROCESS_BASIC_INFORMATION pbi;
		NTSTATUS isok = NtQuery(hProcess, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
		if (NT_SUCCESS(isok))
		{
			PEB peb;
			RTL_USER_PROCESS_PARAMETERS upps;
			PVOID rtlUserProcParamsAddress;
			if (ReadProcessMemory(hProcess, &(((_PEB*)pbi.PebBaseAddress)->ProcessParameters), &rtlUserProcParamsAddress, sizeof(PVOID), NULL))
			{
				if (ReadProcessMemory(hProcess,
					&(((_RTL_USER_PROCESS_PARAMETERS*)rtlUserProcParamsAddress)->CommandLine),
					&commandLine, sizeof(commandLine), NULL))
				{
					commandLineContents = (TCHAR*)malloc(commandLine.Length + sizeof(TCHAR));
					memset(commandLineContents, 0, commandLine.Length + sizeof(TCHAR));
					ReadProcessMemory(hProcess, commandLine.Buffer,
						commandLineContents, commandLine.Length, NULL);
				}
			}
		}
	}
	return commandLineContents;
}



// 在窗口中列出所有进程
BOOL CChooseProcessDlg::ListProcess() {
	if (m_pListData != NULL) {
		m_pListData->~CListData();
	}	
	m_pListData = new CListData(&m_List);


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
		
		// PID
		DWORD dwPID = pe.th32ProcessID;
		CString csPID;
		csPID.Format(L"%.8X", dwPID);

		// 标题
		CString csProcMainWinTitle = CString(_T(""));
		csProcMainWinTitle.GetBufferSetLength(512);
		BOOL bRet = GetWinText(dwPID, &csProcMainWinTitle, 512);

		// 命令行参数
		PTCHAR tszCmdLine = GetProcessCommandLine(dwPID);
		CString csProcCmdLine = (tszCmdLine == NULL) ? CString(_T("")) : CString(tszCmdLine);

		// 向ListData类中添加一行数据
		m_pListData->AddRow(&csPID, &(CString(pe.szExeFile)), &csProcMainWinTitle, &GetProcessExePath(dwPID), &csProcCmdLine);
	}

	CString csKeyword;
	m_EditTextBox.GetWindowTextW(csKeyword);
	if (csKeyword == _T("")) {
		// 在ListCtrl中绘制所有行的数据
		m_pListData->DisplayData();
	}
	else {
		// 在ListCtrl中过滤后的行的数据
		m_pListData->DisplayFilterData(csKeyword);
	}


	CloseHandle(hSnapshot);
	return TRUE;
}


// 通过关键字筛选进程
void CChooseProcessDlg::OnEnChangeEdit1()
{
	CString csKeyword;
	m_EditTextBox.GetWindowTextW(csKeyword);

	if (csKeyword == _T("")) {
		m_pListData->DisplayData();
	}
	else {
		m_pListData->DisplayFilterData(csKeyword);
	}
}


// 刷新进程列表
void CChooseProcessDlg::OnBnClickedButton1()
{
	ListProcess();
}


// 单击表头的某一列自动触发，改变排序方式，并排序
void CChooseProcessDlg::OnLvnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);	

	// 获取点击的是第几列
	int dwCol = pNMLV->iSubItem;

	// 改变排序方式，并排序
	m_pListData->ChangeSortType(dwCol);

	*pResult = 0;
}


// 确定选择某个进程
void CChooseProcessDlg::OnBnClickedButton2()
{
	int nIndex = m_List.GetSelectionMark();				//获取选中行的行号
	if (nIndex == -1) 
		return;

	CString csPID = m_List.GetItemText(nIndex, 0);		//获取PID
	CString csProcName = m_List.GetItemText(nIndex, 1);	//获取进程名

	PWCHAR wszPID = (LPWSTR)(LPCTSTR)csPID;
	DWORD dwPID = wcstol(wszPID, NULL, 16);				// 16表示16进制

	CString* pcsProcName = new CString(csProcName);
	
	::PostMessage(AfxGetMainWnd()->m_hWnd, WM_GET_CHOOSE_PROCESS_ID, dwPID, (LPARAM)pcsProcName);		// 第一个参数为主窗口句柄

	EndDialog(0);
}







// 以下为CListData

CListData::CListData(CListCtrl* pListCtrl) {
	this->pListCtrl = pListCtrl;
}


CListData::~CListData() {
	if (pListCtrl != NULL) {
		_ListRowData* pData = pFirst;
		_ListRowData* pTemp = NULL;
		if (pFirst != NULL) {
			while (true) {
				pTemp = pData->pNext;
				delete pData;
				if (pTemp == NULL)
					break;
				else
					pData = pTemp;
			}
		}
	}
}


// 添加一行数据（不是向ListCtrl中添加，而是向我们自定义的CListData类中添加）
void CListData::AddRow(CString* pcsPID, CString* pcsProcName, CString* pcsProcWinText, CString* pcsProcPath, CString* pcsProcCmdLine) {
	_ListRowData* pListRowData = new _ListRowData(*pcsPID, *pcsProcName, *pcsProcWinText, *pcsProcPath, *pcsProcCmdLine);

	if (pFirst == NULL) {
		pFirst = pListRowData;
		pLast = pFirst;
	}
	else {
		pLast->pNext = pListRowData;
		pLast = pListRowData;
	}
}


// 向ListCtrl中绘制我们自定义的CListData类中的所有行的数据（本函数不负责排序）
void CListData::DisplayData() {
	//插入过程禁止刷新界面
	pListCtrl->LockWindowUpdate();
	pListCtrl->SetRedraw(FALSE);//插入时如果设置了被选中状态就会引发重绘

	// 删除之前的ListCtrl中的数据
	pListCtrl->DeleteAllItems();

	if (pFirst != NULL) {
		_ListRowData* pData = pFirst;
		while (true) {

			DWORD dwInsertIndex = pListCtrl->GetItemCount();

			LV_ITEM   lvitemData = { 0 };
			lvitemData.mask = LVIF_PARAM;
			lvitemData.iItem = dwInsertIndex;
			lvitemData.lParam = (LPARAM)(NULL);	// 这里的指针可以用于保存额外的信息

			pListCtrl->InsertItem(&lvitemData);

			pListCtrl->SetItemText(dwInsertIndex, 0, pData->csPID);
			pListCtrl->SetItemText(dwInsertIndex, 1, pData->csProcName);
			pListCtrl->SetItemText(dwInsertIndex, 2, pData->csProcWinText);
			pListCtrl->SetItemText(dwInsertIndex, 3, pData->csProcPath);
			pListCtrl->SetItemText(dwInsertIndex, 4, pData->csProcCmdLine);

			if (pData->pNext == NULL)
				break;
			else
				pData = pData->pNext;

		}
	}

	// 如果此前设置过排序方式，则按照之前的设置进行排序
	int iCol = CheckSortColNum();
	if (iCol >= 0) {
		SortDataByCol(iCol);
	}

	// 插入完所有数据后就可以刷新界面了
	pListCtrl->SetRedraw(TRUE);
	pListCtrl->UnlockWindowUpdate();
}


// 无视大小写判断字符串包含
BOOL CListData::ContainsTextNoCase(CString csText, CString csKeyword) {
	BOOL bRet = (csText.MakeUpper().Find(csKeyword.MakeUpper()) != -1);
	return bRet;
}


// 向ListCtrl中绘制我们自定义的CListData类中的数据（经过了关键词的过滤）（本函数不负责排序）
void CListData::DisplayFilterData(CString csFilterKeyword)
{
	//插入过程禁止刷新界面
	pListCtrl->LockWindowUpdate();
	pListCtrl->SetRedraw(FALSE);//插入时如果设置了被选中状态就会引发重绘

	// 删除之前的ListCtrl中的数据
	pListCtrl->DeleteAllItems();

	if (pFirst != NULL) {
		_ListRowData* pData = pFirst;

		while (true) {
			// 对于当前行，只要5列中有一列的数据符合关键字（均无视大小写），则绘制本行
			if (ContainsTextNoCase(pData->csPID, csFilterKeyword) ||
				ContainsTextNoCase(pData->csProcName, csFilterKeyword) ||
				ContainsTextNoCase(pData->csProcWinText, csFilterKeyword) ||
				ContainsTextNoCase(pData->csProcPath, csFilterKeyword) ||
				ContainsTextNoCase(pData->csProcCmdLine, csFilterKeyword)
				) {
				DWORD dwInsertIndex = pListCtrl->GetItemCount();

				LV_ITEM   lvitemData = { 0 };
				lvitemData.mask = LVIF_PARAM;
				lvitemData.iItem = dwInsertIndex;
				lvitemData.lParam = (LPARAM)(NULL);				// 这里的指针可以用于保存额外的信息

				pListCtrl->InsertItem(&lvitemData);

				pListCtrl->SetItemText(dwInsertIndex, 0, pData->csPID);
				pListCtrl->SetItemText(dwInsertIndex, 1, pData->csProcName);
				pListCtrl->SetItemText(dwInsertIndex, 2, pData->csProcWinText);
				pListCtrl->SetItemText(dwInsertIndex, 3, pData->csProcPath);
				pListCtrl->SetItemText(dwInsertIndex, 4, pData->csProcCmdLine);
			}

			if (pData->pNext == NULL)
				break;
			else
				pData = pData->pNext;
		}
	}

	// 如果此前设置过排序方式，则按照之前的设置进行排序
	int iCol = CheckSortColNum();
	if (iCol >= 0) {
		SortDataByCol(iCol);
	}

	// 插入完所有数据后就可以刷新界面了
	pListCtrl->SetRedraw(TRUE);
	pListCtrl->UnlockWindowUpdate();
}


struct _SortData {
	CListCtrl* m_lpList;
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
	pListCtrl->GetColumn(dwCol, &Vol);
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
void CListData::SortDataByCol(DWORD dwCol) {
	// 设置索引，不然没法排序
	for (int i = 0; i < pListCtrl->GetItemCount(); i++) {
		pListCtrl->SetItemData(i, i);
	}

	_SortData SortData;
	SortData.m_lpList = pListCtrl;
	SortData.dwCol = dwCol;

	// 排序
	pListCtrl->SortItems(ListCompare, (LPARAM)&SortData);
}


// 检查是哪一列需要排序
int CListData::CheckSortColNum() {
	// 如果某列的表头有▲or▼，则按此列排序
	CString csColName(_T(""));
	csColName.GetBufferSetLength(32);

	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.pszText = csColName.GetBuffer();
	lvColumn.cchTextMax = 32;

	for (int i = 0; i < 5; i++) {		// 5列
		pListCtrl->GetColumn(i, &lvColumn);
		csColName.ReleaseBuffer();
		if (csColName.Right(1) == _T("▲") || csColName.Right(1) == _T("▼")) {
			return i;
			break;
		}
	}
	return -1;
}


// 点击表头时，更改本列的排序方式
void CListData::ChangeSortType(DWORD dwCol) {
	CString csColName(_T(""));
	csColName.GetBufferSetLength(32);

	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.pszText = csColName.GetBuffer();
	lvColumn.cchTextMax = 32;
	pListCtrl->GetColumn(dwCol, &lvColumn);

	// GetBuffer之后会把CString的缓冲区长度锁定，导致CString拼接失败。必须再次调用ReleaseBuffer
	csColName.ReleaseBuffer();

	if (csColName.Right(1) == _T("▲"))
		csColName.SetAt(csColName.GetLength() - 1, _T('▼'));
	else if (csColName.Right(1) == _T("▼"))
		//csColName.Delete(csColName.GetLength() - 1, 1);
		csColName.SetAt(csColName.GetLength() - 1, _T('▲'));
	else
		csColName += _T("▲");

	pListCtrl->SetColumn(dwCol, &lvColumn);


	// 如果其他列有▲or▼则清除
	for (int i = 0; i < 5; i++) {		// 5列
		if (i == dwCol)
			continue;

		pListCtrl->GetColumn(i, &lvColumn);

		csColName.ReleaseBuffer();

		if (csColName.Right(1) == _T("▲") || csColName.Right(1) == _T("▼")) {
			csColName.Delete(csColName.GetLength() - 1, 1);
			pListCtrl->SetColumn(i, &lvColumn);
		}
	}

	// 按照新的设置排序
	SortDataByCol(dwCol);
}

