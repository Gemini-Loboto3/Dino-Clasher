/////////////////////////////////////////////////////////////////////////////
/* 
DESCRIPTION:
	CFolderDialog  - Folder Selection Dialog Class
	Copyright(C) Armen Hakobyan, 2002 - 2005
	http://www.codeproject.com/dialog/cfolderdialog.asp

VERSION HISTORY:
	24 Mar 2002 - First release
	30 Mar 2003 - Some minor changes
				- Added missing in old Platform SDK new flag definitions  
				- Added support for both MFC 6.0 and 7.0
				- Added OnIUnknown handler for Windows XP folder filtration
				- Added SetExpanded and SetOKText and GetSelectedFolder functions
	24 May 2003 - Added OnSelChanged implementation
	14 Jul 2003 - Added custom filtration for Windows XP, thanks to Arik Poznanski
	29 Nov 2003 - Added SetRootFolder, thanks to Eckhard Schwabe ( and Jose Insa )
	02 Jan 2004 - Added GetRootFolder, uncomment if needed
	15 Feb 2005 - Small bug fix in DoModal, thanks to WindSeven
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FolderDlg.h"

/////////////////////////////////////////////////////////////////////////////

#ifndef BFFM_VALIDATEFAILED
	#ifndef UNICODE
		#define BFFM_VALIDATEFAILED		3
	#else
		#define BFFM_VALIDATEFAILED		4	
	#endif
#endif

#ifndef BFFM_IUNKNOWN
	#define BFFM_IUNKNOWN				5
#endif

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog

CFolderDialog::CFolderDialog( IN LPCTSTR pszTitle	 /*NULL*/,
							  IN LPCTSTR pszSelPath	 /*NULL*/,
							  IN CWnd*	 pWndParent	 /*NULL*/,
							  IN UINT	 uFlags		 /*BIF_RETURNONLYFSDIRS*/ )
{
	hWnd=NULL;
	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );
	::ZeroMemory( m_szFolPath, MAX_PATH );
	::ZeroMemory( m_szSelPath, MAX_PATH );
	
	// Fill data

	m_bi.hwndOwner	= *pWndParent;
	m_bi.pidlRoot	= NULL;
	m_bi.lpszTitle	= pszTitle;
	m_bi.ulFlags	= uFlags;
	m_bi.lpfn		= (BFFCALLBACK)&BrowseCallbackProc;
	m_bi.lParam		= (LPARAM)this;

	// The size of this buffer is assumed to be MAX_PATH bytes:

	m_bi.pszDisplayName = new TCHAR[ MAX_PATH ];

	SAFE_ZEROMEMORY( 
		m_bi.pszDisplayName, ( MAX_PATH * sizeof( TCHAR ) )
	);

	if( pszSelPath )
		SetSelectedFolder( pszSelPath );
}

CFolderDialog::~CFolderDialog( VOID )
{	
	SAFE_COTASKMEMFREE( m_bi.pidlRoot );
	SAFE_DELETE2( m_bi.pszDisplayName );

	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );
}

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog message handlers

// SetRootFolder By Jose Insa
// Microsoft knowledge Base Article
// ID Q132750: Convert a File Path to an ITEMIDLIST

BOOL CFolderDialog::SetRootFolder( IN LPCTSTR pszPath )
{
	if( !pszPath )
	{
		SAFE_COTASKMEMFREE( m_bi.pidlRoot );
		return TRUE;
	}

	HRESULT		  hResult	  = S_FALSE;
	IShellFolder* pDeskFolder = NULL;

	hResult = ::SHGetDesktopFolder( &pDeskFolder );
	if ( hResult == S_OK )
	{
		LPITEMIDLIST pidlRoot = NULL;
		LPTSTR       pszRoot  = const_cast< LPTSTR >( pszPath );

		// Convert the path to an ITEMIDLIST:
		hResult = pDeskFolder->ParseDisplayName(
			NULL, NULL, pszRoot, NULL, &pidlRoot, NULL 
		);

		if( hResult == S_OK )
		{
			SAFE_COTASKMEMFREE( m_bi.pidlRoot );
			m_bi.pidlRoot = pidlRoot;
		}

		SAFE_RELEASE( pDeskFolder );		
	}
	
	return ( hResult == S_OK );
} 

// NOTE: pszPath buffer must be at least
// MAX_PATH characters in size:

BOOL CFolderDialog::GetRootFolder( IN OUT LPTSTR pszPath )
{
	return ::SHGetPathFromIDList( m_bi.pidlRoot, pszPath );
} 

BOOL CFolderDialog::SetSelectedFolder( IN LPCTSTR pszPath )
{
	if(lstrcpy(m_szSelPath, pszPath)==NULL) return 0;
	else return 1;
}

/////////////////////////////////////////////////////////////////////////////

int CFolderDialog::DoModal( VOID )
{
	
	int nRet = -1;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder( &m_bi );
	if( pItemIDList )
	{
		if( ::SHGetPathFromIDList( pItemIDList, m_szFolPath ) )
			nRet = IDOK;

		SAFE_COTASKMEMFREE( pItemIDList );
	}
	else
		nRet = IDCANCEL;

	return ( nRet );
}

/////////////////////////////////////////////////////////////////////////////
// Overridables:

VOID CFolderDialog::OnInitialized( VOID )
{
	if( ::lstrlen( m_szSelPath ) > 0 )
		SetSelection( m_szSelPath );
}

VOID CFolderDialog::OnSelChanged( IN LPITEMIDLIST pItemIDList )
{
	if( m_bi.ulFlags & BIF_STATUSTEXT )
	{
		TCHAR szSelFol[ MAX_PATH ] = { 0 };
		if( ::SHGetPathFromIDList( pItemIDList, szSelFol ) )
			SetStatusText( szSelFol );
	}
}

INT CFolderDialog::OnValidateFailed( IN LPCTSTR /*pszPath*/ )
{	
	::MessageBeep( MB_ICONHAND );
	return 1; // Return 1 to leave dialog open, 0 - to end one
}

VOID CFolderDialog::OnIUnknown( IN IUnknown* /*pIUnknown*/ )
{
}

/////////////////////////////////////////////////////////////////////////////
// Callback function used with the SHBrowseForFolder function. 

INT CALLBACK CFolderDialog::BrowseCallbackProc( IN HWND   hWnd, 
												IN UINT   uMsg, 
												IN LPARAM lParam, 
												IN LPARAM lpData )
{
	CFolderDialog* pThis = (CFolderDialog*)lpData;

	INT nRet = 0;
	pThis->hWnd = hWnd;

	switch( uMsg )
	{
	case BFFM_INITIALIZED:
		pThis->OnInitialized();
		break;
	case BFFM_SELCHANGED:
		pThis->OnSelChanged( (LPITEMIDLIST)lParam );
		break;
	case BFFM_VALIDATEFAILED:
		nRet = pThis->OnValidateFailed( (LPCTSTR)lParam );
		break;
	case BFFM_IUNKNOWN:
		pThis->OnIUnknown( (IUnknown*)lParam );
		break;
	default:
		break;
	}

	pThis->hWnd = NULL;
	return nRet;	
}

/////////////////////////////////////////////////////////////////////////////
// Commands, valid to call only from handlers

VOID CFolderDialog::SetExpanded( IN LPCTSTR pszPath )
{
	::SendMessage( 
		*this, BFFM_SETEXPANDED, 
		(WPARAM)TRUE, (LPARAM)pszPath
	);
}

VOID CFolderDialog::SetOKText( IN LPCTSTR pszText )
{
	::SendMessage( 
		*this, BFFM_SETOKTEXT, 
		(WPARAM)0, (LPARAM)pszText
	);
}

VOID CFolderDialog::EnableOK( IN BOOL bEnable /*TRUE*/ )
{
	::SendMessage( 
		*this, BFFM_ENABLEOK, (WPARAM)bEnable, 0L
	);
}

VOID CFolderDialog::SetSelection( IN LPITEMIDLIST pItemIDList )
{
	::SendMessage( 
		*this, BFFM_SETSELECTION, 
		(WPARAM)FALSE, (LPARAM)pItemIDList
	);
}

VOID CFolderDialog::SetSelection( IN LPCTSTR pszPath )
{
	::SendMessage( 
		*this, BFFM_SETSELECTION, 
		(WPARAM)TRUE, (LPARAM)pszPath
	);
}

VOID CFolderDialog::SetStatusText( IN LPCTSTR pszText )
{
	::SendMessage( 
		*this, BFFM_SETSTATUSTEXT, 
		(WPARAM)0, (LPARAM)pszText
	);
}

// Shell version 5.0 or later:

VOID CFolderDialog::SetExpanded( IN LPITEMIDLIST pItemIDList )
{
	::SendMessage( 
		*this, BFFM_SETEXPANDED, 
		(WPARAM)FALSE, (LPARAM)pItemIDList
	);
}

/////////////////////////////////////////////////////////////////////////////