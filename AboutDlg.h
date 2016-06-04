#pragma once
#include "dialog.h"
#include "resource.h"

class CAboutDlg :
	public CDialog
{
public:
	enum { IDD = IDD_ABOUTBOX };
	CAboutDlg(HWND hWnd);
	//~CAboutDlg(void);

	void OnBnClickSource();
	void OnBnClickThread();
};
