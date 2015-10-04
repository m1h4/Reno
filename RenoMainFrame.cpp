#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRenoMainFrame

IMPLEMENT_DYNAMIC(CRenoMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CRenoMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,			// Status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
};

// CRenoMainFrame construction/destruction

CRenoMainFrame::CRenoMainFrame()
{
}

CRenoMainFrame::~CRenoMainFrame()
{
}

INT CRenoMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create our statusbar
	if(!m_StatusBar.Create(this,WS_CHILD|WS_VISIBLE|CBRS_BOTTOM|WS_CLIPCHILDREN|WS_CLIPSIBLINGS) ||
		!m_StatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT)))
	{
		TRACE(TEXT("Failed to create status bar\n"));
		return -1;
	}

	// Make the statusbar a bit larger
	m_StatusBar.GetStatusBarCtrl().SetMinHeight(20);

	// Remove the ugly border around the other panes
	m_StatusBar.SetPaneStyle(1,SBPS_NOBORDERS);
	m_StatusBar.SetPaneStyle(2,SBPS_NOBORDERS);

	// Remove the client edge style flag from the mdi host
	::SetWindowLong(m_hWndMDIClient,GWL_EXSTYLE,::GetWindowLong(m_hWndMDIClient,GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE);

	// Update the window to reflect the style change
	::SetWindowPos(m_hWndMDIClient,NULL,0,0,0,0,SWP_FRAMECHANGED|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOCOPYBITS);

	return 0;
}

BOOL CRenoMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CMDIFrameWnd::PreCreateWindow(cs))
		return FALSE;

	// Modify the Window class or styles here
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style |= FWS_PREFIXTITLE;

	return TRUE;
}

// CMainFrame message handlers
BOOL CRenoMainFrame::DestroyWindow()
{
	WINDOWPLACEMENT placement;
	GetWindowPlacement(&placement);

	// Save the window placement
	//theApp.WriteProfileBinary(TEXT("Window Placement"),TEXT("Main Window"),(LPBYTE)&placement,sizeof(placement));
	// FIXME Move to ini

	return CMDIFrameWnd::DestroyWindow();
}
