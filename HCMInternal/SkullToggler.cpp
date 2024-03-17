#include "pch.h"
#include "SkullToggler.h"
#include "SkullEnum.h"
#include "BitBoolPointer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"

class SkullTogglerImpl : public ISkullTogglerImpl
{
private:

	// game
	GameState mGame;

	// cache stuff
	bool cacheValid = false;
	std::map<SkullEnum, std::shared_ptr<MultilevelPointer>> skullDataPointers;

	// event callbacks
	ScopedCallback<eventpp::CallbackList<void (GameState)>> updateSkullBitBoolCollectionEventCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// hotkey callbacks
	ScopedCallback<ActionEvent> skullAngerToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullAssassinsToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullBlackEyeToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullBlindToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullCatchToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullEyePatchToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullFamineToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullFogToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullForeignToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullIronToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullJackedToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullMasterblasterToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullMythicToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullRecessionToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullSoAngryToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullStreakingToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullSwarmToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullThatsJustWrongToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullTheyComeBackToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullThunderstormToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullTiltToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullToughLuckToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullBandannaToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullBondedPairToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullBoomToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullCowbellToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullEnvyToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullFeatherToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullGhostToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullGruntBirthdayPartyToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullGruntFuneralToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullIWHBYDToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullMalfunctionToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullPinataToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullProphetBirthdayPartyToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullScarabToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullSputnikToggleHotkeyEventCallback;
	ScopedCallback<ActionEvent> skullAcrophobiaToggleHotkeyEventCallback;

	// injected services
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;


	// called by skull toggle gui before it wants to use the skullBitBoolCollection
	void onUpdateSkullBitBoolCollectionEvent(GameState game)
	{
		if (game != mGame) return; // not our game, not our problem

		if (cacheValid == true)
		{
			return; // no work to do
		}
		else // update cache!
		{
			try
			{
				lockOrThrow(settingsWeak, settings);
				
				if (settings->skullBitBoolCollectionInUse) // wait atomic bool
				{
					settings->skullBitBoolCollectionInUse.wait(true);
				}

				ScopedAtomicBool lock(settings->skullBitBoolCollectionInUse); // lock atomic bool

				settings->skullBitBoolCollection.clear(); // clear cache

				for (auto& [skullEnumKey, dataPointer] : skullDataPointers) // loop thru our valid data pointers and update cache
				{
					uintptr_t skullPointer;
					if (dataPointer->resolve(&skullPointer))
						settings->skullBitBoolCollection.insert({ skullEnumKey, BitBoolPointer(skullPointer, dataPointer->getBitOffset()) });
					else
						PLOG_ERROR << "error resolving skull pointer " << magic_enum::enum_name(skullEnumKey) << ": " << MultilevelPointer::GetLastError();
				}

				cacheValid = true;
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
			
		}

	}

	// event to fire on mcc state change (set flag saying we need to update cache)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing SkullTogglerImpl caches";
		cacheValid = false;
	}



	void onHotkeyToggle(SkullEnum whichSkullToFlip)
	{
		try
		{
			PLOG_DEBUG << "SkullTogglerImpl::onHotkeyToggle";
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;
			PLOG_DEBUG << "SkullTogglerImpl::onHotkeyToggle has correct game";

			if (skullDataPointers.contains(whichSkullToFlip) == false) return;
			PLOG_DEBUG << "SkullTogglerImpl::onHotkeyToggle has that pointer data";

			
			onUpdateSkullBitBoolCollectionEvent(mGame);
			lockOrThrow(settingsWeak, settings);

			if (settings->skullBitBoolCollectionInUse) // wait atomic bool
			{
				settings->skullBitBoolCollectionInUse.wait(true);
			}
			ScopedAtomicBool lock(settings->skullBitBoolCollectionInUse); // lock atomic bool
			if (settings->skullBitBoolCollection.contains(whichSkullToFlip) == false) throw HCMRuntimeException(std::format("Couldn't resolve skull pointer for {}", magic_enum::enum_name(whichSkullToFlip)));
			auto& bitBoolPointer = settings->skullBitBoolCollection.at(whichSkullToFlip);

			bool previousValue = bitBoolPointer.operator const bool();
			PLOG_DEBUG << "SkullTogglerImpl::onHotkeyToggle flipping value from " << (previousValue ? "true" : "false") << " to " << (previousValue ? "false" : "true");
			bitBoolPointer.set(!previousValue); // flip value
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}



public:
	SkullTogglerImpl(GameState gameImpl, IDIContainer& dicon)
		: 
		mGame(gameImpl),
		updateSkullBitBoolCollectionEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->updateSkullBitBoolCollectionEvent, [this](GameState g) { onUpdateSkullBitBoolCollectionEvent(g); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); }),

		// scoped callbacks for all the goddamn skull hotkeys lol
		skullAngerToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullAngerToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Anger); }),
		skullAssassinsToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullAssassinsToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Assassins); }),
		skullBlackEyeToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullBlackEyeToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::BlackEye); }),
		skullBlindToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullBlindToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Blind); }),
		skullCatchToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullCatchToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Catch); }),
		skullEyePatchToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullEyePatchToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::EyePatch); }),
		skullFamineToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullFamineToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Famine); }),
		skullFogToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullFogToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Fog); }),
		skullForeignToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullForeignToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Foreign); }),
		skullIronToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullIronToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Iron); }),
		skullJackedToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullJackedToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Jacked); }),
		skullMasterblasterToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullMasterblasterToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Masterblaster); }),
		skullMythicToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullMythicToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Mythic); }),
		skullRecessionToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullRecessionToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Recession); }),
		skullSoAngryToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullSoAngryToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::SoAngry); }),
		skullStreakingToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullStreakingToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Streaking); }),
		skullSwarmToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullSwarmToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Swarm); }),
		skullThatsJustWrongToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullThatsJustWrongToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::ThatsJustWrong); }),
		skullTheyComeBackToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullTheyComeBackToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::TheyComeBack); }),
		skullThunderstormToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullThunderstormToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Thunderstorm); }),
		skullTiltToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullTiltToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Tilt); }),
		skullToughLuckToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullToughLuckToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::ToughLuck); }),
		skullBandannaToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullBandannaToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Bandanna); }),
		skullBondedPairToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullBondedPairToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::BondedPair); }),
		skullBoomToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullBoomToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Boom); }),
		skullCowbellToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullCowbellToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Cowbell); }),
		skullEnvyToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullEnvyToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Envy); }),
		skullFeatherToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullFeatherToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Feather); }),
		skullGhostToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullGhostToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Ghost); }),
		skullGruntBirthdayPartyToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullGruntBirthdayPartyToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::GruntBirthdayParty); }),
		skullGruntFuneralToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullGruntFuneralToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::GruntFuneral); }),
		skullIWHBYDToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullIWHBYDToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::IWHBYD); }),
		skullMalfunctionToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullMalfunctionToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Malfunction); }),
		skullPinataToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullPinataToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Pinata); }),
		skullProphetBirthdayPartyToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullProphetBirthdayPartyToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::ProphetBirthdayParty); }),
		skullScarabToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullScarabToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Scarab); }),
		skullSputnikToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullSputnikToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Sputnik); }),
		skullAcrophobiaToggleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->skullAcrophobiaToggleHotkeyEvent, [this]() { onHotkeyToggle(SkullEnum::Acrophobia); })




	{
		std::map < SkullEnum, std::set<GameState::Value>> skullEnumToSupportedGames =
		{
			{SkullEnum::Anger, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Assassins, {GameState::Value::Halo2, }},
			{SkullEnum::BlackEye, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Blind, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Catch, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::EyePatch, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Famine, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Fog, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Foreign, {GameState::Value::Halo1, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Iron, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Jacked, {GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Masterblaster, {GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Mythic, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Recession, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::SoAngry, {GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Streaking, {GameState::Value::Halo2, }},
			{SkullEnum::Swarm, {GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::ThatsJustWrong, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::TheyComeBack, {GameState::Value::Halo2, GameState::Value::Halo3, }},
			{SkullEnum::Thunderstorm, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Tilt, {GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::ToughLuck, {GameState::Value::Halo1, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Bandanna, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::BondedPair, {GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Boom, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Cowbell, {GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Envy, {GameState::Value::Halo2, }},
			{SkullEnum::Feather, {GameState::Value::Halo2, }},
			{SkullEnum::Ghost, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::GruntBirthdayParty, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::GruntFuneral, {GameState::Value::Halo1, GameState::Value::Halo2, }},
			{SkullEnum::IWHBYD, {GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
			{SkullEnum::Malfunction, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::Pinata, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, }},
			{SkullEnum::ProphetBirthdayParty, {GameState::Value::Halo2, }},
			{SkullEnum::Scarab, {GameState::Value::Halo2, }},
			{SkullEnum::Sputnik, {GameState::Value::Halo1, GameState::Value::Halo2, }},
			{SkullEnum::Acrophobia, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4, }},
		};

		auto pointerDataStore = dicon.Resolve<PointerDataStore>().lock();
		for (auto& [skullEnum, supportedGameCollection] : skullEnumToSupportedGames)
		{
			if (supportedGameCollection.contains(gameImpl))
			{
				std::string pointerName = std::format("skull{}Pointer", magic_enum::enum_name(skullEnum));
				try
				{
					auto pointer = pointerDataStore->getData<std::shared_ptr<MultilevelPointer>>(pointerName, gameImpl);
					skullDataPointers.emplace(skullEnum, pointer);
				}
				catch(HCMInitException ex)
				{
					PLOG_ERROR << "skull toggler could not resolve " << pointerName << " for game " << gameImpl.toString() << ": " << ex.what();
				}
			}

		}

		if (skullDataPointers.empty()) throw HCMInitException("Could not resolve any data pointers for skulls!");
	}
};







SkullToggler::SkullToggler(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<SkullTogglerImpl>(gameImpl, dicon);
}

SkullToggler::~SkullToggler() = default;