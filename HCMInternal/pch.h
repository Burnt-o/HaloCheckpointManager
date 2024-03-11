// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// Windows 
#define NOMINMAX 
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <Psapi.h>
#include <winternl.h>
#include <Dbghelp.h>


// Standard library
#include <iostream>
//#include <memory>
#include <vector>
#include <cstdint>
#include <optional>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <thread>
//#include <format>
#include <string>
#include <unordered_map>
#include <functional>
#include <utility>
#include <set>
#include <fstream>
#include <source_location>
#include <algorithm>
#include <string_view>
#include <array>
#include <tuple>
#include <inttypes.h>
#include <ranges>
#include <math.h>
#include <any>
#include <type_traits>
#include <variant>
#include <concepts>



// External Libraries
#include "gsl\gsl" //https://github.com/microsoft/GSL
// hooking
#include "safetyhook.hpp"






// logging
#define PLOG_OMIT_LOG_DEFINES //both plog and rpc define these
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Initializers/ConsoleInitializer.h>

//https://github.com/Neargye/magic_enum/blob/master/doc/limitations.md
#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 256
#include "magic_enum_all.hpp" // enum reflection https://github.com/Neargye/magic_enum



#include <eventpp/eventdispatcher.h>
#include "eventpp/callbacklist.h"
#include "eventpp/utilities/scopedremover.h"

// directx
#include <directxtk\SimpleMath.h>
using namespace DirectX;
#include "HCM_imconfig.h" 





// boost
#include "boost\stacktrace.hpp"
#include "boost\algorithm\string\predicate.hpp"
#include "boost\bimap.hpp"
#include "boost\assign.hpp"
#include <boost\preprocessor.hpp>

// Custom utilities
#include "CustomExceptions.h"
#include "ControlDefs.h"
#include "WindowsUtilities.h"
#include "ScopedCallback.h"
#include "ScopedAtomicBool.h"

// Some gui constant definitions
constexpr int GUIFrameHeight = 19;
constexpr int GUISpacing = 4;
constexpr int GUIFrameHeightWithSpacing = GUIFrameHeight + GUISpacing;

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
//#define LOG_ONCE_CAPTURE(x, y, z)   once([y, z](){},[y, z](){ x; });
#define ResourceType(x) std::string("WA") + #x
#define resConsume(arg1, arg2) s##arg1##Play##arg2##ou##arg1


#define FAR_CLIP_3D 10000.f

// it's a RECT but single-precision
struct RECTF
{
    float left, top, right, bottom;
};

typedef uint8_t bitOffsetT; // TODO: add constraint to this (0-7)

#endif //PCH_H


