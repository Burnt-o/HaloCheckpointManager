#include "pch.h"
#include "TriggerOverlay.h"
#include "GetTriggerData.h"
#include "Render3DEventProvider.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "RenderTextHelper.h"
#include "UpdateTriggerLastChecked.h"

using namespace SettingsEnums;

template<GameState::Value mGame>
class TriggerOverlayImpl : public TriggerOverlayUntemplated 
{
private:
	// callbacks
	std::unique_ptr<ScopedCallback<Render3DEvent>> mRenderEventCallback;
	std::atomic_bool renderingMutex = false;
	ScopedCallback<ToggleEvent> triggerOverlayToggleEventCallback;

	// injected services
	std::weak_ptr<Render3DEventProvider> render3DEventProviderWeak;
	std::weak_ptr<GetTriggerData> getTriggerDataWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::optional<std::weak_ptr<UpdateTriggerLastChecked>> updateTriggerLastCheckedOptionalWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// data
	bool gameValid = false;

	void onToggleChange(bool& newValue)
	{
		lockOrThrow(mccStateHookWeak, mccStateHook);
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false)
		{
			if (renderingMutex)
			{
				renderingMutex.wait(true);
			}
			mRenderEventCallback.reset();
			return;
		}


		// subscribe to 3d render event if enabled, unsubscribe otherwise
		if (newValue)
		{
			try
			{

				lockOrThrow(render3DEventProviderWeak, render3DEventProvider);
				mRenderEventCallback = std::make_unique<ScopedCallback<Render3DEvent>>(render3DEventProvider->render3DEvent, [this](GameState g, IRenderer3D* n) {onRenderEvent(g, n); });

			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}

		}
		else
		{
			if (renderingMutex)
			{
				renderingMutex.wait(true);
			}
			mRenderEventCallback.reset();
		}
	}


	// new frame, render
	void onRenderEvent(GameState game, IRenderer3D* renderer)
	{
		if (game.operator GameState::Value() != mGame) return;





		try // renderer funcs can throw HCMRuntime exceptions
		{
			ScopedAtomicBool lockRender(renderingMutex);


			lockOrThrow(settingsWeak, settings);

			auto& renderStyle = settings->triggerOverlayRenderStyle->GetValue();
			auto& interiorStyle = settings->triggerOverlayInteriorStyle->GetValue();
			
			auto& labelStyle = settings->triggerOverlayLabelStyle->GetValue();
			auto labelScale = settings->triggerOverlayLabelScale->GetValue() / (15.f);

			auto noAlpha = settings->triggerOverlayNormalColor->GetValue(); // needs to be a copy since mutable
			auto triggerColor = SimpleMath::Vector4(noAlpha.x, noAlpha.y, noAlpha.z, settings->triggerOverlayAlpha->GetValue());
			// TODO: cache colours and set by last checked interval

			lockOrThrow(getTriggerDataWeak, getTriggerData);
			auto filteredTriggerData = getTriggerData->getTriggerData()->filteredTriggers;

			for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
			{
				renderer->renderTriggerModel(triggerData.model, triggerColor, renderStyle, interiorStyle, labelStyle, labelScale);
			}

		}
		catch (HCMRuntimeException ex)
		{
			PLOG_ERROR << "Trigger Overlay Rendering error: " << std::endl << ex.what();
			runtimeExceptions->handleMessage(ex);
			
			try
			{
				lockOrThrow(settingsWeak, settings);
				settings->triggerOverlayToggle->GetValueDisplay() = false;
				settings->triggerOverlayToggle->UpdateValueWithInput();
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
		}
	}

public:
	TriggerOverlayImpl(GameState game, IDIContainer& dicon) :
		render3DEventProviderWeak(resolveDependentCheat(Render3DEventProvider)),
		getTriggerDataWeak(resolveDependentCheat(GetTriggerData)),
		triggerOverlayToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayToggle->valueChangedEvent, [this](bool& n) {onToggleChange(n); }),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>())
	{

		try
		{
			updateTriggerLastCheckedOptionalWeak = resolveDependentCheat(UpdateTriggerLastChecked);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Trigger overlay could not resolve UpdateTriggerLastChecked, continuing anyway";
		}
	}


};



TriggerOverlay::TriggerOverlay(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<TriggerOverlayImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<TriggerOverlayImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<TriggerOverlayImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<TriggerOverlayImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<TriggerOverlayImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<TriggerOverlayImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

TriggerOverlay::~TriggerOverlay() = default;