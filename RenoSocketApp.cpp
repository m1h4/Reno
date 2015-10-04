#include "RenoGlobals.h"
#include "Reno.h"
#include "RenoSocketApp.h"
#include "RenoDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRenoSocketApp

IMPLEMENT_DYNCREATE(CRenoSocketApp, CWinApp)

BEGIN_MESSAGE_MAP(CRenoSocketApp, CWinApp)
END_MESSAGE_MAP()

CRenoSocketApp::CRenoSocketApp() :
	m_WorkerThreads(NULL),
	m_ResolveQueue(NULL),
	m_ResolveCount(NULL),
	m_WorkerThreadCount(RENO_DEFAULT_WORKER_THREAD_COUNT),
	m_CompletionPort(INVALID_HANDLE_VALUE),
	m_ResolveEvent(INVALID_HANDLE_VALUE),
	m_ResolveThread(INVALID_HANDLE_VALUE)
{
	ZeroMemory(&m_ResolveSection,sizeof(m_ResolveSection));
}

CRenoSocketApp::~CRenoSocketApp()
{
}

BOOL CRenoSocketApp::InitInstance()
{
	// Initialize the woker threads and completion port
	if(!InitCompletionPort())
		return FALSE;

	// Initialize the resolve thread
	if(!InitResolver())
		return FALSE;

	return TRUE;
}

INT CRenoSocketApp::ExitInstance()
{
	// Clean up threads and port
	UnInitCompletionPort();

	// Clean up the resolver
	UnInitResolver();

	return CWinApp::ExitInstance();
}

// CRenoSocketApp message handlers

ULONG WINAPI CompletionPortWorkerThread(HANDLE completionPort)
{
	// Set thread name for debugger
	SetThreadName("Completion Port Worker Thread");

	LPRENOOVERLAPPED overlapped;
	ULONG transfered;
	HWND notify;

	while(true)
	{
		// Wait for a completion event
		BOOL result = GetQueuedCompletionStatus(completionPort,&transfered,(PULONG_PTR)&notify,(LPOVERLAPPED*)&overlapped,INFINITE);
		if(!notify)
			break;

		// Check if the operation was successfull
		if(!transfered || !result)
		{
			if(!transfered)
				TRACE(TEXT("%u: Socket handle associated with completion port was closed\n"),GetCurrentThreadId());

			// Free any passed in overlapped structure
			if(overlapped)
			{
				delete[] overlapped->Buffer.buf;
				delete overlapped;
			}

			continue;
		}

		if(overlapped->Receive)
		{
			// Find the last cr-lf
			ULONG read = 0;
			for(ULONG i = 0; i < transfered - 1; ++i)
				if(overlapped->Buffer.buf[i] == TEXT('\r') && overlapped->Buffer.buf[i + 1] == TEXT('\n'))
					read = i + 2;

			// TODO There is a condition where the receive buffer is too small to contain one complete message, so the
			// code just keeps looping in this section because no cr-lf can ever be found (in the case of IRC this is fixed
			// by using a receive buffer larger than 512 bytes cause that is the maximum message length and a cr-lf is guaranteed
			// to be found inside such a large buffer

			// Remove the data from the winsock receive buffer up to the last properly terminated message
			ULONG flags = 0;
			overlapped->Buffer.len = read;
			if(WSARecv(overlapped->Socket,&overlapped->Buffer,1,&transfered,&flags,NULL,NULL))
				TRACE(TEXT("%u: Error, should not block on WSARecv %d\n"),GetCurrentThreadId(),WSAGetLastError());

			// Notify that messages were received, the notified thread is responsible for freeing the passed buffer
			if(transfered)
			{
				if(!PostMessage(notify,WM_COMPLETION_NOTIFY,(WPARAM)overlapped->Buffer.buf,transfered))
					delete[] overlapped->Buffer.buf;
			}
			else
			{
				// Free the buffer
				delete[] overlapped->Buffer.buf;
			}

			// Reuse the overlapped structure for the next read

			overlapped->Internal = 0;
			overlapped->InternalHigh = 0;
			overlapped->Offset = 0;
			overlapped->OffsetHigh = 0;
			overlapped->Event = 0;

			overlapped->Buffer.len = RENO_NETWORK_BUFFER_SIZE;
			overlapped->Buffer.buf = new char[RENO_NETWORK_BUFFER_SIZE];
			overlapped->Receive = TRUE;	// This is a receive operation

			flags = MSG_PEEK;
			if(WSARecv(overlapped->Socket,&overlapped->Buffer,1,&read,&flags,(LPOVERLAPPED)overlapped,NULL))
			{
				if(WSAGetLastError() != WSA_IO_PENDING)
					TRACE(TEXT("%u: Error occured while WSARecv %d\n"),GetCurrentThreadId(),WSAGetLastError());
			}
		}
		else
		{
			// A send operation completed, delete the buffers used
			// TODO Could add check to see if data successfully sent

			delete[] overlapped->Buffer.buf;
			delete overlapped;
		}
	}

	return 0;
}

BOOL CRenoSocketApp::InitCompletionPort()
{
	ASSERT(m_CompletionPort == INVALID_HANDLE_VALUE);
	ASSERT(!m_WorkerThreads);

	// Find out number of processors and threads
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	m_WorkerThreadCount = RENO_WORKER_THREADS_PER_PROCESSOR * systemInfo.dwNumberOfProcessors;
	ASSERT(m_WorkerThreadCount);

	TRACE(TEXT("Info: Using %u processor(s) and %d thread(s) for processing.\n"),systemInfo.dwNumberOfProcessors,m_WorkerThreadCount);

	// Allocate memory to store thread handless
	m_WorkerThreads = new HANDLE[m_WorkerThreadCount];
	ASSERT(m_WorkerThreads);

	// Setup completion ports
	m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,NULL);
	ASSERT(m_CompletionPort);

	// Create worker threads
	for(ULONG i = 0; i < m_WorkerThreadCount; ++i)
		m_WorkerThreads[i] = ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)CompletionPortWorkerThread,m_CompletionPort,NULL,NULL);

	return TRUE;
}

VOID CRenoSocketApp::UnInitCompletionPort()
{
	// Tell worker threads to exit by giving them a null context
	for(ULONG i = 0; i < m_WorkerThreadCount; ++i)
		PostQueuedCompletionStatus(m_CompletionPort,NULL,NULL,NULL);

	// Wait for all threads to finish
	if(m_WorkerThreads)
		WaitForMultipleObjects(m_WorkerThreadCount,m_WorkerThreads,TRUE,INFINITE);

	CloseHandle(m_CompletionPort);
	m_CompletionPort = INVALID_HANDLE_VALUE;

	// Free the thread array
	delete[] m_WorkerThreads;
	m_WorkerThreads = NULL;
}

BOOL CRenoSocketApp::AssociateCompletionPort(SOCKET socket,HWND notify)
{
	ASSERT(m_CompletionPort != INVALID_HANDLE_VALUE);
	ASSERT(socket != INVALID_SOCKET);
	ASSERT(notify);

	// Associate new completion handle
	return CreateIoCompletionPort((HANDLE)socket,m_CompletionPort,(ULONG_PTR)notify,NULL) != NULL;
}

ULONG WINAPI ResolveThread(CRenoSocketApp* context)
{
	// Set our name for the debugger
	SetThreadName("Resolve Thread");

	while(true)
	{
		// Wait till a operaion is queued
		WaitForSingleObject(context->m_ResolveEvent,INFINITE);

		// Lock the queue
		EnterCriticalSection(&context->m_ResolveSection);

		// Are we being signaled to exit
		if(!context->m_ResolveCount)
		{
			// Release the list
			LeaveCriticalSection(&context->m_ResolveSection);
			break;
		}

		// Process while there are queued operations
		while(true)
		{
			CHAR address[NI_MAXHOST + 32];
			CHAR port[NI_MAXSERV + 32];

			// Convert the host name and service name to multibyte
			WideCharToMultiByte(CP_ACP,NULL,context->m_ResolveQueue[context->m_ResolveCount - 1].Host,-1,address,sizeof(address),NULL,NULL);
			WideCharToMultiByte(CP_ACP,NULL,context->m_ResolveQueue[context->m_ResolveCount - 1].Service,-1,port,sizeof(port),NULL,NULL);

			// Store the window handle from the context cause it will be used for sending notification messages later
			HWND notify = context->m_ResolveQueue[context->m_ResolveCount - 1].Notify;

			// Remove the top context
			--context->m_ResolveCount;

			// Unlock the resolve list
			LeaveCriticalSection(&context->m_ResolveSection);

			addrinfo hints;
			ZeroMemory(&hints,sizeof(hints));

			// Set the hints host address structure to out specifications
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			LPADDRINFO info = NULL;
			INT nError = getaddrinfo(address,port,&hints,&info);
			if(nError)
			{
				// Send error information
				PostMessage(notify,WM_RESOLVE_NOTIFY,nError,NULL);
			}
			else
			{
				// TODO Maybe write code that randomly picks an address from the list if there are more than one
				if(info->ai_next)
					TRACE(TEXT("Info: Resolved host (%S:%S) has multiple addresses.\n"),address,port);

				// Send resolved address information, the handler of the notification is responsible for freeing the information structure
				if(!PostMessage(notify,WM_RESOLVE_NOTIFY,NULL,(LRESULT)info))
					freeaddrinfo(info);
			}

			// Lock the queue
			EnterCriticalSection(&context->m_ResolveSection);
			
			// Have we cleared the queue
			if(!context->m_ResolveCount)
			{
				// Unlock the list
				LeaveCriticalSection(&context->m_ResolveSection);

				// Clear the event
				ResetEvent(context->m_ResolveEvent);
				break;
			}
		}
	}

	return 0;
}

BOOL CRenoSocketApp::InitResolver()
{
	ASSERT(m_ResolveEvent == INVALID_HANDLE_VALUE);
	ASSERT(m_ResolveThread == INVALID_HANDLE_VALUE);
	ASSERT(!m_ResolveCount);
	ASSERT(!m_ResolveQueue);

	// Create resolve critical section
	InitializeCriticalSection(&m_ResolveSection);

	// Create resolve queue
	m_ResolveQueue = new RESOLVECONTEXT[RENO_RESOLVE_QUEUE_MAX];

	// Create resolve event
	m_ResolveEvent = CreateEvent(NULL,NULL,NULL,NULL);

	// Create resolve thread
	m_ResolveThread = ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ResolveThread,this,NULL,NULL);

	return TRUE;
}

VOID CRenoSocketApp::UnInitResolver()
{
	// If we were not initialized exit
	if(!m_ResolveSection.LockCount)
		return;

	// Remove all resolve jobs
	EnterCriticalSection(&m_ResolveSection);

	// Free the queue
	delete[] m_ResolveQueue;

	m_ResolveQueue = NULL;
	m_ResolveCount = NULL;

	LeaveCriticalSection(&m_ResolveSection);

	// Signal the reslove thread to exit
	SetEvent(m_ResolveEvent);

	// Wait for resolve thread
	WaitForSingleObject(m_ResolveThread,INFINITE);

	CloseHandle(m_ResolveThread);
	m_ResolveThread = INVALID_HANDLE_VALUE;

	CloseHandle(m_ResolveEvent);
	m_ResolveEvent = INVALID_HANDLE_VALUE;

	// Release the critical section
	DeleteCriticalSection(&m_ResolveSection);
}

BOOL CRenoSocketApp::AssociateResolve(LPCTSTR host,HWND notify)
{
	ASSERT(host);
	ASSERT(notify != INVALID_HANDLE_VALUE);

	// Check if one more element will fit into the queue
	if(m_ResolveCount + 1 >= RENO_RESOLVE_QUEUE_MAX)
		return FALSE;

	// Need to split the host name from the service name
	LPCTSTR service = host;

	// Find the seperator char
	while(service[0] && service[0] != TEXT(':'))
		++service;

	// Skip the service name seperator
	if(*service)
		++service;

	// Lock the list
	EnterCriticalSection(&m_ResolveSection);

	// Create a address information context that will be passed to the resolve thread
	lstrcpyn(m_ResolveQueue[m_ResolveCount].Host,host,min(sizeof(m_ResolveQueue[m_ResolveCount].Host)/sizeof(TCHAR),service - host));
	lstrcpyn(m_ResolveQueue[m_ResolveCount].Service,service,sizeof(m_ResolveQueue[m_ResolveCount].Service)/sizeof(TCHAR));
	m_ResolveQueue[m_ResolveCount].Notify = notify;

	++m_ResolveCount;

	// Unlock the list
	LeaveCriticalSection(&m_ResolveSection);

	// Notify the thread
	SetEvent(m_ResolveEvent);

	return TRUE;
}