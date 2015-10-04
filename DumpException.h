#pragma once

// Uncomment the following line to disable compressing the dump file with microsoft cabinet compresser
//#define DUMP_EXCEPTION_NO_COMPRESS

// Call SetUnhandledExceptionFilter at the begining of your program and pass the following function as the parameter
LONG WINAPI DumpException(LPEXCEPTION_POINTERS exceptionInfo);