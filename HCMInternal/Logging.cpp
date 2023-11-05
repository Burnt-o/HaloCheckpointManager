#include "pch.h"
#include "Logging.h"





constexpr std::string_view logFileName = "HCMInternal_Logging";

enum AppenderID {
    ConsoleAppender = 1,
    FileAppender = 2
};

void Logging::initConsoleLogging()
{

    Logging::openConsole();

    // Setup console appender
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init<ConsoleAppender>(plog::verbose, &consoleAppender);


    // log using both appenders
     // Always set this to verbose, the appenders will filter according to their own severity
    plog::init(plog::verbose).addAppender(plog::get<ConsoleAppender>());
}

 void Logging::initFileLogging(std::string dirPath)
{
     // Setup file appender
     // Get log file path

     SYSTEMTIME t;
     GetSystemTime(&t);
     logFileDestination = dirPath;
     logFileDestination += std::format("{}_{:04}{:02}{:02}_{:02}{:02}{:02}.txt", logFileName, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
   

     // test that the path is good
     std::fstream testPath(logFileDestination, std::ios::out | std::ios::in | std::ios::app);
     testPath << "test";
     if (testPath.bad())
     {
         // change logfiledest to MCC directory. Better than nothing.
         wchar_t buffer[MAX_PATH];
         GetModuleFileName(NULL, buffer, MAX_PATH);
         logFileDestination = wstr_to_str(buffer);
     }
     testPath.close();

     // Delete the file if it already exists
     remove(logFileDestination.c_str());

     // rolling behaviour disabled by setting maxSize to 0
     static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(logFileDestination.c_str(), 0, 0);
     plog::init<FileAppender>(plog::verbose, &fileAppender);

    // log using both appenders
     // Always set this to verbose, the appenders will filter according to their own severity
    plog::init(plog::verbose).addAppender(plog::get<FileAppender>());
}


 void Logging::openConsole()
 {
     if (consoleOpen) return;

     DWORD attachError = 0;
     DWORD allocError = 0;
     if (AttachConsole(GetCurrentProcessId()) == 0) // Try to attach to existing console
     {
         attachError = GetLastError();
         // A console probably didn't exist yet, so try allocating a new one
         if (AllocConsole() == 0) 
         {
             allocError = GetLastError(); 
         }
     }

     // open file streams
     FILE* fDummy;
     freopen_s(&fDummy, "CONIN$", "r", stdin);
     freopen_s(&fDummy, "CONOUT$", "w", stderr);
     freopen_s(&fDummy, "CONOUT$", "w", stdout);

     if (allocError != 0)
     {
         PLOG_ERROR << "Something went wrong setting up console logging" << std::endl
             << std::format("attach error code: {}, alloc error code: {}", attachError, allocError);
     }
     else
     {
         consoleOpen = true;
         PLOG_DEBUG << (attachError == 0 ? "Attached to existing console." : "Created a new console.");
     }

     

 }

  void Logging::closeConsole()
 {
      if (consoleOpen)
      {
          fclose(stdout);
          fclose(stdin);
          fclose(stderr);
          FreeConsole();
          consoleOpen = false;
      }

 }


  void Logging::SetConsoleLoggingLevel(plog::Severity newLevel)
  {
      plog::get<ConsoleAppender>()->setMaxSeverity(newLevel);
}


  void Logging::SetFileLoggingLevel(plog::Severity newLevel)
  {
      plog::get<FileAppender>()->setMaxSeverity(newLevel);
  }



