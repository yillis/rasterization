#ifndef UTIL__H
#define UTIL__H
#pragma once

#include <Windows.h>

inline HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024                 // Buffer size for console window titles.
	HWND hwndFound;                     // This is what is returned to the caller.
	char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
										// WindowTitle.
	char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
										// WindowTitle.

	// Fetch current window title.
	GetConsoleTitle((LPWSTR)pszOldWindowTitle, MY_BUFSIZE);

	// Format a "unique" NewWindowTitle.
	wsprintf((LPWSTR)pszNewWindowTitle, (LPWSTR)"%d/%d",
		GetTickCount(),
		GetCurrentProcessId());

	// Change current window title.
	SetConsoleTitle((LPWSTR)pszNewWindowTitle);

	// Ensure window title has been updated.
	Sleep(40);

	// Look for NewWindowTitle.
	hwndFound = FindWindow(NULL, (LPWSTR)pszNewWindowTitle);

	// Restore original window title.
	SetConsoleTitle((LPWSTR)pszOldWindowTitle);

	return(hwndFound);
}

static HWND console_hwnd = GetConsoleHwnd();
static HDC console_hdc = GetWindowDC(console_hwnd);

#endif // !UTIL__H