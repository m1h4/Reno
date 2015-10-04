#include "RenoGlobals.h"
#include "RenoDebug.h"

#ifdef _DEBUG
LPCTSTR GetSocketErrorString(INT error)
{
	switch(error)
	{
	case WSAEINTR:
		return TEXT("A blocking operation was interrupted by a call to WSACancelBlockingCall");
	case WSAEBADF:
		return TEXT("The file handle supplied is not valid");
	case WSAEACCES:
		return TEXT("An attempt was made to access a socket in a way forbidden by its access permissions");
	case WSAEFAULT:
		return TEXT("The system detected an invalid pointer address in attempting to use a pointer argument in a call");
	case WSAEINVAL:
		return TEXT("An invalid argument was supplied");
	case WSAEMFILE:
		return TEXT("Too many open sockets");
	case WSAEWOULDBLOCK:
		return TEXT("A non-blocking socket operation could not be completed immediately");
	case WSAEINPROGRESS:
		return TEXT("A blocking operation is currently executing");
	case WSAEALREADY:
		return TEXT("An operation was attempted on a non-blocking socket that already had an operation in progress");
	case WSAENOTSOCK:
		return TEXT("An operation was attempted on something that is not a socket");
	case WSAEDESTADDRREQ:
		return TEXT("A required address was omitted from an operation on a socket");
	case WSAEMSGSIZE:
		return TEXT("A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself");
	case WSAEPROTOTYPE:
		return TEXT("A protocol was specified in the socket function call that does not support the semantics of the socket type requested");
	case WSAENOPROTOOPT:
		return TEXT("An unknown, invalid, or unsupported option or level was specified in a getsockopt or setsockopt call");
	case WSAEPROTONOSUPPORT:
		return TEXT("The requested protocol has not been configured into the system, or no implementation for it exists");
	case WSAESOCKTNOSUPPORT:
		return TEXT("The support for the specified socket type does not exist in this address family");
	case WSAEOPNOTSUPP:
		return TEXT("The attempted operation is not supported for the type of object referenced");
	case WSAEPFNOSUPPORT:
		return TEXT("The protocol family has not been configured into the system or no implementation for it exists");
	case WSAEAFNOSUPPORT:
		return TEXT("An address incompatible with the requested protocol was used");
	case WSAEADDRINUSE:
		return TEXT("Only one usage of each socket address (protocol/network address/port) is normally permitted");
	case WSAEADDRNOTAVAIL:
		return TEXT("The requested address is not valid in its context");
	case WSAENETDOWN:
		return TEXT("A socket operation encountered a dead network");
	case WSAENETUNREACH:
		return TEXT("A socket operation was attempted to an unreachable network");
	case WSAENETRESET:
		return TEXT("The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress");
	case WSAECONNABORTED:
		return TEXT("An established connection was aborted by the software in your host machine");
	case WSAECONNRESET:
		return TEXT("An existing connection was forcibly closed by the remote host");
	case WSAENOBUFS:
		return TEXT("An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full");
	case WSAEISCONN:
		return TEXT("A connect request was made on an already connected socket");
	case WSAENOTCONN:
		return TEXT("A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied");
	case WSAESHUTDOWN:
		return TEXT("A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call");
	case WSAETOOMANYREFS:
		return TEXT("Too many references to some kernel object");
	case WSAETIMEDOUT:
		return TEXT("A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond");
	case WSAECONNREFUSED:
		return TEXT("No connection could be made because the target machine actively refused it");
	case WSAELOOP:
		return TEXT("Cannot translate name");
	case WSAENAMETOOLONG:
		return TEXT("Name component or name was too long");
	case WSAEHOSTDOWN:
		return TEXT("A socket operation failed because the destination host was down");
	case WSAEHOSTUNREACH:
		return TEXT("A socket operation was attempted to an unreachable host");
	case WSAENOTEMPTY:
		return TEXT("Cannot remove a directory that is not empty");
	case WSAEPROCLIM:
		return TEXT("A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously");
	case WSAEUSERS:
		return TEXT("Ran out of quota");
	case WSAEDQUOT:
		return TEXT("Ran out of disk quota");
	case WSAESTALE:
		return TEXT("File handle reference is no longer available");
	case WSAEREMOTE:
		return TEXT("Item is not available locally");
	case WSASYSNOTREADY:
		return TEXT("WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable");
	case WSAVERNOTSUPPORTED:
		return TEXT("The Windows Sockets version requested is not supported");
	case WSANOTINITIALISED:
		return TEXT("Either the application has not called WSAStartup, or WSAStartup failed");
	case WSAEDISCON:
		return TEXT("Returned by WSARecv or WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence");
	case WSAENOMORE:
		return TEXT("No more results can be returned by WSALookupServiceNext");
	case WSAECANCELLED:
		return TEXT("A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled");
	case WSAEINVALIDPROCTABLE:
		return TEXT("The procedure call table is invalid");
	case WSAEINVALIDPROVIDER:
		return TEXT("The requested service provider is invalid");
	case WSAEPROVIDERFAILEDINIT:
		return TEXT("The requested service provider could not be loaded or initialized");
	case WSASYSCALLFAILURE:
		return TEXT("A system call that should never fail has failed");
	case WSASERVICE_NOT_FOUND:
		return TEXT("No such service is known. The service cannot be found in the specified name space");
	case WSATYPE_NOT_FOUND:
		return TEXT("The specified class was not found");
	case WSA_E_NO_MORE:
		return TEXT("No more results can be returned by WSALookupServiceNext");
	case WSA_E_CANCELLED:
		return TEXT("A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled");
	case WSAEREFUSED:
		return TEXT("A database query failed because it was actively refused");
	case WSAHOST_NOT_FOUND:
		return TEXT("No such host is known");
	case WSATRY_AGAIN:
		return TEXT("This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server");
	case WSANO_RECOVERY:
		return TEXT("A non-recoverable error occurred during a database lookup");
	case WSANO_DATA:
		return TEXT("The requested name is valid, but no data of the requested type was found");
	case WSA_QOS_RECEIVERS:
		return TEXT("At least one reserve has arrived");
	case WSA_QOS_SENDERS:
		return TEXT("At least one path has arrived");
	case WSA_QOS_NO_SENDERS:
		return TEXT("There are no senders");
	case WSA_QOS_NO_RECEIVERS:
		return TEXT("There are no receivers");
	case WSA_QOS_REQUEST_CONFIRMED:
		return TEXT("Reserve has been confirmed");
	case WSA_QOS_ADMISSION_FAILURE:
		return TEXT("Error due to lack of resources");
	case WSA_QOS_POLICY_FAILURE:
		return TEXT("Rejected for administrative reasons - bad credentials");
	case WSA_QOS_BAD_STYLE:
		return TEXT("Unknown or conflicting style");
	case WSA_QOS_BAD_OBJECT:
		return TEXT("Problem with some part of the filterspec or providerspecific buffer in general");
	case WSA_QOS_TRAFFIC_CTRL_ERROR:
		return TEXT("Problem with some part of the flowspec");
	case WSA_QOS_GENERIC_ERROR:
		return TEXT("General QOS error");
	case WSA_QOS_ESERVICETYPE:
		return TEXT("An invalid or unrecognized service type was found in the flowspec");
	case WSA_QOS_EFLOWSPEC:
		return TEXT("An invalid or inconsistent flowspec was found in the QOS structure");
	case WSA_QOS_EPROVSPECBUF:
		return TEXT("Invalid QOS provider-specific buffer");
	case WSA_QOS_EFILTERSTYLE:
		return TEXT("An invalid QOS filter style was used");
	case WSA_QOS_EFILTERTYPE:
		return TEXT("An invalid QOS filter type was used");
	case WSA_QOS_EFILTERCOUNT:
		return TEXT("An incorrect number of QOS FILTERSPECs were specified in the FLOWDESCRIPTOR");
	case WSA_QOS_EOBJLENGTH:
		return TEXT("An object with an invalid ObjectLength field was specified in the QOS provider-specific buffer");
	case WSA_QOS_EFLOWCOUNT:
		return TEXT("An incorrect number of flow descriptors was specified in the QOS structure");
	case WSA_QOS_EUNKOWNPSOBJ:
		return TEXT("An unrecognized object was found in the QOS provider-specific buffer");
	case WSA_QOS_EPOLICYOBJ:
		return TEXT("An invalid policy object was found in the QOS provider-specific buffer");
	case WSA_QOS_EFLOWDESC:
		return TEXT("An invalid QOS flow descriptor was found in the flow descriptor list");
	case WSA_QOS_EPSFLOWSPEC:
		return TEXT("An invalid or inconsistent flowspec was found in the QOS provider specific buffer");
	case WSA_QOS_EPSFILTERSPEC:
		return TEXT("An invalid FILTERSPEC was found in the QOS provider-specific buffer");
	case WSA_QOS_ESDMODEOBJ:
		return TEXT("An invalid shape discard mode object was found in the QOS provider specific buffer");
	case WSA_QOS_ESHAPERATEOBJ:
		return TEXT("An invalid shaping rate object was found in the QOS provider-specific buffer");
	case WSA_QOS_RESERVED_PETYPE:
		return TEXT("A reserved policy element was found in the QOS provider-specific buffer");
	default:
		return TEXT("Unknown error)");
	}
}
#endif

// Debug help function for assigning a name to a thread
#ifdef _DEBUG
typedef struct
{
	DWORD dwType;		// Must be 0x1000
	LPCSTR szName;		// Pointer to name (in user address space)
	DWORD dwThreadID;	// Thread ID (-1 = caller thread)
	DWORD dwFlags;		// Reserved for future use, must be zero
} THREADNAMEINFO,*LPTHREADNAMEINFO;

VOID SetThreadName(LPCSTR szThreadName)
{
	THREADNAMEINFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = -1;
	info.dwFlags = 0;

	__try
	{
		RaiseException(0x406D1388,0,sizeof(info)/sizeof(DWORD),(LPDWORD)&info);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}
#endif