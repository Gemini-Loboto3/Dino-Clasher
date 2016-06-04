#include "stdafx.h"
#include "Dino Clasher.h"
#include "MainDlg.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
CMainDlg m_dlg;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

HINSTANCE GetInstance() { return hInst; }

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst=hInstance;
	MSG msg;

	m_dlg.Create(0);
	m_dlg.SetWindowPos(NULL,128,128,0,0);
	ShowWindow(m_dlg,nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(m_dlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}
