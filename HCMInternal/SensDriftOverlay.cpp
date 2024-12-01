#include "pch.h"
#include "SensDriftOverlay.h"
#include "GameTickEventHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "IMCCStateHook.h"
#include "GlobalKill.h"
#include "RenderTextHelper.h"
#include "IMakeOrGetCheat.h"
#include "imgui.h"
#include "RevertEventHook.h"
#include "ModuleHook.h"
#include "MidhookContextInterpreter.h"
#include "MultilevelPointer.h"
#include "IMessagesGUI.h"
#include "GetPlayerViewAngle.h"
#include "PointerDataStore.h"
#include "ControlServiceContainer.h"
#include "ManagedResource.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


// helper funcs

int move_towards(int value, int target, int step = 1)
{
	return std::max(std::min(value + step, target), value - step);
}

// how many "subpixels" (smallest possible representation) between two floating point values. 
// adapted from https://stackoverflow.com/a/56213511
long cardinal_float_distance(float a, float b)
{
	/*  Define a value with only the high bit of a uint32_t set.  This is also the
	encoding of floating-point -0.
	*/
	constexpr uint32_t HighBit = UINT32_MAX ^ UINT32_MAX >> 1;

	uint32_t ae = *reinterpret_cast<uint32_t*>(&a);
	uint32_t be = *reinterpret_cast<uint32_t*>(&b);

	/*  For represented values from +0 to infinity, the IEEE 754 binary
		floating-points are in ascending order and are consecutive.  So we can
		simply subtract two encodings to get the number of representable values
		between them (including one endpoint but not the other).

		Unfortunately, the negative numbers are not adjacent and run the other
		direction.  To deal with this, if the number is negative, we transform
		its encoding by subtracting from the encoding of -0.  This gives us a
		consecutive sequence of encodings from the greatest magnitude finite
		negative number to the greatest finite number, in ascending order
		except for wrapping at the maximum uint32_t value.

		Note that this also maps the encoding of -0 to 0 (the encoding of +0),
		so the two zeroes become one point, so they are counted only once.
	*/

	if (HighBit & ae) ae = HighBit - ae;
	if (HighBit & be) be = HighBit - be;

	//  Return the distance between the two transformed encodings.
	return be - ae;
}

// if no subpixel drift were to occur (one dot per frame)
float calc_perfect_viewangle_delta(float currentSensitivity, int dotCount)
{
	// order of operations matters here
	float increment = currentSensitivity;
	increment *= 0.02222222276;
	increment *= 3.141592741f;
	increment /= 180.f;

	float va_delta = 0;

	if (dotCount > 0)
	{
		for (int i = 0; i < abs(dotCount); i++)
		{
			va_delta += increment;
		}
	}
	else
	{
		for (int i = 0; i < abs(dotCount); i++)
		{
			va_delta -= increment;
		}
	}


	return va_delta;


}

int floatToInt(float in)
{
	return *reinterpret_cast<int*>(&in);
}


// modulo a number with 2pi (6.28). In the way MCC does it so the math matches
float MCCModTwoPi(float in)
{
	constexpr float twoPiRadians = 6.283185482f;
	constexpr float negtwoPiRadians = -6.283185482f;

	while (in < 0.f)
	{
		in += twoPiRadians;
	}

	while (in > twoPiRadians)
	{
		in += negtwoPiRadians;
	}

	return in;
}

template<GameState::Value gameT>
class SensDriftOverlayImpl : public SensDriftOverlayUntemplated
{
private:
	// singleton for hook purposes
	static inline SensDriftOverlayImpl<gameT>* instance = nullptr;
	static inline std::atomic_bool mouseMovementHookRunningMutex = false;

	// callbacks
	std::unique_ptr<ScopedCallback<RenderEvent>> mRenderEventCallback; // only not null when running
	std::shared_ptr<RenderEvent> mRenderEvent;

	std::unique_ptr<ScopedCallback<ActionEvent>> mRevertEventCallback; // only not null when running

	ScopedCallback<ActionEvent> mResetCountEventCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleSetCallback;

	ScopedCallback<ToggleEvent> sensDriftOverlayToggleEventCallback;
	ScopedCallback<eventpp::CallbackList<void(const MCCState&)>> mGameStateChangedCallback;



	std::shared_ptr<ModuleMidHook> mouseMovementHook;
	std::shared_ptr<MidhookContextInterpreter> mouseMovementContextInterpreter;
	std::shared_ptr<MultilevelPointer> gameInputIsActive;

	struct LeftRightCounter
	{
		int leftCount = 0;
		int rightCount = 0;
		int totalLeftCount = 0; // Left minus Right
	};

	// Could make atomic but simple POD types so should be fine. We don't want to negatively affect fps since fps affects overdots/drift 
	LeftRightCounter overDotCount = LeftRightCounter();
	LeftRightCounter subpixelDriftCount = LeftRightCounter();
	double subpixelDriftAngle = 0;


	// injected services
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<IMessagesGUI> messagesWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::optional<std::weak_ptr<RevertEventHook>> revertEventHookOptionalWeak;
	std::weak_ptr<GetPlayerViewAngle> getPlayerViewAngleWeak;


	std::optional<std::weak_ptr<BlockGameInput>> blockGameInputOptionalWeak;

	int lastOverDot = 0;
	int lastSubpixelDrift = 0;

	ManagedResource geigSoundA = ManagedResource(114, "WAVE");
	ManagedResource geigSoundB = ManagedResource(115, "WAVE");

	/* isDrift as opposoed to overdot*/
	void playGeigerCounterSound(bool isDrift)
	{
		if (isDrift)
			PlaySoundA((LPCSTR)geigSoundB.hData, NULL, SND_ASYNC | SND_MEMORY);
		else
			PlaySoundA((LPCSTR)geigSoundA.hData, NULL, SND_ASYNC | SND_MEMORY);
	}

	static void mouseMovementHookFunction(SafetyHookContext& ctx)
	{

		if (!instance) PLOG_FATAL << "null instance!";

		ScopedAtomicBool lock(mouseMovementHookRunningMutex);

		try
		{
			// bail early if the game isn't correctly accepting input (menu pause)
			byte gameInputIsActiveFlag;
			if (!instance->gameInputIsActive->readData(&gameInputIsActiveFlag) || gameInputIsActiveFlag != 1 )
				return;

			// bail early if the game isn't correctly accepting input (hcm blocking game input)
			if (instance->blockGameInputOptionalWeak.has_value())
			{
				lockOrThrow(instance->blockGameInputOptionalWeak.value(), blockGameInput)

					if (blockGameInput->serviceIsRequested())
						return;
			}

			enum class param
			{
				pObservedAngleDelta,
				pQueuedDots,
				pSensitivitySetting
			};
			auto* ctxInterpreter = instance->mouseMovementContextInterpreter.get();

			


				LOG_ONCE_CAPTURE(PLOG_DEBUG << "pQueuedDots: " << p, p = ctxInterpreter->getParameterRef(ctx, (int)param::pQueuedDots))

			int queuedDots = *(int*)ctxInterpreter->getParameterRef(ctx, (int)param::pQueuedDots);


			// drift only possible if more than one dot (in either direction) in a frame.

				int overDotThreshold = 1;
#ifdef HCM_DEBUG
				overDotThreshold = 0;
#endif

			if (abs(queuedDots) > overDotThreshold)
			{
				LOG_ONCE(PLOG_DEBUG << "Overdot event!");
				// overdot calculations
				{


					auto overDots = move_towards(queuedDots, 0);

					instance->lastOverDot = -overDots;
					instance->overDotCount.totalLeftCount += overDots;
					if (queuedDots > 0)
						instance->overDotCount.rightCount += abs(overDots);
					else
						instance->overDotCount.leftCount += abs(overDots);
				}

				// subpixel drift calculations
				{

					float observedAngleDelta = *(float*)ctxInterpreter->getParameterRef(ctx, (int)param::pObservedAngleDelta);
					float sensitivitySetting = *(float*)ctxInterpreter->getParameterRef(ctx, (int)param::pSensitivitySetting);
					lockOrThrow(instance->getPlayerViewAngleWeak, getPlayerViewAngle);

					auto previousViewAngle = getPlayerViewAngle->getPlayerViewAngle().x;


					float perfect_viewangle_delta = calc_perfect_viewangle_delta(sensitivitySetting, queuedDots);

					auto perfectNewAngle = MCCModTwoPi(previousViewAngle - perfect_viewangle_delta);
					float actualNewAngle = MCCModTwoPi(previousViewAngle - observedAngleDelta); // yeah it's actually a subtraction - right dots are positive. left angle increase is positive.



					if (perfectNewAngle != actualNewAngle)
					{
						
						auto subpixelDistance = cardinal_float_distance(perfectNewAngle, actualNewAngle);
						instance->lastSubpixelDrift = subpixelDistance;

#ifdef HCM_DEBUG
						PLOG_DEBUG << "queuedDots: " << queuedDots;
						PLOG_DEBUG << "observedAngleDelta " <<  shortestStringRepresentation(observedAngleDelta);
						PLOG_DEBUG << "perfect_viewangle_delta " << shortestStringRepresentation(perfect_viewangle_delta);
						PLOG_DEBUG << "perfectNewAngle " << shortestStringRepresentation(perfectNewAngle) << "(" << floatToInt(perfectNewAngle) << ")";
						PLOG_DEBUG << "actualNewAngle " << shortestStringRepresentation(actualNewAngle) << "(" << floatToInt(actualNewAngle) << ")";
						PLOG_DEBUG << "subpixelDistance " << subpixelDistance;
#endif

						instance->subpixelDriftCount.totalLeftCount += subpixelDistance;
						// directions here are flipped from above; left overdots give right subpixel drift
						if (subpixelDistance > 0)
							instance->subpixelDriftCount.leftCount += abs(subpixelDistance);
						else
							instance->subpixelDriftCount.rightCount += abs(subpixelDistance);



						instance->subpixelDriftAngle += (double)actualNewAngle - (double)perfectNewAngle;
					}
				}
			}


				


		}
		catch (HCMRuntimeException ex)
		{
			lockOrThrow(instance->settingsWeak, settings);
			settings->sensDriftOverlayToggle->GetValueDisplay() = false;
			settings->sensDriftOverlayToggle->UpdateValueWithInput();
			instance->mouseMovementHook->setWantsToBeAttached(false);
			instance->runtimeExceptions->handleMessage(ex);
		}


	}


	// fires every frame, render abilityData depending on options
	void onRenderEvent(SimpleMath::Vector2 screenSize)
	{

		try
		{
			lockOrThrow(settingsWeak, settings);
			lockOrThrow(messagesWeak, messages);
			// calculate screen positions

			const SimpleMath::Vector2& sensScreenOffset = settings->sensScreenOffset->GetValue();
			SimpleMath::Vector2 sensOverlayPosition;
			switch (settings->sensAnchorCorner->GetValue())
			{
			case SettingsEnums::ScreenAnchorEnum::TopLeft:
				sensOverlayPosition = sensScreenOffset;
				break;

			case SettingsEnums::ScreenAnchorEnum::TopRight:
				sensOverlayPosition = { screenSize.x - sensScreenOffset.x, sensScreenOffset.y };
				break;

			case SettingsEnums::ScreenAnchorEnum::BottomRight:
				sensOverlayPosition = { screenSize.x - sensScreenOffset.x, screenSize.y - sensScreenOffset.y };
				break;

			case SettingsEnums::ScreenAnchorEnum::BottomLeft:
				sensOverlayPosition = { sensScreenOffset.x, screenSize.y - sensScreenOffset.y };
				break;
			}

			
			auto sensFontSize = settings->sensFontSize->GetValue();
			auto sensFontColour = ImGui::ColorConvertFloat4ToU32(settings->sensFontColour->GetValue());


			// calculate info string
			std::stringstream ss;

			if (settings->sensOverDotFrameToggle->GetValue())
			{
				ss << "OverDots:" << std::endl;
				ss << "  Left: " << overDotCount.leftCount << std::endl;
				ss << "  Right: " << overDotCount.rightCount << std::endl;
				ss << "  LeftTotal: " << overDotCount.totalLeftCount << std::endl;
			}

			if (settings->sensSubpixelDriftToggle->GetValue())
			{
				ss << "Subpixel Drift:" << std::endl;
				ss << "  Left: " << subpixelDriftCount.leftCount << std::endl;
				ss << "  Right: " << subpixelDriftCount.rightCount << std::endl;
				ss << "  LeftTotal: " << subpixelDriftCount.totalLeftCount << std::endl;
				ss << "  Angle: " << subpixelDriftAngle << std::endl;
			}

			// render it
			if (ss.rdbuf()->in_avail() != 0) // check if ss empty or not first
			{
					RenderTextHelper::drawOutlinedText(ss.str(), sensOverlayPosition, sensFontColour, sensFontSize);
			}

			// do messages or sounds if necessary
			if (lastOverDot != 0 && settings->sensMessageOnOverDotToggle->GetValue())
			{
				const char* leftRightMsg = lastOverDot > 0 ? "Left" : "Right";
				messages->addMessage(std::format("OverDot: {} ({})", lastOverDot, leftRightMsg));
			}
			if (lastSubpixelDrift != 0 && settings->sensMessageOnSubpixelDriftToggle->GetValue())
			{
				const char* leftRightMsg = lastSubpixelDrift > 0 ? "Left" : "Right";
				messages->addMessage(std::format("Subpixel Drift: {} ({})", lastSubpixelDrift, leftRightMsg));
			}

			if (lastOverDot != 0 && settings->sensSoundOnOverDotToggle->GetValue())
			{
				playGeigerCounterSound(false);
				//playGeigerCounterSound(false, abs(lastOverDot), lastOverDot > 0);
			}
			if (lastSubpixelDrift != 0 && settings->sensSoundOnSubpixelDriftToggle->GetValue())
			{
				playGeigerCounterSound(true);
				//playGeigerCounterSound(true, abs(lastSubpixelDrift), lastSubpixelDrift > 0);
			}

			lastOverDot = 0;
			lastSubpixelDrift = 0;


		}
		catch (HCMRuntimeException ex)
		{
			RenderTextHelper::drawOutlinedText(std::format("Sensitivity Drift Overlay: a weird error occured: \n{}", ex.what()), { 500, 400 }, 0xFFFFFFFF);
		}

	}

	void toggleSensDrift(bool newValue)
	{
		// subsribe to renderEvent, revert event, and enable mouseMovement hook
		if (newValue)
		{
			PLOG_DEBUG << "enabling sens drift overlay for game: " << ((GameState)gameT).toString();
			mRenderEventCallback = std::make_unique<ScopedCallback<RenderEvent>>(mRenderEvent, [this](SimpleMath::Vector2 ss) {onRenderEvent(ss); });


			if (revertEventHookOptionalWeak.has_value())
			{
				try
				{
					lockOrThrow(revertEventHookOptionalWeak.value(), revertEventHook);
					mRevertEventCallback = revertEventHook->getRevertEvent()->subscribe([this]() { onRevertEvent(); });
				}
				catch (HCMRuntimeException ex)
				{
					runtimeExceptions->handleMessage(ex);
				}
			}

			resetCounts();
			mouseMovementHook->setWantsToBeAttached(true);
		}
		else
		{
			PLOG_DEBUG << "disabling sens drift overlay for game: " << ((GameState)gameT).toString();
			mRenderEventCallback.reset();
			mRevertEventCallback.reset();
			mouseMovementHook->setWantsToBeAttached(false);
		}
	}

	void resetCounts()
	{
		// reset counts 
		overDotCount = LeftRightCounter();
		subpixelDriftCount = LeftRightCounter();
		subpixelDriftAngle = 0;
	}

	void onRevertEvent()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			if (settings->sensResetCountOnRevertToggle->GetValue())
			{
				resetCounts();
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}

	void onMCCStateChangedEvent(const MCCState& newState)
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			toggleSensDrift(newState.currentGameState == (GameState)gameT && settings->sensDriftOverlayToggle->GetValue());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			toggleSensDrift(false);
		}
	}

	void onSensDriftOverlayToggleEvent(bool& newValue)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			toggleSensDrift(mccStateHook->isGameCurrentlyPlaying((GameState)gameT) && newValue);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			toggleSensDrift(false);
		}
	}

public:
	SensDriftOverlayImpl(GameState game, IDIContainer& dicon)
		: runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesWeak(dicon.Resolve<IMessagesGUI>()),
		sensDriftOverlayToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->sensDriftOverlayToggle->valueChangedEvent, [this](bool& n) {onSensDriftOverlayToggleEvent(n); }),
		mGameStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) {onMCCStateChangedEvent(n); }),
		mRenderEvent(dicon.Resolve<RenderEvent>()),
		getPlayerViewAngleWeak(resolveDependentCheat(GetPlayerViewAngle)),
		mResetCountEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->sensResetCountsEvent, [this]() { resetCounts();  }),
		blockGameInputOptionalWeak(dicon.Resolve<ControlServiceContainer>().lock()->blockGameInputService),
		editPlayerViewAngleSetCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleSet, [this]() { resetCounts(); })
	{
		try
		{
			auto revertEventHookWeak = resolveDependentCheat(RevertEventHook);
			revertEventHookOptionalWeak = revertEventHookWeak;
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "SensDriftOverlayImpl could not resolve optional service: " << nameof(varName) << ", error: " << ex.what();
		}

		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto mouseMovementFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(mouseMovementFunction), game);
		mouseMovementHook = ModuleMidHook::make(game.toModuleName(), mouseMovementFunction, mouseMovementHookFunction);
		mouseMovementContextInterpreter = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(mouseMovementContextInterpreter), game);
		gameInputIsActive = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameInputIsActive), game);




		instance = this;
	}

	~SensDriftOverlayImpl()
	{
		PLOG_DEBUG << "~SensDriftOverlayImpl";
		if (mouseMovementHookRunningMutex)
		{
			PLOG_INFO << "Waiting for mouseMovementHook to finish execution";
			mouseMovementHookRunningMutex.wait(true);
		}

		mouseMovementHook.reset();

		instance = nullptr;
	}

	


};













SensDriftOverlay::SensDriftOverlay(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<SensDriftOverlayImpl<GameState::Value::Halo1>>(game, dicon); break;
	//case GameState::Value::Halo2: pimpl = std::make_unique<SensDriftOverlayImpl<GameState::Value::Halo2>>(game, dicon); break;
	//case GameState::Value::Halo3: pimpl = std::make_unique<SensDriftOverlayImpl<GameState::Value::Halo3>>(game, dicon); break;
	//case GameState::Value::Halo3ODST: pimpl = std::make_unique<SensDriftOverlayImpl<GameState::Value::Halo3ODST>>(game, dicon); break;
	//case GameState::Value::HaloReach: pimpl = std::make_unique<SensDriftOverlayImpl<GameState::Value::HaloReach>>(game, dicon); break;
	//case GameState::Value::Halo4: pimpl = std::make_unique<SensDriftOverlayImpl<GameState::Value::Halo4>>(game, dicon); break;
	default: throw HCMInitException("Not impl");
	}
}

SensDriftOverlay::~SensDriftOverlay() = default;