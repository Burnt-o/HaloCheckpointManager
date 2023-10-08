#pragma once
#include "IGUIRequiredServices.h"
#include "GetMCCVersion.h"
#include "OptionalCheatInfo.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "ISharedMemory.h"
#include "PointerManager.h"
// constructs and stores optionalcheats, updating OptionalCheatInfoManager as it constructs



class OptionalCheatManager
{
private:
	class OptionalCheatManagerImpl;
	std::shared_ptr<OptionalCheatManagerImpl> pimpl;
public:
	OptionalCheatManager(std::shared_ptr<IGUIRequiredServices>, std::shared_ptr<OptionalCheatInfo>,  
		/* rest is cheat construction stuff that will get stuffed into a DIContainer later*/
		std::shared_ptr<SettingsStateAndEvents>, std::shared_ptr<PointerManager>, std::shared_ptr<IGetMCCVersion>, std::shared_ptr<IMCCStateHook>, std::shared_ptr<ISharedMemory>, std::shared_ptr<IMessagesGUI>, std::shared_ptr<RuntimeExceptionHandler>, std::string dirPath);
	~OptionalCheatManager();
};

