#pragma once
#include "Option.h"



namespace OptionsState
{
	extern eventpp::CallbackList<void()> forceCheckpointEvent;
	extern eventpp::CallbackList<void()> injectCheckpointEvent;
	extern eventpp::CallbackList<void()> dumpCheckpointEvent;

	extern std::vector<SerialisableOption*> allSerialisableOptions;
};

