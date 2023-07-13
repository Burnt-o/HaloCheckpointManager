// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "WindowsUtilities.h"
#include "GlobalKill.h"
#include "Logging.h"
#include "boost\interprocess\ipc\message_queue.hpp"

void foo() {
    PLOG_DEBUG << "foo was called!" << std::endl;
}

using namespace boost::interprocess;

// Main Execution Loop
void RealMain(HMODULE dllHandle)
{


    acquire_global_unhandled_exception_handler();




    Logging::initLogging();
    Logging::SetConsoleLoggingLevel(plog::verbose);
    Logging::SetFileLoggingLevel(plog::verbose);
    PLOG_DEBUG << "RPC TEST RUNNING";
    // Do RPC stuff? 
    try {
        //Create a message_queue.
        message_queue mqE
        (open_or_create               //only create
            , "External_Messages"           //name
            , 100                       //max message number
            , sizeof(int)               //max message size
        );

        message_queue mqI
        (open_or_create               //only create
            , "Internal_Messages"           //name
            , 100                       //max message number
            , sizeof(int)               //max message size
        );

        unsigned int priority;
        std::size_t recvd_size;

        //Receive 100 numbers
        for (int i = 0; i < 100; ++i) {
            int number;
            mqE.receive(&number, sizeof(number), recvd_size, priority);

            if (number != i || recvd_size != sizeof(number))
                PLOG_ERROR << "eh" << number;
            else
            PLOG_DEBUG << number;

        }

        int h = 69;
        mqI.send(&h, sizeof(h), 0);

    }
    catch (interprocess_exception& ex) {
        PLOG_ERROR << ex.what() << std::endl;
    }
    //message_queue::remove("message_queue");



   


        // We live in this loop 99% of the time
        while (!GlobalKill::isKillSet()) {

            if (GetKeyState(VK_END) & 0x8000)
            {
                GlobalKill::killMe();
            }
            Sleep(10);

        }

    // Auto managed resources have fallen out of scope
    PLOG_INFO << "Closing down test";


    release_global_unhandled_exception_handler(); PLOG_INFO << "Released exception handler";


    PLOG_DEBUG << "Closing console";
    Logging::closeConsole();




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

