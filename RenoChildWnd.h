#pragma once

// CRenoChildWnd frame

class CRenoChildWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CRenoChildWnd)

protected:
	CRenoChildWnd();			// Protected constructor used by dynamic creation
	virtual ~CRenoChildWnd();

// Message Map
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg VOID OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg VOID OnDestroy();
};