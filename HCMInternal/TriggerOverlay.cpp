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
#include "GetPlayerTriggerPosition.h"
#include "ScopedServiceRequest.h"

using namespace SettingsEnums;

template<GameState::Value mGame>
class TriggerOverlayImpl : public TriggerOverlayUntemplated 
{
private:
	// callbacks
	std::unique_ptr<ScopedCallback<Render3DEvent>> mRenderEventCallback;
	ScopedCallback<ToggleEvent> triggerOverlayToggleEventCallback;
	ScopedCallback<ToggleEvent> triggerOverlayCheckFlashHitCallback;
	ScopedCallback<ToggleEvent> triggerOverlayCheckFlashMissCallback;
	ScopedCallback<ToggleEvent> triggerOverlayMessageOnCheckHitCallback;
	ScopedCallback<ToggleEvent> triggerOverlayMessageOnCheckMissCallback;

	// injected services
	std::weak_ptr<Render3DEventProvider> render3DEventProviderWeak;
	std::weak_ptr<GetTriggerData> getTriggerDataWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr< GameTickEventHook> gameTickEventHookWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::optional<std::weak_ptr< GetPlayerTriggerPosition>> getPlayerTriggerPositionOptionalWeak;
	std::optional<std::weak_ptr<UpdateTriggerLastChecked>> updateTriggerLastCheckedOptionalWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	std::unique_ptr<ScopedServiceRequest> updateTriggerLastCheckedRequest;

	void onUpdateTriggerLastCheckedSettingChanged()
	{
		if (updateTriggerLastCheckedOptionalWeak)
		{
			try
			{
				lockOrThrow(settingsWeak, settings);

				bool shouldUpdateTriggers = settings->triggerOverlayToggle->GetValue()
					&& (
						settings->triggerOverlayCheckHitToggle->GetValue()
						|| settings->triggerOverlayCheckMissToggle->GetValue()
						|| settings->triggerOverlayMessageOnCheckHit->GetValue()
						|| settings->triggerOverlayMessageOnCheckMiss->GetValue()
						);

				if (shouldUpdateTriggers)
				{
					lockOrThrow(updateTriggerLastCheckedOptionalWeak.value(), updateTriggerLastChecked);
					updateTriggerLastCheckedRequest = updateTriggerLastChecked->makeRequest(nameof(TriggerOverlay));
				}
				else
					updateTriggerLastCheckedRequest.reset();

			}
			catch (HCMRuntimeException ex)
			{
				updateTriggerLastCheckedRequest.reset();
				runtimeExceptions->handleMessage(ex);
			}
		}
		
	}


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
				mRenderEventCallback = render3DEventProvider->getRender3DEvent()->subscribe([this](GameState g, IRenderer3D* n) {onRenderEvent(g, n); });

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


		try
		{
			lockOrThrow(messagesGUIWeak, messages);
			messages->addMessage(newValue ? "Trigger Overlay enabled." : "Trigger Overlay disabled.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
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

			// draw player trigger vertex
			if (getPlayerTriggerPositionOptionalWeak.has_value() && settings->triggerOverlayPositionToggle->GetValue())
			{
				lockOrThrow(getPlayerTriggerPositionOptionalWeak.value(), getPlayerTriggerPosition);
				auto playerTriggerPosition = getPlayerTriggerPosition->getPlayerTriggerPosition();

				if (playerTriggerPosition)
				{
					const bool& isWireframe = settings->triggerOverlayPositionWireframe->GetValue();

					auto vertexColor = settings->triggerOverlayPositionColor->GetValue(); // make a copy since we will mutate
					auto scale = settings->triggerOverlayPositionScale->GetValue();
					renderer->renderSphere(playerTriggerPosition.value(), vertexColor, scale, isWireframe);

					// draw a smaller sphere at 1/10th size and extra opacity
					vertexColor.w += (1.f - vertexColor.w) / 2.f;
					renderer->renderSphere(playerTriggerPosition.value(), vertexColor, scale / 10.f, isWireframe);
				}
					
			}




			const auto& renderStyle = settings->triggerOverlayRenderStyle->GetValue();
			const auto& interiorStyle = settings->triggerOverlayInteriorStyle->GetValue();
			std::optional<TextureEnum> interiorTexture = std::nullopt;
			if (interiorStyle == TriggerInteriorStyle::Patterned)
				interiorTexture = TextureEnum::SplotchyPattern;
			
			const auto& labelStyle = settings->triggerOverlayLabelStyle->GetValue();
			const auto labelScale = settings->triggerOverlayLabelScale->GetValue();

			const bool triggerOverlayMessageOnCheckHit = settings->triggerOverlayMessageOnCheckHit->GetValue();
			const bool triggerOverlayMessageOnCheckMiss = settings->triggerOverlayMessageOnCheckMiss->GetValue();

			// calculate colours

			const bool shouldFlashOnCheckHit = settings->triggerOverlayCheckHitToggle->GetValue();
			const bool shouldFlashOnCheckMiss = settings->triggerOverlayCheckMissToggle->GetValue();

			const auto& triggerOverlayNormalColor = settings->triggerOverlayNormalColor->GetValue(); 

			const auto& triggerOverlayBSPColor = settings->triggerOverlayBSPColor->GetValue();
			const auto& triggerOverlaySectorColor = settings->triggerOverlaySectorColor->GetValue();

			const auto& triggerOverlayCheckMissColor = settings->triggerOverlayCheckMissColor->GetValue();

			const auto& triggerOverlayCheckHitColor = settings->triggerOverlayCheckHitColor->GetValue();

			const float& triggerOverlayAlpha = settings->triggerOverlayAlpha->GetValue();
			const float& triggerOverlayWireframeAlpha = settings->triggerOverlayWireframeAlpha->GetValue();


			const uint32_t triggerOverlayCheckHitFalloff = settings->triggerOverlayCheckHitFalloff->GetValue();
			const uint32_t triggerOverlayCheckMissFalloff = settings->triggerOverlayCheckMissFalloff->GetValue();

			lockOrThrow(getTriggerDataWeak, getTriggerData);
			auto filteredTriggerData = getTriggerData->getFilteredTriggers();

			lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
			 uint32_t currentTick = gameTickEventHook->getCurrentGameTick();

			 if (mGame == GameState::Value::Halo2)
				 currentTick--;


			auto& cameraFrustum = renderer->getCameraFrustum();
			auto& cameraPosition = renderer->getCameraPosition();

			// calculate colors for each trigger on this tick
			for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
			{

				// check if trigger is even on screen
				if (cameraFrustum.Intersects(triggerData.model.getBoundingBox()) == false)
					continue;



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
						(triggerData.isSectorType ? triggerOverlaySectorColor : triggerOverlayNormalColor);


					triggerColor.w *= triggerOverlayAlpha;

					triggerData.setColor(triggerColor, triggerOverlayWireframeAlpha);

				}


				// render solid volumes
				if (renderStyle == TriggerRenderStyle::Solid || renderStyle == TriggerRenderStyle::SolidAndWireframe)
				{
					// sector types don't have implemented containment test so we'll just always assume the camera is outside for those.
					bool cameraIsInsideTrigger = triggerData.isSectorType == false && triggerData.model.getBoundingBox().Contains(cameraPosition) == DirectX::ContainmentType::CONTAINS;

					if (cameraIsInsideTrigger && interiorStyle == TriggerInteriorStyle::DontRender)
					{
						// do nothing
					}
					else
					{
						if (cameraIsInsideTrigger)
							renderer->drawTriangleCollection(&triggerData.model, triggerData.currentColor, CullingOption::CullNone, interiorTexture);
						else
							renderer->drawTriangleCollection(&triggerData.model, triggerData.currentColor, CullingOption::CullNone, std::nullopt);
					}

				}

				// render wireframes
				if (renderStyle == TriggerRenderStyle::Wireframe || renderStyle == TriggerRenderStyle::SolidAndWireframe)
				{
					renderer->drawEdgeCollection(&triggerData.model, triggerData.currentColorWireframe);
				}

				// render labels
				if (labelStyle != TriggerLabelStyle::None)
				{
					bool isCentered = labelStyle == TriggerLabelStyle::Center;

					auto& labelPosition = isCentered ? triggerData.model.getCenter() : triggerData.model.getCorner();

					auto pointOnScreen = !renderer->pointBehindCamera(labelPosition);


					if (pointOnScreen)
					{
						auto screenPosition = renderer->worldPointToScreenPosition(labelPosition, false);

						auto dynLabelScale = labelScale * RenderTextHelper::scaleTextDistance(renderer->cameraDistanceToWorldPoint(labelPosition));

						// draw main label text
						if (isCentered)
						{
							RenderTextHelper::drawCenteredOutlinedText(
								triggerData.model.getLabel(),
								{ screenPosition.x, screenPosition.y },
								triggerData.currentColorU32 | 0xFFC0C0C0, // remove alpha, brigten a lil bit
								dynLabelScale
							);
						}
						else
						{
							RenderTextHelper::drawOutlinedText(
								triggerData.model.getLabel(),
								{ screenPosition.x, screenPosition.y },
								triggerData.currentColorU32 | 0xFFC0C0C0, // remove alpha, brigten a lil bit
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
		triggerOverlayToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayToggle->valueChangedEvent, [this](bool& n) {onToggleChange(n); onUpdateTriggerLastCheckedSettingChanged(); }),
		triggerOverlayCheckFlashHitCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayCheckHitToggle->valueChangedEvent, [this](bool& n) {onUpdateTriggerLastCheckedSettingChanged(); }),
		triggerOverlayCheckFlashMissCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayCheckMissToggle->valueChangedEvent, [this](bool& n) {onUpdateTriggerLastCheckedSettingChanged(); }),
		triggerOverlayMessageOnCheckHitCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayMessageOnCheckHit->valueChangedEvent, [this](bool& n) {onUpdateTriggerLastCheckedSettingChanged(); }),
		triggerOverlayMessageOnCheckMissCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayMessageOnCheckMiss->valueChangedEvent, [this](bool& n) {onUpdateTriggerLastCheckedSettingChanged(); }),
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

		try
		{
			getPlayerTriggerPositionOptionalWeak = resolveDependentCheat(GetPlayerTriggerPosition);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Trigger overlay could not resolve getPlayerTriggerPositionWeak, continuing anyway";
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