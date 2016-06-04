#include "MainDlg.h"
#include "AboutDlg.h"
#include "FolderDlg.h"

static LPCTSTR dc_types[]=
{
	_T("Generic data"),		// 0
	_T("Texture"),			// 1
	_T("Palette"),			// 2
	_T("Gian header"),		// 3
	_T("Gian body"),		// 4
	_T("Gian tracker"),		// 5
	_T("Unknown"),			// 6
	_T("Lzss data"),		// 7
	_T("Lzss texture")		// 8
};

CMainDlg::CMainDlg()
	: CDialog(NULL,IDD)
{
	// menu handlers
	// FILE
	EventClick(ID_FILE_OPEN, CMainDlg::OnMenuClickFileOpen);
	// TOOLS
	EventClick(ID_TOOLS_EXPORTRAWDATA, CMainDlg::OnMenuClickExportRawData);
	// ABOUT
	EventClick(IDM_ABOUT, CMainDlg::OnMenuClickAbout);

	EventNmClick(IDC_LIST, CMainDlg::OnListClick);
	EventLvnItemChanged(IDC_LIST, CMainDlg::OnListClick);

	EventCbnSelChange(IDC_DEPTH, CMainDlg::OnCbnSelChangeDepth);
	EventCbnSelChange(IDC_PALETTE, CMainDlg::OnCbnSelChangePalette);

	cur_cx=cur_cy=0;
}

void CMainDlg::OnCbnSelChangeDepth()
{
	OnListClick();
}

void CMainDlg::OnCbnSelChangePalette()
{
	int cur_pal=m_palette.GetCurSel();
	int x=0, y=0;

	// if we're using a real palette and not greyscale
	if(cur_pal>0)
	{
		// get segment reference to this palette
		int ref=pal_ref[cur_pal-1];
		DC2_ENTRY_GFX* pal_entry=(DC2_ENTRY_GFX*)&pack.ent[ref];
		// retrieve ranges for slider controls
		x=pal_entry->w/16-1;
		y=pal_entry->h-1;
	}

	// set slider ranges
	m_palx.EnableWindow(x>0 ? 1 : 0);
	m_paly.EnableWindow(y>0 ? 1 : 0);
	m_palx.SetRange(0,x);
	m_paly.SetRange(0,y);
	// always restore positions
	m_palx.SetPos(0);
	m_paly.SetPos(0);

	OnListClick();
}

void CMainDlg::OnListClick()
{
	cur_sel=m_list.GetCurSel();

	if(cur_sel==-1) return;

	cur_cx=m_palx.GetPos();
	cur_cy=m_paly.GetPos();

	int cur_pal=m_palette.GetCurSel();
	DC2_ENTRY_GFX* pal_entry=NULL;
	u16* pal_data=NULL;
	// search palette index
	if(cur_pal>0)
	{
		int ref=pal_ref[cur_pal-1];
		pal_entry=(DC2_ENTRY_GFX*)&pack.ent[ref];
		pal_data=(u16*)pack.segment[ref];
	}

	switch(pack.ent[cur_sel].type)
	{
	case GET_TEXTURE:
	case GET_LZSS1:
	case GET_PALETTE:
		{
			m_address.EnableWindow(0); m_address.SetText(_T(""));
			m_bytesize.EnableWindow(0); m_bytesize.SetWindowText(_T("---"));
			GString str;
			DC2_ENTRY_GFX* g=(DC2_ENTRY_GFX*)&pack.ent[cur_sel];
			m_gfx_x.EnableWindow(); m_gfx_x.SetValue(g->x);
			m_gfx_y.EnableWindow(); m_gfx_y.SetValue(g->y);
			str.Format(_T("%d"),g->w); m_gfx_w.EnableWindow(); m_gfx_w.SetWindowText(str);
			str.Format(_T("%d"),g->h); m_gfx_h.EnableWindow(); m_gfx_h.SetWindowText(str);
		}
		m_texture.EnableWindow();
		m_texture.SetPalette(pal_entry,pal_data,m_depth.GetCurSel(),cur_cx*16,cur_cy);
		// if we're handling a palette, do not show the texture
		if(pack.ent[cur_sel].type==GET_PALETTE) m_graphics.ShowWindow(SW_HIDE);
		// otherwise enable both palette and texture displays
		else
		{
			m_graphics.SetGraphics((DC2_ENTRY_GFX*)&pack.ent[cur_sel],pack.segment[cur_sel],pal_entry,pal_data,m_depth.GetCurSel(),cur_cx*16,cur_cy);
			m_graphics.ShowWindow(SW_SHOWNOACTIVATE);
		}
		break;
	default:
		{
			GString str;
			m_address.EnableWindow(); str.Format(_T("%X"),pack.ent[cur_sel].reserve[0]); m_address.SetText(str);
			m_bytesize.EnableWindow(); str.Format(_T("%d"),pack.ent[cur_sel].size); m_bytesize.SetWindowText(str);
			m_gfx_x.EnableWindow(0); m_gfx_x.SetText(_T(""));
			m_gfx_y.EnableWindow(0); m_gfx_y.SetText(_T(""));
			m_gfx_w.EnableWindow(0); m_gfx_w.SetWindowText(_T("---"));
			m_gfx_h.EnableWindow(0); m_gfx_h.SetWindowText(_T("---"));
			m_texture.EnableWindow(0);
		}
		m_texture.SetVoid();
		m_graphics.ShowWindow(SW_HIDE);
	}
}

void CMainDlg::OnMenuClickFileOpen()
{
	CFileDialog dlg;
	dlg.Create(*this,_T("Dino Crisis 1/2 common formats|*.DAT;*.TEX;*.DBS|All files (*.*)|*.*||"),_T("Open Dino Crisis data"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST);
	extern TCHAR szTitle[100];

	if(dlg.DoModal())
	{
		GString str;
		// update title
		str.Format(_T("%s [%s]"),szTitle,(LPCTSTR)dlg.GetFileName());
		SetWindowText(str);
		// load file
		pack.Open(dlg.GetFileName());
		// fill list
		PopulateList();
		FillPaletteList();
		// set first entry in the list
		m_list.SetCurSel(pack.ent.size()>0 ? 0 : -1);
		OnListClick();
	}
}

void CMainDlg::OnMenuClickExportRawData()
{
	CFileDialog dlg;
	dlg.Create(*this,_T("XML project (*.xml)|*.XML|All files (*.*)|*.*||"),_T("Save extraction project"),OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,false);

	if(dlg.DoModal()==IDOK)
	{
		pack.ExtractRaw(dlg.GetFileName());
	}
}

void CMainDlg::OnMenuClickAbout()
{
	CAboutDlg dlg(*this);
	dlg.DoModal();
}

void CMainDlg::FillPaletteList()
{
	m_palette.Reset();
	GString str;

	// empty palette reference
	pal_ref.clear();

	// set the first entry to always be a greyscale
	m_palette.AddString(_T("Greyscale"));
	// cycle all palette entries and add them to the list
	for(int i=0, cnt=0; i<(int)pack.ent.size(); i++)
	{
		if(pack.ent[i].type==GET_PALETTE)
		{
			DC2_ENTRY_GFX *g=(DC2_ENTRY_GFX*)&pack.ent[i];
			// add palette reference
			pal_ref.push_back(i);
			// add palette to list
			str.Format(_T("%d (%dx%d)"),cnt+1,g->w,g->h);
			m_palette.AddString(str);
			cnt++;
		}
	}

	m_palette.SetCurSel(0);
}

void CMainDlg::OnInitDialog()
{
	SendMessage(*this, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_DINOCLASHER)));
	SendMessage(*this, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_DINOCLASHER)));

	// -- ASSIGN CONTROLS TO WRAPPERS --
	m_list.SetDlgItem(*this,IDC_LIST);
	m_texture.SetDlgItem(*this,IDC_PREVIEW);
	// generic data
	m_address.SetDlgItem(*this,IDC_ADDRESS);
	m_bytesize.SetDlgItem(*this,IDC_BYTESIZE);
	// graphics data
	m_gfx_x.SetDlgItem(*this,IDC_GFX_X); m_gfx_sx.SetDlgItem(*this,IDC_GFX_SX); m_gfx_sx.SetRange(0,1023);
	m_gfx_y.SetDlgItem(*this,IDC_GFX_Y); m_gfx_sy.SetDlgItem(*this,IDC_GFX_SY); m_gfx_sy.SetRange(0,511);
	m_gfx_w.SetDlgItem(*this,IDC_GFX_W);
	m_gfx_h.SetDlgItem(*this,IDC_GFX_H);
	// graphics preview
	m_depth.SetDlgItem(*this,IDC_DEPTH);
	m_palette.SetDlgItem(*this,IDC_PALETTE);
	m_palx.SetDlgItem(*this,IDC_PALX);
	m_paly.SetDlgItem(*this,IDC_PALY);

	// set empty ranges for clut sliders
	m_palx.SetRange(0,0);
	m_paly.SetRange(0,0);

	// create the gfx display window
	CRect rect;
	GetWindowRect(*this,&rect);
	m_graphics.Create(*this,rect);

	// set data for the depth combobox
	m_depth.AddString(_T("4 bit"));
	m_depth.AddString(_T("8 bit"));
	m_depth.AddString(_T("15 bit"));
	m_depth.SetCurSel(0);

	// set initial data for clut combobox
	m_palette.AddString(_T("Greyscale"));
	m_palette.SetCurSel(0);

	// list always shows selection and renders as grid
	SendMessage(m_list, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// set headers for segment list
	LV_COLUMN lvC;
	GetClientRect(m_list,&rect);
	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.fmt = LVCFMT_LEFT;	// left-align column
	lvC.cx = 25;			// width of column in pixels
	lvC.pszText = _T("ID");
	ListView_InsertColumn(m_list, 0, &lvC);
	// type
	lvC.pszText = _T("Type");
	lvC.cx = 85;
	ListView_InsertColumn(m_list, 1, &lvC);
	// address
	lvC.pszText = _T("Address");
	lvC.cx = rect.Width()-lvC.cx-25-GetSystemMetrics(SM_CXVSCROLL); // width of column in pixels
	ListView_InsertColumn(m_list, 2, &lvC);

	// initial dialog update
	PopulateList();
}

void CMainDlg::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	if(m_palx.GetWnd()==(HWND)lParam || m_paly.GetWnd()==(HWND)lParam)
		OnListClick();
}

void CMainDlg::OnInitMenuPopUp(HMENU hMenu, LPARAM lParam)
{
	switch((UINT)LOWORD(lParam))
	{
	case 0:	// FILE
		EnableMenuItem(hMenu,ID_FILE_SAVE,MF_DISABLED | MF_GRAYED);
		break;
	case 1:	// TOOLS
		EnableMenuItem(hMenu,ID_TOOLS_EXPORTRAWDATA,pack.ent.size()>0 ? MF_ENABLED : MF_DISABLED | MF_GRAYED);
		break;
	case 2:	// HELP
		break;
	}
}

void CMainDlg::PopulateList()
{
	LV_ITEM lvI;
	lvI.mask = LVIF_TEXT | LVIF_STATE;
	lvI.state = 0;
	lvI.stateMask = 0;

	ListView_DeleteAllItems(m_list);

	GString str;
	for(int i=0; i<(int)pack.ent.size(); i++)
	{
		str.Format(_T("%d"),i);
		lvI.pszText=str.GetBuffer();
		lvI.iItem = i;
		lvI.iSubItem = 0;
		ListView_InsertItem(m_list, &lvI);
		ListView_SetItemText(m_list,i,1,(LPTSTR)dc_types[pack.ent[i].type]);

		switch(pack.ent[i].type)
		{
			
			case GET_DATA:		// raw data
			case GET_UNK:		// unknown entry
			case GET_LZSS0:		// compressed data
			case GET_SNDH:		// VAG header 'Gian'
			case GET_SNDB:		// VAG body
			case GET_SNDE:		// configuration for sound samples?
				str.Format(_T("%08X"),pack.ent[i].reserve[0]);
				break;
			case GET_TEXTURE:	// stripped TIM pixel
			case GET_PALETTE:	// stripped TIM clut
			case GET_LZSS1:		// compressed texture
				{
					DC2_ENTRY_GFX *e=(DC2_ENTRY_GFX*)&pack.ent[i];
					str.Format(_T("x=%d, y=%d"),e->x,e->y);
				}
				break;
		}
		ListView_SetItemText(m_list,i,2,str.GetBuffer());
	}

	//for(int i=0; i<(int)pack.ent.size(); i++)
	//{
	//	str.Format(_T("%02d.bin"),i);
	//	FlushFile(str,pack.segment[i],pack.ent[i].size);
	//}
}
