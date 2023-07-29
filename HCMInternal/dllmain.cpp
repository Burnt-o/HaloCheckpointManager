// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <fstream>
#include <string>

#include "Logging.h"
#include "HCMDirPath.h"
#include "curl\curl.h"
#include "GlobalKill.h"
#include "ModuleCache.h"
#include "ModuleHookManager.h"
#include "D3D11Hook.h"
#include "ImGuiManager.h"
#include "PointerManager.h"
#include "HCMInternalGUI.h"
#include "MessagesGUI.h"
#include "GameStateHook.h"
#include "CheatManager.h"
#include "RPCClient.h"
#include "HeartbeatTimer.h"
//both plog and rpc define these
#ifdef LOG_INFO 
#define LOG_INFO LOG_INFO_P
#endif
#ifdef LOG_ERROR 
#define LOG_ERROR LOG_ERROR_P
#endif
#ifdef LOG_DEBUG 
#define LOG_DEBUG LOG_DEBUG_P
#endif







// Main Execution Loop
void RealMain(HMODULE dllHandle)
{
    acquire_global_unhandled_exception_handler();
    try
    {

        // rpc also inits logging
       auto rpc = std::make_shared<RPCClient>();


    curl_global_init(CURL_GLOBAL_DEFAULT);

    PLOG_INFO << "HCMInternal initializing";
    PLOG_DEBUG << "HCM path: " << HCMDirPath::GetHCMDirPath();


        ModuleCache::initialize();
        auto mhm = std::make_unique<ModuleHookManager>();
        auto ptr = std::make_unique<PointerManager>(); // must be after moduleCache but before anything that uses it in it's constructor
        auto gsh = std::make_unique<GameStateHook>(); // needs pointer manager




        auto d3d = std::make_unique<D3D11Hook>();
        auto imm = std::make_unique<ImGuiManager>(d3d.get()->presentHookEvent);

        auto HCMGUI = std::make_unique<HCMInternalGUI>(imm.get()->ImGuiRenderCallback, d3d.get()->resizeBuffersHookEvent, gsh.get()->gameLoadEvent); // needs to be after cheatManager
        
        auto mesGUI = std::make_unique<MessagesGUI>(imm.get()->ImGuiRenderCallback);
        auto exp = std::make_unique<RuntimeExceptionHandler>(imm.get()->ImGuiRenderCallback);
        auto cm = std::make_unique<CheatManager>(); // needs to be after MessagesGUI and PointerManager


        auto hb = std::make_unique<HeartbeatTimer>(rpc);





        PLOG_INFO << "All services succesfully initialized! Entering main loop";
        Sleep(100);
        // Shutdown the console on successful init, at least in release mode.
        // If an initialization error occurs before this point, console will be left up so user can look at it.
        MessagesGUI::addMessage("HCM successfully initialised!");


#ifndef HCM_DEBUG
        PLOG_DEBUG << "Closing console";
        Logging::closeConsole();
#endif 


        // We live in this loop 99% of the time
        while (!GlobalKill::isKillSet()) {
#ifdef HCM_DEBUG
            if (GetKeyState(VK_END) & 0x8000)
            {
                PLOG_INFO << "HCMInternal manually killed";
                GlobalKill::killMe();
            }
#endif


            Sleep(10);

        }

    }
    catch (HCMInitException& ex)
    {
        PLOG_FATAL << "Failed initializing: " << ex.what();
        PLOG_FATAL << "Please send Burnt the log file located at: " << std::endl << Logging::GetLogFileDestination();
        std::cout << "Press Enter to shutdown HCMInternal";
        GlobalKill::killMe();
        std::cin.ignore();
    }

    // Auto managed resources have fallen out of scope
    PLOG_INFO << "HCMInternal singletons successfully shut down";

    curl_global_cleanup(); PLOG_INFO << "Curl cleaned up";
    release_global_unhandled_exception_handler(); PLOG_INFO << "Released exception handler";

#ifdef HCM_DEBUG
    PLOG_DEBUG << "Closing console";
    Logging::closeConsole();
#endif 

}

// This thread is created by the dll when loaded into the process, see RealMain() for the actual event loop.
// Do NOT put any allocations in this function because the call to FreeLibraryAndExitThread()
// will occur before they fall out of scope and will not be cleaned up properly! This is very
// important for being able to hotload the DLL multiple times without restarting the game.
DWORD WINAPI MainThread(HMODULE hDLL)
{

    RealMain(hDLL);

    Sleep(200);
    FreeLibraryAndExitThread(hDLL, NULL);
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{

    DWORD dwThreadID;

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(0, 0x1000, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, &dwThreadID);
    }

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;

}

