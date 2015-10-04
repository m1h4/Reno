#pragma once

// TODO Load colors from ini file
#define BACKGROUND_COLOR RGB(240,240,240)	// Used for background

#include "Resource.h"
#include "RenoSocketApp.h"
#include "RenoMisc.h"

// Fonts used by the application's gui
#define RENO_FONT_NORMAL 0
#define RENO_FONT_BOLD 1
#define RENO_FONT_ITALIC 2
#define RENO_FONT_UNDERLINE 3
#define RENO_FONT_COUNT 4

// CRenoApp

class CRenoApp : public CRenoSocketApp
{
public:
	CRenoApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual INT ExitInstance();

// Implementation
protected:
	HMENU m_hCommandMenu;
	HACCEL m_hCommandAccelerator;

	HMENU m_hChatMenu;
	HACCEL m_hChatAccelerator;

// Settings
protected:
	CStringPtrArray m_Servers;
	CStringPtrArray m_Nicks;

	CString m_UserName;
	CString m_RealName;

	CFont m_Fonts[RENO_FONT_COUNT];

// Functions
public:
	BOOL InitFonts();
	VOID UnInitFonts();

	BOOL LoadSettings();
	BOOL SaveSettings();

	CStringPtrArray& GetServerList() { return m_Servers; }
	CStringPtrArray& GetNickList() { return m_Nicks; }

	CString& GetUserName() { return m_UserName; }
	CString& GetRealName() { return m_RealName; }

	CFont& GetFont(ULONG id = RENO_FONT_NORMAL);

	// DEBUG Remove
	VOID NewChatWindow();

// Message Map
public:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID OnAppAbout();
	afx_msg VOID OnFileNew();
	afx_msg VOID OnToolsOptions();
	afx_msg VOID OnHelp();
};

extern CRenoApp theApp;