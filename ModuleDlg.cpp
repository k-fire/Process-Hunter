// ModuleDlg.cpp: 实现文件
//

#include "pch.h"
#include "PC_hunter.h"
#include "ModuleDlg.h"
#include "afxdialogex.h"
#include "tlhelp32.h"
#include "PC_hunterDlg.h"



// ModuleDlg 对话框

IMPLEMENT_DYNAMIC(ModuleDlg, CDialogEx)

ModuleDlg::ModuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MODULE, pParent)
{

}

ModuleDlg::~ModuleDlg()
{
}

void ModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_modlistbox);
}


BEGIN_MESSAGE_MAP(ModuleDlg, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &ModuleDlg::OnNMRClickList1)
	ON_COMMAND(ID_32777, &ModuleDlg::On32777)
	ON_COMMAND(ID_32779, &ModuleDlg::On32779)
	ON_COMMAND(ID_32774, &ModuleDlg::On32774)
END_MESSAGE_MAP()


// ModuleDlg 消息处理程序


BOOL ModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	m_modlistbox.GetClientRect(&rect);

	// 为列表视图控件添加全行选中和栅格风格   
	m_modlistbox.SetExtendedStyle(m_modlistbox.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 为列表视图控件添加三列   
	m_modlistbox.InsertColumn(0, _T("序号"), LVCFMT_LEFT, rect.Width() / 14, 0);
	m_modlistbox.InsertColumn(1, _T("模块Name"), LVCFMT_LEFT, rect.Width() / 7 * 2, 0);
	m_modlistbox.InsertColumn(2, _T("模块路径"), LVCFMT_LEFT, rect.Width() / 14*8, 0);
	showmodule();
	return TRUE;
	  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void ModuleDlg::showmodule()
{
	m_modlistbox.DeleteAllItems();
	//获取选中的进程号
	CPChunterDlg* p;
	p = (CPChunterDlg*)GetParent();
	int npid = p->npid;
	if (npid == 0)
	{
		return;
	}

	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);
	//创建模块快照
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, npid);
	if (snap == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("查看模块快照失败！"), _T("提示"), MB_OK);
		EndDialog(0);
		return;
	}
	BOOL bret = Module32First(snap, &me32);
	int i = 0;
	CString str;
	while (bret)
	{
		str.Format(_T("%d"), i);
		m_modlistbox.InsertItem(i, str);
		m_modlistbox.SetItemText(i, 1, me32.szModule);
		m_modlistbox.SetItemText(i, 2, me32.szExePath);
		i++;
		bret = Module32Next(snap, &me32);
	}
	CloseHandle(snap);
}

void ModuleDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();

		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		CMenu menu;
		menu.LoadMenu(IDR_MENU2);
		CMenu* pop = menu.GetSubMenu(0);
		pop->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
	}
	*pResult = 0;
}


void ModuleDlg::On32777()
{
	// 刷新
	showmodule();
}


void ModuleDlg::On32779()
{
	// TODO: 在此添加命令处理程序代码
	POSITION Pos = m_modlistbox.GetFirstSelectedItemPosition();
	int nSelect = -1;
	while (Pos)
	{
		nSelect = m_modlistbox.GetNextSelectedItem(Pos);
	}
	//假设在列表框中没有进行选择。则报错    
	if (-1 == nSelect)
	{
		MessageBox(_T("请选择进程！"), _T("提示"), MB_OK);
		return;
	}
	//获取列表框中显示值    
	wchar_t id[10] = { 0 };
	wchar_t name[20] = { 0 };
	wchar_t path[200] = { 0 };
	m_modlistbox.GetItemText(nSelect, 0, id, 10);
	m_modlistbox.GetItemText(nSelect, 1, name, 20);
	m_modlistbox.GetItemText(nSelect, 2, path, 200);
	wchar_t data[100] = { 0 };
	wchar_t i[3] = L" ";
	wcscat_s(data, id); wcscat_s(data, i);
	wcscat_s(data, name); wcscat_s(data, i);
	wcscat_s(data, path);
	//剪贴板操作
	if (OpenClipboard())
	{
		HGLOBAL clipbuffer;
		TCHAR* buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, sizeof(data) + 1);
		buffer = (TCHAR*)GlobalLock(clipbuffer);
		lstrcpy(buffer, data);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_UNICODETEXT, clipbuffer);
		CloseClipboard();
	}
}


void ModuleDlg::On32774()
{
	CPChunterDlg* p;
	p = (CPChunterDlg*)GetParent();
	int npid = p->npid;

	//获取列表框中所选中的位置  
	POSITION Pos = m_modlistbox.GetFirstSelectedItemPosition();
	int nSelect = -1;
	while (Pos)
	{
		nSelect = m_modlistbox.GetNextSelectedItem(Pos);
	}
	//如果在列表框中没有进行选择，则报错      
	if (-1 == nSelect)
	{
		MessageBox(_T("请选择模块！"), _T("提示"), MB_OK);
		return;
	}
	//获取列表框中DLL的名称      
	wchar_t  szDllName[MAX_PATH] = { 0 };
	m_modlistbox.GetItemText(nSelect, 1, szDllName, MAX_PATH);

	UnInjectDll(szDllName,npid);
	showmodule();
}


bool ModuleDlg::UnInjectDll(const TCHAR* pszDllFile, DWORD dwProcessId)
{
	// 参数无效
	if (NULL == pszDllFile || 0 == ::_tcslen(pszDllFile))
	{
		return false;
	}

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;

	// 获取模块快照
	hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (INVALID_HANDLE_VALUE == hModuleSnap)
	{
		return false;
	}

	MODULEENTRY32 me32;
	memset(&me32, 0, sizeof(MODULEENTRY32));
	me32.dwSize = sizeof(MODULEENTRY32);

	// 开始遍历
	if (FALSE == ::Module32First(hModuleSnap, &me32))
	{
		::CloseHandle(hModuleSnap);
		return false;
	}

	// 遍历查找指定模块
	bool isFound = false;
	do
	{
		isFound = (0 == ::_tcsicmp(me32.szModule, pszDllFile) || 0 == ::_tcsicmp(me32.szExePath, pszDllFile));
		if (isFound) // 找到指定模块
		{
			break;
		}
	} while (TRUE == ::Module32Next(hModuleSnap, &me32));

	::CloseHandle(hModuleSnap);

	if (false == isFound)
	{
		return false;
	}

	// 获取目标进程句柄
	hProcess = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, FALSE, dwProcessId);
	if (NULL == hProcess)
	{
		return false;
	}

	// 从 Kernel32.dll 中获取 FreeLibrary 函数地址
	LPTHREAD_START_ROUTINE lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "FreeLibrary");
	if (NULL == lpThreadFun)
	{
		::CloseHandle(hProcess);
		return false;
	}

	// 创建远程线程调用 FreeLibrary
	hThread = ::CreateRemoteThread(hProcess, NULL, 0, lpThreadFun, me32.modBaseAddr /* 模块地址 */, 0, NULL);
	if (NULL == hThread)
	{
		::CloseHandle(hProcess);
		return false;
	}

	// 等待远程线程结束
	::WaitForSingleObject(hThread, INFINITE);

	// 清理
	::CloseHandle(hThread);
	::CloseHandle(hProcess);

	return true;
}