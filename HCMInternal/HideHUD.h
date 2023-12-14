#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ScopedServiceRequest.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "IMessagesGUI.h"

class HideHUD : public IOptionalCheat
{
private:
	GameState mGame;
	std::shared_ptr<GenericScopedServiceProvider> pimpl; // must be shared because of shared_from_this

	// event callbacks
	ScopedCallback <ToggleEvent> mHideHUDToggleCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	std::optional<std::unique_ptr<ScopedServiceRequest>> currentServiceRequest = std::nullopt; // starts null. null means hideHUD isn't requesting pimpls services.

	void onHideHUDToggleEvent(bool& newValue)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false)
			{
				currentServiceRequest = std::nullopt;
				return;
			}


			if (newValue)
			{
				currentServiceRequest = pimpl->makeRequest(std::format("{}:{}", nameof(HideHUD), mGame.toString()));
			}
			else
			{
				currentServiceRequest = std::nullopt;
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}

public:
	HideHUD(GameState gameImpl, IDIContainer& dicon);

	~HideHUD();

	std::string_view getName() override { return nameof(AdvanceTicks); }

	std::unique_ptr<ScopedServiceRequest> makeRequest(std::string callerID) { return pimpl->makeRequest(callerID); }

};