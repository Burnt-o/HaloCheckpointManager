// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H



// Windows 
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




#endif //PCH_H
