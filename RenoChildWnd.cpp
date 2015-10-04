#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoChildWnd.h"
#include "RenoMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRenoChildWnd

IMPLEMENT_DYNCREATE(CRenoChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CRenoChildWnd, CMDIChildWnd)
	ON_WM_MDIACTIVATE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CRenoChildWnd::CRenoChildWnd()
{
}

CRenoChildWnd::~CRenoChildWnd()
{
}

// CRenoChildWnd message handlers

VOID CRenoChildWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	// Remove the fucking client edge set by UpdateClientEdge

	DWORD oldStyle = GetWindowLong(m_hWnd,GWL_STYLE);

	if(~oldStyle & WS_MAXIMIZE)
		SetWindowLong(m_hWnd,GWL_STYLE,oldStyle|WS_MAXIMIZE);

	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if(~oldStyle & WS_MAXIMIZE)
		SetWindowLong(m_hWnd,GWL_STYLE,oldStyle);
}

VOID CRenoChildWnd::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	// Remove the fucking client edge set by UpdateClientEdge

	DWORD oldStyle = GetWindowLong(m_hWnd,GWL_STYLE);

	if(~oldStyle & WS_MAXIMIZE)
		SetWindowLong(m_hWnd,GWL_STYLE,oldStyle|WS_MAXIMIZE);

	CMDIChildWnd::OnWindowPosChanging(lpwndpos);

	if(~oldStyle & WS_MAXIMIZE)
		SetWindowLong(m_hWnd,GWL_STYLE,oldStyle);
}

VOID CRenoChildWnd::OnDestroy()
{
	// Remove the fucking client edge set by UpdateClientEdge
	
	//CMDIChildWnd::OnDestroy();
	CFrameWnd::OnDestroy();
}