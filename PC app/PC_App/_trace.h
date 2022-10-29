////////////////////////////////////////////////////////////////////////////
//	_trace.h
//
//	Cee'z
//	Jan-23rd-2010
//	
//	This is used for Debugging. Instead of break the program, we use 
//  output window of visual studio that print the results.
//	
//	#include "_trace.h"
//	Any time we need a TRACE we use
//			TRACE(...) TRACELN (...)or _trace(...) or _tracenl(...)
//	Example:
//			LPTSTR buffer;
//			buffer = (LPTSTR)GetSomeName(SomeValue);  // GetSomeName return LPSTR
//			_tracenl(buffer);
//			strcpy(buffer, "Hello world"); 
//			_tracenl(buffer);
//			_tracenl(_T("String Length = %d"),strlen());
//			
//
////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#ifdef _DEBUG_
bool _trace(TCHAR *format, ...);
bool _tracenl(TCHAR *format, ...);
#define TRACE _trace
#define TRACELN _traceln
#else
#define TRACE false && 
#define TRACELN false && 
#endif

#ifdef _DEBUG_
bool _trace(TCHAR *format, ...)
{
	TCHAR buffer[1000];

	va_list argptr;
	va_start(argptr, format);
	wvsprintf(buffer, format, argptr);
	va_end(argptr);

	OutputDebugString(buffer);

	return true;
}

bool _traceln(TCHAR *format, ...)
{
	TCHAR buffer[1000];

	va_list argptr;
	va_start(argptr, format);
	wvsprintf(buffer, format, argptr);
	va_end(argptr);

	OutputDebugString(buffer);
	
	return _trace((LPTSTR)TEXT("\n"));
}
#endif
