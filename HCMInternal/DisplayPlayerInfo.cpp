#include "pch.h"
#include "DisplayPlayerInfo.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "GameTickEventHook.h"
#include "IMakeOrGetCheat.h"
#include "GetPlayerDatum.h"
#include "GetPlayerViewAngle.h"
#include "GetObjectPhysics.h"
#include "GetCurrentDifficulty.h"
#include "GetObjectAddress.h"
#include "GetAggroData.h"
#include "GetObjectHealth.h"
#include "GetObjectTagName.h"
#include "GetBipedsVehicleDatum.h"
#include "GetTagName.h"
#include "GetNextObjectDatum.h"

#include "GetGameDataAsString.h"
#include "GetPlayerDataAsString.h"
#include "GetEntityDataAsString.h"

#include "ProggyVectorRegularFont.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GlobalKill.h"







template <GameState::Value gameT>
class DisplayPlayerInfoImpl : public DisplayPlayerInfoImplUntemplated
{
private:
	// data
	GameState mGame;
	bool isActive = false;
	GetGameDataAsString<gameT> getGameDataAsString;
	GetPlayerDataAsString<gameT> getPlayerDataAsString;
	GetEntityDataAsString<gameT> getEntityDataAsString;



	// we'll swap between rendering dataA and B on every tick event to prevent having to use a blocking mutex (since rendering & updating both want to access the strings and run from different threads)
	bool useDataA = true; // false means use dataB
	bool currentlyTrackingPlayer;
	bool currentlyTrackingCustomObject;

	std::string dataStringA = std::string( 500, '\0');
	std::string dataStringB = std::string( 500, '\0');

	// main callbacks
	ScopedCallback<RenderEvent> mRenderEventCallback;
	ScopedCallback<eventpp::CallbackList<void(int)>> mGameTickEventCallback;
	ScopedCallback<ToggleEvent> mDisplayPlayerInfoToggleEventCallback;
	ScopedCallback<eventpp::CallbackList<void(const MCCState&)>> mGameStateChangedCallback;
	ScopedCallback<eventpp::CallbackList<void(int&)>> display2DInfoFontSizeCallback;





	// injected services
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<GetPlayerDatum> getPlayerDatumWeak;
	std::weak_ptr<GetObjectAddress> getObjectAddressWeak;


	// fires every game tick, updates info (dataString) if isActive. Alternate which data string to use every game tick
	void onGameTickEvent(int currentGameTick)
	{
		LOG_ONCE(PLOG_VERBOSE << "onGameTickEvent");
		if (!isActive) return;
		if (GlobalKill::isKillSet()) return;
		LOG_ONCE(PLOG_DEBUG << "Updating displayPlayerInfo data");

		try
		{
			getGameDataAsString.updateGameData(!useDataA, currentGameTick);

			if (currentlyTrackingPlayer) getPlayerDataAsString.updatePlayerData(!useDataA);
			if (currentlyTrackingCustomObject) getEntityDataAsString.updateEntityData(!useDataA);

			std::string* dataString = !useDataA ? &dataStringA : &dataStringB;
			*dataString = getGameDataAsString.getDataString(!useDataA);
			if (currentlyTrackingPlayer) dataString->append(getPlayerDataAsString.getDataString(!useDataA));
			if (currentlyTrackingCustomObject) dataString->append(getEntityDataAsString.getDataString(!useDataA));

			useDataA = !useDataA;
		}
		catch (HCMRuntimeException ex)
		{
			ex.prepend("An error occured while displaying 2D Info!\n");
			runtimeExceptions->handleMessage(ex);

			lockOrThrow(settingsWeak, settings)
			settings->display2DInfoToggle->GetValueDisplay() = false;
			settings->display2DInfoToggle->UpdateValueWithInput();
			
		}
	
	}

	// fires every frame, render dataString if isActive.
	void onRenderEvent(SimpleMath::Vector2 screenSize)
	{
		LOG_ONCE(PLOG_VERBOSE << "onRenderEvent");
		if (!isActive) return;
		LOG_ONCE(PLOG_DEBUG << "Rendering displayPlayerInfo data");
		try
		{
			// select which data string to render

			// just as a temporary implentation of the UI measure until implementation complete
			//ImGui::SetNextWindowSize({100, 100});

			lockOrThrow(settingsWeak, settings);

			// calculate screen position of display info window
			SimpleMath::Vector2& cornerOffset = settings->display2DInfoScreenOffset->GetValue();
			SimpleMath::Vector2 displayInfoScreenPosition;
			switch ((SettingsStateAndEvents::Display2DInfoAnchorEnum)settings->display2DInfoAnchorCorner->GetValue())
			{
			case SettingsStateAndEvents::Display2DInfoAnchorEnum::TopLeft:
				displayInfoScreenPosition = cornerOffset ;
				break;

			case SettingsStateAndEvents::Display2DInfoAnchorEnum::TopRight:
				displayInfoScreenPosition = {screenSize.x - cornerOffset.x, cornerOffset.y};
				break;

			case SettingsStateAndEvents::Display2DInfoAnchorEnum::BottomRight:
				displayInfoScreenPosition = { screenSize.x - cornerOffset.x, screenSize.y - cornerOffset.y };
				break;

			case SettingsStateAndEvents::Display2DInfoAnchorEnum::BottomLeft:
				displayInfoScreenPosition = { cornerOffset.x, screenSize.y - cornerOffset.y };
				break;

			default:
					throw HCMRuntimeException(std::format("Invalid enum value for Display2DInfoAnchorEnum: ", settings->display2DInfoAnchorCorner->GetValue()));
			}

			//std::vector<std::string_view> stringsToRender;
			//stringsToRender.push_back(getGameDataAsString.getDataString(useDataA));
			//if (currentlyTrackingPlayer) stringsToRender.push_back(getPlayerDataAsString.getDataString(useDataA));
			//if (currentlyTrackingCustomObject) stringsToRender.push_back(getEntityDataAsString.getDataString(useDataA));

			auto windowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize;

			auto fontColour = ImGui::ColorConvertFloat4ToU32(settings->display2DInfoFontColour->GetValue());

			// https://github.com/ocornut/imgui/issues/6037#issuecomment-1368377977
			// this call is submitting to implicit debug window. How can I change font scale OUTSIDE of a window?
			
			ImFont* currentFont = ImGui::GetFont();
			float fontScale = fontSize / (15.f * 2.f);
			currentFont->Scale = fontScale;
			ImGui::PushFont(currentFont);


			std::string* stringToRender = useDataA ? &dataStringA : &dataStringB;

			if (settings->display2DInfoOutline->GetValue())
			{
				for (int i = 0; i < 9; i++)
				{
					auto outlineColour = ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.8f }); // black
					SimpleMath::Vector2 textPos;

					switch (i)
					{
					case 0: textPos = { displayInfoScreenPosition.x - fontScale, displayInfoScreenPosition.y - fontScale }; break;
					case 1: textPos = { displayInfoScreenPosition.x                , displayInfoScreenPosition.y - fontScale }; break;
					case 2: textPos = { displayInfoScreenPosition.x + fontScale, displayInfoScreenPosition.y - fontScale }; break;

					case 3: textPos = { displayInfoScreenPosition.x - fontScale, displayInfoScreenPosition.y + fontScale }; break;
					case 4: textPos = { displayInfoScreenPosition.x                , displayInfoScreenPosition.y + fontScale }; break;
					case 5: textPos = { displayInfoScreenPosition.x + fontScale, displayInfoScreenPosition.y + fontScale }; break;

					case 6: textPos = { displayInfoScreenPosition.x - fontScale, displayInfoScreenPosition.y }; break;
					case 7: textPos = { displayInfoScreenPosition.x + fontScale, displayInfoScreenPosition.y }; break;


					case 8: textPos = displayInfoScreenPosition; break;
						
					}

					auto* colourToRender = i == 8 ? &fontColour : &outlineColour;

					ImGui::GetBackgroundDrawList()->AddText(textPos, *colourToRender, stringToRender->c_str());
				}
			}
			else
			{
					ImGui::GetBackgroundDrawList()->AddText(displayInfoScreenPosition, fontColour, stringToRender->c_str());
			}

			ImGui::PopFont();



		}
		catch (HCMRuntimeException ex)
		{
			isActive = false;
			runtimeExceptions->handleMessage(ex);
		}


	}

	// fires when user toggles displayPlayerInfo checkbox. sets isActive true if current game is mGame.
	void onDisplayPlayerInfoToggleEvent(bool& newValue)
	{
		try
		{
			lockOrThrow(messagesGUIWeak, messagesGUI);
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (!mccStateHook->isGameCurrentlyPlaying(mGame))
			{
				PLOG_DEBUG << "Game is not currently playing " << mGame.toString() << "so NOT enabling DisplayPlayerInfo.isActive";
				isActive = false;
				return;
			}

			messagesGUI->addMessage(newValue ? "Enabling Player Info overlay." : "Disabling Player Info overlay");
			isActive = newValue;

		}
		catch (HCMRuntimeException ex)
		{
			isActive = false;
			runtimeExceptions->handleMessage(ex);
		}
	}

	// fires when gameState changed, setting isActive true if gameState is this cheats mGame && displayInfoToggle is enabled
	void onGameStateChanged(const MCCState& newMCCState)
	{
		try
		{
			PLOG_DEBUG << "DisplayPlayerInfo running onGameStateChanged";
			lockOrThrow(settingsWeak, settings);
			isActive = (settings->display2DInfoToggle->GetValue() && newMCCState.currentGameState == mGame && newMCCState.currentPlayState == PlayState::Ingame);
		}
		catch (HCMRuntimeException ex)
		{
			isActive = false;
			runtimeExceptions->handleMessage(ex);
		}
	}


	// settings callbacks (all bind to same func)
	ScopedCallback<ToggleEvent> display2DInfoShowGameTickCallback; 
	ScopedCallback<ToggleEvent> display2DInfoShowAggroCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowNextObjectDatumCallback;
	ScopedCallback<ToggleEvent> display2DInfoTrackPlayerCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerViewAngleCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerPositionCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerVelocityCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerVelocityAbsCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerVelocityXYCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerVelocityXYZCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerHealthCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerRechargeCooldownCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowPlayerVehicleHealthCallback;
	ScopedCallback<ToggleEvent> display2DInfoTrackCustomObjectCallback;
	ScopedCallback<eventpp::CallbackList<void(uint32_t&)>> display2DInfoCustomObjectDatumCallback; 
	ScopedCallback<ToggleEvent> display2DInfoShowEntityObjectTypeCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityTagNameCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityPositionCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityVelocityCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityVelocityAbsCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityVelocityXYCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityVelocityXYZCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityHealthCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityRechargeCooldownCallback;
	ScopedCallback<ToggleEvent> display2DInfoShowEntityVehicleHealthCallback;
	ScopedCallback<eventpp::CallbackList<void(int&)>> display2DInfoFloatPrecisionCallback;



	void onSettingsUpdate()
	{
		// update getEntityDataAsString and getGameDataAsString with services or nullopt as settings dictate.
		// any setting changes causes all settings to be updated - settings changes don't happen often so performance impact is whatever.
	
		try
		{
			lockOrThrow(settingsWeak, settings);
			getGameDataAsString.showGameTick = settings->display2DInfoShowGameTick->GetValue();
			getGameDataAsString.getAggroDataOptionalWeak = settings->display2DInfoShowAggro->GetValue() ? this->getAggroDataOptionalWeak : std::nullopt;
			getGameDataAsString.getNextObjectDatumOptionalWeak = settings->display2DInfoShowNextObjectDatum->GetValue() ? this->getNextObjectDatumOptionalWeak : std::nullopt;
			getGameDataAsString.ss << std::setprecision(settings->display2DInfoFloatPrecision->GetValue());

			currentlyTrackingPlayer = settings->display2DInfoTrackPlayer->GetValue();
			getPlayerDataAsString.getPlayerViewAngleOptionalWeak = settings->display2DInfoShowPlayerViewAngle->GetValue() ? this->getPlayerViewAngleOptionalWeak : std::nullopt;
			getPlayerDataAsString.getObjectPhysicsOptionalWeakPos = settings->display2DInfoShowPlayerPosition->GetValue() ? this->getObjectPhysicsOptionalWeak : std::nullopt;
			getPlayerDataAsString.getObjectPhysicsOptionalWeakVel = settings->display2DInfoShowPlayerVelocity->GetValue() ? this->getObjectPhysicsOptionalWeak : std::nullopt;
			getPlayerDataAsString.velocityAbsolute = settings->display2DInfoShowPlayerVelocityAbs->GetValue();
			getPlayerDataAsString.velocityXY = settings->display2DInfoShowPlayerVelocityXY->GetValue();
			getPlayerDataAsString.velocityXYZ = settings->display2DInfoShowPlayerVelocityXYZ->GetValue();
			getPlayerDataAsString.getObjectHealthOptionalWeak = settings->display2DInfoShowPlayerHealth->GetValue() ? this->getObjectHealthOptionalWeak : std::nullopt;
			getPlayerDataAsString.showHealthShieldCooldown = settings->display2DInfoShowPlayerRechargeCooldown->GetValue();
			getPlayerDataAsString.getBipedsVehicleDatumOptionalWeak = settings->display2DInfoShowPlayerVehicleHealth->GetValue() ? this->getBipedsVehicleDatumOptionalWeak : std::nullopt;
			getPlayerDataAsString.ss << std::setprecision(settings->display2DInfoFloatPrecision->GetValue());

			currentlyTrackingCustomObject = settings->display2DInfoTrackCustomObject->GetValue();
			getEntityDataAsString.entityDatum = (Datum)settings->display2DInfoCustomObjectDatum->GetValue();
			getEntityDataAsString.showEntityType = settings->display2DInfoShowEntityObjectType->GetValue();
			getEntityDataAsString.getObjectTagNameOptionalWeak = settings->display2DInfoShowEntityTagName->GetValue() ? this->getObjectTagNameOptionalWeak : std::nullopt;
			getEntityDataAsString.getObjectPhysicsOptionalWeakPos = settings->display2DInfoShowEntityPosition->GetValue() ? this->getObjectPhysicsOptionalWeak : std::nullopt;
			getEntityDataAsString.getObjectPhysicsOptionalWeakVel = settings->display2DInfoShowEntityVelocity->GetValue() ? this->getObjectPhysicsOptionalWeak : std::nullopt;
			getEntityDataAsString.velocityAbsolute = settings->display2DInfoShowEntityVelocityAbs->GetValue();
			getEntityDataAsString.velocityXY = settings->display2DInfoShowEntityVelocityXY->GetValue();
			getEntityDataAsString.velocityXYZ = settings->display2DInfoShowEntityVelocityXYZ->GetValue();
			getEntityDataAsString.getObjectHealthOptionalWeak = settings->display2DInfoShowEntityHealth->GetValue() ? this->getObjectHealthOptionalWeak : std::nullopt;
			getEntityDataAsString.showHealthShieldCooldown = settings->display2DInfoShowEntityRechargeCooldown->GetValue();
			getEntityDataAsString.getBipedsVehicleDatumOptionalWeak = settings->display2DInfoShowEntityVehicleHealth->GetValue() ? this->getBipedsVehicleDatumOptionalWeak : std::nullopt;

			getEntityDataAsString.ss << std::setprecision(settings->display2DInfoFloatPrecision->GetValue());

		}
		catch (HCMRuntimeException ex)
		{
			ex.prepend("Error updating Display2DInfo settings, settings might now be in a desynced/invalid state.");
			runtimeExceptions->handleMessage(ex);
		}
	}


	void onFontSizeChange(int& newValue)
	{
		fontSize = (float)newValue;	
		needToLoadNewFont = true;
	}




	// optional injected services
	// game data
	std::optional<std::weak_ptr<GetAggroData>> getAggroDataOptionalWeak;

	// entity data
	std::optional<std::weak_ptr<GetPlayerViewAngle>> getPlayerViewAngleOptionalWeak;
	std::optional<std::weak_ptr<GetCurrentDifficulty>> getCurrentDifficultyOptionalWeak;
	std::optional<std::weak_ptr<GetObjectHealth>> getObjectHealthOptionalWeak;
	std::optional<std::weak_ptr<GetObjectPhysics>> getObjectPhysicsOptionalWeak;
	std::optional<std::weak_ptr<GetObjectTagName>> getObjectTagNameOptionalWeak;
	std::optional<std::weak_ptr<GetBipedsVehicleDatum>> getBipedsVehicleDatumOptionalWeak;
	std::optional<std::weak_ptr<GetNextObjectDatum>> getNextObjectDatumOptionalWeak;

	float fontSize;
	bool needToLoadNewFont = true;
	ImFont* displayInfoFont = nullptr;
public:


#define setSettingCallback(settingName, callbackType) settingName##Callback(settingsWeak.lock()->settingName->valueChangedEvent, [this](callbackType& n){onSettingsUpdate();})



	// TODO: make gameTIckEvent hook not attach until displayPlayerInfo toggled on for first time, since uh that attach has a small chance to crash the game
	DisplayPlayerInfoImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		mRenderEventCallback(dicon.Resolve<RenderEvent>().lock(), [this](SimpleMath::Vector2 ss) { onRenderEvent(ss); }),
		mGameTickEventCallback(resolveDependentCheat(GameTickEventHook)->getGameTickEvent(), [this](int t) { onGameTickEvent(t); }),
		mDisplayPlayerInfoToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->display2DInfoToggle->valueChangedEvent, [this](bool& n) {onDisplayPlayerInfoToggleEvent(n); }),
		mGameStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& s) {onGameStateChanged(s); }),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		getEntityDataAsString(resolveDependentCheat(GetObjectAddress)),
		getPlayerDataAsString(resolveDependentCheat(GetObjectAddress), resolveDependentCheat(GetPlayerDatum)),
		getGameDataAsString(),
		display2DInfoFontSizeCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->display2DInfoFontSize->valueChangedEvent, [this](int& n) {onFontSizeChange(n); }),
		setSettingCallback(display2DInfoShowGameTick, bool),
		setSettingCallback(display2DInfoShowAggro, bool),
		setSettingCallback(display2DInfoShowNextObjectDatum, bool),
		setSettingCallback(display2DInfoTrackPlayer, bool),
		setSettingCallback(display2DInfoShowPlayerViewAngle, bool),
		setSettingCallback(display2DInfoShowPlayerPosition, bool),
		setSettingCallback(display2DInfoShowPlayerVelocity, bool),
		setSettingCallback(display2DInfoShowPlayerVelocityAbs, bool),
		setSettingCallback(display2DInfoShowPlayerVelocityXY, bool),
		setSettingCallback(display2DInfoShowPlayerVelocityXYZ, bool),
		setSettingCallback(display2DInfoShowPlayerHealth, bool),
		setSettingCallback(display2DInfoShowPlayerRechargeCooldown, bool),
		setSettingCallback(display2DInfoShowPlayerVehicleHealth, bool),
		setSettingCallback(display2DInfoTrackCustomObject, bool),
		setSettingCallback(display2DInfoCustomObjectDatum, uint32_t),
		setSettingCallback(display2DInfoShowEntityObjectType, bool),
		setSettingCallback(display2DInfoShowEntityTagName, bool),
		setSettingCallback(display2DInfoShowEntityPosition, bool),
		setSettingCallback(display2DInfoShowEntityVelocity, bool),
		setSettingCallback(display2DInfoShowEntityVelocityAbs, bool),
		setSettingCallback(display2DInfoShowEntityVelocityXY, bool),
		setSettingCallback(display2DInfoShowEntityVelocityXYZ, bool),
		setSettingCallback(display2DInfoShowEntityHealth, bool),
		setSettingCallback(display2DInfoShowEntityRechargeCooldown, bool),
		setSettingCallback(display2DInfoShowEntityVehicleHealth, bool),
		setSettingCallback(display2DInfoFloatPrecision, int)


	{
		
		bool atLeastOneServiceIsWorking = false;
		
#define resolveOptionalDisplayInfoService(varName, enumName)					\
		try													\
		{													\
			varName = resolveDependentCheat(enumName);				\
			atLeastOneServiceIsWorking = true;				\
		}													\
		catch (HCMInitException ex)							\
		{													\
			PLOG_ERROR << "DisplayPlayerInfoImpl could not resolve optional service: " << nameof(varName) << ", error: " << ex.what();				\
		}
		

		resolveOptionalDisplayInfoService(getObjectPhysicsOptionalWeak, GetObjectPhysics);
		resolveOptionalDisplayInfoService(getPlayerViewAngleOptionalWeak, GetPlayerViewAngle);
		resolveOptionalDisplayInfoService(getCurrentDifficultyOptionalWeak, GetCurrentDifficulty);
		resolveOptionalDisplayInfoService(getAggroDataOptionalWeak, GetAggroData);
		resolveOptionalDisplayInfoService(getObjectHealthOptionalWeak, GetObjectHealth);
		resolveOptionalDisplayInfoService(getObjectTagNameOptionalWeak, GetObjectTagName);
		resolveOptionalDisplayInfoService(getBipedsVehicleDatumOptionalWeak, GetBipedsVehicleDatum);
		resolveOptionalDisplayInfoService(getNextObjectDatumOptionalWeak, GetNextObjectDatum);

		if (atLeastOneServiceIsWorking == false) throw HCMInitException("DisplayPlayerInfoImpl could not resolve any optional services for getting data!");

		fontSize = (float)dicon.Resolve<SettingsStateAndEvents>().lock()->display2DInfoFontSize->GetValue();

		// init getString thingy settings
		onSettingsUpdate();

	}

};









DisplayPlayerInfo::DisplayPlayerInfo(GameState gameImpl, IDIContainer& dicon)
{
	PLOG_DEBUG << "Creating DisplayPlayerInfo";
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<DisplayPlayerInfoImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<DisplayPlayerInfoImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<DisplayPlayerInfoImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<DisplayPlayerInfoImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<DisplayPlayerInfoImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<DisplayPlayerInfoImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("Not impl yet");
	}
}

DisplayPlayerInfo::~DisplayPlayerInfo()
{
	PLOG_DEBUG << "~" << getName();
}