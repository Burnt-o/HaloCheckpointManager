// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <map>
#include <string>
#include <format>
#include <fstream>
#include <memory>
#include <array>

#include "magic_enum\magic_enum_all.hpp" // enum reflection https://github.com/Neargye/magic_enum

// logging
#include <plog\Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog\Formatters\TxtFormatter.h>
#include <plog/Formatters/FuncMessageFormatter.h>

// for logging
template <typename T, typename F>
void once(T t, F f) {
    static bool first = true;
    if (first) {
        f();
        first = false;
    }
}
#define LOG_ONCE(x)   once([](){},[](){ x; });
#define LOG_ONCE_THIS(x)   once([this](){},[this](){ x; });
#define LOG_ONCE_CAPTURE(x, y)   once([y](){},[y](){ x; });

#endif //PCH_H
