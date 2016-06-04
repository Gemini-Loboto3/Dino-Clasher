#include "StdAfx.h"
#include "AboutDlg.h"
#include "Shellapi.h"

CAboutDlg::CAboutDlg(HWND hWnd)
	: CDialog(hWnd,IDD)
{
	EventClick(IDC_SOURCE,CAboutDlg::OnBnClickSource);
	EventClick(IDC_THREAD,CAboutDlg::OnBnClickThread);
}

void CAboutDlg::OnBnClickSource()
{
	ShellExecute(NULL, _T("open"), _T("http://code.google.com/p/dinoclasher"), NULL, NULL, SW_SHOWNORMAL);
}

void CAboutDlg::OnBnClickThread()
{
	ShellExecute(NULL, _T("open"), _T("http://z4.invisionfree.com/Resident_Evil_1_2_3/index.php?showtopic=2236"), NULL, NULL, SW_SHOWNORMAL);
}
