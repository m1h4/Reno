#pragma once

#define RENO_BUFFER_MARGIN_VERTICAL 2
#define RENO_BUFFER_MARGIN_HORIZONTAL 2
#define RENO_BUFFER_MAX_LINES 2048

// CRenoBuffer

class CRenoBuffer : public CWnd
{
	DECLARE_DYNAMIC(CRenoBuffer)

public:
	CRenoBuffer();
	virtual ~CRenoBuffer();

// Overrides
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Implementation
protected:
	CStringPtrArray m_Buffer;

	BOOL m_BufferNewLine;
	LONG m_BufferScrollLine;

// Functions
public:
	BOOL Print(LPCTSTR buffer);
	BOOL PrintFormat(LPCTSTR format,...);

	VOID Clear();

// Message Map
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID OnPaint();
	afx_msg VOID OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};