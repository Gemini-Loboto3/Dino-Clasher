#pragma once
#include "wnd.h"
#include "..\common\dinocrisis2.h"

class CTextureCtrl : public CDialogCtrl
{
public:
	CTextureCtrl(void);
	~CTextureCtrl(void);

	void Create(HWND parent, CRect &rect);

	virtual void OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageEx(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnPaint();

	void Draw(HDC hdc);
	void SetGreyscale(int depth);

	void SetPalette(DC2_ENTRY_GFX *gfx_entry, u16 *gfx_data, int depth=0, int cx=0, int cy=0);
	void SetGraphics(DC2_ENTRY_GFX *gfx_entry, u8 *gfx_data, DC2_ENTRY_GFX *pal_entry, u16 *pal_data, int depth, int cx, int cy);
	void SetVoid();

	void OnRButtonUp(int cx, int cy);

private:
	RGBQUAD *palette;
	CBitmap texture, grid;
	Image tex_cache;
	bool floating;
};
