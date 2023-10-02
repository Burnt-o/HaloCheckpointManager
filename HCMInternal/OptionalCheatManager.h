#pragma once
#include "IGUIRequiredServices.h"
#include "GetMCCVersion.h"
#include "OptionalCheatInfo.h"
#include "MCCStateHook.h"
#include "RPCClientInternal.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
// constructs and stores optionalcheats, updating OptionalCheatInfoManager as it constructs



class OptionalCheatManager
{
private:
	class OptionalCheatManagerImpl;
	std::shared_ptr<OptionalCheatManagerImpl> pimpl;
public:
	OptionalCheatManager(std::shared_ptr<IGUIRequiredServices>, std::shared_ptr<OptionalCheatInfo>,  
		/* rest is cheat construction stuff that will get stuffed into a DIContainer later*/
		std::shared_ptr<SettingsStateAndEvents>, std::shared_ptr<PointerManager>, std::shared_ptr<IGetMCCVersion>, std::shared_ptr<MCCStateHook>, std::shared_ptr<RPCClientInternal>, std::shared_ptr<MessagesGUI>, std::shared_ptr<RuntimeExceptionHandler>, std::string dirPath);
	~OptionalCheatManager();
};

