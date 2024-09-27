#pragma once
#include <optional>
#include "WinHandle.h"



// Returns a HandlePtr (with QueryLimitedInformation perms) to an active MCC process,
// or nothing if there are no MCC processes or they are all invalid (terminated/terminating)
std::optional<HandlePtr> findValidMCCProcess();


// returns true if EasyAnticheat process is detected
bool anticheatIsEnabled();



bool hasMCCTerminated(HandlePtr& mccHandle);


