// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <fstream>
#include <string>
#include "WindowsUtilities.h"

#include "App.h"





// Main Execution Loop
void RealMain(HMODULE dllHandle)
{
    std::string dirPath;
    bool badDirPath = false;

    //char* buf = nullptr;
    //size_t sz = 0;
    //if (_dupenv_s(&buf, &sz, "HCM_DIRECTORY_PATH") == 0 && buf != nullptr)
    //{
    //    dirPath = buf;
    //    free(buf);
    //}

    //DWORD bufferSize = 65535; //Limit according to http://msdn.microsoft.com/en-us/library/ms683188.aspx
    //std::string buff;
    //buff.resize(bufferSize);
    //bufferSize = GetEnvironmentVariableA("HCM_DIRECTORY_PATH", &buff[0], bufferSize);
    //if (bufferSize)
    //{
    //    dirPath = std::string(buff);
    //}

 
        rpc::client client{"127.0.0.1", 8069};
        client.set_timeout(1000);
        std::string result = "none";
        try
        {
            dirPath = client.call("requestHCMDirectory").as<std::string>();
        }
        catch (rpc::rpc_error& e)
        {
            badDirPath = true;
            dirPath = GetMCCExePath();
        }

    


    App app{ dirPath, badDirPath, client }; // app blocks at the end of it's constructor until it's kill condition is met

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

