#pragma once
#include "pch.h"
#include "RebindableHotkey.h"
#include "EventOnPressHotkey.h"

#include "SettingsStateAndEvents.h"
#include "imgui.h"
#include "boost\preprocessor.hpp"
class HotkeyDefinitions {
public:

	HotkeyDefinitions(std::shared_ptr<SettingsStateAndEvents> settings) : mSettings(settings)
	{
		PLOG_DEBUG << "HotkeyDefinitions constructor";

		assert(rebindableOnlyCount >= 0 && "How the fuck do you have fewer rebindable hotkeys than eventonpresshotkeys?!");



		// need to manually setup allRebindableHotkeys since it's too much of pain to do with initializer lists

		// all rebindable inclues all eventsOnPress hotkeys
		for (auto& [key, val] : allEventOnPressHotkeys)
		{
			allRebindableHotkeys.insert(std::make_pair(key, val));
		}

		// add rebindable-only data
		for (auto& [key, val] : allRebindableOnlyHotkeysData)
		{
			allRebindableHotkeys.insert(std::make_pair(key, val));
		}

		PLOG_VERBOSE << "std::size(allEventOnPressHotkeys) " << std::size(allEventOnPressHotkeys);
		PLOG_VERBOSE << "allRebindableHotkeyEnumCount " << allRebindableHotkeyEnumCount;
		PLOG_VERBOSE << "std::size(allRebindableHotkeys) " << std::size(allRebindableHotkeys);
		PLOG_VERBOSE << "allEventOnPressHotkeyEnumCount " << allEventOnPressHotkeyEnumCount;
		PLOG_VERBOSE << "rebindableOnlyCount " << rebindableOnlyCount;

		assert(std::size(allEventOnPressHotkeys) == allEventOnPressHotkeyEnumCount && "Bad eventonpresshotkey definition count - do you have a hotkey defined for each enum entry?");
		assert(std::size(allRebindableHotkeys) == allRebindableHotkeyEnumCount && "Bad rebindablehotkey definition count - do you have a hotkey defined for each enum entry?");


	}

	~HotkeyDefinitions()
	{
		PLOG_DEBUG << "~HotkeyDefinitions";
	}
	//friend class HotkeyManager; 
	const std::map<RebindableHotkeyEnum, std::shared_ptr<EventOnPressHotkey>>& getAllEventOnPressHotkeys() { return allEventOnPressHotkeys; }
	const std::map<RebindableHotkeyEnum, std::shared_ptr<RebindableHotkey>>& getAllRebindableHotkeys() { return allRebindableHotkeys; }

private:
	std::shared_ptr< SettingsStateAndEvents> mSettings;

	typedef std::vector<BindingCombo> vsk;

#define initEventOnPressHotkey_impl(enumName, hotkeyEvent, defaultBinding, ignoresHotkeyDisabler) \
		{RebindableHotkeyEnum::enumName, std::make_shared<EventOnPressHotkey>\
		(hotkeyEvent,\
		RebindableHotkeyEnum::enumName,\
		defaultBinding,\
		ignoresHotkeyDisabler) }

#define initEventOnPressHotkey(enumName, hotkeyEvent, defaultBinding) initEventOnPressHotkey_impl(enumName, hotkeyEvent, defaultBinding, false)
#define initEventOnPressHotkey_ignoreDisabler(enumName, hotkeyEvent, defaultBinding) initEventOnPressHotkey_impl(enumName, hotkeyEvent, defaultBinding, true)


#define initRebindableHotkey_impl(enumName, defaultBinding, ignoresHotkeyDisabler) \
		{RebindableHotkeyEnum::enumName, std::make_shared<RebindableHotkey>\
		(RebindableHotkeyEnum::enumName,\
		defaultBinding,\
		ignoresHotkeyDisabler) }

#define initRebindableHotkey(enumName, defaultBinding) initRebindableHotkey_impl(enumName, defaultBinding, false)
#define initRebindableHotkey_ignoreDisabler(enumName, defaultBinding) initRebindableHotkey_impl(enumName, defaultBinding, true)



	// have to split up the counting here since BOOST_PP_TUPLE_SIZE can only count up to 64
	static constexpr inline int allEventOnPressHotkeyEnumCount = BOOST_PP_TUPLE_SIZE((ALL_EVENTONPRESS_HOTKEYS)) + BOOST_PP_TUPLE_SIZE((SKULL_HOTKEYS));
	static constexpr inline int allRebindableHotkeyEnumCount = BOOST_PP_TUPLE_SIZE((ALL_EVENTONPRESS_HOTKEYS)) + BOOST_PP_TUPLE_SIZE((NOEVENT_HOTKEYS)) + BOOST_PP_TUPLE_SIZE((SKULL_HOTKEYS));
	static constexpr inline int rebindableOnlyCount = allRebindableHotkeyEnumCount - allEventOnPressHotkeyEnumCount;

	const std::map<RebindableHotkeyEnum, std::shared_ptr<EventOnPressHotkey>>::value_type allEventOnPressHotkeysData[allEventOnPressHotkeyEnumCount]
	{

		initEventOnPressHotkey(toggleGUI,
		mSettings->toggleGUIHotkeyEvent,
		vsk{{ ImGuiKey_GraveAccent }}),

		initEventOnPressHotkey(togglePause,
		mSettings->togglePauseHotkeyEvent,
		vsk{{ ImGuiKey_P }}),

		initEventOnPressHotkey(advanceTicks,
		mSettings->advanceTicksEvent,
		vsk{}),

		initEventOnPressHotkey(forceCheckpoint,
		mSettings->forceCheckpointEvent,
		vsk{{ ImGuiKey_F1 }}),

		initEventOnPressHotkey(forceRevert,
		mSettings->forceRevertEvent,
		vsk{{ ImGuiKey_F2 }}),

		initEventOnPressHotkey(forceDoubleRevert,
		mSettings->forceDoubleRevertEvent,
		vsk{{ ImGuiKey_F3 }}),

		initEventOnPressHotkey(forceCoreSave,
		mSettings->forceCoreSaveEvent,
		vsk{}),

		initEventOnPressHotkey(forceCoreLoad,
		mSettings->forceCoreLoadEvent,
		vsk{}),

		initEventOnPressHotkey(injectCheckpoint,
		mSettings->injectCheckpointEvent,
		vsk{}),

		initEventOnPressHotkey(dumpCheckpoint,
		mSettings->dumpCheckpointEvent,
		vsk{}),

		initEventOnPressHotkey(injectCore,
		mSettings->injectCoreEvent,
		vsk{}),

		initEventOnPressHotkey(dumpCore,
		mSettings->dumpCoreEvent,
		vsk{}),

				initEventOnPressHotkey(forceMissionRestart,
		mSettings->forceMissionRestartEvent,
		vsk{}),

		initEventOnPressHotkey(speedhack,
		mSettings->speedhackHotkeyEvent,
		vsk{{ ImGuiKey_F4 }}),

		initEventOnPressHotkey(invuln,
		mSettings->invulnerabilityHotkeyEvent,
		vsk{{ ImGuiKey_F5 }}),

		initEventOnPressHotkey(aiFreeze,
		mSettings->aiFreezeHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(forceTeleport,
		mSettings->forceTeleportEvent,
		vsk{}),

		initEventOnPressHotkey(forceLaunch,
		mSettings->forceLaunchEvent,
		vsk{}),

		initEventOnPressHotkey(medusa,
		mSettings->medusaHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(naturalCheckpointDisable,
		mSettings->naturalCheckpointDisableHotkeyEvent,
		vsk{}),


		initEventOnPressHotkey(infiniteAmmo,
		mSettings->infiniteAmmoHotkeyEvent,
		vsk{}),


		initEventOnPressHotkey(bottomlessClip,
		mSettings->bottomlessClipHotkeyEvent,
		vsk{}),


		initEventOnPressHotkey(display2DInfo,
		mSettings->display2DInfoHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(freeCamera,
		mSettings->freeCameraHotkeyEvent,
		vsk{}),

				initEventOnPressHotkey(freeCameraTeleportToCameraHotkey,
		mSettings->freeCameraTeleportToCameraEvent,
		vsk{}),

		initEventOnPressHotkey(freeCameraGameInputDisable,
		mSettings->freeCameraGameInputDisableHotkeyEvent,
		vsk{}),

			initEventOnPressHotkey(freeCameraCameraInputDisable,
		mSettings->freeCameraCameraInputDisableHotkeyEvent,
		vsk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraIncreaseTranslationSpeedHotkey,
		mSettings->freeCameraUserInputCameraIncreaseTranslationSpeedHotkey,
		vsk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraDecreaseTranslationSpeedHotkey,
		mSettings->freeCameraUserInputCameraDecreaseTranslationSpeedHotkey,
		vsk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraSetPosition,
		mSettings->freeCameraUserInputCameraSetPosition,
		vsk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraSetRotation,
		mSettings->freeCameraUserInputCameraSetRotation,
		vsk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraMaintainVelocity,
		mSettings->freeCameraUserInputCameraMaintainVelocityHotkeyEvent,
		vsk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraSetVelocity,
		mSettings->freeCameraUserInputCameraSetVelocity,
		vsk{}),

		initEventOnPressHotkey(freeCameraAnchorPositionToObjectPosition,
			mSettings->freeCameraAnchorPositionToObjectPositionHotkeyEvent,
			vsk{}),

		initEventOnPressHotkey(freeCameraAnchorPositionToObjectRotation,
			mSettings->freeCameraAnchorPositionToObjectRotationHotkeyEvent,
			vsk{}),

		initEventOnPressHotkey(freeCameraAnchorRotationToObjectPosition,
			mSettings->freeCameraAnchorRotationToObjectPositionHotkeyEvent,
			vsk{}),

		initEventOnPressHotkey(freeCameraAnchorRotationToObjectFacing,
			mSettings->freeCameraAnchorRotationToObjectFacingHotkeyEvent,
			vsk{}),

		initEventOnPressHotkey(freeCameraAnchorFOVToObjectDistance,
			mSettings->freeCameraAnchorFOVToObjectDistanceHotkeyEvent,
			vsk{}),

		initEventOnPressHotkey(editPlayerViewAngleSet,
			mSettings->editPlayerViewAngleSet,
			vsk{}),

		initEventOnPressHotkey(editPlayerViewAngleAdjustHorizontal,
			mSettings->editPlayerViewAngleAdjustHorizontal,
			vsk{}),

		initEventOnPressHotkey(editPlayerViewAngleAdjustVertical,
			mSettings->editPlayerViewAngleAdjustVertical,
			vsk{}),

			initEventOnPressHotkey(editPlayerViewAngleIDSet,
				mSettings->editPlayerViewAngleIDSet,
				vsk{}),

			initEventOnPressHotkey(editPlayerViewAngleIDAdjustNegative,
				mSettings->editPlayerViewAngleIDAdjustNegative,
				vsk{}),

			initEventOnPressHotkey(editPlayerViewAngleIDAdjustPositive,
				mSettings->editPlayerViewAngleIDAdjustPositive,
				vsk{}),

		initEventOnPressHotkey(switchBSP,
			mSettings->switchBSPEvent,
			vsk{}),

		initEventOnPressHotkey(switchBSPSet,
			mSettings->switchBSPSetLoadSetEvent,
			vsk{}),

			


		initEventOnPressHotkey(hideHUDToggle,
			mSettings->hideHUDToggleHotkeyEvent,
			vsk{}),

			initEventOnPressHotkey(abilityMeterToggleHotkey,
			mSettings->abilityMeterToggleHotkeyEvent,
			vsk{}),

			initEventOnPressHotkey(sensResetCountHotkey,
			mSettings->sensResetCountsEvent,
			vsk{}),

			initEventOnPressHotkey(sensDriftOverlayToggleHotkey,
				mSettings->sensDriftOverlayToggleHotkeyEvent,
				vsk{}),
			

			initEventOnPressHotkey_ignoreDisabler(commandConsoleHotkey,
			mSettings->commandConsoleHotkeyEvent,
			vsk{{ImGuiKey_F8}}),


			initEventOnPressHotkey(commandConsoleExecuteBuffer,
			mSettings->commandConsoleExecuteBufferEvent,
			vsk{}),

			
		initEventOnPressHotkey(setPlayerHealth,
		mSettings->setPlayerHealthEvent,
		vsk{}),

		initEventOnPressHotkey(toggleWaypoint3D,
		mSettings->toggleWaypoint3DHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(toggleViewAngleLine3D,
		mSettings->toggleViewAngle3DHotkeyEvent,
		vsk{}),


		initEventOnPressHotkey(triggerOverlayToggleHotkey,
		mSettings->triggerOverlayToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(softCeilingOverlayToggleHotkey,
		mSettings->softCeilingOverlayToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(disableBarriers,
		mSettings->disableBarriersHotkeyEvent,
		vsk{}),

		/// skullllllllls
		initEventOnPressHotkey(skullAngerHotkey,
		mSettings->skullAngerToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullAssassinsHotkey,
		mSettings->skullAssassinsToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullBlackEyeHotkey,
		mSettings->skullBlackEyeToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullBlindHotkey,
		mSettings->skullBlindToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullCatchHotkey,
		mSettings->skullCatchToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullEyePatchHotkey,
		mSettings->skullEyePatchToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullFamineHotkey,
		mSettings->skullFamineToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullFogHotkey,
		mSettings->skullFogToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullForeignHotkey,
		mSettings->skullForeignToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullIronHotkey,
		mSettings->skullIronToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullJackedHotkey,
		mSettings->skullJackedToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullMasterblasterHotkey,
		mSettings->skullMasterblasterToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullMythicHotkey,
		mSettings->skullMythicToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullRecessionHotkey,
		mSettings->skullRecessionToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullSoAngryHotkey,
		mSettings->skullSoAngryToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullStreakingHotkey,
		mSettings->skullStreakingToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullSwarmHotkey,
		mSettings->skullSwarmToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullThatsJustWrongHotkey,
		mSettings->skullThatsJustWrongToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullTheyComeBackHotkey,
		mSettings->skullTheyComeBackToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullThunderstormHotkey,
		mSettings->skullThunderstormToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullTiltHotkey,
		mSettings->skullTiltToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullToughLuckHotkey,
		mSettings->skullToughLuckToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullBandannaHotkey,
		mSettings->skullBandannaToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullBondedPairHotkey,
		mSettings->skullBondedPairToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullBoomHotkey,
		mSettings->skullBoomToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullCowbellHotkey,
		mSettings->skullCowbellToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullEnvyHotkey,
		mSettings->skullEnvyToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullFeatherHotkey,
		mSettings->skullFeatherToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullGhostHotkey,
		mSettings->skullGhostToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullGruntBirthdayPartyHotkey,
		mSettings->skullGruntBirthdayPartyToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullGruntFuneralHotkey,
		mSettings->skullGruntFuneralToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullIWHBYDHotkey,
		mSettings->skullIWHBYDToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullMalfunctionHotkey,
		mSettings->skullMalfunctionToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullPinataHotkey,
		mSettings->skullPinataToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullProphetBirthdayPartyHotkey,
		mSettings->skullProphetBirthdayPartyToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullScarabHotkey,
		mSettings->skullScarabToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullSputnikHotkey,
		mSettings->skullSputnikToggleHotkeyEvent,
		vsk{}),

		initEventOnPressHotkey(skullAcrophobiaHotkey,
		mSettings->skullAcrophobiaToggleHotkeyEvent,
		vsk{}),



		

	};

	const std::map<RebindableHotkeyEnum, std::shared_ptr<RebindableHotkey>>::value_type allRebindableOnlyHotkeysData[rebindableOnlyCount]
	{

						initRebindableHotkey(cameraTranslateUpBinding,
			(vsk{{ ImGuiKey_Space}, { ImGuiKey_GamepadFaceDown }})),

						initRebindableHotkey(cameraTranslateDownBinding,
			(vsk{{ ImGuiKey_LeftCtrl }, { ImGuiKey_GamepadL3}})),

						initRebindableHotkey(cameraRollLeftBinding,
			(vsk{{ ImGuiKey_G }, { ImGuiKey_GamepadR1}})),

						initRebindableHotkey(cameraRollRightBinding,
			(vsk{{ ImGuiKey_T }, { ImGuiKey_GamepadL1 }})),

						initRebindableHotkey(cameraFOVIncreaseBinding,
			(vsk{{ ImGuiKey_Y }, { ImGuiKey_GamepadL2 }})),

						initRebindableHotkey(cameraFOVDecreaseBinding,
			(vsk{{ ImGuiKey_H }, { ImGuiKey_GamepadR2 }})),


	};

	 std::map<RebindableHotkeyEnum, std::shared_ptr<EventOnPressHotkey>> allEventOnPressHotkeys = std::map<RebindableHotkeyEnum, std::shared_ptr<EventOnPressHotkey>>
		(std::begin(allEventOnPressHotkeysData), std::end(allEventOnPressHotkeysData) );

	 std::map<RebindableHotkeyEnum, std::shared_ptr<RebindableHotkey>> allRebindableHotkeys; // defined in constructor

	 friend class HotkeyManager;




};
