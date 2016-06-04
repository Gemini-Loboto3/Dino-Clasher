#pragma once
#include "dialog.h"
#include "TextureCtrl.h"
#include "..\common\dinocrisis2.h"

class CMainDlg : public CDialog
{
public:
	enum { IDD = IDD_MAIN };
	CMainDlg();

	virtual void OnInitDialog();
	virtual void OnKillDialog(WPARAM wParam) { SendMessage(*this,WM_CLOSE,0,0); }
	virtual int OnClose() { DestroyWindow(*this); return TRUE; }
	virtual int OnDestroy() { PostQuitMessage(0); return TRUE; }
	virtual void OnHScroll(WPARAM wParam, LPARAM lParam);
	virtual void OnInitMenuPopUp(HMENU hMenu, LPARAM lParam);

	void OnMenuClickFileOpen();
	void OnMenuClickExportRawData();
	void OnMenuClickAbout();

	void OnListClick();
	void OnCbnSelChangeDepth();
	void OnCbnSelChangePalette();

	void PopulateList();
	void FillPaletteList();

	CListCtrl m_list;			// segment selector
	CTextureCtrl m_texture;		// texture preview
	CComboBoxCtrl m_depth, m_palette;
	CTextureCtrl m_graphics;	// texture floating bar
	CSliderCtrl m_palx, m_paly;	// texture palette sliders
	CEditCtrl m_address, m_gfx_x, m_gfx_y;
	CSpinControl m_gfx_sx, m_gfx_sy;
	CDialogCtrl m_bytesize, m_gfx_w, m_gfx_h;

	CPackageDC pack;	// dc data wrapper
	int cur_sel, cur_cx, cur_cy;

	std::vector<int> pal_ref;
};
