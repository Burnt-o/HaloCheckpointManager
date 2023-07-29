#pragma once
#include "Option.h"



namespace OptionsState
{
	extern eventpp::CallbackList<void()> forceCheckpointEvent;
	extern eventpp::CallbackList<void()> forceRevertEvent;
	extern eventpp::CallbackList<void()> forceDoubleRevertEvent;
	extern eventpp::CallbackList<void()> injectCheckpointEvent;
	extern eventpp::CallbackList<void()> dumpCheckpointEvent;


	extern Option<bool> injectDumpCores;

	extern std::vector<SerialisableOption*> allSerialisableOptions;
};

