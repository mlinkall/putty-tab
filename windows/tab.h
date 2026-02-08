/*
 * tab.h - the PuTTY(tel)/pterm main program, which runs a PuTTY
 */

#ifndef TAB_H
#define TAB_H

#define MAX_WINDOW 20
extern WinGuiSeat *_wgs[MAX_WINDOW];

#define IDC_PAGE1 1002
#define IDC_PAGE2 1003

extern HINSTANCE hinst;
extern HWND hDialog;
extern HWND hTabCtrl;
extern HWND hPages[MAX_WINDOW];
extern int tabActive;
extern int tabTotal;

void tabInit(void);

int tabPageAdd(const char *pageTitle);
int tabPageDelete(int aIndex);
int tabPageCount();

int tabAdd(WinGuiSeat **aWgs);
int tabRemove(WinGuiSeat *aWgs);
int tabCount();
WinGuiSeat *tabGet(int aIndex);
void tabSetActive(int aIndex);
int tabGetActiveIndex(void);
int tabGetIndexByHwnd(HWND hwnd);
WinGuiSeat *tabGetActive(void);

LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int DialogAdd(HINSTANCE inst, int guess_width, int guess_height);

#endif // TAB_H
