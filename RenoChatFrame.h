#pragma once

#include "RenoChildWnd.h"

// CRenoChatFrame frame

class CRenoChatFrame : public CRenoChildWnd
{
	DECLARE_DYNCREATE(CRenoChatFrame)

protected:
	CRenoChatFrame();           // Protected constructor used by dynamic creation
	virtual ~CRenoChatFrame();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Message Map
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID OnFileClose();
};