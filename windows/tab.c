/*
 * window.c - the PuTTY(tel)/pterm main program, which runs a PuTTY
 * terminal emulator and backend in a window.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <wchar.h>

#define COMPILE_MULTIMON_STUBS

#include "log.h"
#include "putty.h"
#include "ssh.h"
#include "terminal.h"
#include "storage.h"
#include "putty-rc.h"
#include "win-gui-seat.h"
#include "tab.h"

#include <imm.h>
#include <commctrl.h>
#include <richedit.h>
#include <mmsystem.h>

WinGuiSeat *_wgs[MAX_WINDOW] = {NULL, };

HWND hDialog;
HWND hTabCtrl;
HWND hPages[MAX_WINDOW] = {NULL, };
int tabActive = 0;
int tabTotal = 0;
int tabPageSum = 0;

void
tabInit(void)
{
	int i;

	PUTTY_LOG(PUTTY_LOG_DEBUG, "Enter");
	for (i = 0; i < MAX_WINDOW; i++)
	{
		_wgs[i] = NULL;

		hPages[i] = 0;
	}

	tabActive = -1;

	tabTotal = 0;
}

int tabPageAdd(const char *pageTitle)
{
    TCITEMW tie = {0};
    tie.mask = TCIF_TEXT;

    tie.pszText = pageTitle;
    TabCtrl_InsertItem(hTabCtrl, tabPageSum, &tie);

    ++tabPageSum;

	return tabPageSum;
}

int tabPageDelete(int aIndex)
{
	return 0;
}

int tabPageCount()
{
	return tabPageSum;
}


int tabAdd(WinGuiSeat **aWgs)
{
	PUTTY_LOG(PUTTY_LOG_DEBUG, "Enter");

	if (tabTotal >= MAX_WINDOW)
	{
		return -1;
	}

	WinGuiSeat *wgs = snew(WinGuiSeat);

	int i;
	for (i = 0; i < MAX_WINDOW; i++)
	{
		if (_wgs[i] == NULL)
		{
			_wgs[i] = wgs;
			*aWgs = wgs;

			++tabTotal;

			return i;
		}
	}

	return -1;
}

int tabRemove(WinGuiSeat *aWgs)
{
	int i;

	PUTTY_LOG(PUTTY_LOG_DEBUG, "Enter");

	for (i = 0; i < MAX_WINDOW; i++)
	{
		if (_wgs[i] == aWgs)
		{
			_wgs[i] = NULL;

			--tabTotal;

			return i;
		}
	}

	return -1;
}

int tabCount()
{
	return tabTotal;
}

WinGuiSeat *
tabGet(int aIndex)
{
	if (aIndex >= MAX_WINDOW)
	{
		return NULL;
	}

	return _wgs[aIndex];
}

void
tabSetActive(int aIndex)
{
	tabActive = aIndex;

    WinGuiSeat *wgs = _wgs[aIndex];
    if (wgs != NULL)
    {
    	SetWindowLongPtr(wgs->term_hwnd, GWLP_USERDATA, (LONG_PTR)wgs);
    }
}

int
tabGetActiveIndex(void)
{
	return tabActive;
}

int tabGetIndexByHwnd(HWND hwnd)
{
	int i;

	for (i = 0; i < MAX_WINDOW; i++)
	{
		if (_wgs[i]->term_hwnd == hwnd)
		{
			return i;
		}
	}

	return 0;
}

WinGuiSeat *
tabGetActive(void)
{
	if (tabActive <0 || tabActive >= MAX_WINDOW)
	{
		return NULL;
	}

	return _wgs[tabActive];
}

int DialogAdd(HINSTANCE inst, int guess_width, int guess_height)
{
    WNDCLASSW wc = {0};

    wc.lpfnWndProc = DialogProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"CustomDialogClass";
    RegisterClassW(&wc);

    hDialog = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        L"CustomDialogClass",
        L"PuTTY Terminal",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, guess_width, guess_height,
        NULL, NULL, wc.hInstance, NULL);

    centre_window(hDialog);

    //ShowWindow(hDialog, SW_SHOW);
    ShowWindow(hDialog, SW_MAXIMIZE);

    RECT clientRect;
    GetClientRect(hDialog, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    hTabCtrl = CreateWindowExW(
        0,
        L"SysTabControl32",
        L"",
        WS_CHILD | WS_VISIBLE |
        WS_CLIPSIBLINGS |
        WS_TABSTOP,
        0, 0,
		clientWidth, clientHeight,
		hDialog,
        (HMENU)IDC_TABCONTROL,
        inst,
        NULL
    );
    ShowWindow(hTabCtrl, SW_SHOW);
    UpdateWindow(hTabCtrl);

	tabPageAdd("Page1");
	tabPageAdd("New ...");

    return 0;
}
