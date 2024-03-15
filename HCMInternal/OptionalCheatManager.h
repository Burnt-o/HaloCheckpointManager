#pragma once
#include "IGUIRequiredServices.h"
#include "GetMCCVersion.h"
#include "OptionalCheatInfo.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "ISharedMemory.h"
#include "PointerDataStore.h"
#include "ModalDialogRenderer.h"
#include "ControlServiceContainer.h"
#include "IMakeOrGetCheat.h"
#include "HotkeyDefinitions.h"
#include "DirectXRenderEvent.h"
// constructs and stores optionalcheats, updating OptionalCheatInfoManager as it constructs

class OptionalCheatConstructor;

class OptionalCheatManager
{
public:
	class OptionalCheatStore;
private:

	std::shared_ptr<OptionalCheatStore> storePimpl;

	std::shared_ptr<OptionalCheatConstructor> constructorPimpl;
public:
	OptionalCheatManager(std::shared_ptr<IGUIRequiredServices>, std::shared_ptr<OptionalCheatInfo>,  
		/* rest is cheat construction stuff that will get stuffed into a DIContainer later*/
		std::shared_ptr<SettingsStateAndEvents>, 
		std::shared_ptr<PointerDataStore>, 
		std::shared_ptr<IGetMCCVersion>, 
		std::shared_ptr<IMCCStateHook>, 
		std::shared_ptr<ISharedMemory>, 
		std::shared_ptr<IMessagesGUI>, 
		std::shared_ptr<RuntimeExceptionHandler>, 
		std::string dirPath, 
		std::shared_ptr<ModalDialogRenderer> modal,
		std::shared_ptr<ControlServiceContainer> control,
		std::shared_ptr<RenderEvent> overlayRenderEvent,
		std::shared_ptr<DirectXRenderEvent> foregroundDirectXRenderEvent,
		std::shared_ptr<HotkeyDefinitions> hotkeyDefinitions);
	;
	~OptionalCheatManager();

};

