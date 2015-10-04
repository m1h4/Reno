#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoChatFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRenoChatFrame

IMPLEMENT_DYNCREATE(CRenoChatFrame, CRenoChildWnd)

BEGIN_MESSAGE_MAP(CRenoChatFrame, CRenoChildWnd)
	ON_COMMAND(ID_FILE_CLOSE, &CRenoChatFrame::OnFileClose)
END_MESSAGE_MAP()

CRenoChatFrame::CRenoChatFrame()
{
}

CRenoChatFrame::~CRenoChatFrame()
{
}

// CRenoChatFrame message handlers

BOOL CRenoChatFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CRenoChildWnd::PreCreateWindow(cs))
		return FALSE;

	// Modify the Window class or styles
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(NULL,::LoadCursor(NULL,IDC_ARROW),NULL,::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME)));

	return TRUE;
}

VOID CRenoChatFrame::OnFileClose() 
{
	// To close the frame, just send a WM_CLOSE, which is the equivalent
	// choosing close from the system menu
	SendMessage(WM_CLOSE);
}
