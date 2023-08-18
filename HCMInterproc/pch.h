// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <Psapi.h>
#include <winternl.h>
#include <Dbghelp.h>

#include <map>
#include <string>
#include <format>
#include <fstream>
#include <memory>
#include <array>
#include <TlHelp32.h>


// logging
#include <plog\Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog\Formatters\TxtFormatter.h>
#include <plog/Formatters/FuncMessageFormatter.h>

//boost

#endif //PCH_H
