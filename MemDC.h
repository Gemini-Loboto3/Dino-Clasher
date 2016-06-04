#pragma once

class CMemDC
{
public:
	BOOL m_bUseMemoryDC;

	CMemDC(HDC& dc, HWND pWnd, bool scroll=true) :
		m_dc (dc),
		m_bMemDC (FALSE),
		m_pOldBmp (NULL),
		m_bUseMemoryDC(TRUE)
	{
		//ASSERT_VALID(pWnd);

		GetClientRect (pWnd,&m_rect);

		if(scroll)
		{
			m_rect.right += GetScrollPos (pWnd, SB_HORZ);
			m_rect.bottom += GetScrollPos (pWnd, SB_VERT);
		}

		m_dcMem=CreateCompatibleDC (m_dc);
		m_bmp=CreateCompatibleBitmap (m_dc, m_rect.right-m_rect.left, m_rect.bottom-m_rect.top);
		if (m_dcMem && m_bmp)
		{
			//-------------------------------------------------------------
			// Off-screen DC successfully created. Better paint to it then!
			//-------------------------------------------------------------
			m_bMemDC = TRUE;
			m_pOldBmp = (HBITMAP)SelectObject (m_dcMem, m_bmp);
		}
	}

	virtual ~CMemDC()
	{
		if (m_bMemDC)
		{
			//--------------------------------------
			// Copy the results to the on-screen DC:
			//-------------------------------------- 
			RECT rectClip;
			int nClipType = GetClipBox (m_dc, &rectClip);

			if (nClipType != NULLREGION)
			{
				//if (nClipType != SIMPLEREGION)
				//{
					rectClip = m_rect;
				//}

				BitBlt (m_dc, rectClip.left, rectClip.top, m_rect.right-m_rect.left, m_rect.bottom-m_rect.top,
							   m_dcMem, rectClip.left, rectClip.top, SRCCOPY);
			}

			SelectObject (m_dcMem, m_pOldBmp);

			DeleteObject(m_dcMem);
			DeleteObject(m_bmp);
		}
	}

	HDC& GetDC ()			{	return m_bMemDC ? m_dcMem : m_dc;	}
	BOOL IsMemDC () const	{	return m_bMemDC;	}
	HBITMAP* GetBitmap()	{	return &m_bmp;	}
	operator HDC() const	{	return m_bMemDC ? m_dcMem : m_dc;	}

protected:
	HDC&		m_dc;
	BOOL		m_bMemDC;
	HDC			m_dcMem;
	HBITMAP		m_bmp;
	HBITMAP		m_pOldBmp;
	RECT		m_rect;
};
