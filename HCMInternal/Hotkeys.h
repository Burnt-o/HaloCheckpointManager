#pragma once
#include "Hotkey.h"

namespace Hotkeys
{
	 extern std::shared_ptr<Hotkey> forceCheckpoint;

	//static inline Hotkey forceCheckpointHotkey;

	//static inline std::vector<std::shared_ptr<Hotkey>> allHotkeys {std::make_shared<Hotkey>(forceCheckpointHotkey)};
	extern std::vector<std::shared_ptr<Hotkey>> allHotkeys;
}
