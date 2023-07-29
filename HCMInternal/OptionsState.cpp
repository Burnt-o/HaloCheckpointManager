#include "pch.h"
#include "OptionsState.h"



namespace OptionsState
{
	eventpp::CallbackList<void()> forceCheckpointEvent;
	eventpp::CallbackList<void()> forceRevertEvent;
	eventpp::CallbackList<void()> forceDoubleRevertEvent;
	eventpp::CallbackList<void()> injectCheckpointEvent;
	eventpp::CallbackList<void()> dumpCheckpointEvent;

	Option<bool> injectDumpCores(
		false,
		[](bool newValue) { return true; },
		nameof(injectDumpCores)

	);


	std::vector<SerialisableOption*> allSerialisableOptions{ &injectDumpCores};
}

