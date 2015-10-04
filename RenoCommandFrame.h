#pragma once

#include "RenoEdit.h"
#include "RenoBuffer.h"
#include "RenoSocketWnd.h"
#include "RenoCommand.h"

#define RENO_COMMAND_FRAME_SEPERATOR_MARGIN 2

#define WM_PARENT_DISCONNECT WM_USER+3
#define WM_CHILD_DISCONNECT WM_USER+4

// CRenoCommandFrame

class CRenoCommandFrame : public CRenoSocketWnd
{
	DECLARE_DYNCREATE(CRenoCommandFrame)

public:
	CRenoCommandFrame();
	virtual ~CRenoCommandFrame();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual VOID RecalcLayout(BOOL bNotify = TRUE);

// Implementation
protected:
	CRenoBuffer m_Buffer;
	CRenoEdit m_Edit;

public:
	BOOL ExecuteCommand(const CString& command);
	BOOL PrintFormat(LPCTSTR format,...);
	VOID Clear();

// Socket Overrides
	virtual VOID OnReceive(INT nErrorCode,LPSTR buffer,ULONG received);
	virtual VOID OnConnect(INT nErrorCode);
	virtual VOID OnClose(INT nErrorCode);
	virtual VOID OnResolved(INT nErrorCode,LPADDRINFO info);

// Command Map
public:
	DECLARE_COMMAND_MAP()

	DECLARE_COMMAND(CmdServer);
	DECLARE_COMMAND(CmdHosts);
	DECLARE_COMMAND(CmdStatus);
	DECLARE_COMMAND(CmdNick);
	DECLARE_COMMAND(CmdUser);
	DECLARE_COMMAND(CmdLogin);
	DECLARE_COMMAND(CmdDisconnect);
	DECLARE_COMMAND(CmdMsg);
	DECLARE_COMMAND(CmdHelp);
	DECLARE_COMMAND(CmdQuit);
	DECLARE_COMMAND(CmdExit);
	DECLARE_COMMAND(CmdClose);
	DECLARE_COMMAND(CmdPrivate);
	DECLARE_COMMAND(CmdClear);
	DECLARE_COMMAND(CmdJoin);
	DECLARE_COMMAND(CmdPart);
	DECLARE_COMMAND(CmdMotd);
	DECLARE_COMMAND(CmdTime);
	DECLARE_COMMAND(CmdNames);
	DECLARE_COMMAND(CmdLoad);
	DECLARE_COMMAND(CmdSave);
	DECLARE_COMMAND(CmdAdd);
	DECLARE_COMMAND(CmdRemove);
	DECLARE_COMMAND(CmdTopic);

	DECLARE_PARAMETER(CmdParamsNull) { return TRUE; }
	DECLARE_PARAMETER(CmdParamsServer);
	DECLARE_PARAMETER(CmdParamsAdd);
	DECLARE_PARAMETER(CmdParamsRemove);

// Reply Map
public:
	DECLARE_REPLY_MAP()

	DECLARE_REPLY(ReplyNull);
	DECLARE_REPLY(ReplyPing);
	DECLARE_REPLY(ReplyPrivate);
	DECLARE_REPLY(ReplyPart);
	DECLARE_REPLY(ReplyJoin);
	DECLARE_REPLY(ReplyQuit);
	DECLARE_REPLY(ReplyMode);
	DECLARE_REPLY(ReplyNotice);
	DECLARE_REPLY(ReplyMotd);
	DECLARE_REPLY(ReplyNickChange);
	DECLARE_REPLY(ReplyInfo);
	DECLARE_REPLY(ReplyError);
	DECLARE_REPLY(ReplyTime);
	DECLARE_REPLY(ReplyTopic);
	DECLARE_REPLY(ReplyTopicChange);

// Message Map
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg VOID OnFileClose();
	afx_msg VOID OnSetFocus(CWnd* pOldWnd);
	afx_msg INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg VOID OnPaint();
	afx_msg VOID OnDestroy();
};