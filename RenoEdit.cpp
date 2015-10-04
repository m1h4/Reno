#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoEdit.h"
#include "RenoCommandFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRenoEdit

IMPLEMENT_DYNAMIC(CRenoEdit, CEdit)

CRenoEdit::CRenoEdit() :
	m_EditBrush(BACKGROUND_COLOR),
	m_HistoryCurrent(NULL)
{
}

CRenoEdit::~CRenoEdit()
{
}

BEGIN_MESSAGE_MAP(CRenoEdit, CEdit)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SIZE()
	ON_CONTROL_REFLECT(EN_UPDATE, &CRenoEdit::OnEnUpdate)
END_MESSAGE_MAP()

// CRenoEdit message handlers
INT CRenoEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Add your specialized creation code here
	SendMessage(WM_SETFONT,(WPARAM)theApp.GetFont().GetSafeHandle());

	return 0;
}

HBRUSH CRenoEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// Change any attributes of the DC here
	pDC->SetBkColor(BACKGROUND_COLOR);	// Otherwise text would have old background behind it

	// Return a non-NULL brush if the parent's handler should not be called
	return (HBRUSH)m_EditBrush.GetSafeHandle();
}

VOID CRenoEdit::OnSize(UINT nType, INT cx, INT cy)
{
	CEdit::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);

	rect.DeflateRect(RENO_EDIT_MARGIN_HORIZONTAL,RENO_EDIT_MARGIN_VERTICAL);

	// Refresh the text margin
	SetRect(rect);
}

BOOL MatchCommand(const CString& commandPartial,const CString& commandBase,const CStringPtrArray& commandList,CString& commandMatch)
{
	// Wildcard match
	if(commandList.GetCount() == 1 && commandList[0] == TEXT("*"))
	{
		// Wildcard matches always leave the commandBase to what it is, just like when a singular command is completed
		commandMatch = commandBase;
		return TRUE;
	}

	for(LONG i = 0; i < commandList.GetCount(); ++i)
	{
		// Has a match already been found
		if(!commandMatch.GetLength() && !_wcsnicmp(commandPartial,commandList[i],commandPartial.GetLength()))	// Is the current command a match
			commandMatch = commandList[i];

		// Check if this match was already set
		if(commandMatch.GetLength() && !lstrcmpi(commandBase,commandList[i]))
		{
			// Reset the match
			commandMatch.Empty();

			// Search for the next match to the end of the command list
			for(LONG j = i + 1; j < commandList.GetCount() && !commandMatch.GetLength(); ++j)
				if(!_wcsnicmp(commandPartial,commandList[j],commandPartial.GetLength()) && lstrcmpi(commandBase,commandList[j]))
					commandMatch = commandList[j];

			// Wrap search from the start of the command list
			for(LONG j = 0; j < i && !commandMatch.GetLength(); ++j)
				if(!_wcsnicmp(commandPartial,commandList[j],commandPartial.GetLength()) && lstrcmpi(commandBase,commandList[j]))
					commandMatch = commandList[j];

			// Exit if we have found a match
			if(commandMatch.GetLength())
				break;
		}
	}

	return commandMatch.GetLength();
}

VOID BuildCommandList(const RENO_COMMAND* commandMap,CStringPtrArray& commandList)
{
	ASSERT(commandMap);

	for(ULONG i = 0; commandMap[i].function; ++i)
		commandList.Add(commandMap[i].name);
}

const RENO_COMMAND* FindCommand(const CString& commandBase,const RENO_COMMAND* commandMap)
{
	ASSERT(commandMap);

	for(ULONG i = 0;commandMap[i].function; ++i)
		if(!lstrcmpi(commandBase,commandMap[i].name))
			return &commandMap[i];

	return NULL;
}

BOOL IsBaseCollection(const CString& base,const CStringPtrArray& collection)
{
	// Check for wildcard
	if(collection.GetCount() == 1 && collection[0] == TEXT("*"))
		return TRUE;

	for(LONG i = 0; i < collection.GetCount(); ++i)
		if(!lstrcmpi(base,collection[i]))
			return TRUE;

	return FALSE;
}

VOID SplitCommandQuotes(const CString& commandBase,CStringPtrArray& commandElements)
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

void CRenoEdit::OnEnUpdate()
{
	// Reset the autocompletion partial command buffer
	if(m_PartialCommand.GetLength())
	{
		m_PartialCommand.Empty();
		//TRACE(TEXT("Info: Partial command buffer reset\n"));
	}
}

LRESULT CRenoEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_SETFONT)
	{
		// Update our height
		TEXTMETRIC metrics;

		HDC hDC = CreateCompatibleDC(::GetDC(NULL));
		HGDIOBJ oldFont = SelectObject(hDC,(HFONT)wParam);
		GetTextMetrics(hDC,&metrics);
		SelectObject(hDC,oldFont);
		DeleteDC(hDC);

		SetWindowPos(NULL,0,0,0,metrics.tmHeight + RENO_EDIT_MARGIN_VERTICAL * 2,SWP_NOZORDER|SWP_NOREDRAW|SWP_NOREPOSITION|SWP_NOMOVE|SWP_NOACTIVATE);

		// TODO Maybe tell the parent to RecalcLayout
	}

	// Capture the special text control characters
	if(message == WM_CHAR && HIWORD(GetAsyncKeyState(VK_CONTROL)))
	{
		switch(wParam)
		{
		// Control + B
		case 2:
			// Bold
			ReplaceSel(TEXT("\x02"));
			return TRUE;

		// Control + U
		case 21:
			// Underline
			ReplaceSel(TEXT("\x1F"));
			return TRUE;

		// Control + I
		case 9:
			// Italic
			ReplaceSel(TEXT("\x16"));
			return TRUE;

		// Control + R
		case 18:
			// Normal/Reset
			ReplaceSel(TEXT("\x0F"));
			return TRUE;
		}
	}

	if(message == WM_CHAR && wParam == VK_RETURN)
	{
		CString command;

		GetWindowText(command);
		SetWindowText(TEXT(""));

		// Trim leading spaces
		command.TrimLeft();

		if(command.GetLength())
		{
			// Add the command to history
			if(m_History.GetCount())
			{
				// Do not add if last comand was the same one
				if(command != m_History[0])
					m_History.InsertAt(0,command);
			}
			else
			{
				// Add the command
				m_History.InsertAt(0,command);
			}

			// Reset current history position
			m_HistoryCurrent = 0;

			// Remove tail
			while(m_History.GetCount() > RENO_EDIT_MAX_HISTORY)
				m_History.RemoveAt(m_History.GetCount()-1);

			// Call the parent's command function
			STATIC_DOWNCAST(CRenoCommandFrame, GetParent())->ExecuteCommand(command);
			return TRUE;
		}
	}
	else if(message == WM_KEYDOWN && wParam == VK_UP && m_History.GetCount())
	{
		// Move up history position
		if(m_HistoryCurrent < m_History.GetCount())
			m_HistoryCurrent++;

		// Set control text
		SetWindowText(m_History[m_HistoryCurrent - 1]);
		SetSel(m_History[m_HistoryCurrent - 1].GetLength(),m_History[m_HistoryCurrent - 1].GetLength());

		// Reset partial command buffer
		m_PartialCommand.Empty();
	}
	else if(message == WM_KEYDOWN && wParam == VK_DOWN && m_History.GetCount())
	{
		// Move down history
		if(m_HistoryCurrent)
			m_HistoryCurrent--;

		if(m_HistoryCurrent)
		{
			// Set control text
			SetWindowText(m_History[m_HistoryCurrent - 1]);
			SetSel(m_History[m_HistoryCurrent - 1].GetLength(),m_History[m_HistoryCurrent - 1].GetLength());
		}
		else	// Else just clear the control
		{
			SetWindowText(TEXT(""));
		}

		// Reset partial command buffer
		m_PartialCommand.Empty();
	}
	else if(message == WM_CHAR && wParam == VK_TAB && !HIWORD(GetAsyncKeyState(VK_SHIFT)))	// Shift cancles the completion
	{
		CString command;
		GetWindowText(command);

		// Clean up the leading whitespaces
		command.TrimLeft();

		CStringPtrArray commandElements;

		// Build command elements
		// FIXME This currently destructs any enclosed quotes (which is not prefered here), because
		// this behaviour is required for command execution parsing where it is necessary, would
		// probably need to fork the function or add a extra flag parameter (witch is ugly)
		SplitCommandQuotes(command,commandElements);

		// Remember trailing space
		BOOL space = FALSE;
		if(command.GetLength())
			space = command.GetAt(command.GetLength() - 1) == TEXT(' ');

		// Reconstruct the command
		command.Empty();

		for(LONG i = 0; i < commandElements.GetCount(); ++i)
		{
			// Enclose the command in quotes if it contains a space
			if(commandElements[i].Find(TEXT(' ')) == -1)
				command += commandElements[i];
			else
			{
				command += TEXT("\"");
				command += commandElements[i];
				command += TEXT("\"");
			}
			
			if(i < commandElements.GetCount() - 1)
				command += TEXT(" ");
		}

		// Re-add the trailing space
		if(space)
			command += TEXT(" ");

		SetWindowText(command);
		SetSel(command.GetLength(),command.GetLength());

		if(!commandElements.GetCount())
		{
			// Invalid command
			MessageBeep(MB_ICONMASK);
			return TRUE;
		}

		const RENO_COMMAND* commandList = STATIC_DOWNCAST(CRenoCommandFrame, GetParent())->GetCommandMap();
		ASSERT(commandList);

		for(LONG i = 0; i < commandElements.GetCount(); ++i)
		{
			if(i == 0)
			{
				// Are we the first of a series of commands/parameters
				if(commandElements.GetCount() > 1)
				{
					// Check if this command exists
					if(!FindCommand(commandElements[0],commandList))
					{
						// Invalid command
						MessageBeep(MB_ICONMASK);
						return TRUE;
					}
				}
				else
				{
					// Set the partial command match if it was reset
					if(!m_PartialCommand.GetLength())
						m_PartialCommand = commandElements[0];

					CString commandMatch;

					CStringPtrArray commandNameList;

					// Build a list of valid command names
					BuildCommandList(commandList,commandNameList);

					// Set the command buffer to the matched command if one has been found
					if(MatchCommand(m_PartialCommand,commandElements[0],commandNameList,commandMatch))
					{
						// Enclose the match in quotes if it contains a space
						if(commandMatch.Find(TEXT(' ')) != -1)
							commandMatch = TEXT("\"") + commandMatch + TEXT("\"");

						// Add a space after the match for easier parameter adding
						commandMatch += TEXT(" ");

						SetWindowText(commandMatch);
						SetSel(commandMatch.GetLength(),commandMatch.GetLength());
					}
					else
					{
						// No command match
						MessageBeep(MB_ICONMASK);
						return TRUE;
					}
				}
			}
			else
			{
				// Are we not the last command/parameter
				if(commandElements.GetCount() > i + 1)
				{
					CStringPtrArray parameters;

					for(LONG j = 0; j < i; ++j)
						parameters.Add(commandElements[j]);

					CStringPtrArray parameterElements;

					// Get a list of possible parameters/commands for the current command/parameter sequence
					(GetParent()->*FindCommand(commandElements[0],commandList)->parameters)(parameters,parameterElements);

					// Are we a part of the current possible list of commands/parameters
					if(!IsBaseCollection(commandElements[i],parameterElements))
					{
						// Invalid command
						MessageBeep(MB_ICONMASK);
						return TRUE;
					}
				}
				else
				{
					// Set the partial command match if it was reset
					if(!m_PartialCommand.GetLength())
						m_PartialCommand = commandElements[i];

					CString commandMatch;

					CStringPtrArray parameters;

					for(LONG j = 0; j < i; ++j)
						parameters.Add(commandElements[j]);

					CStringPtrArray parameterElements;

					// Get a list of possible parameters/commands for the current command/parameter sequence
					(GetParent()->*FindCommand(commandElements[0],commandList)->parameters)(parameters,parameterElements);

					// Set the command buffer to the matched command/parameter if one has been found
					if(MatchCommand(m_PartialCommand,commandElements[i],parameterElements,commandMatch))
					{
						// Enclose the match in quotes if it contains a space
						if(commandMatch.Find(TEXT(' ')) != -1)
							commandMatch = TEXT("\"") + commandMatch + TEXT("\"");

						// Add a space after the match for easier parameter adding
						commandMatch += TEXT(" ");

						// Build a list of the current parameters/commands
						CString text = commandElements[0];

						// TODO The code currently does not check if the command element has spaces, but this is very unlikely to occure

						for(LONG j = 1; j < i; ++j)
						{
							text += TEXT(" ");
							
							// Enclose the command in quotes if it contains a space
							if(commandElements[j].Find(TEXT(' ')) == -1)
								text += commandElements[j];
							else
							{
								text += TEXT("\"");
								text += commandElements[j];
								text += TEXT("\"");
							}
						}

						// And finally add the matched command/parameter
						text += TEXT(" ") + commandMatch;

						SetWindowText(text);
						SetSel(text.GetLength(),text.GetLength());
					}
					else
					{
						// No command/parameter match
						MessageBeep(MB_ICONMASK);
						return TRUE;
					}
				}
			}
		}

		return TRUE;
	}

	return CEdit::WindowProc(message, wParam, lParam);
}
