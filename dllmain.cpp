// dllmain.cpp : Defines the entry point for the DLL application.
#define _WIN32_WINNT 0x0500 // for KBDLLHOOKSTRUCT
#include "stdafx.h"

extern HINSTANCE g_appInstance;
//extern int InstallKbdHook();
//extern int UninstallKbdHook();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// Needed by Hook function	
		if (g_appInstance == NULL)
		{
			g_appInstance = hModule;
		}
		OutputDebugString("Process attached.\n");
		//InstallKbdHook();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		OutputDebugString("Process detached.\n");
		//UninstallKbdHook();
		break;
	}
	return TRUE;
}