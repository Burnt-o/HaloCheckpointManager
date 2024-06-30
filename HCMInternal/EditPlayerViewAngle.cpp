#include "pch.h"
#include "EditPlayerViewAngle.h"
#include "RuntimeExceptionHandler.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "IMakeOrGetCheat.h"
#include "GetPlayerViewAngle.h"
#include "IMCCStateHook.h"

class EditPlayerViewAngle::EditPlayerViewAngleImpl
{
private:

	GameState mImplGame;

	//event callbacks
	ScopedCallback<ActionEvent> editPlayerViewAngleSetCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleAdjustHorizontalCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleAdjustVerticalCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleFillCurrentCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleCopyCallback;
	ScopedCallback<ActionEvent> editPlayerViewAnglePasteCallback;


	// injected services
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<GetPlayerViewAngle> getPlayerViewAngleWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	void onEditPlayerViewAngleSet()
	{
		try
		{
			
				lockOrThrow(mccStateHookWeak, mccStateHook);
				if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;
			


			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			lockOrThrow(settingsWeak, settings);
			
			getPlayerViewAngle->setPlayerViewAngle(settings->editPlayerViewAngleVec2->GetValue());

			
				lockOrThrow(messagesGUIWeak, messagesGUI);
				messagesGUI->addMessage(std::format("Setting view angles: {}", vec2ToString(settings->editPlayerViewAngleVec2->GetValue())));
			
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleAdjustHorizontal()
	{
		try
		{
			
				lockOrThrow(mccStateHookWeak, mccStateHook);
				if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;
			

			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			lockOrThrow(settingsWeak, settings);
			auto currentViewAngle = getPlayerViewAngle->getPlayerViewAngle();
			currentViewAngle.x = std::fmodf(currentViewAngle.x + settings->editPlayerViewAngleAdjustFactor->GetValue(), (std::numbers::pi_v<float> * 2.f)); // rolls over at 2pi
			getPlayerViewAngle->setPlayerViewAngle(currentViewAngle);

			{
				lockOrThrow(messagesGUIWeak, messagesGUI);
				messagesGUI->addMessage(std::format("Setting view horizontal: {}", currentViewAngle.x));
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleAdjustVertical()
	{
		try
		{
			
				lockOrThrow(mccStateHookWeak, mccStateHook);
				if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;
			

			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			lockOrThrow(settingsWeak, settings);
			auto currentViewAngle = getPlayerViewAngle->getPlayerViewAngle();
			currentViewAngle.y = std::clamp(currentViewAngle.y + settings->editPlayerViewAngleAdjustFactor->GetValue(), (std::numbers::pi_v<float> / 2.f) * -1.f, (std::numbers::pi_v<float> / 2.f));
			getPlayerViewAngle->setPlayerViewAngle(currentViewAngle);

			
				lockOrThrow(messagesGUIWeak, messagesGUI);
				messagesGUI->addMessage(std::format("Setting view vertical: {}", currentViewAngle.y));
			
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleFillCurrent()
	{
		try
		{
			
				lockOrThrow(mccStateHookWeak, mccStateHook);
				if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;
			

			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			lockOrThrow(settingsWeak, settings);
			auto playerViewAngle = getPlayerViewAngle->getPlayerViewAngle();
			PLOG_DEBUG << "filling viewAngle with " << playerViewAngle;
			settings->editPlayerViewAngleVec2->GetValueDisplay() = playerViewAngle;
			settings->editPlayerViewAngleVec2->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleCopy()
	{
		try
		{
			
				lockOrThrow(mccStateHookWeak, mccStateHook);
				if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;
			

			lockOrThrow(settingsWeak, settings);
			settings->editPlayerViewAngleVec2->serialiseToClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAnglePaste()
	{
		try
		{
			
				lockOrThrow(mccStateHookWeak, mccStateHook);
				if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;
			

			lockOrThrow(settingsWeak, settings);
			settings->editPlayerViewAngleVec2->deserialiseFromClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}


public:
	EditPlayerViewAngleImpl(GameState game, IDIContainer& dicon)
		:mImplGame(game),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		editPlayerViewAngleSetCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleSet, [this]() {onEditPlayerViewAngleSet(); }),
		editPlayerViewAngleAdjustHorizontalCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleAdjustHorizontal, [this]() {onEditPlayerViewAngleAdjustHorizontal(); }),
		editPlayerViewAngleAdjustVerticalCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleAdjustVertical, [this]() {onEditPlayerViewAngleAdjustVertical(); }),
		editPlayerViewAngleFillCurrentCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleFillCurrent, [this]() {onEditPlayerViewAngleFillCurrent(); }),
		editPlayerViewAngleCopyCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleCopy, [this]() {onEditPlayerViewAngleCopy(); }),
		editPlayerViewAnglePasteCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAnglePaste, [this]() {onEditPlayerViewAnglePaste(); }),
		getPlayerViewAngleWeak(resolveDependentCheat(GetPlayerViewAngle))
	{

	}
};




EditPlayerViewAngle::EditPlayerViewAngle(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< EditPlayerViewAngle::EditPlayerViewAngleImpl>(game, dicon);
}

EditPlayerViewAngle::~EditPlayerViewAngle()
{
	PLOG_DEBUG << "~" << getName();
}