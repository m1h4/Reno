#pragma once

#include "RenoChildWnd.h"

#define WM_SOCKET_NOTIFY WM_USER

// Network connection states
#define RENO_NETWORK_DISCONNECTED 0
#define RENO_NETWORK_RESOLVING 1
#define RENO_NETWORK_CONNECTING 2
#define RENO_NETWORK_CONNECTED 3
#define RENO_NETWORK_DISCONNECTING 4

// CRenoSocketWnd frame

class CRenoSocketWnd : public CRenoChildWnd
{
	DECLARE_DYNCREATE(CRenoSocketWnd)

protected:
	CRenoSocketWnd();           // Protected constructor used by dynamic creation
	virtual ~CRenoSocketWnd();

// Implementation
public:
	SOCKET m_Socket;
	ULONG m_State;

// Functions
public:
	INT SendFormat(LPCTSTR format,...);
	INT Send(LPCTSTR message,ULONG size);
	INT Send(LPCSTR buffer,ULONG size,ULONG flags = NULL);
	INT Receive(LPSTR buffer,ULONG size,ULONG flags = NULL);

	BOOL Resolve(LPCTSTR lpszHostName);
	BOOL Connect(LPSOCKADDR address,DWORD addressLength);
	VOID Close();

	BOOL IOCtl(LONG lCommand, LPDWORD lpArgument);

	BOOL AddressToString(LPSOCKADDR address,DWORD addressLength,LPTSTR lpszHostName,DWORD length);
	BOOL StringToAddress(LPCTSTR lpszHostName,LPSOCKADDR address,LPINT addressLength);

// Overridable Callbacks
	virtual VOID OnReceive(INT nErrorCode,LPSTR buffer,ULONG received);
	virtual VOID OnConnect(INT nErrorCode);
	virtual VOID OnClose(INT nErrorCode);
	virtual VOID OnResolved(INT nErrorCode,LPADDRINFO info);

public:
// Overrides
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Message Map
protected:
	DECLARE_MESSAGE_MAP()
};


