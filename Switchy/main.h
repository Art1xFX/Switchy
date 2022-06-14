#pragma once

#include <Windows.h>
#if _DEBUG
#include <stdio.h>
#endif // _DEBUG

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

typedef struct {
	BOOL popup;
} Settings;

DWORD GetOSVersion();
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hHook;
BOOL enabled = TRUE;
BOOL keystrokeProcessed = FALSE;
BOOL winPressed = FALSE;

Settings settings = {
	.popup = FALSE
};

inline void ShowError(LPCSTR message)
{
	MessageBox(NULL, message, "Error", MB_OK | MB_ICONERROR);
}

inline void PressKey(int keyCode)
{
	keybd_event(keyCode, 0, 0, 0);
}

inline void ReleaseKey(int keyCode)
{
	keybd_event(keyCode, 0, KEYEVENTF_KEYUP, 0);
}

inline void ToggleCapsLockState()
{
	PressKey(VK_CAPITAL);
	ReleaseKey(VK_CAPITAL);
#if _DEBUG
	printf("Caps Lock state has been toggled\n");
#endif // _DEBUG
}