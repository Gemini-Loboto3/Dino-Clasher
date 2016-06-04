#pragma once
#include <windows.h>
#include <Commdlg.h>
#include <vector>
#include "Wnd.h"
#include "resource.h"
#include "commctrl.h"
#include "commdlg.h"

enum DialogMessages
{
	DLGMESS_CLOSE,	// closes a dialog
	DLGMESS_OPEN	// leave open
};

#define EventClick(uID,handler)		SetClickControl(uID,(CDialog::FuncPtr)&handler)

#define EventEnChange(uID,handler)	SetEnChangeControl(uID,(CDialog::FuncPtr)&handler)

#define EventNmClick(uID,handler)	SetNmClickControl(uID,(CDialog::FuncPtr)&handler)
#define EventNmDClick(uID,handler)	SetNmDClickControl(uID,(CDialog::FuncPtr)&handler)
#define EventNmRClick(uID,handler)	SetNmRClickControl(uID,(CDialog::FuncPtr)&handler)
#define EventNmRDClick(uID,handler)	SetNmRDClickControl(uID,(CDialog::FuncPtr)&handler)

#define EventLvnItemChanged(uID,handler)	SetLvnItemControl(uID,(CDialog::FuncPtr)&handler)

#define EventCbnSelChange(uID,handler)		SetCbnSelchangeControl(uID,(CDialog::FuncPtr)&handler)

// the actual dialog data
class CDialog : public CWnd
{
public:
	enum { IDD=NULL };

	//CDialog(UINT id=IDD) {dialogID=id;}
	CDialog(HWND hWnd=NULL, UINT id=IDD);
	virtual ~CDialog(void) {}

	bool Create(HWND hWnd);

	INT_PTR DoModal();
	virtual void OnInitDialog() {}
	virtual UINT OnCommand(WPARAM wParam, LPARAM lParam);
	// return DLGMESS_CLOSE for closing
	virtual UINT OnOK() {return DLGMESS_CLOSE;}
	virtual UINT OnCancel() {return DLGMESS_CLOSE;}
	virtual UINT OnNotify(WPARAM wParam, LPARAM lParam);
	virtual void OnInitMenuPopUp(HMENU hMenu, LPARAM lParam) {}

	virtual void OnKillDialog(WPARAM wParam) { EndDialog(*this,LOWORD(wParam)); }
	virtual int OnClose() { return FALSE; }
	virtual int OnDestroy() { return FALSE; }
	virtual void OnHScroll(WPARAM wParam, LPARAM lParam) { }

	typedef void (CDialog::*FuncPtr)();
	// buttons
	void SetClickControl(UINT uID, CDialog::FuncPtr ptr);
	virtual UINT OnBnClicked(UINT itemID);
	// edit controls
	void SetEnChangeControl(UINT uID, CDialog::FuncPtr ptr);
	virtual void OnEnChange(UINT itemID);
	// lists
	void SetNmClickControl(UINT uID, CDialog::FuncPtr ptr);
	void SetNmDClickControl(UINT uID, CDialog::FuncPtr ptr);
	void SetNmRClickControl(UINT uID, CDialog::FuncPtr ptr);
	void SetNmRDClickControl(UINT uID, CDialog::FuncPtr ptr);
	void SetLvnItemControl(UINT uID, CDialog::FuncPtr ptr);
	virtual void OnNmClicked(UINT itemID);
	virtual void OnLvnItemChanged(LPNMLISTVIEW lpListView);
	// tabs
	virtual void OnTcnSelChanging(LPNMHDR nmhdr) {}
	virtual void OnTcnSelChange(LPNMHDR nmhdr) {}
	// combo boxes
	void SetCbnSelchangeControl(UINT uID, CDialog::FuncPtr ptr);
	virtual void OnCbnSelchange(UINT itemID);

protected:
	// this triggers a dialog
	WINUSERAPI INT_PTR WINAPI DialogBoxParam();
	static LRESULT CALLBACK DialProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessage(UINT iMsg, WPARAM wParam, LPARAM lParam);

	typedef struct tagDlgItem
	{
		UINT id;				// id of the dlg item
		CDialog::FuncPtr ptr;	// function associated
	} DLG_ITEM;

	bool is_modless;
	// buttons
	std::vector<tagDlgItem> bnClickControls;
	// edit controls
	std::vector<tagDlgItem> enChangeControls;
	// lists
	std::vector<tagDlgItem> nmClickControls;
	std::vector<tagDlgItem> nmDClickControls;
	std::vector<tagDlgItem> nmRClickControls;
	std::vector<tagDlgItem> nmRDClickControls;
	std::vector<tagDlgItem> lvnItemChangedControls;
	// combo boxes
	std::vector<tagDlgItem> cbnSelchangeControls;
};

class CFileDialog
{
public:
	CFileDialog();
	~CFileDialog();

	void Create(HWND hWnd, LPCTSTR filter, LPCTSTR title, DWORD flags, bool bOpenDialog=true);
	int DoModal();
	inline GString GetFileName() {return ofn->lpstrFile/*cFileName*/;}
	inline void SetFileName(LPCTSTR filename) { _tcscpy(cFileName,filename); }
private:
	OPENFILENAME *ofn;
	LPTSTR cFileName;
	LPTSTR szTitleName;
	GString szFilter, szTitle;
	bool isOpenDialog;
};
