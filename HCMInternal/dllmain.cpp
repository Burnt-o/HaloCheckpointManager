// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"


void MainThread(void* pHandle)
{

}


BOOL APIENTRY DllMain(HINSTANCE hinstDLL,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Run when DLL injected
        DisableThreadLibraryCalls(hinstDLL);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hinstDLL, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        // Run when DLL unloaded
        break;
    }
    return TRUE;
}

