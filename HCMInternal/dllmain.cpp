// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <fstream>
#include <string>
#include "WindowsUtilities.h"

#include "App.h"
#include "MCCInitialisationCheck.h"


// Main Execution Loop
void RealMain(HMODULE dllHandle)
{
    App app = App(dllHandle); // app blocks at the end of it's constructor until it's kill condition is met
}

// This thread is created by the dll when loaded into the process, see RealMain() for the actual event loop.
// Do NOT put any allocations in this function because the call to FreeLibraryAndExitThread()
// will occur before they fall out of scope and will not be cleaned up properly! This is very
// important for being able to hotload the DLL multiple times without restarting the game.
DWORD WINAPI MainThread(HMODULE hDLL)
{


    auto mccIsInitialised = MCCInitialisationCheck(hDLL);
    while (mccIsInitialised.has_value() && mccIsInitialised.value() == false)
    {
        Sleep(100);
        mccIsInitialised = MCCInitialisationCheck(hDLL);
    }

    RealMain(hDLL);

    Sleep(200);
    FreeLibraryAndExitThread(hDLL, NULL);
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CreateThread(0, 0x1000, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, NULL);
    }

    return TRUE;

}

