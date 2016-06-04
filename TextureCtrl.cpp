#include "TextureCtrl.h"
#include "MemDc.h"
#include "resource.h"
#include "Dialog.h"

CTextureCtrl::CTextureCtrl(void)
{
	palette=new RGBQUAD[256];
	for(int i=0; i<256; i++) palette[i].rgbReserved=0;
	SetGreyscale(8);

	grid.LoadBitmap(MAKEINTRESOURCE(IDB_GRID));

	floating=false;
}

CTextureCtrl::~CTextureCtrl(void)
{
	delete[] palette;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/* create control as deattached window */
void CTextureCtrl::Create(HWND parent, CRect &rect)
{
	LPCTSTR szWindowClass=_T("GfxWndClass");
	LPCTSTR szTitle=_T("Graphics display");

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;
	RegisterClassEx(&wcex);

	POINT point={0};
	ClientToScreen(parent,&point);

	hParent=parent;
	hWnd=CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPED | WS_CAPTION ,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		60, 60,
		parent,
		NULL,
		GetModuleHandle(NULL),
		this);

	floating=true;
}

// handler for deattached window
CTextureCtrl *pWindow;

// procedure for deattached window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CREATESTRUCT* pCS;

	switch (message)
	{
	case WM_CREATE:
		pCS = (CREATESTRUCT*) lParam;
		pWindow = (CTextureCtrl*) pCS->lpCreateParams;
		//pWindow->OnCreate(hWnd,message,wParam,lParam);
#pragma warning(disable:4244)
		SetWindowLongPtr(hWnd,GWL_USERDATA,(LONG_PTR)pWindow);
#pragma warning(default:4244)
		break;
	default:
		if(pWindow) return pWindow->OnMessageEx(message, wParam, lParam);
		else return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// message handler for dialog control
void CTextureCtrl::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_ERASEBKGND:
		break;
	}
}

#define IDM_SAVE_AS	WM_USER+1

// message handler for deattached window
LRESULT CTextureCtrl::OnMessageEx(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_RBUTTONUP:
		OnRButtonUp(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		break;
	case WM_ERASEBKGND:
		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void CTextureCtrl::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint (hWnd, &ps);

	Draw(hdc);

	EndPaint(hWnd,&ps);
}

// enable menu for bitmap export with right click
void CTextureCtrl::OnRButtonUp(int cx, int cy)
{
	POINT pt;
	pt.x = cx;
	pt.y = cy;
	ClientToScreen(*this, &pt); 

	HMENU menu=CreatePopupMenu();
	AppendMenu(menu,MF_STRING,IDM_SAVE_AS, _T("&Save as..."));
	UINT msg=TrackPopupMenu(menu,TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN,pt.x,pt.y,0,*this,NULL);

	if(msg==IDM_SAVE_AS)
	{
		CFileDialog dlg;
		dlg.Create(*this,_T("Bitmap image (*.bmp)|*.BMP|All files (*.*)|*.*||"),_T("Save converted texture"),OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,false);
		if(dlg.DoModal()==IDOK)
		{
			tex_cache.SaveBitmap(dlg.GetFileName());
		}
	}
}

void CTextureCtrl::Draw(HDC hdc)
{
	// buffered context
	CMemDC memdc(hdc,*this,false);

	CRect rect;
	CBrush brush;

	GetClientRect(&rect);

	// draw a grey rect if disabled
	if(!IsWindowEnabled())
	{
		brush.CreateSolidBrush(RGB(128,128,128));
		FillRect(memdc,&rect,brush);
		return;
	}

	// create destination context for drawing the bitmaps
	HDC bmp_dc=CreateCompatibleDC(memdc);

	// select grid
	HBITMAP old_bmp=(HBITMAP)SelectObject(bmp_dc,grid);
	// draw grid
	for(int x=0; x<rect.Width(); x+=grid.w)
		for(int y=0; y<rect.Height(); y+=grid.h)
			BitBlt(memdc,x,y,64,64,bmp_dc,0,0,SRCCOPY);

	// draw bitmap, if one is loaded
	if(texture.w>0 && texture.h>0)
	{
		SelectObject(bmp_dc,texture);
		//TransparentBlt(memdc,0,0,texture.w,texture.h,bmp_dc,0,0,texture.w,texture.h,RGB(255,0,255));
		BitBlt(memdc,0,0,texture.w,texture.h,bmp_dc,0,0,SRCCOPY);
	}

	// flush memory dc
	SelectObject(bmp_dc,old_bmp);
	DeleteObject(bmp_dc);
}

void CTextureCtrl::SetGreyscale(int depth)
{
	switch(depth)
	{
	case 4:
		for(int i=0; i<16; i++) palette[i].rgbBlue=palette[i].rgbGreen=palette[i].rgbRed=i*16;
		break;
	case 8:
		for(int i=0; i<256; i++) palette[i].rgbBlue=palette[i].rgbGreen=palette[i].rgbRed=i;
		break;
	}
}

static inline u16 GetClutColor(DC2_ENTRY_GFX *pal_entry, u16 *pal_data, int x, int y)
{
	// return transparent if out of range
	if(x>=pal_entry->w || y>=pal_entry->h) return 0;
	// otherwise get the actual color
	return pal_data[x+y*pal_entry->w];
}

#define COLOR_CELL	8

void CTextureCtrl::SetPalette(DC2_ENTRY_GFX *gfx_entry, u16 *gfx_data, int depth, int cx, int cy)
{
	Image img;

	switch(depth)
	{
	case 0: depth=4; break;		// 4 bpp, get 16 colors
	case 1: depth=8; break;		// 8 bpp, get 256 colors
	case 2:		// 15 bpp, no palette needed here
		texture.w=texture.h=0;
		Invalidate();
		return;
	}

	u16* clut=new u16[1<<depth];
	// create greyscale clut
	if(!gfx_entry && !gfx_data)
	{
		SetGreyscale(depth);
		for(int i=0; i<(1<<depth); i++)
			clut[i]=TIMRGB(palette[i].rgbRed,palette[i].rgbGreen,palette[i].rgbBlue);
	}
	// otherwise copy section
	else
		for(int i=0; i<(1<<depth); i++)
			clut[i]=GetClutColor(gfx_entry,gfx_data,i+cx,cy);

	// determine client size
	CRect rect;
	GetClientRect(&rect);

	// create canvas
	img.Create(rect.Width()/COLOR_CELL*COLOR_CELL,rect.Height(),24,NULL);
	// draw clut to bitmap
	for(int i=0; i<(1<<depth); i++)
	{
		RGBQUAD pixel=ClutToRgb(clut[i],0);

		int x=(i%(rect.Width()/COLOR_CELL))*COLOR_CELL;
		int y=(i/(rect.Width()/COLOR_CELL))*COLOR_CELL;
		// set color
		for(int xi=0; xi<COLOR_CELL; xi++)
			for(int yi=0; yi<COLOR_CELL; yi++)
				img.SetPixelAt(x+xi,y+yi,*((u32*)&pixel));
	}

	texture.LoadBitmap(img);
	Invalidate();

	delete[] clut;
}

void CTextureCtrl::SetGraphics(DC2_ENTRY_GFX *gfx_entry, u8 *gfx_data, DC2_ENTRY_GFX *pal_entry, u16 *pal_data, int depth, int cx, int cy)
{
	Tim tim;
	int width;

	switch(depth)
	{
	case 0: width=gfx_entry->w*4; depth=4; break;	// 4 bpp
	case 1: width=gfx_entry->w*2; depth=8; break;	// 8 bpp
	case 2: width=gfx_entry->w; depth=15; break;	// 15 bpp
	}

	if(!pal_entry && !pal_data) SetGreyscale(depth);

	// create and copy pixel
	tim.CreatePixel(width,gfx_entry->h,gfx_entry->x,gfx_entry->y,depth);
	memcpy(tim.image,gfx_data,gfx_entry->w*2*gfx_entry->h);

	if(depth<=8)
	{
		tim.CreateClut(1<<depth,1,0,0);

		// if palette is defined make a straight copy
		if(pal_entry && pal_data)
		{
			for(int i=0; i<(1<<depth); i++)
				tim.clut[i]=GetClutColor(pal_entry,pal_data,cx+i,cy);
		}
		// otherwise set it to greyscale
		else
		{
			for(int i=0; i<(1<<depth); i++)
				tim.clut[i]=TIMRGB(palette[i].rgbRed,palette[i].rgbGreen,palette[i].rgbBlue);
		}
	}
	tex_cache.CreateFromTim(&tim,0,0);
	texture.LoadBitmap(tex_cache);

	if(floating)
	{
		CRect rcClient, rcWind;
		POINT ptDiff;
		GetClientRect(&rcClient);
		GetWindowRect(*this, &rcWind);
		ptDiff.x = rcWind.Width() - rcClient.right;
		ptDiff.y = rcWind.Height() - rcClient.bottom;
		::SetWindowPos(*this,NULL,0,0,width+ptDiff.x, gfx_entry->h+ptDiff.y,SWP_NOMOVE | SWP_NOACTIVATE);
	}
	Invalidate();
}

void CTextureCtrl::SetVoid()
{
	texture.w=texture.h=0;

	Invalidate();
}
