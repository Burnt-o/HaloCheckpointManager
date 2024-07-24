#include "pch.h"
#include "AbilityMeterOverlay.h"
#include "GetAbilityData.h"
#include "GameTickEventHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "IMCCStateHook.h"
#include "GlobalKill.h"
#include "RenderTextHelper.h"
#include "IMakeOrGetCheat.h"
#include "imgui.h"

template<GameState::Value gameT>
class AbilityMeterOverlayImpl : public AbilityMeterOverlayUntemplated
{
private:
	// callbacks
	std::unique_ptr<ScopedCallback<RenderEvent>> mRenderEventCallback; // only not null when running
	std::shared_ptr<RenderEvent> mRenderEvent;

	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(uint32_t)>>> mGameTickEventCallback; // only not null when running
	std::shared_ptr<ObservedEvent<GameTickEvent>> mGameTickEvent;


	ScopedCallback<ToggleEvent> abilityMeterOverlayToggleEventCallback;
	ScopedCallback<eventpp::CallbackList<void(const MCCState&)>> mGameStateChangedCallback;
	// TODO: callbacks for options

	// locked when being updated or rendering. 
	// TODO: how does this atomic thing work?
	std::atomic<std::optional<AbilityData>> abilityData = std::nullopt;
	pHCMError lastError;


	// injected services
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<GetAbilityData> getAbilityData;

	// fires every game tick, updates info (abilityData) if isActive. Alternate which data to use every game tick
	void onGameTickEvent(uint32_t currentGameTick)
	{
		LOG_ONCE(PLOG_VERBOSE << "onGameTickEvent");
	
		if (GlobalKill::isKillSet()) return;
		LOG_ONCE(PLOG_DEBUG << "Updating displayPlayerInfo data");

		auto settings = settingsWeak.lock();
		if (!settings)
		{
			abilityData.store(std::nullopt);
			lastError = std::make_shared<BadWeakPtrError>(nameof(settings));
			return;
		}
		bool forceHeroAssist = gameT == GameState::Value::Halo4 && settings->abilityMeterHeroAssistToggle->GetValue();


		auto curabilityData = getAbilityData->getAbilityData(forceHeroAssist);
		if (curabilityData)
			abilityData.store(curabilityData.value());
		else
		{
			abilityData.store(std::nullopt);
			lastError = curabilityData.error();
		}
	}


	// fires every frame, render abilityData depending on options
	void onRenderEvent(SimpleMath::Vector2 screenSize)
	{

		try
		{
			lockOrThrow(settingsWeak, settings);
			// calculate screen positions

			const SimpleMath::Vector2& abilityMeterAbilityCornerOffset = settings->abilityMeterAbilityScreenOffset->GetValue();
			SimpleMath::Vector2 abilityMeterAbilityBarPosition;
			switch (settings->abilityMeterAbilityAnchorCorner->GetValue())
			{
			case SettingsEnums::ScreenAnchorEnum::TopLeft:
				abilityMeterAbilityBarPosition = abilityMeterAbilityCornerOffset;
				break;

			case SettingsEnums::ScreenAnchorEnum::TopRight:
				abilityMeterAbilityBarPosition = { screenSize.x - abilityMeterAbilityCornerOffset.x, abilityMeterAbilityCornerOffset.y };
				break;

			case SettingsEnums::ScreenAnchorEnum::BottomRight:
				abilityMeterAbilityBarPosition = { screenSize.x - abilityMeterAbilityCornerOffset.x, screenSize.y - abilityMeterAbilityCornerOffset.y };
				break;

			case SettingsEnums::ScreenAnchorEnum::BottomLeft:
				abilityMeterAbilityBarPosition = { abilityMeterAbilityCornerOffset.x, screenSize.y - abilityMeterAbilityCornerOffset.y };
				break;
			}

			const SimpleMath::Vector2& abilityMeterCooldownCornerOffset = settings->abilityMeterCooldownScreenOffset->GetValue();
			SimpleMath::Vector2 abilityMeterCooldownBarPosition;
			switch (settings->abilityMeterCooldownAnchorCorner->GetValue())
			{
			case SettingsEnums::ScreenAnchorEnum::TopLeft:
				abilityMeterCooldownBarPosition = abilityMeterCooldownCornerOffset;
				break;

			case SettingsEnums::ScreenAnchorEnum::TopRight:
				abilityMeterCooldownBarPosition = { screenSize.x - abilityMeterCooldownCornerOffset.x, abilityMeterCooldownCornerOffset.y };
				break;

			case SettingsEnums::ScreenAnchorEnum::BottomRight:
				abilityMeterCooldownBarPosition = { screenSize.x - abilityMeterCooldownCornerOffset.x, screenSize.y - abilityMeterCooldownCornerOffset.y };
				break;

			case SettingsEnums::ScreenAnchorEnum::BottomLeft:
				abilityMeterCooldownBarPosition = { abilityMeterCooldownCornerOffset.x, screenSize.y - abilityMeterCooldownCornerOffset.y };
				break;
			}

			auto abilityMeterAbilityBackgroundColor = ImGui::ColorConvertFloat4ToU32(settings->abilityMeterAbilityBackgroundColor->GetValue());
			auto abilityMeterAbilityForegroundColor = ImGui::ColorConvertFloat4ToU32(settings->abilityMeterAbilityForegroundColor->GetValue());
			auto abilityMeterAbilityHighlightColor = ImGui::ColorConvertFloat4ToU32(settings->abilityMeterAbilityHighlightColor->GetValue());

			auto abilityMeterCooldownBackgroundColor = ImGui::ColorConvertFloat4ToU32(settings->abilityMeterCooldownBackgroundColor->GetValue());
			auto abilityMeterCooldownForegroundColor = ImGui::ColorConvertFloat4ToU32(settings->abilityMeterCooldownForegroundColor->GetValue());
			auto abilityMeterCooldownHighlightColor = ImGui::ColorConvertFloat4ToU32(settings->abilityMeterCooldownHighlightColor->GetValue());

			auto mAbilityData = abilityData.load();

			if (!mAbilityData)
			{
				if (lastError)
					RenderTextHelper::drawOutlinedText(lastError->what(), abilityMeterAbilityBarPosition, abilityMeterAbilityHighlightColor);
				else
					RenderTextHelper::drawOutlinedText("Waiting for ability data..", abilityMeterAbilityBarPosition, abilityMeterAbilityHighlightColor);

				return;
			}
			


			constexpr auto barWinFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground;


			ImGui::PushStyleColor(ImGuiCol_Border, abilityMeterAbilityHighlightColor);
			ImGui::PushStyleColor(ImGuiCol_Text, abilityMeterAbilityHighlightColor);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, abilityMeterAbilityBackgroundColor);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, abilityMeterAbilityForegroundColor);

			ImGui::SetNextWindowPos(abilityMeterAbilityBarPosition);
			//ImGui::SetNextWindowSize(settings->abilityMeterAbilitySize->GetValue());
			ImGui::Begin("abilityPercent", nullptr, barWinFlags);
			ImGui::ProgressBar(mAbilityData.value().abilityPercent, settings->abilityMeterAbilitySize->GetValue());
			ImGui::End();

			ImGui::PopStyleColor(4);




			if (settings->abilityMeterCooldownToggle->GetValue())
			{
				float cooldownPercent = mAbilityData.value().abilityMaxCooldown > 0 ? ((float)mAbilityData.value().abilityCooldown / (float)mAbilityData.value().abilityMaxCooldown) : 0.f;

				std::string cooldownString = std::format("{}/{}", mAbilityData.value().abilityCooldown, mAbilityData.value().abilityMaxCooldown);

				ImGui::PushStyleColor(ImGuiCol_Border, abilityMeterCooldownHighlightColor);
				ImGui::PushStyleColor(ImGuiCol_Text, abilityMeterCooldownHighlightColor);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, abilityMeterCooldownBackgroundColor);
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, abilityMeterCooldownForegroundColor);

				ImGui::SetNextWindowPos(abilityMeterCooldownBarPosition);
				//ImGui::SetNextWindowSize(settings->abilityMeterCooldownSize->GetValue());
				ImGui::Begin("cooldownPercent", nullptr, barWinFlags);
				ImGui::ProgressBar(cooldownPercent, settings->abilityMeterCooldownSize->GetValue(), cooldownString.c_str());
				ImGui::End();

				ImGui::PopStyleColor(4);
			}
		

		}
		catch (HCMRuntimeException ex)
		{
			RenderTextHelper::drawOutlinedText(std::format("Ability Meter: a weird error occured: \n{}", ex.what()), { 500, 400 }, 0xFFFFFFFF);
		}

	}

	void toggleAbilityMeter(bool newValue)
	{
		// subsribe to renderEvent and gameTick event
		if (newValue)
		{
			PLOG_DEBUG << "enabling ability meter for game: " << ((GameState)gameT).toString();
			mRenderEventCallback = std::make_unique<ScopedCallback<RenderEvent>>(mRenderEvent, [this](SimpleMath::Vector2 ss) {onRenderEvent(ss); });
			mGameTickEventCallback = mGameTickEvent->subscribe([this](uint32_t t) {onGameTickEvent(t); });
		}
		else
		{
			PLOG_DEBUG << "disabling ability meter for game: " << ((GameState)gameT).toString();
			mRenderEventCallback.reset();
			mGameTickEventCallback.reset();
		}
	}

	void onMCCStateChangedEvent(const MCCState& newState)
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			toggleAbilityMeter(newState.currentGameState == (GameState)gameT && settings->abilityMeterOverlayToggle->GetValue());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			toggleAbilityMeter(false);
		}
	}

	void onAbilityMeterOverlayToggleEvent(bool& newValue)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			toggleAbilityMeter(mccStateHook->isGameCurrentlyPlaying((GameState)gameT) && newValue);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			toggleAbilityMeter(false);
		}
	}

public:
	AbilityMeterOverlayImpl(GameState game, IDIContainer& dicon)
		: runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		getAbilityData(resolveDependentCheat(GetAbilityData)),
		abilityMeterOverlayToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->abilityMeterOverlayToggle->valueChangedEvent, [this](bool& n) {onAbilityMeterOverlayToggleEvent(n); }),
		mGameStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) {onMCCStateChangedEvent(n); }),
		mRenderEvent(dicon.Resolve<RenderEvent>())
	{
		auto gameTickEventHook = resolveDependentCheat(GameTickEventHook);
		mGameTickEvent = gameTickEventHook->getGameTickEvent();
	}


};













AbilityMeterOverlay::AbilityMeterOverlay(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<AbilityMeterOverlayImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<AbilityMeterOverlayImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

AbilityMeterOverlay::~AbilityMeterOverlay() = default;