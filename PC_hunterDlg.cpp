
// PC_hunterDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "PC_hunter.h"
#include "PC_hunterDlg.h"
#include "afxdialogex.h"
#include "tlhelp32.h"
#include "ModuleDlg.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPChunterDlg 对话框



CPChunterDlg::CPChunterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PC_HUNTER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPChunterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_prolistbox);
}

BEGIN_MESSAGE_MAP(CPChunterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CPChunterDlg::OnNMRClickList1)
	ON_COMMAND(ID_32771, &CPChunterDlg::On32771)
	ON_COMMAND(ID_32772, &CPChunterDlg::On32772)
	ON_COMMAND(ID_32773, &CPChunterDlg::On32773)
	ON_COMMAND(ID_32775, &CPChunterDlg::On32775)
	ON_COMMAND(ID_32776, &CPChunterDlg::On32776)
	ON_COMMAND(ID_32778, &CPChunterDlg::On32778)
END_MESSAGE_MAP()


// CPChunterDlg 消息处理程序

BOOL CPChunterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect rect;
	m_prolistbox.GetClientRect(&rect);

	// 为列表视图控件添加全行选中和栅格风格   
	m_prolistbox.SetExtendedStyle(m_prolistbox.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 为列表视图控件添加三列   
	m_prolistbox.InsertColumn(0, _T("序号"), LVCFMT_LEFT, rect.Width() / 14, 0);
	m_prolistbox.InsertColumn(1, _T("进程Name"), LVCFMT_LEFT, rect.Width() / 7*2, 0);
	m_prolistbox.InsertColumn(2, _T("进程PID"), LVCFMT_CENTER, rect.Width() / 7, 0);
	m_prolistbox.InsertColumn(3, _T("线程数"), LVCFMT_CENTER, rect.Width() / 7, 0);
	m_prolistbox.InsertColumn(4, _T("父进程PID"), LVCFMT_CENTER, rect.Width() / 7, 0);
	m_prolistbox.InsertColumn(5, _T("线程优先级"), LVCFMT_CENTER, rect.Width() / 7, 5);
	showprocess();
	DebugPrivilege();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPChunterDlg::OnPaint()
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
HCURSOR CPChunterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPChunterDlg::showprocess()
{
	m_prolistbox.DeleteAllItems();
	//创建进程快照
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hsnap == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("创建快照失败！"), _T("提示"), MB_OK);
	}
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	BOOL bret = Process32First(hsnap, &pe32);
	int i = 0;
	CString str;
	while (bret)
	{
		str.Format(_T("%d"), i);
		m_prolistbox.InsertItem(i, str);
		//进程名
		m_prolistbox.SetItemText(i, 1, pe32.szExeFile);
		//进程ID
		str.Format(_T("%d"), pe32.th32ProcessID);
		m_prolistbox.SetItemText(i, 2, str);
		//此进程开启的线程计数
		str.Format(_T("%d"), pe32.cntThreads);
		m_prolistbox.SetItemText(i, 3, str);
		//父进程ID
		str.Format(_T("%d"), pe32.th32ParentProcessID);
		m_prolistbox.SetItemText(i, 4, str);
		//线程优先权
		str.Format(_T("%d"), pe32.pcPriClassBase);
		m_prolistbox.SetItemText(i, 5, str);
		i++;
		bret = Process32Next(hsnap, &pe32);
	}
	CloseHandle(hsnap);
}

//提升进程权限
void CPChunterDlg::DebugPrivilege()
{
	HANDLE htoken = NULL;

	BOOL bret = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &htoken);

	if (bret == TRUE)
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(htoken, FALSE, &tp, sizeof(tp), NULL, NULL);
		if (GetLastError() != ERROR_SUCCESS)
		{
			MessageBox(_T("提升权限失败！"), _T("提示"), MB_OK);
		}
		CloseHandle(htoken);
	}
}


void CPChunterDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();

		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);//IDR_SCReport为菜单ID
		CMenu* pop = menu.GetSubMenu(0);
		pop->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
	}
	*pResult = 0;
}


void CPChunterDlg::On32771()
{
	//刷新
	showprocess();
}


void CPChunterDlg::On32772()
{
	//查看模块
	npid = GetSelectPid();
	ModuleDlg ModuleDlg;
	ModuleDlg.DoModal();
}


int CPChunterDlg::GetSelectPid()
{
	int pid = -1;
	//获取列表框中所选中的位置
	POSITION Pos = m_prolistbox.GetFirstSelectedItemPosition();
	int nSelect = -1;
	while (Pos)
	{
		nSelect = m_prolistbox.GetNextSelectedItem(Pos);
	}
	//假设在列表框中没有进行选择。则报错    
	if (-1 == nSelect)
	{
		MessageBox(_T("请选择进程！"), _T("提示"), MB_OK);
		return -1;
	}
	//获取列表框中显示的PID值    
	wchar_t  szPid[10] = { 0 };
	m_prolistbox.GetItemText(nSelect, 2, szPid, 10);
	pid = _ttoi(szPid);
	return pid;
}


void CPChunterDlg::On32773()
{
	// 结束进程
	int npid = GetSelectPid();
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, npid);
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	showprocess();
}


void CPChunterDlg::On32775()
{
	//暂停进程
	int npid = -1;
	npid = GetSelectPid();

	if (npid ==0)
	{
		return;
	}
	//创建线程快照
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, npid);
	if (hsnap == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("暂停进程操作失败！"), _T("提示"), MB_OK);
	}
	THREADENTRY32 te32 = { 0 };
	te32.dwSize = sizeof(THREADENTRY32);
	BOOL bret = Thread32First(hsnap, &te32);
	while (bret)
	{
		if (te32.th32OwnerProcessID == npid)
		{
			//打开线程
			HANDLE hthread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
			//暂停线程
			SuspendThread(hthread);
			CloseHandle(hthread);

		}
		bret = Thread32Next(hsnap, &te32);
	}
	CloseHandle(hsnap);


}


void CPChunterDlg::On32776()
{
	// 恢复进程
	int npid = -1;
	npid = GetSelectPid();

	if (npid == 0)
	{
		return;
	}
	//创建线程快照
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, npid);
	if (hsnap == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("恢复进程操作失败！"), _T("提示"), MB_OK);
	}
	THREADENTRY32 te32 = { 0 };
	te32.dwSize = sizeof(THREADENTRY32);
	BOOL bret = Thread32First(hsnap, &te32);
	while (bret)
	{
		if (te32.th32OwnerProcessID == npid)
		{
			//打开线程
			HANDLE hthread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
			//恢复线程
			ResumeThread(hthread);
			CloseHandle(hthread);

		}
		bret = Thread32Next(hsnap, &te32);
	}
	CloseHandle(hsnap);
}


void CPChunterDlg::On32778()
{
	// 复制
	POSITION Pos = m_prolistbox.GetFirstSelectedItemPosition();
	int nSelect = -1;
	while (Pos)
	{
		nSelect = m_prolistbox.GetNextSelectedItem(Pos);
	}
	//假设在列表框中没有进行选择。则报错    
	if (-1 == nSelect)
	{
		MessageBox(_T("请选择进程！"), _T("提示"), MB_OK);
		return;
	}
	//获取列表框中显示值    
	wchar_t id[10] = { 0 };
	wchar_t name[50] = { 0 };
	wchar_t pid[10] = { 0 };
	wchar_t threadcount[10] = { 0 };
	wchar_t ppid[10] = { 0 };
	wchar_t level[10] = { 0 };
	m_prolistbox.GetItemText(nSelect, 0, id, 10);
	m_prolistbox.GetItemText(nSelect, 1, name, 50);
	m_prolistbox.GetItemText(nSelect, 2, pid, 10);
	m_prolistbox.GetItemText(nSelect, 3, threadcount, 10);
	m_prolistbox.GetItemText(nSelect, 4, ppid, 10);
	m_prolistbox.GetItemText(nSelect, 5, level, 10);
	wchar_t data[100] = { 0 };
	wchar_t i[3] = L" ";
	wcscat_s(data, id);wcscat_s(data, i);
	wcscat_s(data, name); wcscat_s(data, i);
	wcscat_s(data, pid); wcscat_s(data, i);
	wcscat_s(data, threadcount); wcscat_s(data, i);
	wcscat_s(data, ppid); wcscat_s(data, i);
	wcscat_s(data, level);
	//剪贴板操作
	if (OpenClipboard())
	{
		HGLOBAL clipbuffer;
		TCHAR* buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, sizeof(data) + 1);
		buffer = (TCHAR*)GlobalLock(clipbuffer);
		lstrcpy(buffer,data);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_UNICODETEXT, clipbuffer);
		CloseClipboard();
	}
}
