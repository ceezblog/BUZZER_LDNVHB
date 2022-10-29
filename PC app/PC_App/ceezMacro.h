////////////////////////////////////////////////////////////////////////////
//	Cee'z Macro and utilities
//	Jan-23rd-2010
//	define _CMACRO_ 
//			define macro to work with controls using win32 api
//
//	#define _DEBUG_ to use debug
//	This is used for Debugging. Instead of break the program, we could use 
//  output window of visual studio that print the results.
//	
//	Any time we need a TRACE we use
//			TRACE(...) TRACELN (...)or _trace(...) or _tracenl(...)
//	Example:
//			LPTSTR buffer;
//			buffer = (LPTSTR)GetSomeName(SomeValue);  // GetSomeName return LPSTR
//			_tracenl(buffer);
//			strcpy(buffer, "Hello world"); 
//			_tracenl(buffer);
//			_tracenl(_T("String Length = %d"),strlen());
////////////////////////////////////////////////////////////////////////////


#ifndef _CMACRO_

#include <windows.h>

#define _CMACRO_
//Macro for control
#define cbGetCheck(in_hwnd)				SendMessage(in_hwnd, BM_GETCHECK, 0, 0)		// Get checked status of Check box
#define cbChecked(in_hwnd)				SendMessage(in_hwnd, BM_SETCHECK, BST_CHECKED, 0)	// Set checked status of Check box
#define cbUnChecked(in_hwnd)			SendMessage(in_hwnd, BM_SETCHECK, BST_UNCHECKED, 0)	// Set checked status of Check box
#define SetText(in_hwnd, in_str)		SendMessage(in_hwnd, WM_SETTEXT, 0, (LPARAM)in_str)		//HWnd, Text
#define GetText(in_hwnd, out_str)		SendMessage(in_hwnd, WM_GETTEXT,1024,(LPARAM)out_str)		//HWnd, Text (max:1024)
#define edSetLimitText(in_hwnd, in_int)	SendMessage(in_hwnd, EM_LIMITTEXT, in_int, 0)				//HWnd, int
#define cmDisable(in_hwnd)				SendMessage(in_hwnd, WM_ENABLE, FALSE, 0)				//HWnd
#define cmEnable(in_hwnd)				SendMessage(in_hwnd, WM_ENABLE, TRUE, 0)				//HWnd
#define lbInsertItem(in_hwnd, in_str)	SendMessage(in_hwnd, LB_INSERTSTRING, -1, (LPARAM)in_str)	//HWnd, Text
#define lbInsertItemZ(in_hwnd, in_index, in_str)	SendMessage(in_hwnd, LB_INSERTSTRING, in_index, (LPARAM)in_str)	//HWnd, Index, Text
#define lbRemoveItem(in_hwnd, in_index)	SendMessage(in_hwnd, LB_DELETESTRING, (WPARAM)in_index, 0)	//HWnd, Index
#define lbClearAll(in_hwnd)				SendMessage(in_hwnd, LB_RESETCONTENT, 0, 0)			//HWnd
#define lbGetCount(in_hwnd)				SendMessage(in_hwnd, LB_GETCOUNT, 0, 0)				//HWnd
#define lbGetText(in_hwnd, in_index, out_str)		SendMessage(in_hwnd, LB_GETTEXT, in_index, out_str)				//HWnd
#define lbGetCurSel(in_hwnd)			SendMessage(in_hwnd, LB_GETCURSEL , 0, 0)				//HWnd
#define lbSetCurSel(in_hwnd,in_index)	SendMessage(in_hwnd, LB_SETCURSEL, (WPARAM)in_index, 0)	//HWnd, Index
#define lbSetTopItem(in_hwnd,in_index)	SendMessage(in_hwnd, LB_SETTOPINDEX, (WPARAM)in_index, 0)	//HWnd, Index
#define cbAddItem(in_hwnd,in_index,s)	SendMessage(in_hwnd, CB_ADDSTRING, (WPARAM)in_index, (LPARAM)s)		//HWnd, Index, Text
#define cbRemoveItem(in_hwnd,in_index)	SendMessage(in_hwnd, CB_DELETESTRING, (WPARAM)in_index, 0)	//HWnd, Index
#define cbSetSelectIndex(in_hwnd,in_index)	SendMessage(in_hwnd, CB_SETCURSEL, (WPARAM)in_index, 0)		//HWnd, Index
#define cbGetSelectIndex(in_hwnd)		SendMessage(in_hwnd, CB_GETCURSEL, 0, 0)				//HWnd
#define cbRemoveAll(in_hwnd)			SendMessage(in_hwnd, CB_RESETCONTENT, 0, 0)			//HWnd
#define cbSetLimitText(in_hwnd, in_int)	SendMessage(in_hwnd, CB_LIMITTEXT, in_int, 0)				//HWnd, int

//Macro for String
#define tcstrlen(a)				_tcslen(a) / sizeof(WCHAR)
#define compareString			wcscmp
#define formatString  			swprintf
#define copyString(d,s)	  		swprintf(d,L"%s",s)
#define appendString(d,s)	  	swprintf(d,L"%s%s",d,s)
#define toLowerString(x)		_tcslwr(x)

#endif



#ifdef _DEBUG_

#include <windows.h>

bool _trace(TCHAR *format, ...);
bool _tracenl(TCHAR *format, ...);
#define TRACE _trace
#define TRACELN _traceln

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

#else
#define TRACE false && 
#define TRACELN false && 
#endif
