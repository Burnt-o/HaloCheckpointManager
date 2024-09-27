#pragma once
#include "pch.h"

std::expected<bool, std::string> MCCInitialisationCheck(HMODULE dllHandle);