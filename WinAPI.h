#pragma once
#include <windows.h>
#include "image.h"

void LoadResourcePng(Image &dest, HMODULE hModule, LPCWSTR lpName);

/////////////////////////////////////////////////
class CDC
{
public:
	inline CDC(void) { hdc=NULL; }
	inline CDC(HDC hdc) { this->hdc=hdc; }
	inline ~CDC(void) { DeleteObject(); }

	inline BOOL BitBlt(int srcx, int srcy, int w, int h, HDC hdcSrc, int dstx, int dsty, DWORD mode)
	{
		::BitBlt(*this,srcx,srcy,w,h,hdcSrc,dstx,dsty,mode);
	}
	
	void DeleteObject()
	{
		if(hdc) ::DeleteObject(hdc);
		hdc=NULL;
	}

	HGDIOBJ SelectObject(HGDIOBJ h) { return ::SelectObject(hdc,h); }

	operator HDC() const { return hdc; }
	void operator =(HDC hdc)
	{
		DeleteObject();
		this->hdc=hdc;
	}
	inline HDC GetDC() { return hdc; }

protected:
	HDC hdc;
};

class CClientDC : public CDC
{
public:
	inline ~CClientDC(void) { DeleteObject(); }

	HDC CreateCompatibleDC(HDC hdc)
	{
		DeleteObject();
		return this->hdc=::CreateCompatibleDC(hdc);
	}
};

/////////////////////////////////////////////////
class CBrush
{
public:
	inline CBrush(void) {m_hBrush=NULL;}
	inline ~CBrush(void) {DeleteObject();}

	void DeleteObject()
	{
		if(m_hBrush) ::DeleteObject(m_hBrush);
		m_hBrush=NULL;
	}

	void CreateSolidBrush(COLORREF color)
	{
		DeleteObject();
		m_hBrush=::CreateSolidBrush(color);
	}

	operator HBRUSH() const { return m_hBrush; }

protected:
	HBRUSH m_hBrush;
};

/////////////////////////////////////////////////
class CBitmap
{
public:
	inline CBitmap(void) { m_hBitmap=NULL; w=h=depth=0; }
	inline ~CBitmap(void) { DeleteObject(); }

	void DeleteObject()
	{
		if(m_hBitmap) ::DeleteObject(m_hBitmap);
		m_hBitmap=NULL;
	}

	void LoadBitmap(LPCTSTR lpBitmapName)
	{
		BITMAP bitmapInfo;
		DeleteObject();
		m_hBitmap=::LoadBitmap(GetModuleHandle(NULL),lpBitmapName);
		GetObject(m_hBitmap,sizeof(BITMAP),&bitmapInfo);
		w=bitmapInfo.bmWidth;
		h=bitmapInfo.bmHeight;
		depth=bitmapInfo.bmBitsPixel;
	}

	void LoadBitmap(Image &image)
	{
		DeleteObject();
		w=image.width;
		h=image.height;
		depth=image.depth;
		m_hBitmap=image.GetHandle();
	}

	operator HBITMAP() const { return m_hBitmap; }

	int w, h, depth;
protected:
	HBITMAP m_hBitmap;
};

/////////////////////////////////////////////////
class CRect : public RECT
{
public:
	inline CRect() { left=right=top=bottom=0; }
	inline int Width() { return right-left; }
	inline int Height() { return bottom-top; }
};
