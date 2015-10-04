#pragma once

#define RENO_NETWORK_BUFFER_SIZE 1024	// Must be larger than 512 bytes
#define RENO_RESOLVE_QUEUE_MAX 6

#define WM_RESOLVE_NOTIFY WM_USER+1
#define WM_COMPLETION_NOTIFY WM_USER+2

#define RENO_DEFAULT_WORKER_THREAD_COUNT 2
#define RENO_WORKER_THREADS_PER_PROCESSOR 2

// Structure used for overlapped io calls, contains extra members for passing info to io threads
typedef struct
{
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
	DWORD Offset;
	DWORD OffsetHigh;
    HANDLE Event;
	BOOL Receive;	// Set to TRUE if the overlapped operation with which this structure is associated is a receive operation, FALSE if it's a send
	WSABUF Buffer;
	SOCKET Socket;
} RENOOVERLAPPED,*LPRENOOVERLAPPED;

// Structure used by the host name resolve system to store info about a resolve operation
typedef struct
{
	TCHAR Host[NI_MAXHOST];
	TCHAR Service[NI_MAXSERV];
	HWND Notify;
} RESOLVECONTEXT,*LPRESOLVECONTEXT;

// CRenoSocketApp

class CRenoSocketApp : public CWinApp
{
	DECLARE_DYNCREATE(CRenoSocketApp)

protected:
	CRenoSocketApp();			// Protected constructor used by dynamic creation
	virtual ~CRenoSocketApp();

// Implementation
public:
	ULONG m_WorkerThreadCount;
	LPHANDLE m_WorkerThreads;
	HANDLE m_CompletionPort;
	HANDLE m_ResolveThread;
	HANDLE m_ResolveEvent;
	CRITICAL_SECTION m_ResolveSection;
	LPRESOLVECONTEXT m_ResolveQueue;
	ULONG m_ResolveCount;

	BOOL InitCompletionPort();
	VOID UnInitCompletionPort();

	BOOL InitResolver();
	VOID UnInitResolver();

	BOOL AssociateCompletionPort(SOCKET socket,HWND notify);
	BOOL AssociateResolve(LPCTSTR host,HWND notify);

// Overrides
public:
	virtual BOOL InitInstance();
	virtual INT ExitInstance();

// Message Map
protected:
	DECLARE_MESSAGE_MAP()
};