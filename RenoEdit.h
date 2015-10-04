#pragma once

#define RENO_EDIT_MARGIN_VERTICAL 2
#define RENO_EDIT_MARGIN_HORIZONTAL 2
#define RENO_EDIT_MAX_HISTORY 64

// CRenoEdit

class CRenoEdit : public CEdit
{
	DECLARE_DYNAMIC(CRenoEdit)

public:
	CRenoEdit();
	virtual ~CRenoEdit();

// Overrides
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Implementation
public:
	CBrush m_EditBrush;
	CString m_PartialCommand;
	CStringPtrArray m_History;
	LONG m_HistoryCurrent;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg VOID OnSize(UINT nType, INT cx, INT cy);
	afx_msg VOID OnEnUpdate();
};


