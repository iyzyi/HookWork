#pragma once

struct _ListRowData {
	CString				csPID;
	CString				csProcName;
	CString				csProcWinText;
	CString				csProcPath;
	CString				csProcCmdLine;
	_ListRowData*		pNext;

	_ListRowData(CString a, CString b, CString c, CString d, CString e) {
		csPID = a;
		csProcName = b;
		csProcWinText = c;
		csProcPath = d;
		csProcCmdLine = e;
		pNext = NULL;
	}
};

class CListData {
private:
	CListCtrl* pListCtrl = NULL;

	_ListRowData* pFirst = NULL;
	_ListRowData* pLast = NULL;

public:
	CListData(CListCtrl* pListCtrl) {
		this->pListCtrl = pListCtrl;
	}

	~CListData() {
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
						pData = pData->pNext;
				}
			}
		}
	}


	// 添加一行数据（不是向ListCtrl中添加，而是向我们自定义的CListData类中添加）
	void AddRow(CString* pcsPID, CString* pcsProcName, CString* pcsProcWinText, CString* pcsProcPath, CString* pcsProcCmdLine) {
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
	void DisplayData() {
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
		
		// 插入完所有数据后就可以刷新界面了
		pListCtrl->SetRedraw(TRUE);
		pListCtrl->UnlockWindowUpdate();
	}

	// 无视大小写判断字符串包含
	BOOL ContainsTextNoCase(CString csText, CString csKeyword) {
		BOOL bRet = (csText.MakeUpper().Find(csKeyword.MakeUpper()) != -1);
		return bRet;
	}

	// 向ListCtrl中绘制我们自定义的CListData类中的数据（经过了关键词的过滤）（本函数不负责排序）
	void DisplayFilterData(CString csFilterKeyword)
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

		// 插入完所有数据后就可以刷新界面了
		pListCtrl->SetRedraw(TRUE);
		pListCtrl->UnlockWindowUpdate();
	}
};




// CChooseProcess 对话框
class CChooseProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CChooseProcess)

public:
	CChooseProcess(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CChooseProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHOOSE_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	CListCtrl		m_List;

	CEdit			m_EditTextBox;

	CListData*		m_pListData = NULL;


public:
	BOOL ListProcess();
	afx_msg void OnEnChangeEdit1();
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLvnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();

	void SortDataByCol(DWORD dwCol);
	afx_msg void OnBnClickedButton2();
};


