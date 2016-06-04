#include "stdafx.h"
#include "Dialog.h"

CDialog::CDialog(HWND hWnd, UINT id)
{
	hParent=hWnd;
	uID=id;
	is_modless=false;
}

bool CDialog::Create(HWND hWnd)
{
	is_modless=true;
	hParent = hWnd;
	this->hWnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(uID), hParent, (DLGPROC)DialProc, (LPARAM)this);

	if(*this) return true;
	
	return false;
}

INT_PTR CDialog::DoModal()
{
	return ::DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE (uID), hParent, (DLGPROC)DialProc, (LPARAM) this);
}

/* return true for handled message, otherwise return false */
LRESULT CALLBACK CDialog::DialProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
// disable annoying warnings about type sizes
#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4244)

	CDialog* dlg = NULL;

	switch (iMsg)
	{
	case WM_INITDIALOG:
		dlg = (CDialog*) lParam;

		if(dlg)
		{
			// Save it in the window handle.
			SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)dlg);
			// Store the window handle in the object.
			dlg->hWnd = hWnd;
			//dlg->hParent = hWnd;
			dlg->OnInitDialog();
		}
		return TRUE;
	default:
		dlg = (CDialog*) GetWindowLongPtr(hWnd, GWL_USERDATA);
	}

	if(dlg)
		return dlg->OnMessage(iMsg,wParam,lParam);

	return FALSE;
	//return DefWindowProc (hWnd, iMsg, wParam, lParam)
#pragma warning(pop)
}

LRESULT CDialog::OnMessage(UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{
	case WM_COMMAND:
		return OnCommand(wParam,lParam);
	case WM_NOTIFY:
		return OnNotify(wParam,lParam);
	case WM_CLOSE:
		return OnClose();
	case WM_DESTROY:
		return OnDestroy();
	case WM_HSCROLL:
		OnHScroll(wParam,lParam);
	case WM_INITMENUPOPUP:
		OnInitMenuPopUp((HMENU)wParam,lParam);
	}

	return FALSE;
}

UINT CDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int result=DLGMESS_OPEN;

	//if((HWND)lParam)
	{
		//switch(LOWORD(wParam))
		//{
		//case IDCANCEL:
		//	if(OnCancel()==DLGMESS_CLOSE) KillDialog(wParam);
		//	return TRUE;
		//case IDOK:
		//	if(OnOK()==DLGMESS_CLOSE) KillDialog(wParam);
		//	return TRUE;
		//}

		switch(HIWORD(wParam))
		{
		case BN_CLICKED:
			result=OnBnClicked(LOWORD(wParam));
			break;
		case EN_CHANGE:
			OnEnChange(LOWORD(wParam));
			break;
		case CBN_SELCHANGE:
			OnCbnSelchange(LOWORD(wParam));
			break;
		}
	}

	if(result==DLGMESS_CLOSE) OnKillDialog(wParam);

	return TRUE;
}

UINT CDialog::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR h=(LPNMHDR)lParam;
	GString str;

	switch(h->code)
	{
	case NM_CLICK:
		OnNmClicked((UINT)h->idFrom);
		break;
	//case NM_DBLCLK:
	//	MessageBox(_T("NM_DBLCLK"));
	//	break;
	//case NM_KILLFOCUS:
	//	MessageBox(_T("NM_KILLFOCUS"));
	//	break;
	//case NM_RCLICK:
	//	MessageBox(_T("NM_RCLICK"));
	//	break;
	//case NM_RDBLCLK:
	//	MessageBox(_T("NM_RDBLCLK"));
	//	break;
	//case NM_RETURN:
	//	MessageBox(_T("NM_RETURN"));
	//	break;
	//case NM_SETFOCUS:
	//	MessageBox(_T("NM_SETFOCUS"));
	//	break;
	case LVN_ITEMCHANGED:
		OnLvnItemChanged((LPNMLISTVIEW)lParam);
		break;
	case TCN_SELCHANGING:
		OnTcnSelChanging(h);
		break;
	case TCN_SELCHANGE:
		OnTcnSelChange(h);
		break;
	default:
		return FALSE;
	//case TCN_KEYDOWN:
	//	MessageBox(_T("TCN_KEYDOWN"));
	//	break;
	}

	return TRUE;
}

typedef void (CDialog::*func)();

// ---- buttons ----
UINT CDialog::OnBnClicked(UINT itemID)
{
	switch(itemID)
	{
	case IDOK:
		return OnOK();
	case IDCANCEL:
		return OnCancel();
	}

	for(int i=0, size=(int)bnClickControls.size(); i<size; i++)
	{
		if(bnClickControls[i].id==itemID)
		{
			(this->*bnClickControls[i].ptr)();
			break;
		}
	}

	return DLGMESS_OPEN;
}

void CDialog::SetClickControl(UINT uID, CDialog::FuncPtr ptr)
{
	tagDlgItem i={uID,ptr};
	bnClickControls.push_back(i);
}

// ---- edit controls ---
void CDialog::SetEnChangeControl(UINT uID, CDialog::FuncPtr ptr)
{
	tagDlgItem i={uID,ptr};
	enChangeControls.push_back(i);
}

void CDialog::OnEnChange(UINT itemID)
{
	for(int i=0, size=(int)enChangeControls.size(); i<size; i++)
	{
		if(enChangeControls[i].id==itemID)
		{
			(this->*enChangeControls[i].ptr)();
			break;
		}
	}
}

// ---- lists ----
void CDialog::SetNmClickControl(UINT uID, CDialog::FuncPtr ptr)
{
	tagDlgItem i={uID,ptr};
	nmClickControls.push_back(i);
}

void CDialog::OnNmClicked(UINT itemID)
{
	for(int i=0, size=(int)nmClickControls.size(); i<size; i++)
	{
		if(nmClickControls[i].id==itemID)
		{
			(this->*nmClickControls[i].ptr)();
			break;
		}
	}
}

void CDialog::SetNmDClickControl(UINT uID, CDialog::FuncPtr ptr)
{
	tagDlgItem i={uID,ptr};
	nmDClickControls.push_back(i);
}

void CDialog::SetNmRClickControl(UINT uID, CDialog::FuncPtr ptr)
{
	tagDlgItem i={uID,ptr};
	nmRClickControls.push_back(i);
}

void CDialog::SetNmRDClickControl(UINT uID, CDialog::FuncPtr ptr)
{
	tagDlgItem i={uID,ptr};
	nmRDClickControls.push_back(i);
}

void CDialog::SetLvnItemControl(UINT uID, CDialog::FuncPtr ptr)
{
	tagDlgItem i={uID,ptr};
	lvnItemChangedControls.push_back(i);
}

void CDialog::OnLvnItemChanged(LPNMLISTVIEW lpListView)
{
	for(int i=0, size=(int)lvnItemChangedControls.size(); i<size; i++)
	{
		if(nmClickControls[i].id==lpListView->hdr.idFrom)
		{
			(this->*lvnItemChangedControls[i].ptr)();
			break;
		}
	}
}

// ---- control boxes ---
void CDialog::SetCbnSelchangeControl(UINT uID, CDialog::FuncPtr ptr)
{
	tagDlgItem i={uID,ptr};
	cbnSelchangeControls.push_back(i);
}

void CDialog::OnCbnSelchange(UINT itemID)
{
	for(int i=0, size=(int)cbnSelchangeControls.size(); i<size; i++)
	{
		if(cbnSelchangeControls[i].id==itemID)
		{
			(this->*cbnSelchangeControls[i].ptr)();
			break;
		}
	}
}

////////////////////////////////////////////////
////////////////////////////////////////////////
CFileDialog::CFileDialog()
{
	cFileName=new TCHAR[_MAX_PATH];
	szTitleName=new TCHAR[_MAX_FNAME + _MAX_EXT];
	ofn=new OPENFILENAME;
}

CFileDialog::~CFileDialog()
{
	delete[] cFileName;
	delete[] szTitleName;
	delete[] ofn;
}

void CFileDialog::Create(HWND hWnd, LPCTSTR filter, LPCTSTR title, DWORD flags, bool bOpenDialog)
{
	isOpenDialog=bOpenDialog;
	cFileName[0] = '\0';

	// assign and fix filter
	szFilter=filter;
	szFilter.Replace(_T('|'),('\0'));
	// assign title
	szTitle=title;

	ZeroMemory(ofn,sizeof(OPENFILENAME));
	ofn->lStructSize = sizeof (OPENFILENAME);
	ofn->hwndOwner = hWnd;
	//ofn->hInstance = NULL;
	ofn->lpstrFilter = szFilter;
	//ofn->lpstrCustomFilter = NULL;
	//ofn->nMaxCustFilter = 0;
	//ofn->nFilterIndex = 0;
	ofn->lpstrFile = cFileName;
	ofn->nMaxFile = _MAX_PATH;
	ofn->lpstrFileTitle = szTitleName;
	ofn->nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
	//ofn->lpstrInitialDir = NULL;
	ofn->lpstrTitle = szTitle.GetBuffer();
	ofn->Flags = flags;
	//ofn->nFileOffset = 0;
	//ofn->nFileExtension = 0;
	//ofn->lpstrDefExt = NULL;
	//ofn->lCustData = 0L;
	//ofn->lpfnHook = NULL;
	//ofn->lpTemplateName = NULL;
}

int CFileDialog::DoModal()
{
	if(isOpenDialog) return GetOpenFileName (ofn);
	
	return GetSaveFileName(ofn);
}
