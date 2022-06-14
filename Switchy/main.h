#pragma once

#include <Windows.h>

#if _DEBUG
	#include <stdio.h>
#endif // _DEBUG

HHOOK hHook;
BOOL enabled = TRUE;
BOOL keystrokeProcessed = FALSE;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

inline void ShowError(LPCSTR message)
{
	MessageBox(NULL, message, "Error", MB_OK | MB_ICONERROR);
}

inline void ToggleCapsLockState()
{
	keybd_event(VK_CAPITAL, 0, 0, 0);
	keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);

#if _DEBUG
	printf("Caps Lock state has been toggled\n");
#endif // _DEBUG
}