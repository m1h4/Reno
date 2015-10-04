#pragma once

// Debug helper for getting a string representation of a winsock error code
#ifdef _DEBUG
LPCTSTR GetSocketErrorString(INT error);
#else
#define GetSocketErrorString __noop
#endif

// Debug helper for settina a thread's name for the deubgger
#ifdef _DEBUG
VOID SetThreadName(LPCSTR szThreadName);
#else
#define SetThreadName __noop
#endif