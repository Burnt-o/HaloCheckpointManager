// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Events.h"

// Custom appender for plog calls LogEventInvoke (and thus pass the log message to HCMExternal.Services.InterprocService)
namespace plog
{
    class MyAppender : public IAppender
    {
    public:
        virtual void write(const Record& record) PLOG_OVERRIDE
        {
            util::nstring str = TxtFormatter::format(record);
            LogEvent_Invoke(str.c_str());
        }

    }myAppender;
}


static bool initflag = false;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!initflag)
        {
            initflag = true;
            plog::init(plog::verbose, &plog::myAppender); // Initialize the logger with our appender.
        }

        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }



    return TRUE;
}




