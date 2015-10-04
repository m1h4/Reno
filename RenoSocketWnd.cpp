#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoSocketWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRenoSocketWnd

IMPLEMENT_DYNCREATE(CRenoSocketWnd, CRenoChildWnd)

BEGIN_MESSAGE_MAP(CRenoSocketWnd, CRenoChildWnd)
END_MESSAGE_MAP()

CRenoSocketWnd::CRenoSocketWnd() :
	m_Socket(INVALID_SOCKET),
	m_State(RENO_NETWORK_DISCONNECTED)
{
}

CRenoSocketWnd::~CRenoSocketWnd()
{
	// Make sure the socket is closed
	Close();
}

// CRenoSocketWnd message handlers

BOOL CRenoSocketWnd::Resolve(LPCTSTR lpszHostName)
{
	ASSERT(m_Socket == INVALID_SOCKET);
	ASSERT(m_State == RENO_NETWORK_DISCONNECTED);
	
	// Queue the resolve operation
	if(!theApp.AssociateResolve(lpszHostName,m_hWnd))
	{
		TRACE(TEXT("Failed to queue resolve operation for %s\n"),lpszHostName);
		return FALSE;
	}

	// We are now in the resolving state
	m_State = RENO_NETWORK_RESOLVING;
	return TRUE;
}

BOOL CRenoSocketWnd::IOCtl(LONG lCommand, LPDWORD lpArgument)
{
	ASSERT(m_Socket != INVALID_SOCKET);

	return !ioctlsocket(m_Socket,lCommand,lpArgument);
}

INT CRenoSocketWnd::Send(LPCSTR buffer,ULONG size,ULONG flags)
{
	ASSERT(m_Socket != INVALID_SOCKET);
	ASSERT(m_State >= RENO_NETWORK_CONNECTED);

	// Create a new overlapped context
	LPRENOOVERLAPPED overlapped = new RENOOVERLAPPED;
	ZeroMemory(overlapped,sizeof(RENOOVERLAPPED));

	overlapped->Buffer.len = size;
	overlapped->Buffer.buf = new char[size];
	CopyMemory(overlapped->Buffer.buf,buffer,size);
	overlapped->Receive = FALSE;	// This is a send operation
	overlapped->Socket = m_Socket;

	DWORD sent = 1;	// If the overlapped operation did not complete immediately it will not update this

	if(WSASend(m_Socket,&overlapped->Buffer,1,&sent,NULL,(LPOVERLAPPED)overlapped,NULL))
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			// Free the overlapped structure cause the completion notification will not be sent
			delete[] overlapped->Buffer.buf;
			delete overlapped;

			TRACE(TEXT("Error while WSASend %d\n"),WSAGetLastError());
		}
	}

	return sent;
}

INT CRenoSocketWnd::Receive(LPSTR buffer,ULONG size,ULONG flags)
{
	ASSERT(m_Socket != INVALID_SOCKET);
	ASSERT(m_State >= RENO_NETWORK_CONNECTED);

	WSABUF buff;

	buff.buf = buffer;
	buff.len = size;

	ULONG received;
	if(WSARecv(m_Socket,&buff,1,&received,&flags,NULL,NULL))
	{
		TRACE(TEXT("CRenoSocketWnd::Receive Error occurred %u\n"),WSAGetLastError());
		return 0;
	}

	return received;
}

BOOL CRenoSocketWnd::Connect(LPSOCKADDR address,DWORD addressLength)
{
	ASSERT(m_Socket == INVALID_SOCKET);
	ASSERT(m_State <= RENO_NETWORK_RESOLVING);
	ASSERT(address);
	ASSERT(addressLength);

	// Create the socket first
	m_Socket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,NULL,WSA_FLAG_OVERLAPPED);
	if(m_Socket == INVALID_SOCKET)
	{
		TRACE(TEXT("Failed to create socket %d\n"),WSAGetLastError());
		return FALSE;
	}

	// Associate an io port with our socket
	if(!theApp.AssociateCompletionPort(m_Socket,GetSafeHwnd()))
	{
		// Make sure the socket is closed
		Close();

		TRACE(TEXT("Failed to create io completion port %d.\n"),GetLastError());
		return FALSE;
	}

	// Set the events we need to receive
	if(WSAAsyncSelect(m_Socket,m_hWnd,WM_SOCKET_NOTIFY,FD_CONNECT|FD_CLOSE) == SOCKET_ERROR)
	{
		// Make sure the socket is closed
		Close();

		TRACE(TEXT("Failed to set socket events %d\n"),WSAGetLastError());
		return FALSE;
	}

	// Try to connect to the given address
	if(WSAConnect(m_Socket,address,addressLength,NULL,NULL,NULL,NULL) == SOCKET_ERROR)
	{
		// Check if it was the blocking singal
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			// We seem to have stumbled upon a error
			TRACE(TEXT("Error connecting %d.\n"),WSAGetLastError());

			// Make sure the socket is closed
			Close();

			return FALSE;
		}
	}

	// We are connecting
	m_State = RENO_NETWORK_CONNECTING;

	return TRUE;
}

VOID CRenoSocketWnd::Close()
{
	// Reset our network state
	m_State = RENO_NETWORK_DISCONNECTED;

	if(m_Socket != INVALID_SOCKET)
	{
		// Close the socket handle and set it to a invalid value
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

LRESULT CRenoSocketWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if we got a event message
	if(message == WM_SOCKET_NOTIFY)
	{
		// Get the event code from the param
		switch(WSAGETSELECTEVENT(lParam))
		{
		case FD_CONNECT:
			OnConnect(WSAGETSELECTERROR(lParam));
			break;

		case FD_CLOSE:
			OnClose(WSAGETSELECTERROR(lParam));
			break;

		default:
			// This should not happen
			AfxThrowNotSupportedException();
			break;
		}

		return TRUE;
	}
	else if(message == WM_RESOLVE_NOTIFY)
	{
		OnResolved(wParam,(LPADDRINFO)lParam);

		// We must free the resolve information
		freeaddrinfo((LPADDRINFO)lParam);

		return TRUE;
	}
	else if(message == WM_COMPLETION_NOTIFY)
	{
		OnReceive(NULL,(LPSTR)wParam,lParam);

		// We must free the receive buffer
		delete[] (LPSTR)wParam;

		return TRUE;
	}

	// Do default processing
	return CRenoChildWnd::WindowProc(message, wParam, lParam);
}

INT CRenoSocketWnd::Send(LPCTSTR message,ULONG size)
{
	// Get size of translate buffer
	INT bufferSize = WideCharToMultiByte(CP_UTF8,NULL,message,size,NULL,NULL,NULL,NULL);
	if(!bufferSize)
	{
		TRACE(TEXT("Error occurred while converting string to multibyte %d.\n"),GetLastError());
		return 0;
	}

	LPSTR buffer = new CHAR[bufferSize];
	ASSERT(buffer);

	// Convert the string to multibyte, string will not be null terminated
	bufferSize = WideCharToMultiByte(CP_UTF8,NULL,message,size,buffer,bufferSize,NULL,NULL);

	bufferSize = Send(buffer,bufferSize);

	delete[] buffer;

	return bufferSize;
}

INT CRenoSocketWnd::SendFormat(LPCTSTR format,...)
{
	static TCHAR buffer[4096];

    va_list args;
    va_start(args,format);
	INT written = vswprintf_s(buffer,sizeof(buffer)/sizeof(buffer[0]),format,args);
    va_end(args);

	if(written < 0)
		return 0;

	return Send(buffer,written);
}

BOOL CRenoSocketWnd::AddressToString(LPSOCKADDR address,DWORD addressLength,LPTSTR lpszHostName,DWORD length)
{
	ASSERT(lpszHostName);

	return WSAAddressToString(address,addressLength,NULL,lpszHostName,&length) == NULL;
}

BOOL CRenoSocketWnd::StringToAddress(LPCTSTR lpszHostName,LPSOCKADDR address,LPINT addressLength)
{
	ASSERT(address);
	ASSERT(addressLength);

	return WSAStringToAddress((LPWSTR)lpszHostName,AF_INET,NULL,address,addressLength) == NULL;
}

VOID CRenoSocketWnd::OnConnect(INT nErrorCode)
{
	if(nErrorCode)
	{
		// Make sure we are closed
		Close();
		return;
	}

	// We are now connected
	m_State = RENO_NETWORK_CONNECTED;

	// Queue inital receive which will chain all other receives
	LPRENOOVERLAPPED overlapped = new RENOOVERLAPPED;
	ZeroMemory(overlapped,sizeof(RENOOVERLAPPED));

	overlapped->Buffer.len = RENO_NETWORK_BUFFER_SIZE;
	overlapped->Buffer.buf = new char[RENO_NETWORK_BUFFER_SIZE];
	overlapped->Receive = TRUE;	// This is a receive operation
	overlapped->Socket = m_Socket;

	DWORD received;
	DWORD flags = MSG_PEEK;
	if(WSARecv(m_Socket,&overlapped->Buffer,1,&received,&flags,(LPOVERLAPPED)overlapped,NULL))
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			TRACE(TEXT("Error on WSARecv %d\n"),WSAGetLastError());

			// Close us
			Close();
		}
	}
}

VOID CRenoSocketWnd::OnResolved(INT nErrorCode,LPADDRINFO info)
{
	if(nErrorCode)
	{
		// Make sure we are closed
		Close();
		return;
	}

	// Execute the connect operation
	if(!Connect(info->ai_addr,info->ai_addrlen))
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			// Close us
			Close();
		}
	}
}

VOID CRenoSocketWnd::OnReceive(INT nErrorCode,LPSTR buffer,ULONG received)
{
	if(nErrorCode)
	{
		// Make sure we are closed
		Close();
	}
}

VOID CRenoSocketWnd::OnClose(INT nErrorCode)
{
	// Make sure our socket is closed
	Close();
}