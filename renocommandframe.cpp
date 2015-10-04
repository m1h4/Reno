#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoCommandFrame.h"
#include "RenoMainFrame.h"
#include "RenoIrc.h"
#include "RenoMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRenoCommandFrame

IMPLEMENT_DYNCREATE(CRenoCommandFrame, CRenoSocketWnd)

BEGIN_MESSAGE_MAP(CRenoCommandFrame, CRenoSocketWnd)
	ON_COMMAND(ID_FILE_CLOSE, &CRenoCommandFrame::OnFileClose)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CRenoCommandFrame construction/destruction

CRenoCommandFrame::CRenoCommandFrame()
{
}

CRenoCommandFrame::~CRenoCommandFrame()
{
}

BOOL CRenoCommandFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CRenoSocketWnd::PreCreateWindow(cs))
		return FALSE;

	// Modify the Window class or styles
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(NULL,::LoadCursor(NULL,IDC_ARROW),NULL,::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME)));

	return TRUE;
}

// CRenoCommandFrame message handlers

VOID CRenoCommandFrame::OnFileClose() 
{
	// To close the frame, just send a WM_CLOSE, which is the equivalent
	// choosing close from the system menu
	SendMessage(WM_CLOSE);
}

INT CRenoCommandFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// Create our buffer control
	if(!m_Buffer.Create(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_VSCROLL,CRect(0,0,0,0),this,ID_CHILD_VIEW))
	{
		TRACE(TEXT("Failed to create rich edit window\n"));
		return -1;
	}

	// Create our input edit control
	// FIXME Should compute height value from actual font height
	if(!m_Edit.Create(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|ES_AUTOHSCROLL|ES_MULTILINE|ES_LEFT,CRect(0,0,0,0),this,ID_CHILD_INPUT))
	{
		TRACE(TEXT("Failed to create edit window\n"));
		return -1;
	}

	// Call super after we have created our controls cause this calls the inital RecalcLayout
	if(CRenoSocketWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

VOID CRenoCommandFrame::RecalcLayout(BOOL bNotify)
{
	CRenoSocketWnd::RecalcLayout(bNotify);

	CRect clientRect;
	GetClientRect(clientRect);

	CRect editRect;
	m_Edit.GetWindowRect(&editRect);

	// Reposition/resize the controls
	m_Buffer.SetWindowPos(NULL,0,0,clientRect.Width(),clientRect.Height() - editRect.Height() - RENO_COMMAND_FRAME_SEPERATOR_MARGIN,SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOZORDER);
	m_Edit.SetWindowPos(NULL,0,clientRect.Height() - editRect.Height(),clientRect.Width(),editRect.Height(),SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOZORDER);

	// TODO Buffer needs repainting
	m_Buffer.InvalidateRect(NULL);
}

BOOL CRenoCommandFrame::PrintFormat(LPCTSTR format,...)
{
	ASSERT(format);

	static TCHAR buffer[4096];

    va_list args;
    va_start(args,format);
	INT written = vswprintf_s(buffer,sizeof(buffer)/sizeof(buffer[0]),format,args);
    va_end(args);

	if(written < 0)
		return FALSE;

	return m_Buffer.Print(buffer);
}

VOID CRenoCommandFrame::Clear()
{
	// Just clear the string buffer
	m_Buffer.Clear();
}

VOID CRenoCommandFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CRenoSocketWnd::OnSetFocus(pOldWnd);

	// Set the focus on the input control
	m_Edit.SetFocus();
}

VOID CRenoCommandFrame::OnPaint()
{
	// Do not call CRenoSocketFrame::OnPaint() for painting messages
	CPaintDC dc(this);

	CRect editRect;
	m_Edit.GetWindowRect(editRect);

	CRect clientRect;
	GetClientRect(clientRect);

	// Draw the seperator margin
	dc.FillRect(CRect(0,clientRect.Height() - editRect.Height() - RENO_COMMAND_FRAME_SEPERATOR_MARGIN,clientRect.Width(),clientRect.Height() - editRect.Height()),&CBrush(GetSysColor(COLOR_BTNFACE)));
}

VOID CRenoCommandFrame::OnDestroy()
{
	// Do a graceful disconnect
	if(m_State >= RENO_NETWORK_CONNECTED)
	{
		SendFormat(TEXT("QUIT\r\n"));
		TRACE(TEXT("Info: Sent quit to connected host before destroy.\n"));
	}

	CRenoSocketWnd::OnDestroy();
}

VOID SplitCommand(const CString& commandBase,CStringPtrArray& commandElements)
{
	static TCHAR buffer[1024];
	BOOL quotes = FALSE;
	LONG index = 0;
	LONG length = 0;

	while(index < commandBase.GetLength())
	{
		if(commandBase.GetAt(index) == TEXT('\\') && commandBase.GetAt(index + 1) == TEXT('\"'))
			
			// Destructive quote handling
			buffer[length++] = commandBase.GetAt(++index);
		
			// Non-destructive quote handling
		//{
		//	buffer[length++] = commandBase.GetAt(index);
		//	buffer[length++] = commandBase.GetAt(++index);
		//}
		
		else if(commandBase.GetAt(index) == TEXT('\"'))
			quotes = !quotes;
		else if(commandBase.GetAt(index) == TEXT(' ') && !quotes)
		{
			if(length)
			{
				buffer[length] = 0;
				commandElements.Add(buffer);
				length = 0;
			}
		}
		else
			buffer[length++] = commandBase.GetAt(index);

		++index;
	}

	if(length)
	{
		buffer[length] = 0;
		commandElements.Add(buffer);
	}
}

BOOL CRenoCommandFrame::ExecuteCommand(const CString& command)
{
	CStringPtrArray commandElements;

	// Split the command into arguments
	SplitCommand(command,commandElements);

	// Check if null command passed in
	if(!commandElements.GetCount())
		return FALSE;

	// Get our command map
	const RENO_COMMAND* commandMap = GetCommandMap();
	ASSERT(commandMap);

	// Find the command that was called
	for(ULONG i = 0; commandMap[i].name && commandMap[i].function; ++i)
		if(!lstrcmpi(commandMap[i].name,commandElements[0]))
			return (this->*commandMap[i].function)(commandElements);	// TODO Maybe just always return TRUE since the passed in command name was valid

	// Unknown command
	PrintFormat(TEXT("* Unknown command: %s\r\n"),commandElements[0]);

	return FALSE;
}

VOID SplitMessage(const CString& messageBase,CStringPtrArray& messageElements)
{
	LONG index = 0;
	while(index < messageBase.GetLength())
	{
		LONG find = messageBase.Find(TEXT("\r\n"),index);
		if(find != -1)
		{
			messageElements.Add(messageBase.Mid(index,find - index));
			index = find + 2;
		}
		else
		{
			messageElements.Add(messageBase.Mid(index));
			break;
		}
	}
}

VOID ExtractParameters(const CString& messageBase,CStringPtrArray& parameterElements)
{
	LONG index = 0;
	LONG length = 0;

	if(messageBase.GetAt(0) == TEXT(':'))
	{
		parameterElements.Add(messageBase.Mid(1));
		return;
	}

	while(index < messageBase.GetLength())
	{
		if(messageBase.GetAt(index) == TEXT(' '))
		{
			if(length)
			{
				parameterElements.Add(messageBase.Mid(index - length,length));
				length = 0;
			}

			if(messageBase.GetAt(index + 1) == TEXT(':'))
			{
				parameterElements.Add(messageBase.Mid(index + 2));
				return;
			}
		}
		else
			++length;

		++index;
	}

	if(length)
		parameterElements.Add(messageBase.Mid(index - length,length));
}

VOID CRenoCommandFrame::OnReceive(INT nErrorCode,LPSTR buffer,ULONG received)
{
	// TODO Write better error handling code here, maybe we do not have to panic on every error code
	if(nErrorCode)
	{
		// Write the error message
		PrintFormat(TEXT("* Error occurred during receive. Error code %d\r\n"),nErrorCode);

		// Make sure we disconnect
		CRenoCommandFrame::OnReceive(nErrorCode,buffer,received);

		return;
	}

	ASSERT(buffer);

	// Get the size of the translate buffer
	INT messageSize = MultiByteToWideChar(CP_UTF8,NULL,buffer,received,NULL,NULL);

	// Create the buffer that will hold the translated message
	LPTSTR message = new TCHAR[messageSize + 1];
	ASSERT(message);

	// Translate the buffer
	messageSize = MultiByteToWideChar(CP_UTF8,NULL,buffer,received,message,messageSize);

	// Terminate the buffer
	message[messageSize] = NULL;

	CStringPtrArray messageElements;

	// Split the message buffer into individual messages
	SplitMessage(message,messageElements);

	// Do not need the translated buffer anymore
	delete[] message;

	// Process output
	for(LONG i = 0; i < messageElements.GetCount(); ++i)
	{
		// TODO Remove any unsupported characters
		//for(LONG j = 0; j < messageElements[i].GetLength(); ++j)
		//	if(!_ismbcprint(messageElements[i].GetAt(j)))
		//		messageElements[i].SetAt(j,TEXT('\v'));

		// Process the message element
		ULONG position = 0;

		CString prefix;

		// Is there a prefix
		if(messageElements[i].GetAt(0) == TEXT(':'))
		{
			// Extract the prefix
			prefix = messageElements[i].Mid(1, messageElements[i].Find(TEXT(' ')) - 1);
			position += prefix.GetLength() + 2;
		}

		CString command;

		// Are there any parameters after the command
		if(messageElements[i].Find(TEXT(' '), position))
		{
			// Extract the command
			command = messageElements[i].Mid(position, messageElements[i].Find(TEXT(' '), position) - position);
			position += command.GetLength() + 1;
		}
		else
		{
			// The whole element is just a command
			command = messageElements[i].Mid(position);
			position += command.GetLength();
		}

		CStringPtrArray parameterElements;

		// Extract parameters
		ExtractParameters(messageElements[i].GetBuffer() + position, parameterElements);

		// Find the reply handler
		const RENO_REPLY* replyMap = GetReplyMap();
		ASSERT(replyMap);

		for(ULONG j = 0; replyMap[j].command ; ++j)
		{
			if(!_wcsicmp(replyMap[j].command,command) || !_wcsicmp(replyMap[j].command,TEXT("*")))
			{
				// Execute the handler and stop further searching
				(this->*replyMap[j].handler)(prefix,command,parameterElements);
				break;
			}
		}
	}
}

VOID CRenoCommandFrame::OnConnect(INT nErrorCode)
{
	switch(nErrorCode)
	{
	case NULL:
		PrintFormat(TEXT("* Connection established\r\n"));
		break;

	case WSAECONNREFUSED:
		PrintFormat(TEXT("* Failed to connect. No connection could be made because the target computer actively refused it\r\n"));
		break;

	case WSAENETUNREACH:
		PrintFormat(TEXT("* Failed to connect. The network cannot be reached from this host at this time\r\n"));
		break;

	case WSAETIMEDOUT:
		PrintFormat(TEXT("* Failed to connect. The connected party did not properly respond after a period of time\r\n"));
		break;

	default:
		PrintFormat(TEXT("* Failed to connect. Connection error %d\r\n"),nErrorCode);
		break;
	}

	CRenoSocketWnd::OnConnect(nErrorCode);
}

VOID CRenoCommandFrame::OnResolved(INT nErrorCode,LPADDRINFO info)
{
	if(nErrorCode || !info)
	{
		switch(nErrorCode)
		{
		case NULL:
			PrintFormat(TEXT("* Failed to resolve name\r\n"));
			break;

		case WSATRY_AGAIN:
			PrintFormat(TEXT("* Failed to resolve name. Temporary error occurred during host name resolution\r\n"));
			break;

		case WSANO_RECOVERY:
			PrintFormat(TEXT("* Failed to resolve name. Non-recoverable error occurred during database lookup\r\n"));
			break;

		case WSAHOST_NOT_FOUND:
			PrintFormat(TEXT("* Failed to resolve name. The specified name could not be found in the database\r\n"));
			break;

		case WSATYPE_NOT_FOUND:
			PrintFormat(TEXT("* Failed to resolve name. The specified class could not be found in the database\r\n"));
			break;

		default:
			PrintFormat(TEXT("* Failed to resolve name. Resolution error %d\r\n"),nErrorCode);
			break;
		}

		CRenoSocketWnd::OnResolved(nErrorCode,NULL);
	}
	else
	{
		// Tell the user the resolved address that we are connecting to
		TCHAR address[NI_MAXHOST];
		AddressToString(info->ai_addr,info->ai_addrlen,address,sizeof(address)/sizeof(TCHAR));
		PrintFormat(TEXT("* Connecting to %s\r\n"),address);

		// This will actualy call connect
		CRenoSocketWnd::OnResolved(nErrorCode,info);
	}
}

VOID CRenoCommandFrame::OnClose(INT nErrorCode)
{
	CRenoSocketWnd::OnClose(nErrorCode);

	PrintFormat(TEXT("* Remote host disconnected\r\n"));
}