#pragma once

// CRenoMainFrame

class CRenoMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CRenoMainFrame)

public:
	CRenoMainFrame();
	virtual ~CRenoMainFrame();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();

// Implementation
public:
	CStatusBar m_StatusBar;

// Message Map
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg INT OnCreate(LPCREATESTRUCT lpCreateStruct);
};


