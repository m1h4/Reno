#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoCommandFrame.h"
#include "RenoMainFrame.h"
#include "RenoIrc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRenoCommandFrame

BEGIN_COMMAND_MAP(CRenoCommandFrame)
	COMMAND(TEXT(":server"), TEXT("Connects to a specified host"), &CRenoCommandFrame::CmdServer, &CRenoCommandFrame::CmdParamsServer)
	COMMAND(TEXT(":hosts"), TEXT("Displays the host list"), &CRenoCommandFrame::CmdHosts, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":status"), TEXT("Displays the current connection status"), &CRenoCommandFrame::CmdStatus, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":nick"), TEXT("Changes the nick being used"), &CRenoCommandFrame::CmdNick, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":user"), TEXT("Sends the user information to the host"), &CRenoCommandFrame::CmdUser, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":login"), TEXT("Executes the automatic login procedure"), &CRenoCommandFrame::CmdLogin, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":disconnect"), TEXT("Forcefully disconnects from the host"), &CRenoCommandFrame::CmdDisconnect, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":msg"), TEXT("Sends a raw message to the host"), &CRenoCommandFrame::CmdMsg, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":help"), TEXT("Displays a command list with descriptions"), &CRenoCommandFrame::CmdHelp, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":quit"), TEXT("Sends a quit command to the server to initiate a disconnect"), &CRenoCommandFrame::CmdQuit, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":exit"), TEXT("Closes any connections and exits the application"), &CRenoCommandFrame::CmdExit, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":close"), TEXT("Closes any connections and closes the current command window"), &CRenoCommandFrame::CmdClose, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":private"), TEXT("Sends a private message to the specified channel or nick"), &CRenoCommandFrame::CmdPrivate, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":clear"), TEXT("Clears the message buffer"), &CRenoCommandFrame::CmdClear, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":join"), TEXT("Joins a specified channel"), &CRenoCommandFrame::CmdJoin, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":part"), TEXT("Parts from the specified channel"), &CRenoCommandFrame::CmdPart, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":motd"), TEXT("Tells the host to display the message of the day"), &CRenoCommandFrame::CmdMotd, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":time"), TEXT("Tells the host to display the current time/date"), &CRenoCommandFrame::CmdTime, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":load"), TEXT("Reloads the configuration data"), &CRenoCommandFrame::CmdLoad, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":save"), TEXT("Saves the configuration data"), &CRenoCommandFrame::CmdSave, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":add"), TEXT("Adds a host/nick to the list"), &CRenoCommandFrame::CmdAdd, &CRenoCommandFrame::CmdParamsAdd)
	COMMAND(TEXT(":remove"), TEXT("Removes a host/nick from the list"), &CRenoCommandFrame::CmdRemove, &CRenoCommandFrame::CmdParamsRemove)
	COMMAND(TEXT(":names"), TEXT("Lists the names in a channel"), &CRenoCommandFrame::CmdNames, &CRenoCommandFrame::CmdParamsNull)
	COMMAND(TEXT(":topic"), TEXT("Views/changes the topic in a channel"), &CRenoCommandFrame::CmdTopic, &CRenoCommandFrame::CmdParamsNull)
END_COMMAND_MAP()

BEGIN_REPLY_MAP(CRenoCommandFrame)
	REPLY(TEXT("ping"), &CRenoCommandFrame::ReplyPing)
	REPLY(TEXT("privmsg"), &CRenoCommandFrame::ReplyPrivate)
	REPLY(TEXT("part"), &CRenoCommandFrame::ReplyPart)
	REPLY(TEXT("join"), &CRenoCommandFrame::ReplyJoin)
	REPLY(TEXT("quit"), &CRenoCommandFrame::ReplyQuit)
	REPLY(TEXT("mode"), &CRenoCommandFrame::ReplyMode)
	REPLY(TEXT("notice"), &CRenoCommandFrame::ReplyNotice)
	REPLY(TEXT("nick"), &CRenoCommandFrame::ReplyNickChange)
	REPLY(TEXT("error"), &CRenoCommandFrame::ReplyError)
	REPLY(TEXT("topic"), &CRenoCommandFrame::ReplyTopicChange)
	REPLY(IRC_RPL_MOTDSTART, &CRenoCommandFrame::ReplyMotd)
	REPLY(IRC_RPL_MOTD, &CRenoCommandFrame::ReplyMotd)
	REPLY(IRC_RPL_ENDOFMOTD, &CRenoCommandFrame::ReplyMotd)
	REPLY(IRC_RPL_TIME, &CRenoCommandFrame::ReplyTime)
	REPLY(IRC_RPL_WELCOME, &CRenoCommandFrame::ReplyInfo)
	REPLY(IRC_RPL_YOURHOST, &CRenoCommandFrame::ReplyInfo)
	REPLY(IRC_RPL_CREATED, &CRenoCommandFrame::ReplyInfo)
	//REPLY(IRC_RPL_MYINFO, &CRenoCommandFrame::ReplyInfo)
	//REPLY(IRC_RPL_BOUNCE, &CRenoCommandFrame::ReplyInfo)
	REPLY(IRC_RPL_TOPIC, &CRenoCommandFrame::ReplyTopic)
	REPLY(IRC_RPL_VERSION, &CRenoCommandFrame::ReplyInfo)
	//REPLY(IRC_RPL_LUSERCLIENT, &CRenoCommandFrame::ReplyInfo)
	//REPLY(IRC_RPL_LUSEROP, &CRenoCommandFrame::ReplyInfo)
	//REPLY(IRC_RPL_LUSERCHANNELS, &CRenoCommandFrame::ReplyInfo)
	//REPLY(IRC_RPL_STATSDLINE, &CRenoCommandFrame::ReplyInfo)
	REPLY(TEXT("*"), &CRenoCommandFrame::ReplyNull)	// Wildcard always goes last
END_REPLY_MAP()

BOOL CRenoCommandFrame::CmdServer(const CStringPtrArray& commandElements)
{
	// Maybe we are already connected
	if(m_State != RENO_NETWORK_DISCONNECTED)
	{
		if(m_State == RENO_NETWORK_CONNECTED)
			PrintFormat(TEXT("* Disconnect first\r\n")); 
		else
			PrintFormat(TEXT("* Connection in progress\r\n")); 

		return FALSE;
	}

	if(commandElements.GetCount() != 2)
	{
		PrintFormat(TEXT("* Usage: %s <address> [:port]\r\n"),commandElements[0]);
		return FALSE;
	}
	
	CString hostName = commandElements[1];

	// Get server list
	for(LONG i = 0; i < theApp.GetServerList().GetCount(); ++i)
	{
		LONG index = theApp.GetServerList()[i].Find(TEXT('|'));
		if(index != -1 && hostName == theApp.GetServerList()[i].GetBuffer() + index + 1)
			hostName = theApp.GetServerList()[i].Left(index);
	}

	// Check if port specified and add default port if not
	if(hostName.Find(TEXT(':')) == -1)
	{
		hostName += TEXT(':');
		hostName += IRC_DEFAULT_PORT;
	}

	// Check if the address is numeric (no need to resolve it then)
	sockaddr_in address;
	INT length = sizeof(address);
	if(StringToAddress(hostName,(LPSOCKADDR)&address,&length))
	{
		PrintFormat(TEXT("* Connecting to %s\r\n"),hostName);

		// Connect directly
		if(!Connect((LPSOCKADDR)&address,length))
		{
			switch(WSAGetLastError())
			{
			case WSAEWOULDBLOCK:
				//m_Board.InsertString(TEXT("* Connection in progress\r\n"));
				return TRUE;	// Skip the Close()

			default:
				PrintFormat(TEXT("* Failed to connect. Connection error %d\r\n"),WSAGetLastError());
				break;
			}

			Close();
		}
	}
	else if(!Resolve(hostName))
		PrintFormat(TEXT("* Failed to initiate resolve. Resolve queue full\r\n"));
	else
		PrintFormat(TEXT("* Resolving host %s\r\n"),hostName);

	return TRUE;
}

BOOL CRenoCommandFrame::CmdParamsServer(const CStringPtrArray& commandElements,CStringPtrArray& parameterElements)
{

	if(commandElements.GetCount() < 2)
	{
		// Get server list
		for(LONG i = 0; i < theApp.GetServerList().GetCount(); ++i)
		{
			LONG index = theApp.GetServerList()[i].Find(TEXT('|'));

			if(index != -1)
				parameterElements.Add(theApp.GetServerList()[i].Left(index));
			else
				parameterElements.Add(theApp.GetServerList()[i]);
		}

		// Get server list
		for(LONG i = 0; i < theApp.GetServerList().GetCount(); ++i)
		{
			LONG index = theApp.GetServerList()[i].Find(TEXT('|'));

			if(index != -1)
				parameterElements.Add(theApp.GetServerList()[i].GetBuffer() + index + 1);
		}

		return TRUE;
	}

/*
	if(commandElements.GetCount() < 2)
	{
		parameterElements.Add(TEXT("*"));	// We accept any input
		return TRUE;
	}
	else if(commandElements.GetCount() < 3)
	{
		parameterElements.Add(TEXT("6667"));
		parameterElements.Add(TEXT("6668"));
		parameterElements.Add(TEXT("6669"));
		parameterElements.Add(TEXT("6670"));
		parameterElements.Add(TEXT("6671"));
		return TRUE;
	}
*/
	return FALSE;
}

BOOL CRenoCommandFrame::CmdStatus(const CStringPtrArray& commandElements)
{
	switch(m_State)
	{
	case RENO_NETWORK_DISCONNECTED:
		PrintFormat(TEXT("* Not connected\r\n"));
		break;

	case RENO_NETWORK_CONNECTING:
		PrintFormat(TEXT("* Connecting\r\n"));
		break;

	case RENO_NETWORK_CONNECTED:
		PrintFormat(TEXT("* Connected\r\n"));
		break;

	case RENO_NETWORK_RESOLVING:
		PrintFormat(TEXT("* Resolving host address\r\n"));
		break;

	case RENO_NETWORK_DISCONNECTING:
		PrintFormat(TEXT("* Disconnecting\r\n"));
		break;

	default:
		PrintFormat(TEXT("* Unknown state\r\n"));
		AfxDebugBreak();
		break;
	}

	return TRUE;
}

BOOL CRenoCommandFrame::CmdNick(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() != 2)
	{
		PrintFormat(TEXT("* Usage: %s <nick>\r\n"),commandElements[0]);
		return FALSE;
	}

	return SendFormat(TEXT("NICK %s\r\n"),commandElements[1]);
}

BOOL CRenoCommandFrame::CmdUser(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() != 4)
	{
		PrintFormat(TEXT("* Usage: %s <user> <mode> <realname>\r\n"),commandElements[0]);
		return FALSE;
	}

	return SendFormat(TEXT("USER %s %s * :%s\r\n"),commandElements[1],commandElements[2],commandElements[3]);
}

BOOL CRenoCommandFrame::CmdLogin(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(!theApp.GetNickList().GetCount())
	{
		PrintFormat(TEXT("* No nicks entered in configuration\r\n"));
		return FALSE;
	}

	if(!theApp.GetRealName().GetLength())
	{
		PrintFormat(TEXT("* No real name entered in configuration\r\n"));
		return FALSE;
	}

	if(!theApp.GetUserName().GetLength())
	{
		PrintFormat(TEXT("* No user name entered in configuration\r\n"));
		return FALSE;
	}

	return SendFormat(TEXT("PASS ghjfhjfgjgfh\r\nUSER %s 0 * :%s\r\nNICK %s\r\n"),theApp.GetUserName(),theApp.GetRealName(),theApp.GetNickList()[0]);
}

BOOL CRenoCommandFrame::CmdDisconnect(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	// Tell the server we are quitting
	CString cmd = TEXT("QUIT ");

	if(commandElements.GetCount() > 1)
		cmd += TEXT(":");

	for(LONG i = 1; i < commandElements.GetCount(); ++i)
		cmd += commandElements[i] + ((i < commandElements.GetCount() - 1) ? TEXT(" ") : TEXT(""));

	cmd += TEXT("\r\n");

	Send(cmd,cmd.GetLength());

	// Close the connection
	Close();

	PrintFormat(TEXT("* Disconnected\r\n"));

	return TRUE;
}

BOOL CRenoCommandFrame::CmdQuit(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	CString cmd = TEXT("QUIT ");

	if(commandElements.GetCount() > 1)
		cmd += TEXT(":");

	for(LONG i = 1; i < commandElements.GetCount(); ++i)
		cmd += commandElements[i] + ((i < commandElements.GetCount() - 1) ? TEXT(" ") : TEXT(""));

	cmd += TEXT("\r\n");

	return Send(cmd,cmd.GetLength());
}

BOOL CRenoCommandFrame::CmdPart(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() < 2)
	{
		PrintFormat(TEXT("* Usage: %s <channel> [part_message]\r\n"),commandElements[0]);
		return FALSE;
	}

	CString cmd = TEXT("PART ");

	cmd += commandElements[1];

	if(commandElements.GetCount() > 2)
		cmd += TEXT(" :");

	for(LONG i = 2; i < commandElements.GetCount(); ++i)
		cmd += commandElements[i] + ((i < commandElements.GetCount() - 1) ? TEXT(" ") : TEXT(""));

	cmd += TEXT("\r\n");

	return Send(cmd,cmd.GetLength());
}

BOOL CRenoCommandFrame::CmdTopic(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() < 2)
	{
		PrintFormat(TEXT("* Usage: %s <channel> [new_topic]\r\n"),commandElements[0]);
		return FALSE;
	}

	CString cmd = TEXT("TOPIC ");

	cmd += commandElements[1];

	if(commandElements.GetCount() > 2)
		cmd += TEXT(" :");

	for(LONG i = 2; i < commandElements.GetCount(); ++i)
		cmd += commandElements[i] + ((i < commandElements.GetCount() - 1) ? TEXT(" ") : TEXT(""));

	cmd += TEXT("\r\n");

	return Send(cmd,cmd.GetLength());
}

BOOL CRenoCommandFrame::CmdMsg(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() < 2)
	{
		PrintFormat(TEXT("* Usage: %s <command> [parameters]...\r\n"),commandElements[0]);
		return FALSE;
	}

	CString cmd;

	for(LONG i = 1; i < commandElements.GetCount(); ++i)
		cmd += commandElements[i] + ((i < commandElements.GetCount() - 1) ? TEXT(" ") : TEXT(""));

	cmd += TEXT("\r\n");

	return Send(cmd,cmd.GetLength());
}

BOOL CRenoCommandFrame::CmdJoin(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() != 2)
	{
		PrintFormat(TEXT("* Usage: %s <channel>\r\n"),commandElements[0]);
		return FALSE;
	}

	return SendFormat(TEXT("JOIN %s\r\n"),commandElements[1]);
}

BOOL CRenoCommandFrame::CmdMotd(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() != 1)
	{
		PrintFormat(TEXT("* Usage: %s\r\n"),commandElements[0]);
		return FALSE;
	}

	return SendFormat(TEXT("MOTD\r\n"));
}

BOOL CRenoCommandFrame::CmdTime(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() != 1)
	{
		PrintFormat(TEXT("* Usage: %s\r\n"),commandElements[0]);
		return FALSE;
	}

	return SendFormat(TEXT("TIME\r\n"));
}

BOOL CRenoCommandFrame::CmdNames(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() != 1 && commandElements.GetCount() != 2)
	{
		PrintFormat(TEXT("* Usage: %s [channel]\r\n"),commandElements[0]);
		return FALSE;
	}

	if(commandElements.GetCount() == 1)
		return SendFormat(TEXT("NAMES\r\n"));

	return SendFormat(TEXT("NAMES %s\r\n"),commandElements[1]);
}

BOOL CRenoCommandFrame::CmdHelp(const CStringPtrArray& commandElements)
{
	PrintFormat(TEXT("* Supported commands:\r\n"));

	const RENO_COMMAND* commandMap = GetCommandMap();
	ASSERT(commandMap);

	ULONG i;

	for(i = 0; commandMap[i].name && commandMap[i].function; ++i)
		PrintFormat(TEXT("*   %12s - %s\r\n"),commandMap[i].name + 1,commandMap[i].description);

	PrintFormat(TEXT("* %d total command(s)\r\n"),i);

	return TRUE;
}

BOOL CRenoCommandFrame::CmdExit(const CStringPtrArray& commandElements)
{
	if(commandElements.GetCount() != 1)
	{
		PrintFormat(TEXT("* Usage: %s\r\n"),commandElements[0]);
		return FALSE;
	}

	AfxGetMainWnd()->PostMessage(WM_CLOSE);

	return TRUE;
}

BOOL CRenoCommandFrame::CmdClose(const CStringPtrArray& commandElements)
{
	if(commandElements.GetCount() != 1)
	{
		PrintFormat(TEXT("* Usage: %s\r\n"),commandElements[0]);
		return FALSE;
	}

	PostMessage(WM_CLOSE);

	return TRUE;
}

BOOL CRenoCommandFrame::CmdPrivate(const CStringPtrArray& commandElements)
{
	// Maybe we are not connected
	if(m_State < RENO_NETWORK_CONNECTED)
	{
		PrintFormat(TEXT("* Error: Not connected\r\n"));
		return FALSE;
	}

	if(commandElements.GetCount() < 3)
	{
		PrintFormat(TEXT("* Usage: %s <message_target> <text>\r\n"),commandElements[0]);
		return FALSE;
	}

	CString cmd = TEXT("PRIVMSG ");

	cmd += commandElements[1] + TEXT(" :");

	for(LONG i = 2; i < commandElements.GetCount(); ++i)
		cmd += commandElements[i] + ((i < commandElements.GetCount() - 1) ? TEXT(" ") : TEXT(""));

	cmd += TEXT("\r\n");

	if(theApp.GetNickList().GetCount())
		PrintFormat(TEXT("[%s] %s\r\n"),theApp.GetNickList()[0],commandElements[2]);
	else
		PrintFormat(TEXT("[????] %s\r\n"),commandElements[2]);

	return Send(cmd,cmd.GetLength());
}

BOOL CRenoCommandFrame::CmdClear(const CStringPtrArray& commandElements)
{
	if(commandElements.GetCount() != 1)
	{
		PrintFormat(TEXT("* Usage: %s\r\n"),commandElements[0]);
		return FALSE;
	}

	Clear();

	return TRUE;
}

BOOL CRenoCommandFrame::CmdLoad(const CStringPtrArray& commandElements)
{
	if(theApp.LoadSettings())
		PrintFormat(TEXT("* Configuration loaded\r\n"));
	else
		PrintFormat(TEXT("* Error: Failed to load configuration\r\n"));

	return TRUE;
}

BOOL CRenoCommandFrame::CmdSave(const CStringPtrArray& commandElements)
{
	if(theApp.SaveSettings())
		PrintFormat(TEXT("* Configuration saved\r\n"));
	else
		PrintFormat(TEXT("* Error: Failed to save configuration\r\n"));

	return TRUE;
}

BOOL CRenoCommandFrame::CmdHosts(const CStringPtrArray& commandElements)
{
	PrintFormat(TEXT("* Host list:\r\n"));

	for(LONG i = 0; i < theApp.GetServerList().GetCount(); ++i)
	{
		LONG index = theApp.GetServerList()[i].Find(TEXT('|'));
		if(index != -1)
			PrintFormat(TEXT("*   %s [%s]\r\n"),theApp.GetServerList()[i].Left(index),theApp.GetServerList()[i].GetBuffer() + index + 1);
		else
			PrintFormat(TEXT("*   %s\r\n"),theApp.GetServerList()[i]);
	}

	PrintFormat(TEXT("* %d host(s)\r\n"),theApp.GetServerList().GetCount());

	return TRUE;
}

BOOL CRenoCommandFrame::CmdAdd(const CStringPtrArray& commandElements)
{
	if(commandElements.GetCount() != 3 && commandElements.GetCount() != 4)
	{
		PrintFormat(TEXT("* Usage: %s <'nick'|'host'> <address/nick> [host_label]\r\n"),commandElements[0]);
		return FALSE;
	}

	if(!_wcsicmp(commandElements[1],TEXT("nick")))
	{
		for(LONG i = 0; i < theApp.GetNickList().GetCount(); ++i)
		{
			if(!_wcsicmp(commandElements[2],theApp.GetNickList()[i]))
			{
				PrintFormat(TEXT("* Error: Nick already in list\r\n"));
				return FALSE;
			}
		}

		theApp.GetNickList().Add(commandElements[2]);

		PrintFormat(TEXT("* Nick '%s' added\r\n"),theApp.GetNickList()[theApp.GetNickList().GetCount() - 1]);
	}
	else if(!_wcsicmp(commandElements[1],TEXT("host")))
	{
		// Check if already exists
		for(LONG i = 0; i < theApp.GetServerList().GetCount(); ++i)
		{
			LONG index = theApp.GetServerList()[i].Find(TEXT('|'));
			if(index != -1)
			{
				if(!_wcsicmp(commandElements[2],theApp.GetServerList()[i].Left(index)))
				{
					PrintFormat(TEXT("* Error: Address already in list\r\n"));
					return FALSE;
				}

				if(commandElements.GetCount() == 4 && !_wcsicmp(commandElements[3],theApp.GetServerList()[i].GetBuffer() + index + 1))
				{
					PrintFormat(TEXT("* Host name label already in use for '%s'\r\n"),theApp.GetServerList()[i].Left(index));
					return FALSE;
				}
			}
			else
			{
				if(!_wcsicmp(commandElements[2],theApp.GetServerList()[i]))
				{
					PrintFormat(TEXT("* Error: Address already in list\r\n"));
					return FALSE;
				}
			}
		}

		if(commandElements.GetCount() == 4)
			theApp.GetServerList().Add(commandElements[2] + TEXT("|") + commandElements[3]);
		else
			theApp.GetServerList().Add(commandElements[2]);

		PrintFormat(TEXT("* Host '%s' added\r\n"),theApp.GetServerList()[theApp.GetServerList().GetCount() - 1]);
	}
	else
	{
		PrintFormat(TEXT("* Error: Unknown type: %s\r\n"),commandElements[1]);
		return FALSE;
	}

	return TRUE;
}

BOOL CRenoCommandFrame::CmdRemove(const CStringPtrArray& commandElements)
{
	if(commandElements.GetCount() != 3)
	{
		PrintFormat(TEXT("* Usage: %s <'nick'|'host'> <address|host_label|nick>\r\n"),commandElements[0]);
		return FALSE;
	}

	if(!_wcsicmp(commandElements[1],TEXT("nick")))
	{
		for(LONG i = 0; i < theApp.GetNickList().GetCount(); ++i)
		{
			if(!_wcsicmp(commandElements[2],theApp.GetNickList()[i]))
			{
				theApp.GetNickList().RemoveAt(i);
				PrintFormat(TEXT("* Removed nick\r\n"));
				return TRUE;
			}
		}

		PrintFormat(TEXT("* Nick with the specified name does not exist\r\n"));
		return FALSE;
	}
	else if(!_wcsicmp(commandElements[1],TEXT("host")))
	{
		for(LONG i = 0; i < theApp.GetServerList().GetCount(); ++i)
		{
			LONG index = theApp.GetServerList()[i].Find(TEXT('|'));
			if(index != -1)
			{
				if(!_wcsicmp(commandElements[2],theApp.GetServerList()[i].Left(index)) || !_wcsicmp(commandElements[2],theApp.GetServerList()[i].GetBuffer() + index + 1))
				{
					theApp.GetServerList().RemoveAt(i);
					PrintFormat(TEXT("* Removed host\r\n"));
					return TRUE;
				}
			}
			else
			{
				if(!_wcsicmp(commandElements[2],theApp.GetServerList()[i]))
				{
					theApp.GetServerList().RemoveAt(i);
					PrintFormat(TEXT("* Removed host\r\n"));
					return TRUE;
				}
			}
		}

		PrintFormat(TEXT("* Host with the specified name/label does not exist\r\n"));
		return FALSE;
	}
	
	PrintFormat(TEXT("* Error: Unknown type: %s\r\n"),commandElements[1]);
	return FALSE;
}

BOOL CRenoCommandFrame::CmdParamsAdd(const CStringPtrArray& commandElements,CStringPtrArray& parameterElements)
{
	if(commandElements.GetCount() == 1)
	{
		parameterElements.Add(TEXT("host"));
		parameterElements.Add(TEXT("nick"));
		return TRUE;
	}

	return FALSE;
}

BOOL CRenoCommandFrame::CmdParamsRemove(const CStringPtrArray& commandElements,CStringPtrArray& parameterElements)
{
	if(commandElements.GetCount() == 1)
	{
		parameterElements.Add(TEXT("host"));
		parameterElements.Add(TEXT("nick"));
		return TRUE;
	}
	else if(commandElements.GetCount() == 2)
	{
		if(!_wcsicmp(commandElements[1],TEXT("host")))
		{
			for(LONG i = 0; i < theApp.GetServerList().GetCount(); ++i)
			{
				LONG index = theApp.GetServerList()[i].Find(TEXT('|'));
				if(index != -1)
					parameterElements.Add(theApp.GetServerList()[i].Left(index));
				else
					parameterElements.Add(theApp.GetServerList()[i]);
			}

			for(LONG i = 0; i < theApp.GetServerList().GetCount(); ++i)
			{
				LONG index = theApp.GetServerList()[i].Find(TEXT('|'));
				if(index != -1)
					parameterElements.Add(theApp.GetServerList()[i].GetBuffer() + index + 1);
			}
		}
		else if(!_wcsicmp(commandElements[1],TEXT("nick")))
		{
			for(LONG i = 0; i < theApp.GetNickList().GetCount(); ++i)
				parameterElements.Add(theApp.GetNickList()[i]);
		}
	}

	return FALSE;
}

BOOL CRenoCommandFrame::ReplyNull(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	CString message = TEXT("'") + prefix + TEXT("' '") + command + TEXT("'");

	for(LONG i = 0; i < parameters.GetCount(); ++i)
		message += TEXT(" '") + parameters[i] + TEXT("'");

	PrintFormat(TEXT("%s\r\n"),message);

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyPing(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	if(parameters.GetCount())
		SendFormat(TEXT("PONG :%s\r\n"),parameters[0]);
	else
		SendFormat(TEXT("PONG\r\n"));

	PrintFormat(TEXT("* Pong!\r\n"));

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyPrivate(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	CString nick;

	if(prefix.GetLength())
	{
		LONG seperator = prefix.Find(TEXT('!'));
		if(seperator == -1)
			nick = prefix;
		else
			nick = prefix.Left(seperator);
	}

	if(!nick.GetLength())
		nick = TEXT("????");

	LPCTSTR ctcpAction = TEXT("\001ACTION ");
	LPCTSTR ctcpVersion = TEXT("\001VERSION\001");
	LPCTSTR ctcpPing = TEXT("\001PING\001");

	if(!_wcsnicmp(parameters[1],ctcpAction,wcslen(ctcpAction)))
	{
		PrintFormat(TEXT("* %s %s\r\n"),nick,parameters[1].Mid(wcslen(ctcpAction),parameters[1].Find(TEXT("\001"),wcslen(ctcpAction)) - wcslen(ctcpAction)));
	}
	else if(!_wcsnicmp(parameters[1],ctcpVersion,wcslen(ctcpVersion)))
	{
		SendFormat(TEXT("NOTICE %s :\001VERSION Reno:0.1:Marko Mihovilic\001\r\n"),nick);
		PrintFormat(TEXT("* Sent version CTCP reply\r\n"));
	}
	else if(!_wcsnicmp(parameters[1],ctcpPing,wcslen(ctcpPing)))
	{
		SendFormat(TEXT("NOTICE %s :\001PONG\001\r\n"),nick);
		PrintFormat(TEXT("* Sent pong CTCP reply\r\n"));
	}
	else
		PrintFormat(TEXT("[%s|%s] %s\r\n"),parameters[0],nick,parameters[1]);

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyQuit(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	CString nick;

	if(prefix.GetLength())
	{
		LONG seperator = prefix.Find(TEXT('!'));
		if(seperator == -1)
			nick = prefix;
		else
			nick = prefix.Left(seperator);
	}

	if(!nick.GetLength())
		nick = TEXT("????");
	
	if(parameters.GetCount() == 1 && parameters[0].GetLength())
		PrintFormat(TEXT("[Reno|Info] User %s has quit IRC (%s)\r\n"),nick,parameters[0]);
	else
		PrintFormat(TEXT("[Reno|Info] User %s has quit IRC\r\n"),nick);

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyJoin(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	CString nick;

	if(prefix.GetLength())
	{
		LONG seperator = prefix.Find(TEXT('!'));
		if(seperator == -1)
			nick = prefix;
		else
			nick = prefix.Left(seperator);
	}

	if(!nick.GetLength())
		nick = TEXT("????");
	
	if(parameters.GetCount() == 2 && parameters[1].GetLength())
		PrintFormat(TEXT("[%s|Info] User %s has joined (%s)\r\n"),parameters[0],nick,parameters[1]);
	else if(parameters.GetCount() > 0)
		PrintFormat(TEXT("[%s|Info] User %s has joined\r\n"),parameters[0],nick);
	else
		PrintFormat(TEXT("[Reno|Info] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyPart(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	CString nick;

	if(prefix.GetLength())
	{
		LONG seperator = prefix.Find(TEXT('!'));
		if(seperator == -1)
			nick = prefix;
		else
			nick = prefix.Left(seperator);
	}

	if(!nick.GetLength())
		nick = TEXT("????");

	if(parameters.GetCount() == 2 && parameters[1].GetLength())
		PrintFormat(TEXT("[Reno|Info] User %s is leaving %s (%s)\r\n"),nick,parameters[0],parameters[1]);
	else if(parameters.GetCount() == 1)
		PrintFormat(TEXT("[Reno|Info] User %s is leaving %s\r\n"),nick,parameters[0]);
	else
		PrintFormat(TEXT("[Reno|Info] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyNickChange(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	CString nick;

	if(prefix.GetLength())
	{
		LONG seperator = prefix.Find(TEXT('!'));
		if(seperator == -1)
			nick = prefix;
		else
			nick = prefix.Left(seperator);
	}

	if(!nick.GetLength())
		nick = TEXT("????");
	
	PrintFormat(TEXT("[Reno|Info] User %s changed his nick to %s\r\n"),nick,parameters[0]);

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyTopicChange(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	CString nick;

	if(prefix.GetLength())
	{
		LONG seperator = prefix.Find(TEXT('!'));
		if(seperator == -1)
			nick = prefix;
		else
			nick = prefix.Left(seperator);
	}

	if(!nick.GetLength())
		nick = TEXT("????");
	
	if(parameters.GetCount() == 2)
		PrintFormat(TEXT("[%s|Info] User %s changed topic to '%s'\r\n"),parameters[0],nick,parameters[1]);
	else
		PrintFormat(TEXT("[Reno|Topic] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());

	return TRUE;
}


BOOL CRenoCommandFrame::ReplyTime(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	switch(parameters.GetCount())
	{
	case 3:
		PrintFormat(TEXT("[%s|Time] %s\r\n"),parameters[1],parameters[2]);
		break;

	case 2:
		PrintFormat(TEXT("[Reno|Time] %s\r\n"),parameters[1]);
		break;

	default:
		PrintFormat(TEXT("[Reno|Time] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());
		break;
	}

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyInfo(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	switch(parameters.GetCount())
	{
	case 2:
		PrintFormat(TEXT("[%s|Info] %s\r\n"),parameters[0],parameters[1]);
		break;

	case 1:
		PrintFormat(TEXT("[Reno|Info] %s\r\n"),parameters[0]);
		break;

	default:
		PrintFormat(TEXT("[Reno|Info] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());
		break;
	}

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyTopic(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	switch(parameters.GetCount())
	{
	case 3:
		PrintFormat(TEXT("[%s|Topic] %s\r\n"),parameters[1],parameters[2]);
		break;

	case 2:
		PrintFormat(TEXT("[%s|Topic] %s\r\n"),parameters[0],parameters[1]);
		break;

	case 1:
		PrintFormat(TEXT("[Reno|Topic] %s\r\n"),parameters[0]);
		break;

	default:
		PrintFormat(TEXT("[Reno|Topic] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());
		break;
	}

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyMode(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	// http://webtoman.com/opera/panel/ircdmodes.html

	PrintFormat(TEXT("[Reno|Info] Mode set to %s for user %s\r\n"),parameters[1],parameters[0]);

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyNotice(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	switch(parameters.GetCount())
	{
	case 2:
		PrintFormat(TEXT("[%s|Notice] %s\r\n"),parameters[0],parameters[1]);
		break;

	case 1:
		PrintFormat(TEXT("[Reno|Notice] %s\r\n"),parameters[0]);
		break;

	default:
		PrintFormat(TEXT("[Reno|Notice] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());
		break;
	}

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyMotd(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	if(parameters.GetCount() == 2)
		PrintFormat(TEXT("[%s|Motd] %s\r\n"),parameters[0],parameters[1]);
	else
		PrintFormat(TEXT("[Reno|Motd] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());

	return TRUE;
}

BOOL CRenoCommandFrame::ReplyError(const CString& prefix,const CString& command,const CStringPtrArray& parameters)
{
	if(parameters.GetCount() == 1)
		PrintFormat(TEXT("[Reno|Error] %s\r\n"),parameters[0]);
	else
		PrintFormat(TEXT("[Reno|Error] Error: Reply in invalid format ('%s' '%s' '%d')\r\n"),prefix,command,parameters.GetCount());

	return TRUE;
}