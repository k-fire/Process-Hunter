#pragma once


// ModuleDlg 对话框

class ModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ModuleDlg)

public:
	ModuleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ModuleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MODULE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void showmodule();
	CListCtrl m_modlistbox;
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On32777();
	afx_msg void On32779();
	afx_msg void On32774();
	bool UnInjectDll(const TCHAR* pszDllFile, DWORD dwProcessId);
};
