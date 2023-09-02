#include "pch.h"
#include "Hotkeys.h"
#include "OptionsState.h"
#include "imgui.h"

typedef std::vector<std::vector<ImGuiKey>> vvk; // doesn't pollute
namespace Hotkeys
{


	std::shared_ptr<Hotkey> forceCheckpoint = std::make_shared<Hotkey>(OptionsState::forceCheckpointEvent, nameof(forceCheckpoint), vvk{{ ImGuiKey_F1 } });
	std::vector<std::shared_ptr<Hotkey>> allHotkeys {forceCheckpoint};
}	 
