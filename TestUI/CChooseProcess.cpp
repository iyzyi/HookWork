// CChooseProcess.cpp: 实现文件
//

#include "pch.h"
#include "TestUI.h"
#include "CChooseProcess.h"
#include "afxdialogex.h"


#include <TlHelp32.h>
#include <tchar.h>


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


	// 以下是List Control
	//标题所需字段
	CString head[] = { TEXT(""),TEXT("PID"), TEXT("名称"), TEXT("标题"), TEXT("路径"), TEXT("命令行参数") };

	//插入列标题
	m_List.InsertColumn(0, head[0], LVCFMT_LEFT, 0);			// 仅用于创建本行，长度设为0，不在图像界面的列表中显示
	m_List.InsertColumn(1, head[1], LVCFMT_LEFT, 70);
	m_List.InsertColumn(2, head[2], LVCFMT_LEFT, 180);
	m_List.InsertColumn(3, head[3], LVCFMT_LEFT, 150);
	m_List.InsertColumn(4, head[4], LVCFMT_LEFT, 150);
	m_List.InsertColumn(5, head[5], LVCFMT_LEFT, 350);

	//设置风格样式
	//LVS_EX_GRIDLINES 网格
	//LVS_EX_FULLROWSELECT 选中整行
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()
		| LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 以上是List Control

	ListProcess();
	DDX_Control(pDX, IDC_EDIT1, m_EditTextBox);
}


BEGIN_MESSAGE_MAP(CChooseProcess, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CChooseProcess::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CChooseProcess::OnBnClickedButton1)
END_MESSAGE_MAP()


// CChooseProcess 消息处理程序


BOOL CChooseProcess::ListProcess() {

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return FALSE;
	}
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe))
	{
		DWORD dwInsertIndex = m_List.GetItemCount();

		LV_ITEM   lvitemData = { 0 };
		lvitemData.mask = LVIF_PARAM;
		lvitemData.iItem = dwInsertIndex;
		lvitemData.lParam = (LPARAM)(NULL);	// 这里的指针可以用于保存额外的信息

		m_List.InsertItem(&lvitemData);

		CHAR szPID[12] = { 0 };
		sprintf_s(szPID, "%.8X", pe.th32ProcessID);

		USES_CONVERSION;

		m_List.SetItemText(dwInsertIndex, 1, A2W(szPID));
		m_List.SetItemText(dwInsertIndex, 2, pe.szExeFile);

	//	dwInsertIndex++;
	}
	CloseHandle(hSnapshot);
	return TRUE;
}

void CChooseProcess::OnEnChangeEdit1()
{
	CString strKeyword;
	m_EditTextBox.GetWindowTextW(strKeyword);

	DWORD dwInsertIndex = m_List.GetItemCount();
	DWORD dwCurrentRow = 0;
	printf("dwInsertIndex = %d\n", dwInsertIndex);
	for (int row = 0; row < dwInsertIndex; row++) {
		BOOL bRet = FALSE;
		for (int col = 2; col <3; col++) {				// 5列，如果增删表头的列数的话，这里得改成对应多少列。
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

void CChooseProcess::OnBnClickedButton1()
{
	ListProcess();
	
}
