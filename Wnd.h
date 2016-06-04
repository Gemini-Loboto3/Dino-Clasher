#pragma once
#include "stdafx.h"
#include <windowsx.h>
#include <commctrl.h>
#include <types.h>
#include "gstring.h"
#include "winapi.h"

////////////////////////////////////
class CWnd
{
public:
	CWnd();

	operator HWND() const {return hWnd;}
	virtual inline HWND GetWnd() {return hWnd;}
	virtual int PreTranslateMessage(MSG* pMsg) { return 0; }

	inline bool IsWindowEnabled() { return ::IsWindowEnabled(*this)&1; }
	inline void EnableWindow(BOOL bEnable=TRUE) { ::EnableWindow(GetWnd(),bEnable); }
	inline void GetClientRect(RECT *rect) { ::GetClientRect(*this,rect); }
	inline void GetClientRect(HWND hWnd, RECT* rect) { ::GetClientRect(hWnd,rect); }
	//inline void GetFocus() { hWnd=::GetFocus (); }
	inline UINT GetID() { return uID; }
	inline HWND GetParent() { return hParent; }
	inline DWORD GetStyle() { return GetWindowLongPtr(hWnd, GWL_STYLE); }
	inline HWND GetTopWindow() { return ::GetTopWindow(*this); }
	inline void SetFocus() { ::SetFocus(*this); }
	inline void SetStyle(DWORD dwStyle) { SetWindowLongPtr(GetWnd(), GWL_STYLE, dwStyle); }
	inline BOOL SetWindowText(LPCTSTR lpString) { return ::SetWindowText(hWnd,lpString); }
	inline int MessageBox(LPCTSTR lpText, LPCTSTR lpCaption=NULL, UINT uType=MB_OK) { return ::MessageBox(hWnd,lpText,lpCaption,uType); }

	void Invalidate(BOOL bErase=TRUE);
	GString GetWindowText();
	GString GetWindowText(UINT uID);
	void SetDlgItem(HWND hWnd, UINT uID);
	void SetWindowPos(HWND hWndInsertAfter, int x, int y, int w, int h);
	void SetWindowSize(int w, int h);
	void ShowWindow(int nCmdShow);

protected:
	HWND hWnd, hParent;
	UINT uID;
};

////////////////////////////////////
class CDialogCtrl : public CWnd
{
public:
	virtual void SetDlgItem(HWND hWnd, UINT uID);

	static LRESULT CALLBACK Procedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	WNDPROC oldProcedure;
};

////////////////////////////////////
class CButton : public CWnd
{
public:
	inline int GetCheck() { return Button_GetCheck(*this); }
	inline void SetCheck(int check) { Button_SetCheck(*this,check); }

	inline int GetState() { return Button_GetState(*this); }
	inline void SetState(int state) { Button_SetState(*this,state); }
};

////////////////////////////////////
class CEditCtrl : public CWnd
{
public:
	inline void SetText(LPCTSTR text) { SetWindowText(text); }
	inline void GetSelection(int &start, int &end) { SendMessage(*this,EM_GETSEL,(WPARAM)&start,(LPARAM)&end); }

	u32 GetValue();
	void SetValue(u32 value, bool has_sign=false);
};

////////////////////////////////////
class CSpinControl : public CWnd
{
public:
	void SetRange(short low, short high);
	void SetRange32(int low, int high);
};

////////////////////////////////////
class CSliderCtrl : public CWnd
{
public:
	void SetRange(short low, short high, bool bRedraw=true);
	void SetRange32(int low, int high, bool bRedraw=true);

	inline int GetPos() { return (int)::SendMessage(*this,TBM_GETPOS,0,0); }
	inline void SetPos(int pos, bool bRedraw=true) { ::SendMessage(*this,TBM_SETPOS,(WPARAM)bRedraw,(LPARAM)pos); }
};

////////////////////////////////////
class CListCtrl : public CWnd
{
public:
	inline void DeleteItem(int index) { ListView_DeleteItem(*this, index); }
	void InsertItem(int index, LPCTSTR text);
	void SetItemText(int index, LPCTSTR text);
	void SetType(int type);

	// combo box style messages for simple lists
	inline int GetCurSel() { return (int)SendMessage(*this,LVM_GETNEXTITEM,-1,LVNI_FOCUSED); }
	inline int GetCount() { return (int)ListView_GetItemCount(*this); }
	void SetCurSel(int index);	// use -1 to deselect

	enum LIST_TYPES
	{
		LT_ICON_SMALL,
		LT_ICON_BIG,
		LT_REPORT,
		LT_LIST
	};
};

////////////////////////////////////
class CComboBoxCtrl : public CWnd
{
public:
	inline void AddString(LPCTSTR text) { SendMessage(*this,CB_ADDSTRING,0,(LPARAM)text); }
	inline int GetCurSel() { return (int)SendMessage(*this,CB_GETCURSEL,0,0); }
	inline void SetCurSel(int index) { SendMessage(*this,CB_SETCURSEL,index,0); }
	inline void Reset() { SendMessage(*this,CB_RESETCONTENT,0,0); }
};
