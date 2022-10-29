/////////////////////////////////////////////////////////////////////////////////
//	_urlStatic.h
//
//	Author :		Cee'z
//	Date :			Jun-15-2009
//	Description:	This is support function for make HyperLink out of normal Static
//
//	HOW TO USE:
//
//	Declare a global variable:
//	HWND hwUrlStatic;		// This is the Static that me make it into hyperlink
//
//	In host DialogProc CALLBACK (message handling function) 
//	add these variable and messages:
//	
//		PAINTSTRUCT ps;
//		HDC hdc;
//
//		WM_INITDIALOG:
//			hwUrlStatic = GetDlgItem(hWnd, IDC_STATIC_ABOUT);
//			setStaticUnderline(hwUrlStatic);	// Make it underlining
//			urlStaticProc  = (WNDPROC)SetWindowLong(hwUrlStatic, GWL_WNDPROC, (LONG)StaticSubProc);
//			return 1; // done init correctly
//		WM_SETCURSOR:
//			setStaticCursor(hwUrlStatic);
//			return 0; // 
//		WM_CTLCOLORSTATIC:
//			if ((HWND)lParam == hwUrlStatic)
//				return setStaticColor(wParam, RGB(0,0,255), GetSysColor(COLOR_3DFACE));
//			return 0; // return FALSE to prevent effect to other Static, return TRUE will effect to other Static
//		WM_COMMAND:
//			switch(wParam) 
//			{
//				case IDC_STATIC_HYPERLINK:	// our Static ID
//				ShellExecute(NULL, L"open", L"http://my_url", NULL, NULL, SW_SHOWNORMAL);
//				return 1;	// return TRUE
//			}
//
/////////////////////////////////////////////////////////////////////////////////

#ifdef _URLSTATIC
#else

#define _URLSTATIC

WNDPROC urlStaticProc;

//  StaticSubProc, just set cursor into forefinger
WNDPROC StaticSubProc(HWND hWnd, UINT message, WPARAM wParam, LONG lParam)
{
	SetCursor(LoadCursor(NULL, IDC_HAND) );
	return (WNDPROC) CallWindowProc(urlStaticProc, hWnd, message, wParam, lParam);
}

// Change static color to blue
HBRUSH setStaticColor(WPARAM wParam, COLORREF textColor, COLORREF bgColor)
{
	SetBkMode((HDC)wParam,TRANSPARENT);
	SetTextColor((HDC)wParam, textColor);
	return CreateSolidBrush(bgColor);
}

// Change cursor into forefinger
void setStaticCursor(HWND hwStatic)
{
	POINT pt;
	RECT rect;
	GetCursorPos(&pt);
	GetWindowRect(hwStatic, &rect);
	if (PtInRect(&rect, pt)) SetCursor(LoadCursor(NULL,IDC_HAND));
	else SetCursor(LoadCursor(NULL,IDC_ARROW));
}

// Make it underlining
void setStaticUnderline(HWND hwStatic)
{
	HFONT hfnt = (HFONT) SendMessage(hwStatic, WM_GETFONT, 0 , 0);
	LOGFONT lf;
	if (GetObject(hfnt, sizeof(lf), &lf))
	{	
		lf.lfUnderline = TRUE;
		HFONT hf = CreateFontIndirect(&lf);
		SendMessage(hwStatic, WM_SETFONT, (WPARAM)hf, TRUE);
	}
}

#endif