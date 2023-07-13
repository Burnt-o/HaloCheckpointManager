#include "pch.h"
#include "OptionsState.h"



namespace OptionsState
{
	eventpp::CallbackList<void()> forceCheckpointEvent;
	eventpp::CallbackList<void()> injectCheckpointEvent;
	eventpp::CallbackList<void()> dumpCheckpointEvent;

	std::vector<SerialisableOption*> allSerialisableOptions{};
}

