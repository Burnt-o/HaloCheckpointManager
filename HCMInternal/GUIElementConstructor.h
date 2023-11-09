#pragma once
#include "OptionalCheatInfo.h"
#include "IGUIRequiredServices.h"
#include "GUIElementStore.h"
#include "GUIServiceInfo.h"
#include "SettingsStateAndEvents.h"
#include "IGetMCCVersion.h"
class GUIElementConstructor
{
private:
	class GUIElementConstructorImpl;
	std::unique_ptr< GUIElementConstructorImpl> pimpl;

public:
	GUIElementConstructor(std::shared_ptr<IGUIRequiredServices>, std::shared_ptr<OptionalCheatInfo>, std::shared_ptr<GUIElementStore>, std::shared_ptr<GUIServiceInfo>, std::shared_ptr<SettingsStateAndEvents>, MCCProcessType);
	~GUIElementConstructor();
};

