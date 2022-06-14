#include "main.h"

int main(int argc, char** argv)
{
	if (argc > 1 && strcmp(argv[1], "nopopup") == 0)
	{
		settings.popup = FALSE;
	}
	else
	{
		settings.popup = GetOSVersion() >= 10;
	}
#if _DEBUG
	printf("Pop-up %s\n", settings.popup ? "enabled" : "disabled");
#endif

	HANDLE hMutex = CreateMutex(0, 0, "Switchy");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		ShowError("Another instance of Switchy is already running!");
		return 1;
	}

	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
	if (hHook == NULL)
	{
		ShowError("Error calling \"SetWindowsHookEx(...)\"");
		return 1;
	}

	MSG messages;
	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	UnhookWindowsHookEx(hHook);

	return 0;
}

DWORD GetOSVersion()
{
	HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
	RTL_OSVERSIONINFOW osvi = { 0 };

	if (hMod)
	{
		RtlGetVersionPtr p = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");

		if (p)
		{
			osvi.dwOSVersionInfoSize = sizeof(osvi);
			p(&osvi);
		}
	}

	return osvi.dwMajorVersion;
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* key = (KBDLLHOOKSTRUCT*)lParam;
	if (nCode == HC_ACTION && !(key->flags & LLKHF_INJECTED))
	{
#if _DEBUG
		const char* keyStatus = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) ? "pressed" : "released";
		printf("Key %d has been %s\n", key->vkCode, keyStatus);
#endif // _DEBUG
		if (key->vkCode == VK_CAPITAL)
		{
			if (wParam == WM_SYSKEYDOWN && !keystrokeProcessed)
			{
				keystrokeProcessed = TRUE;
				enabled = !enabled;
#if _DEBUG
				printf("Switchy has been %s\n", enabled ? "enabled" : "disabled");
#endif // _DEBUG
				return 1;
			}

			if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
			{
				keystrokeProcessed = FALSE;

				if (winPressed)
				{
					winPressed = FALSE;
					ReleaseKey(VK_LWIN);
				}
			}

			if (!enabled)
			{
				return CallNextHookEx(hHook, nCode, wParam, lParam);
			}

			if (wParam == WM_KEYDOWN && !keystrokeProcessed)
			{
				keystrokeProcessed = TRUE;

				if (GetKeyState(VK_LSHIFT) & 0x8000)
				{
					ToggleCapsLockState();
					return 1;
				}
				else
				{
					if (settings.popup)
					{
						PressKey(VK_LWIN);
						PressKey(VK_SPACE);
						ReleaseKey(VK_SPACE);
						winPressed = TRUE;
					}
					else
					{
						HWND hWnd = GetForegroundWindow();
						if (hWnd)
						{
							hWnd = GetAncestor(hWnd, GA_ROOTOWNER);
							PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)HKL_NEXT);
						}
					}
				}
			}

			return 1;
		}
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}