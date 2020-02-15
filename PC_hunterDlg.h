
// PC_hunterDlg.h: 头文件
//

#pragma once


// CPChunterDlg 对话框
class CPChunterDlg : public CDialogEx
{
// 构造
public:
	CPChunterDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PC_HUNTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void showprocess();
	void DebugPrivilege();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_prolistbox;
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On32771();
	afx_msg void On32772();
	int GetSelectPid();
	int npid;
	afx_msg void On32773();
	afx_msg void On32775();
	afx_msg void On32776();
	afx_msg void On32778();
};
