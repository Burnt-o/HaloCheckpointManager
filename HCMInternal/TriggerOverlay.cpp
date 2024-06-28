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
#include "GameTickEventHook.h"
#include "IMessagesGUI.h"

using namespace SettingsEnums;

template<GameState::Value mGame>
class TriggerOverlayImpl : public TriggerOverlayUntemplated 
{
private:
	// callbacks
	std::unique_ptr<ScopedCallback<Render3DEvent>> mRenderEventCallback;
	ScopedCallback<ToggleEvent> triggerOverlayToggleEventCallback;

	// injected services
	std::weak_ptr<Render3DEventProvider> render3DEventProviderWeak;
	std::weak_ptr<GetTriggerData> getTriggerDataWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr< GameTickEventHook> gameTickEventHookWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::optional<std::weak_ptr<UpdateTriggerLastChecked>> updateTriggerLastCheckedOptionalWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// data
	std::atomic_bool renderingMutex = false;

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
				PLOG_DEBUG << "subscribing to render event!";
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

			const auto& renderStyle = settings->triggerOverlayRenderStyle->GetValue();
			const auto& interiorStyle = settings->triggerOverlayInteriorStyle->GetValue();
			
			const auto& labelStyle = settings->triggerOverlayLabelStyle->GetValue();
			const auto labelScale = settings->triggerOverlayLabelScale->GetValue() / (15.f);

			const bool triggerOverlayMessageOnCheckHit = settings->triggerOverlayMessageOnCheckHit->GetValue();
			const bool triggerOverlayMessageOnCheckMiss = settings->triggerOverlayMessageOnCheckMiss->GetValue();

			// calculate colours

			const bool shouldFlashOnCheckHit = settings->triggerOverlayCheckHitToggle->GetValue();
			const bool shouldFlashOnCheckMiss = settings->triggerOverlayCheckMissToggle->GetValue();

			const auto& triggerOverlayNormalColor = settings->triggerOverlayNormalColor->GetValue(); 

			const auto& triggerOverlayBSPColor = settings->triggerOverlayBSPColor->GetValue();

			const auto& triggerOverlayCheckMissColor = settings->triggerOverlayCheckMissColor->GetValue();

			const auto& triggerOverlayCheckHitColor = settings->triggerOverlayCheckHitColor->GetValue();

			const float& triggerOverlayAlpha = settings->triggerOverlayAlpha->GetValue();
			const float& triggerOverlayWireframeAlpha = settings->triggerOverlayWireframeAlpha->GetValue();

			const auto now = std::chrono::steady_clock::now();

			const uint32_t triggerOverlayCheckHitFalloff = settings->triggerOverlayCheckHitFalloff->GetValue();
			const uint32_t triggerOverlayCheckMissFalloff = settings->triggerOverlayCheckMissFalloff->GetValue();

			lockOrThrow(getTriggerDataWeak, getTriggerData);
			auto filteredTriggerData = getTriggerData->getFilteredTriggers();

			lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
			 uint32_t currentTick = gameTickEventHook->getCurrentGameTick();

			if (mGame == GameState::Value::Halo2) 
				currentTick--;

#ifndef HCM_DEBUG
			static_assert(false && "don't release the currentTick-- fix. change the placement of the gametick hook in impl so it's getting the correct value").
#endif


			// calculate colors for each trigger on this tick
			for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
			{
				std::optional<long> ticksSinceLastCheck = std::nullopt;
				if (triggerData.tickLastChecked.has_value())
					ticksSinceLastCheck = (long)currentTick - (long)(triggerData.tickLastChecked.value());
				

				bool shouldFlash = 
					((shouldFlashOnCheckHit && triggerData.lastCheckSuccessful == true) || (shouldFlashOnCheckMiss && triggerData.lastCheckSuccessful == false)) //lastCheckSuccessful matches setting
					&& ticksSinceLastCheck.has_value() // null opt if that trigger has never been checked at all
					&& ticksSinceLastCheck >= 0 // can be negative when player reverts
					&&
					(
						(triggerData.lastCheckSuccessful == true && ticksSinceLastCheck.value() < triggerOverlayCheckHitFalloff) // didn't happen too long ago
						|| (triggerData.lastCheckSuccessful == false && ticksSinceLastCheck.value() < triggerOverlayCheckMissFalloff)
					);


				if (shouldFlash)
				{
					const auto& checkColor = triggerData.lastCheckSuccessful ? triggerOverlayCheckHitColor : triggerOverlayCheckMissColor;
					const auto& durationToFlashFor = triggerData.lastCheckSuccessful ? triggerOverlayCheckHitFalloff : triggerOverlayCheckMissFalloff;

					// need to transition from checkColour to normal colour based on how long its been as fraction of durationToFlashFor
					float normalRatio = ((float)ticksSinceLastCheck.value() / (float)durationToFlashFor);
					float checkRatio = 1.f - normalRatio;



					auto triggerColor = (triggerOverlayNormalColor * normalRatio) + (checkColor * checkRatio);
					//triggerColor.w = checkColor.w;



					// boost alpha on first tick of check
					if (ticksSinceLastCheck.value() == 0)
					{
						
						// print message on check if appropiate setting
						if (triggerData.printedMessageForLastCheck == false)
						{
							triggerData.printedMessageForLastCheck = true; // prevents us from printing multiple times on the tick of the check

							if (triggerData.lastCheckSuccessful && triggerOverlayMessageOnCheckHit)
							{
								lockOrThrow(messagesGUIWeak, messagesGUI);
								messagesGUI->addMessage(std::format("Trigger hit on tick: {}, {}", currentTick, triggerData.name));
							}
							else if (triggerData.lastCheckSuccessful == false && triggerOverlayMessageOnCheckMiss)
							{
								lockOrThrow(messagesGUIWeak, messagesGUI);
								messagesGUI->addMessage(std::format("Trigger miss on tick: {}, {}", currentTick, triggerData.name));
							}
				
						}
					

						triggerColor.w += (1.f - triggerColor.w) / 4.f;
						
					}
					triggerColor.w *= triggerOverlayAlpha;
					
					triggerData.setColor(triggerColor, triggerOverlayWireframeAlpha);
				}
				else
				{
					auto triggerColor = triggerData.isBSPTrigger ? triggerOverlayBSPColor :
						triggerOverlayNormalColor;
					triggerColor.w *= triggerOverlayAlpha;

					triggerData.setColor(triggerColor, triggerOverlayWireframeAlpha);

				}

			}

			// render wireframes
			// idk why but renderering solid volumes first messes up the wireframes positions
			if (renderStyle == TriggerRenderStyle::Wireframe || renderStyle == TriggerRenderStyle::SolidAndWireframe)
			{
				auto primitiveDrawer = renderer->getPrimitiveDrawer();

				for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
				{
					// unfortunately setPrimitiveColor can only apply to one begin call
					primitiveDrawer->Begin();
					renderer->setPrimitiveColor(triggerData.currentColorWireframe);
					for (auto& edge : triggerData.model.edges)
					{
						primitiveDrawer->DrawLine(edge.first, edge.second);
					}
					primitiveDrawer->End();
				}


			}

			// render solid volumes
			if (renderStyle == TriggerRenderStyle::Solid || renderStyle == TriggerRenderStyle::SolidAndWireframe)
			{
				for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
				{
					renderer->renderTriggerModelSolid(triggerData.model, triggerData.currentColor, interiorStyle);
				}
			}

		

			// render labels
			if (labelStyle != TriggerLabelStyle::None)
			{
				bool isCentered = labelStyle == TriggerLabelStyle::Center;
				for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
				{
	
					auto pointOnScreen = isCentered
						? !renderer->pointBehindCamera(triggerData.model.box.Center)
						: !renderer->pointBehindCamera(triggerData.model.box.Center - ((SimpleMath::Vector3)triggerData.model.box.Extents / 2)); // corner (TODO: store corner in triggermodel)

					if (pointOnScreen)
					{
						auto screenPosition = labelStyle == TriggerLabelStyle::Center
							? renderer->worldPointToScreenPosition(triggerData.model.box.Center, false)
							: renderer->worldPointToScreenPosition(triggerData.model.box.Center - ((SimpleMath::Vector3)triggerData.model.box.Extents / 2), false); // corner (TODO: store corner in triggermodel)

						auto dynLabelScale = labelScale * RenderTextHelper::scaleTextDistance(renderer->cameraDistanceToWorldPoint(triggerData.model.box.Center));
						// draw main label text

						if (isCentered)
						{
							RenderTextHelper::drawCenteredOutlinedText(
								triggerData.model.label,
								{ screenPosition.x, screenPosition.y },
								triggerData.currentColorU32 | 0xFFC0C0C0, // remove alpha, brigten a lil bit, 
								dynLabelScale
							);
						}
						else
						{
							RenderTextHelper::drawOutlinedText(
								triggerData.model.label,
								{ screenPosition.x, screenPosition.y },
								triggerData.currentColorU32 | 0xFFC0C0C0, // remove alpha, brigten a lil bit, 
								dynLabelScale
							);
						}

					}
				}
				
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
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook))
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

	~TriggerOverlayImpl()
	{
		if (renderingMutex)
		{
			renderingMutex.wait(true);
		}
		mRenderEventCallback.reset();
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