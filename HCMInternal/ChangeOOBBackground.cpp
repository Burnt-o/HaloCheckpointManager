#include "pch.h"
#include "ChangeOOBBackground.h"
#include "SettingsStateAndEvents.h"
#include "ModuleHook.h"
#include "RuntimeExceptionHandler.h"
#include "MultilevelPointer.h"
#include "IMCCStateHook.h"
#include "PointerDataStore.h"
#include "IMessagesGUI.h"

class ChangeOOBBackground::ChangeOOBBackgroundImpl
{
private:
	GameState mGame;

	// callbacks
	ScopedCallback<ToggleEvent> changeOOBBackgroundToggleCallback;
	ScopedCallback<eventpp::CallbackList<void(SimpleMath::Vector4&)>> changeOOBBackgroundColorCallback;

	// injected services
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	std::shared_ptr<ModulePatch> OOBBackgroundEnablePatchHook;
	std::shared_ptr<MultilevelPointer> OOBBackgroundColorPointer;

	void updateColor()
	{
		lockOrThrow(settingsWeak, settings);

		// needs to be converted to uint32_t of format 0xAARRGGBB. Except the alpha value doesn't matter.
		auto newColorVec = settings->changeOOBBackgroundColor->GetValue();
		uint32_t newColor = ((uint8_t)(newColorVec.x * 255) << 16) | ((uint8_t)(newColorVec.y * 255) << 8) | (uint8_t)(newColorVec.z * 255);

		OOBBackgroundColorPointer->writeData(&newColor, true); // protected memory
	}


	void onChangeOOBBackgroundToggleChanged(bool n)
	{
		try
		{
			OOBBackgroundEnablePatchHook->setWantsToBeAttached(n);
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (n && mccStateHook->isGameCurrentlyPlaying(mGame))
				updateColor();
		
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onChangeOOBBackgroundColorChanged(SimpleMath::Vector4& n)
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			if (settings->changeOOBBackgroundToggle->GetValue())
				updateColor();

		}
		catch(HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

public:
	ChangeOOBBackgroundImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		changeOOBBackgroundToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->changeOOBBackgroundToggle->valueChangedEvent, [this](bool& n) {onChangeOOBBackgroundToggleChanged(n); }),
		changeOOBBackgroundColorCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->changeOOBBackgroundColor->valueChangedEvent, [this](SimpleMath::Vector4& n) {onChangeOOBBackgroundColorChanged(n); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		OOBBackgroundColorPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(OOBBackgroundColorPointer), game);

		auto OOBBackgroundEnablePatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(OOBBackgroundEnablePatchFunction), game);
		auto OOBBackgroundEnablePatchCode = ptr->getVectorData<byte>(nameof(OOBBackgroundEnablePatchCode), game);
		OOBBackgroundEnablePatchHook = ModulePatch::make(game.toModuleName(), OOBBackgroundEnablePatchFunction, *OOBBackgroundEnablePatchCode.get());
	}
};


ChangeOOBBackground::ChangeOOBBackground(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< ChangeOOBBackgroundImpl>(game, dicon);
}

ChangeOOBBackground::~ChangeOOBBackground() = default;