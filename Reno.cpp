#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoMainFrame.h"
#include "RenoCommandFrame.h"
#include "RenoChatFrame.h"
#include "DumpException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only CRenoApp object

CRenoApp theApp;

// CRenoApp

BEGIN_MESSAGE_MAP(CRenoApp, CRenoSocketApp)
	ON_COMMAND(ID_APP_ABOUT, &CRenoApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CRenoApp::OnFileNew)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CRenoApp::OnToolsOptions)
	ON_COMMAND(ID_HELP, &CRenoApp::OnHelp)
END_MESSAGE_MAP()

// CRenoApp construction

CRenoApp::CRenoApp()
{
	// Place all significant initialization in InitInstance
	SetUnhandledExceptionFilter(DumpException);
}

// CRenoApp initialization

BOOL CRenoApp::InitInstance()
{
	// Set common controls standard classes
	INITCOMMONCONTROLSEX commonControls;
	commonControls.dwSize = sizeof(commonControls);
	commonControls.dwICC = ICC_WIN95_CLASSES;

	// Initialize common controls
	if(!InitCommonControlsEx(&commonControls))
	{
		AfxMessageBox(IDP_CONTROLS_INIT_FAILED);
		return FALSE;
	}

	// Initialize winsock
	if(!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Set settings registry key, if not set the ini file will be used
	//SetRegistryKey(TEXT("Miha Software"));

	// Get the current directory and add it to the ini path so the ini does not get stored into win.ini
	TCHAR directory[MAX_PATH];
	GetModuleFileName(NULL,directory,sizeof(directory)/sizeof(TCHAR));
	PathRemoveFileSpec(directory);
	PathAppend(directory,m_pszProfileName);
 
	// Store the ini file path
	free((LPVOID)m_pszProfileName);
	m_pszProfileName = _tcsdup(directory);	// String gets free'd by mfc on app exit

	// Initialize super
	if(!CRenoSocketApp::InitInstance())
		return FALSE;

	// Initialize fonts
	if(!InitFonts())
		return FALSE;

	// Create the main window
	CMDIFrameWnd* pFrame = new CRenoMainFrame;
	if(!pFrame)
		return FALSE;

	// Set our local member pointer to the frame instance
	m_pMainWnd = pFrame;

	// Create main MDI frame window
	if(!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// Try to load shared MDI menus and accelerator table
	// TODO Add additional member variables and load calls for additional menu types your application may need

	m_hCommandMenu = ::LoadMenu(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_COMMANDFRAME));
	m_hCommandAccelerator = ::LoadAccelerators(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_COMMANDFRAME));

	m_hChatMenu = ::LoadMenu(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_CHATFRAME));
	m_hChatAccelerator = ::LoadAccelerators(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_CHATFRAME));

	// Load the settings
	LoadSettings();

	// TODO Load the previous window placement

	// The main window has been initialized, so show and update it
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	// Create a inital command window
	// TODO Make it an option
	OnFileNew();

	return TRUE;
}

INT CRenoApp::ExitInstance() 
{
	// Free the child window's objects
	if(m_hCommandMenu)
		FreeResource(m_hCommandMenu);
	if(m_hCommandAccelerator)
		FreeResource(m_hCommandAccelerator);

	if(m_hChatMenu)
		FreeResource(m_hChatMenu);
	if(m_hChatAccelerator)
		FreeResource(m_hChatAccelerator);

	// Free the fonts
	UnInitFonts();

	return CRenoSocketApp::ExitInstance();
}

BOOL CRenoApp::InitFonts()
{
	// Read from ini
	CString name = GetProfileString(TEXT("Font"),TEXT("Name"),TEXT("Courier New"));
	ULONG height = GetProfileInt(TEXT("Font"),TEXT("Height"),14);

	// Create the default fonts
	m_Fonts[RENO_FONT_NORMAL].CreateFont(height,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,0,0,name);
	m_Fonts[RENO_FONT_BOLD].CreateFont(height,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,0,0,name);
	m_Fonts[RENO_FONT_ITALIC].CreateFont(height,0,0,0,FW_NORMAL,TRUE,0,0,DEFAULT_CHARSET,0,0,0,0,name);
	m_Fonts[RENO_FONT_UNDERLINE].CreateFont(height,0,0,0,FW_NORMAL,0,TRUE,0,DEFAULT_CHARSET,0,0,0,0,name);

	return TRUE;
}

VOID CRenoApp::UnInitFonts()
{
	// Destroy all fonts
	for(ULONG i = 0; i < sizeof(m_Fonts)/sizeof(m_Fonts[0]); ++i)
		m_Fonts[i].DeleteObject();
}

CFont& CRenoApp::GetFont(ULONG id)
{
	ASSERT(id < RENO_FONT_COUNT);
	
	// Return the requested font
	return m_Fonts[id];
}

BOOL CRenoApp::LoadSettings()
{
	ASSERT(m_pszProfileName);

	// Clear current servers
	m_Servers.RemoveAll();

	// Get server list
	for(ULONG i = 0; i < 1024; ++i)
	{
		TCHAR key[128];

		swprintf(key,sizeof(key)/sizeof(TCHAR),TEXT("Server%d"),i);
		CString value = GetProfileString(TEXT("Servers"),key);

		// Stop on first non-specified value
		if(!value.GetLength())
			break;

		m_Servers.Add(value);
	}

	// Clear current nicks
	m_Nicks.RemoveAll();

	// Get nick list
	for(ULONG i = 0; i < 1024; ++i)
	{
		TCHAR key[128];

		swprintf(key,sizeof(key)/sizeof(TCHAR),TEXT("Nick%d"),i);
		CString value = GetProfileString(TEXT("Settings"),key);

		// Stop on first non-specified value
		if(!value.GetLength())
			break;

		m_Nicks.Add(value);
	}

	// Get user name
	m_UserName = GetProfileString(TEXT("Settings"),TEXT("User"));

	// Get real user name
	m_RealName = GetProfileString(TEXT("Settings"),TEXT("Name"));

	return TRUE;
}

BOOL CRenoApp::SaveSettings()
{
	ASSERT(m_pszProfileName);

	// Write server list
	for(LONG i = 0; i < m_Servers.GetCount() ; ++i)
	{
		TCHAR key[128];

		swprintf(key,sizeof(key)/sizeof(TCHAR),TEXT("Server%d"),i);
		WriteProfileString(TEXT("Servers"),key,m_Servers[i]);
	}

	// Write nick list
	for(LONG i = 0; i < m_Nicks.GetCount() ; ++i)
	{
		TCHAR key[128];

		swprintf(key,sizeof(key)/sizeof(TCHAR),TEXT("Nick%d"),i);
		WriteProfileString(TEXT("Settings"),key,m_Nicks[i]);
	}

	// Write user name
	if(m_UserName.GetLength())
		WriteProfileString(TEXT("Settings"),TEXT("User"),m_UserName);

	// Write real user name
	if(m_RealName.GetLength())
		WriteProfileString(TEXT("Settings"),TEXT("Name"),m_RealName);

	return TRUE;
}

// CRenoApp message handlers

VOID CRenoApp::OnFileNew()
{
	// Create a new MDI child window
	STATIC_DOWNCAST(CRenoMainFrame, AfxGetMainWnd())->CreateNewChild(RUNTIME_CLASS(CRenoCommandFrame), IDR_COMMANDFRAME, m_hCommandMenu, m_hCommandAccelerator);
}

VOID CRenoApp::NewChatWindow()
{
	// Create a new MDI child window
	STATIC_DOWNCAST(CRenoMainFrame, AfxGetMainWnd())->CreateNewChild(RUNTIME_CLASS(CRenoChatFrame), IDR_CHATFRAME, m_hChatMenu, m_hChatAccelerator);
}

VOID CRenoApp::OnToolsOptions()
{
	// Create a new options sheet
	CPropertySheet psh(TEXT("Options"),AfxGetMainWnd());
	CPropertyPage pp(IDD_OPTIONS_GENERAL);
	
	psh.m_psh.dwFlags &= ~PSH_HASHELP;
	psh.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	pp.m_psp.dwFlags &= ~PSP_HASHELP;

	psh.AddPage(&pp);

	psh.DoModal();
}

VOID CRenoApp::OnHelp()
{
	// Create a new help window
	MessageBox(AfxGetMainWnd()->GetSafeHwnd(),TEXT("This is not implemented yet."),TEXT("Not Implemented"),MB_OK|MB_ICONEXCLAMATION);
}

// CRenoAbout dialog used for App About

class CRenoAbout : public CDialog
{
public:
	CRenoAbout();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

// Overrides
protected:
	virtual BOOL OnInitDialog();

// Message Map
protected:
	DECLARE_MESSAGE_MAP()
};

CRenoAbout::CRenoAbout() :
	CDialog(CRenoAbout::IDD)
{
}

BEGIN_MESSAGE_MAP(CRenoAbout, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
VOID CRenoApp::OnAppAbout()
{
	CRenoAbout about;

	// Display the about box
	about.DoModal();
}

// CRenoAbout message handlers

BOOL CRenoAbout::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO Add extra initialization here
	SetDlgItemText(IDC_ABOUT_TEXT,TEXT("-- Work In Progress --\r\n\r\nThis is where keyboard shortcuts and/or copyright messages would go."));

	return TRUE;	// Return TRUE unless you set the focus to a control
}