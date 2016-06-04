#include <windowsx.h>
#include "Wnd.h"
#include "dialog.h"

#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Msimg32.lib")

CWnd::CWnd()
{
	hWnd=NULL;
}

void CWnd::SetDlgItem(HWND hWnd, UINT uID)
{
	this->uID=uID;
	this->hWnd=GetDlgItem(hWnd,uID);
	this->hParent=hWnd;
}

void CWnd::SetWindowPos(HWND hWndInsertAfter, int x, int y, int w, int h)
{
	if(w==0 || h==0)
		::SetWindowPos(GetWnd(),hWndInsertAfter,x,y,0,0,SWP_NOSIZE);
	else
		::SetWindowPos(GetWnd(),hWndInsertAfter,x,y,w,h,SWP_SHOWWINDOW);
}

void CWnd::SetWindowSize(int w, int h)
{
	::SetWindowPos(*this,NULL,0,0,w,h,SWP_NOMOVE);
}

void CWnd::ShowWindow(int nCmdShow)
{
	::ShowWindow(hWnd,nCmdShow);
}

// retrieve window text
GString CWnd::GetWindowText()
{
	GString str;

	// retreive item size
	int iWndTextSize=(int)SendMessage(GetWnd(), EM_GETLIMITTEXT, 0, 0);
	// allocate and read
	TCHAR *lpWndText=new TCHAR[iWndTextSize];
	::GetWindowText(GetWnd(), lpWndText, iWndTextSize);
	// copy to the actual string
	str=lpWndText;
	// deallocate buffer
	delete[] lpWndText;

	return str;
}

void CWnd::Invalidate(BOOL bErase)
{
	InvalidateRect (GetWnd(), NULL, bErase);
	UpdateWindow (GetWnd());
}

// retrieve window control text
GString CWnd::GetWindowText(UINT uID)
{
	GString str;

	// retreive item size
	int iWndTextSize=(int)SendMessage(GetWnd(), EM_GETLIMITTEXT, 0, 0);
	// allocate and read
	TCHAR *lpWndText=new TCHAR[iWndTextSize];
	GetDlgItemText(GetWnd(), uID, lpWndText, iWndTextSize);
	// copy to the actual string
	str=lpWndText;
	// deallocate buffer
	delete[] lpWndText;

	return str;
}

//////////////////////////////////////////////////
// Various Windows controls						//
//////////////////////////////////////////////////
void CDialogCtrl::SetDlgItem(HWND hWnd, UINT uID)
{
	this->uID=uID;
	this->hWnd=GetDlgItem(hWnd,uID);
	this->hParent=hWnd;

#pragma warning(disable:4244)
#pragma warning(disable:4312)
	SetWindowLongPtr (this->hWnd, GWL_USERDATA, (LONG_PTR)this);
	oldProcedure=(WNDPROC)SetWindowLongPtr (*this,GWL_WNDPROC,(LONG_PTR)Procedure);
#pragma warning(default:4312)
#pragma warning(default:4244)
}

LRESULT CALLBACK CDialogCtrl::Procedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam/*, UINT_PTR uIdSubclass, DWORD_PTR dwRefData*/)
{
#pragma warning(disable:4312)
	CDialogCtrl *ctrl = (CDialogCtrl*) GetWindowLongPtr(hWnd, GWL_USERDATA);
#pragma warning(default:4312)

	ctrl->OnMessage(uMsg,wParam,lParam);

	return CallWindowProc(ctrl->oldProcedure,hWnd,uMsg,wParam,lParam);
}

void CDialogCtrl::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
}

/*----------------------------------------------*\
 * Edit control									*
\*----------------------------------------------*/
void CEditCtrl::SetValue(u32 value, bool has_sign)
{
	GString str;
	str.Format(_T("%d"),(has_sign ? (signed)value : value));
	SetText(str);
}

u32 CEditCtrl::GetValue()
{
	u32 value;
	GString s=GetWindowText();

	_stscanf(s,_T("%d"),&value);
	
	return value;
}

/*----------------------------------------------*\
 * Spin button control / Up-Down control		*
\*----------------------------------------------*/
void CSpinControl::SetRange(short low, short high)
{
	::SendMessage(*this,UDM_SETRANGE,0,MAKELPARAM(high,low));
}

void CSpinControl::SetRange32(int low, int high)
{
	::SendMessage(*this,UDM_SETRANGE32,(WPARAM)low,(LPARAM)high);
}

/*----------------------------------------------*\
 * Slider control / track bar					*
\*----------------------------------------------*/
void CSliderCtrl::SetRange(short low, short high, bool bRedraw)
{
	::SendMessage(*this,TBM_SETRANGE,(WPARAM)bRedraw,(LPARAM)MAKELONG(low,high));
}

void CSliderCtrl::SetRange32(int low, int high, bool bRedraw)
{
	::SendMessage(*this,TBM_SETRANGEMIN,(WPARAM)0,(LPARAM)low);
	::SendMessage(*this,TBM_SETRANGEMAX,(WPARAM)bRedraw,(LPARAM)high);
}

/*----------------------------------------------*\
 * List control									*
\*----------------------------------------------*/
void CListCtrl::SetCurSel(int index)
{
	if(index==-1)
	{
		ListView_SetItemState(*this,-1,0,LVIS_FOCUSED | LVIS_SELECTED);
		return;
	}
	ListView_SetItemState(*this,index,LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);
}

void CListCtrl::SetType(int type)
{
	DWORD style=GetStyle()&(~LVS_TYPEMASK);
	switch(type)
	{
	case LT_ICON_SMALL:
		style |= LVS_SMALLICON;
		break;
	case LT_ICON_BIG:
		style |= LVS_ICON;
		break;
	case LT_REPORT:
		style |= LVS_REPORT;
		break;
	case LT_LIST:
		style |= LVS_LIST;
		break;
	}
	SetStyle(style);
}
