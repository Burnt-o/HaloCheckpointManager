#pragma once
#include "WinHandle.h"
#include <optional>
// returns nullopt on success, error message on failure. imbue = inject. AV heuristics don't like the latter word.
std::optional<std::wstring> imbueInternal(HandlePtr& mccHandle);