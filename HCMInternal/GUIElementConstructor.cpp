#include "pch.h"
#include "GUIElementConstructor.h"
#include "SettingsStateAndEvents.h"
#include "HotkeysEnum.h"
#include "IGUIElement.h"
#include "GUIElementEnum.h"

#include "GUISimpleButton.h"
#include "GUISimpleToggle.h"
#include "GUISpeedhack.h"
#include "GUIInvulnerability.h"
#include "GUIHeading.h"
#include "GUISubHeading.h"
#include "GUIHotkeyOnly.h"
#include "GUIAdvanceTicks.h"
#include "GUIRadioButton.h"
#include "GUIRadioGroup.h"
#include "GUIFloat.h"
#include "GUIToggleWithChildren.h"
#include "GUIInputString.h"
#include "GUIInputMultilineString.h"
#include "GUIInputDWORD.h"
#include "GUIColourPicker.h"
#include "GUIColourPickerAlpha.h"
#include "GUIInputInt.h"
#include "GUIComboEnum.h"
#include "GUIComboEnumWithChildren.h"
#include "GUIDummyContainer.h"
#include "GUIVec3.h"
#include "GUIVec2.h"
#include "GUIButtonAndInt.h"
#include "GUIWaypointList.h"
#include "GUISkullToggle.h"
#include "GUILabel.h"
#include "GUIButtonAndBinaryInt.h"








class GUIElementConstructor::GUIElementConstructorImpl {
private:
	std::set<std::pair<GUIElementEnum, GameState::Value>> cacheFailedServices;

	std::optional<std::shared_ptr<IGUIElement>> createGUIElementAndStoreResult(GUIElementEnum guielementenum, GameState game, std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings, std::shared_ptr<RuntimeExceptionHandler> exp)
	{
		auto res = createGUIElement(guielementenum, game, guireq, fail, info, settings, exp);
		if (res.has_value()) mStore->mapOfSuccessfullyConstructedGUIElements.at(game).insert(guielementenum);
		return res;
	}

	std::optional<std::shared_ptr<IGUIElement>> createGUIElement(GUIElementEnum guielementenum, GameState game, std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings, std::shared_ptr<RuntimeExceptionHandler> exp)
	{
		if (!guireq->getSupportedGamesPerGUIElement().contains(guielementenum)) throw HCMInitException("getSupportedGamesPerGUIElement missing element, how did that even happen?");
		if (!guireq->getSupportedGamesPerGUIElement().at(guielementenum).contains(game))
		{
			PLOG_DEBUG << "GUIElementEnum::" << magic_enum::enum_name(guielementenum) << " does not support Game::" << game.toString() << ", skipping construction";
			return std::nullopt; // don't construct for unsupported game
		}

		if (cacheFailedServices.contains(std::make_pair(guielementenum, game))) // ignore elements that have already failed construction
		{
			PLOG_DEBUG << "GUIElementEnum::" << magic_enum::enum_name(guielementenum) << " for Game::" << game.toString() << " already failed construction, skipping construction";
			return std::nullopt;
		}


		// stupid override to make core save/dump stuff only construct for steam since I can't be bothered reworking the macro system to do this properly
		if ((magic_enum::enum_name<GUIElementEnum>(guielementenum).contains("core") || magic_enum::enum_name<GUIElementEnum>(guielementenum).contains("Core")) // is the gui elemenet a core save thing? this is the most icky part of this, doing a stringcheck of the enum name.. what if I want to add another cheat that uses the name "core" to mean something else? eh prolly won't come up
			&& game.operator GameState::Value() == GameState::Value::Halo1 // and is halo 1
			&& mProcType == MCCProcessType::WinStore // and current process is winstore
			)
		{
			PLOG_DEBUG << "GUIElementEnum::" << magic_enum::enum_name(guielementenum) << " for game " << game.toString() << " does not support WinStore version of MCC (steam only), skipping construction";
			return std::nullopt; // don't construct 
		}


		PLOG_DEBUG << "attempting to construct guielement: " << magic_enum::enum_name(guielementenum) << " for game: " << game.toString();
		try 
		{
			// check for failed required services
			if (guireq->getRequiredServicesPerGUIElement().contains(guielementenum))
			{
				std::vector<HCMInitException> failures;
				for (auto reqSer : guireq->getRequiredServicesPerGUIElement().at(guielementenum))
				{
					auto info = fail->getOptionalCheatInfo({ game, reqSer });
					if (info.init == false && info.ex.has_value()) failures.push_back(info.ex.value());
				}

				if (!failures.empty())
				{
					info->addFailure(guielementenum, game, failures);

					// TODO: get rid of this
					PLOG_ERROR << "ERROR: ";
					for (auto& fail : failures)
					{
						PLOG_ERROR << fail.what();
					}

					return std::nullopt;
				}
			}


			typedef  std::vector<std::optional<std::shared_ptr<IGUIElement>>> headerChildElements;
			typedef  std::vector<std::vector<std::optional<std::shared_ptr<IGUIElement>>>> vectorOfHeaderChildElements;
#define createNestedElement(elementEnum) createGUIElementAndStoreResult(elementEnum, game, guireq, fail, info, settings, exp) // optional nested element, recursively calls this function




			// ALL GUI ELEMENTS MUST HAVE A CASE HERE, TOP LEVEL OR NOT
			std::optional<std::shared_ptr<IGUIElement>> x;
			switch (guielementenum)
			{
			case GUIElementEnum::controlHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, ToolTipCollection("General HCM options"), "Control", headerChildElements
						{
						createNestedElement(GUIElementEnum::toggleGUIHotkeyGUI),
						createNestedElement(GUIElementEnum::messagesFontSize),
						createNestedElement(GUIElementEnum::messagesFontColor),
						createNestedElement(GUIElementEnum::GUISettingsSubheading),
						createNestedElement(GUIElementEnum::togglePauseGUI),
						createNestedElement(GUIElementEnum::togglePauseSettingsSubheading),
						createNestedElement(GUIElementEnum::showGUIFailuresGUI),
						createNestedElement(GUIElementEnum::OBSBypassToggleGUI),
						createNestedElement(GUIElementEnum::HideWatermarkGUI),
						}));

				case GUIElementEnum::toggleGUIHotkeyGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHotkeyOnly<63>>
						(game, ToolTipCollection("Brings up or collapses this main GUI window"), RebindableHotkeyEnum::toggleGUI, "Toggle this GUI"));

				case GUIElementEnum::messagesFontSize:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam(6.f, 120.f)>>
						(game, ToolTipCollection("Font size of messages that show up under this window"), "Message Font Size", settings->messagesFontSize));


				case GUIElementEnum::messagesFontColor:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPicker<true>>
						(game, ToolTipCollection("Color of messages that show up under this window"), "Messages Font Color", settings->messagesFontColor));


				case GUIElementEnum::GUISettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Options related to this main GUI"), "GUI settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::GUIShowingFreesCursor),
							createNestedElement(GUIElementEnum::GUIShowingBlocksInput),
							createNestedElement(GUIElementEnum::GUIShowingPausesGame),
							}));

					case GUIElementEnum::GUIShowingFreesCursor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically free the curson when this GUI is open"), std::nullopt, "Free cursor when GUI open", settings->GUIShowingFreesCursor));

					case GUIElementEnum::GUIShowingBlocksInput:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically block game input when this GUI is open"), std::nullopt, "Block input when GUI open", settings->GUIShowingBlocksInput));

					case GUIElementEnum::GUIShowingPausesGame:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically pause the game when this GUI is open"), std::nullopt, "Pause game when GUI open", settings->GUIShowingPausesGame));

				case GUIElementEnum::togglePauseGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Pauses the game simulation"), RebindableHotkeyEnum::togglePause, "Pause game", settings->togglePause));

				case GUIElementEnum::togglePauseSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Settings for pausing the game"), "Pause settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::advanceTicksGUI),
							createNestedElement(GUIElementEnum::pauseAlsoBlocksInputGUI),
							createNestedElement(GUIElementEnum::pauseAlsoFreesCursorGUI),
							}));


				case GUIElementEnum::advanceTicksGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIAdvanceTicks<true>>
						(game, ToolTipCollection("Advances the game simulation by x ticks while the game is paused"), RebindableHotkeyEnum::advanceTicks, settings->advanceTicksEvent, settings->advanceTicksCount));

					case GUIElementEnum::pauseAlsoBlocksInputGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Makes this pause feature also block game input"), std::nullopt, "Pause also blocks game input", settings->pauseAlsoBlocksInput));

					case GUIElementEnum::pauseAlsoFreesCursorGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Makes this pause feature also free MCC's cursor"), std::nullopt, "Pause also frees cursor", settings->pauseAlsoFreesCursor));

				case GUIElementEnum::showGUIFailuresGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, ToolTipCollection("Shows a modal list of optional cheat services that failed to initialise, sorted by game"), std::nullopt, "Show optional cheat service failures", settings->showGUIFailures));

					


				case GUIElementEnum::OBSBypassToggleGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
						(game, ToolTipCollection("Causes HCMs GUI to not be captured by recording software like OBS"), std::nullopt, "Bypass OBS Capture", settings->OBSBypassToggle));


				case GUIElementEnum::HideWatermarkGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, false>>
						(game, ToolTipCollection("Hides the HCM Watermark when the GUI is not open"), std::nullopt, "Hide HCM Watermark", settings->hideWatermark, headerChildElements
							{
							createNestedElement(GUIElementEnum::HideWatermarkIncludeMessagesGUI),
							}));

				case GUIElementEnum::HideWatermarkIncludeMessagesGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
						(game, ToolTipCollection("Hides the HCM message log too."), std::nullopt, "Also hide HCM messages", settings->hideWatermarkHideMessages));


			case GUIElementEnum::saveManagementHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, ToolTipCollection("Features for managing your checkpoints (savestates)"), "Save Management", headerChildElements
						{ 
							createNestedElement(GUIElementEnum::forceCheckpointGUI),
							createNestedElement(GUIElementEnum::forceRevertGUI),
							createNestedElement(GUIElementEnum::forceDoubleRevertGUI),
							createNestedElement(GUIElementEnum::forceCoreSaveGUI),
							createNestedElement(GUIElementEnum::forceCoreLoadGUI),
							createNestedElement(GUIElementEnum::injectCheckpointGUI),
							createNestedElement(GUIElementEnum::injectCheckpointSettingsSubheading),
							createNestedElement(GUIElementEnum::dumpCheckpointGUI),
							createNestedElement(GUIElementEnum::dumpCheckpointSettingsSubheading),
							createNestedElement(GUIElementEnum::injectCoreGUI),
							createNestedElement(GUIElementEnum::injectCoreSettingsSubheading),
							createNestedElement(GUIElementEnum::dumpCoreGUI),
							createNestedElement(GUIElementEnum::dumpCoreSettingsSubheading),
							createNestedElement(GUIElementEnum::naturalCheckpointDisableGUI),
							createNestedElement(GUIElementEnum::forceFutureCheckpointGUI),
						}));

				case GUIElementEnum::forceCheckpointGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Forces a checkpoint, regardless of if the player is safe or not"), RebindableHotkeyEnum::forceCheckpoint, "Force Checkpoint", settings->forceCheckpointEvent));

				case GUIElementEnum::forceRevertGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Forces a revert (without having to die or go to the pause menu)"), RebindableHotkeyEnum::forceRevert, "Force Revert", settings->forceRevertEvent));

				case GUIElementEnum::forceDoubleRevertGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Reverts you back to the checkpoint BEFORE the last checkpoint you got"), RebindableHotkeyEnum::forceDoubleRevert, "Force Double Revert", settings->forceDoubleRevertEvent));

				case GUIElementEnum::forceCoreSaveGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Core saves are exactly like checkpoints, but seperate from them (they don't override eachother)"), RebindableHotkeyEnum::forceCoreSave, "Force Core Save", settings->forceCoreSaveEvent));

				case GUIElementEnum::forceCoreLoadGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Reverts to your core save"), RebindableHotkeyEnum::forceCoreLoad, "Force Core Load", settings->forceCoreLoadEvent));

				case GUIElementEnum::injectCheckpointGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Injects the selected checkpoint into the games memory in place of your last actual checkpoint"), RebindableHotkeyEnum::injectCheckpoint, "Inject Checkpoint", settings->injectCheckpointEvent));

				case GUIElementEnum::injectCheckpointSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection(""), "Inject Checkpoint Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::injectCheckpointForcesRevert),
							createNestedElement(GUIElementEnum::injectCheckpointLevelCheck),
							createNestedElement(GUIElementEnum::injectCheckpointVersionCheck),
							createNestedElement(GUIElementEnum::injectCheckpointDifficultyCheck),
							createNestedElement(GUIElementEnum::injectCheckpointIgnoresChecksum)
							}));

					case GUIElementEnum::injectCheckpointForcesRevert:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically revert to the checkpoint you injected when injecting"), std::nullopt, "Force revert after injecting", settings->injectCheckpointForcesRevert));

					case GUIElementEnum::injectCheckpointLevelCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("If you try to inject a checkpoint from a level that ISN'T from the one you're playing on, will warn you with an \"are you sure?\" popup"), std::nullopt, "Warn on injecting to wrong level", settings->injectCheckpointLevelCheck));

					case GUIElementEnum::injectCheckpointVersionCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("If you try to inject a checkpoint from an MCC version that ISN'T the same one you're playing on, will warn you with an \"are you sure?\" popup"), std::nullopt, "Warn on injecting to wrong game version", settings->injectCheckpointVersionCheck));

					case GUIElementEnum::injectCheckpointDifficultyCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("If you try to inject a checkpoint from a difficulty that ISN'T the same one you're playing on, will warn you with an \"are you sure?\" popup"), std::nullopt, "Warn on injecting to wrong difficulty", settings->injectCheckpointDifficultyCheck));

					case GUIElementEnum::injectCheckpointIgnoresChecksum:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Turns off the games checkpoint integrity check system, allowing you to inject checkpoints made by other players or from modified maps"), std::nullopt, "Ignore checkpoint checksum on revert", settings->injectionIgnoresChecksum));

				case GUIElementEnum::dumpCheckpointGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Dumps your current last checkpoint to the HCM checkpoint library for later use"), RebindableHotkeyEnum::dumpCheckpoint, "Dump Checkpoint", settings->dumpCheckpointEvent));

				case GUIElementEnum::dumpCheckpointSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection(""), "Dump Checkpoint Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::dumpCheckpointAutonameGUI),
							createNestedElement(GUIElementEnum::dumpCheckpointForcesSave)
							}));

					case GUIElementEnum::dumpCheckpointAutonameGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically name dumped checkpoints with a timestamp instead of prompting you to input a name"), std::nullopt, "Autoname checkpoints", settings->autonameCheckpoints));

					case GUIElementEnum::dumpCheckpointForcesSave:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically force a new checkpoint before dumping it"), std::nullopt, "Force checkpoint before dumping", settings->dumpCheckpointForcesSave));

				case GUIElementEnum::injectCoreGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Injects the selected core save into the games memory in place of your last actual core save"), RebindableHotkeyEnum::injectCore, "Inject Core Save", settings->injectCoreEvent));

				case GUIElementEnum::injectCoreSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection(""), "Inject Core Save Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::injectCoreForcesRevert),
							createNestedElement(GUIElementEnum::injectCoreLevelCheck),
							createNestedElement(GUIElementEnum::injectCoreVersionCheck),
							createNestedElement(GUIElementEnum::injectCoreDifficultyCheck),
							}));

					case GUIElementEnum::injectCoreForcesRevert:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically revert to the core save you injected when injecting"), std::nullopt, "Force core load after injecting", settings->injectCoreForcesRevert));

					case GUIElementEnum::injectCoreLevelCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("If you try to inject a core save from a level that ISN'T from the one you're playing on, will warn you with an \"are you sure?\" popup"), std::nullopt, "Warn on injecting to wrong level", settings->injectCoreLevelCheck));

					case GUIElementEnum::injectCoreVersionCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("If you try to inject a core save from an MCC version that ISN'T the same one you're playing on, will warn you with an \"are you sure?\" popup"), std::nullopt, "Warn on injecting to wrong game version", settings->injectCoreVersionCheck));

					case GUIElementEnum::injectCoreDifficultyCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("If you try to inject a core save from a difficulty that ISN'T the same one you're playing on, will warn you with an \"are you sure?\" popup"), std::nullopt, "Warn on injecting to wrong difficulty", settings->injectCoreDifficultyCheck));


				case GUIElementEnum::dumpCoreGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Dumps your current last core save to the HCM checkpoint library for later use"), RebindableHotkeyEnum::dumpCore, "Dump Core Save", settings->dumpCoreEvent));

				case GUIElementEnum::dumpCoreSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection(""), "Dump Core Save Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::dumpCoreAutonameGUI),
							createNestedElement(GUIElementEnum::dumpCoreForcesSave)
							}));

					case GUIElementEnum::dumpCoreAutonameGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically name dumped core saves with a timestamp instead of prompting you to input a name"), std::nullopt, "Autoname core saves", settings->autonameCoresaves));

					case GUIElementEnum::dumpCoreForcesSave:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically force a new core save before dumping it"), std::nullopt, "Force core save before dumping", settings->dumpCoreForcesSave));

				case GUIElementEnum::naturalCheckpointDisableGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Disables naturally occuring checkpoints (will break cutscenes)"), RebindableHotkeyEnum::naturalCheckpointDisable, "Disable Natural Checkpoints", settings->naturalCheckpointDisable));

				case GUIElementEnum::forceFutureCheckpointGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Useful for forcing checkpoints in coop, as to avoid a desync both players need to force a checkpoint on the exact same game tick"), "Force Future Checkpoint (co-op)", headerChildElements
							{
							createNestedElement(GUIElementEnum::forceFutureCheckpointToggle),
							createNestedElement(GUIElementEnum::forceFutureCheckpointTick),
							createNestedElement(GUIElementEnum::forceFutureCheckpointFill)
							}));

				case GUIElementEnum::forceFutureCheckpointToggle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
						(game, ToolTipCollection("When ticked, HCM will force a checkpoint on the below tick"), std::nullopt, "Force future checkpoint", settings->forceFutureCheckpointToggle));

				case GUIElementEnum::forceFutureCheckpointTick:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputInt<>>
						(game, ToolTipCollection("Which tick to force a checkpoint on (make sure you use the same value as your coop partner)"), "on tick:", settings->forceFutureCheckpointTick));

				case GUIElementEnum::forceFutureCheckpointFill:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, ToolTipCollection("Sets the tick for 5 seconds from now, giving you enough time to communicate with your coop partner which tick to use"), std::nullopt, "Set for 5s from now", settings->forceFutureCheckpointFillEvent));

					

			case GUIElementEnum::cheatsHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, ToolTipCollection("Stuff like invulnerabiltiy, speedhack, teleport, ai disable etc"), "Useful Cheats", headerChildElements
						{
							createNestedElement(GUIElementEnum::speedhackGUI),
							createNestedElement(GUIElementEnum::invulnGUI),
							createNestedElement(GUIElementEnum::invulnerabilitySettingsSubheading),
							createNestedElement(GUIElementEnum::infiniteAmmoGUI),
							createNestedElement(GUIElementEnum::bottomlessClipGUI),
							createNestedElement(GUIElementEnum::aiFreezeGUI),
							createNestedElement(GUIElementEnum::medusaGUI),
							createNestedElement(GUIElementEnum::forceTeleportGUI),
							createNestedElement(GUIElementEnum::forceTeleportSettingsSubheading),
							createNestedElement(GUIElementEnum::forceLaunchGUI),
							createNestedElement(GUIElementEnum::forceLaunchSettingsSubheading),
							createNestedElement(GUIElementEnum::switchBSPGUI),
							createNestedElement(GUIElementEnum::switchBSPSetGUI),
							createNestedElement(GUIElementEnum::setPlayerHealthSubheadingGUI),
							createNestedElement(GUIElementEnum::skullToggleGUI),
							createNestedElement(GUIElementEnum::playerPositionToClipboardGUI),
							createNestedElement(GUIElementEnum::consoleCommandGUI),
							createNestedElement(GUIElementEnum::consoleCommandSettings),
							createNestedElement(GUIElementEnum::disableBarriersToggle),
						}));

				case GUIElementEnum::speedhackGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISpeedhack>
						(game, ToolTipCollection("Multiply the games clock speed by this value (1 = normal speed). \nFor non-stuttery framerates at low speedhack values, make sure you're running MCC with uncapped framerate."), RebindableHotkeyEnum::speedhack, settings));

				case GUIElementEnum::invulnGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInvulnerability>
						(game, ToolTipCollection("Makes the player invulnerable to all damage and kill triggers"), RebindableHotkeyEnum::invuln, "Invulnerability", settings->invulnerabilityToggle
							)); 


				case GUIElementEnum::invulnerabilitySettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection(""), "InvulnerabilitySettings", headerChildElements
							{
								createNestedElement(GUIElementEnum::invulnNPCGUI),
							}));

					case GUIElementEnum::invulnNPCGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
								(game, ToolTipCollection("Makes all non-player characters invulnerable as well"), std::nullopt, "NPC's invulnerable too", settings->invulnerabilityNPCToggle
								));

				case GUIElementEnum::infiniteAmmoGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Gives the player infinite ammo (doesn't consume on reload) and infinite grenades"), RebindableHotkeyEnum::infiniteAmmo, "Infinite Ammo", settings->infiniteAmmoToggle
						));

				case GUIElementEnum::bottomlessClipGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("The player no longer has to reload"), RebindableHotkeyEnum::bottomlessClip, "Bottomless Clip", settings->bottomlessClipToggle
						));

				case GUIElementEnum::aiFreezeGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Disables NPC AI, causing them to stand around like puppets"), RebindableHotkeyEnum::aiFreeze, "Freeze AI", settings->aiFreezeToggle));

				case GUIElementEnum::medusaGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Automatically kill enemies when they see the player"), RebindableHotkeyEnum::medusa, "Medusa", settings->medusaToggle));


				case GUIElementEnum::forceTeleportGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Teleports an objects physical position"), RebindableHotkeyEnum::forceTeleport, "Force Teleport", settings->forceTeleportEvent));

				case GUIElementEnum::forceTeleportSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection(""), "Force Teleport Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::forceTeleportApplyToPlayer),
							createNestedElement(GUIElementEnum::forceTeleportSettingsRadioGroup),
							}));

					case GUIElementEnum::forceTeleportApplyToPlayer:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenFalse, false>>
							(game, ToolTipCollection("The teleport should be applied to the player, as opposed to some other object"), std::nullopt, "Apply to player", settings->forceTeleportApplyToPlayer, headerChildElements
								{
								createNestedElement(GUIElementEnum::forceTeleportCustomObject),
								}));

						case GUIElementEnum::forceTeleportCustomObject:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
								(game, ToolTipCollection("The datum (unique ID) of the object the teleport should be applied to"), "Custom object datum", settings->forceTeleportCustomObject));



					case GUIElementEnum::forceTeleportSettingsRadioGroup:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioGroup>
							(game, ToolTipCollection(""), "Force Teleport Radio Group", headerChildElements
								{
								createNestedElement(GUIElementEnum::forceTeleportForward),
								createNestedElement(GUIElementEnum::forceTeleportManual)
								}));


						case GUIElementEnum::forceTeleportForward:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioButton>
								(game, ToolTipCollection("Teleport relative to the players position and look-direction"), std::nullopt, "Teleport Relative to Player",   settings->forceTeleportForward, headerChildElements
									{
									createNestedElement(GUIElementEnum::forceTeleportRelativeVec3),
									createNestedElement(GUIElementEnum::forceTeleportForwardIgnoreZ)
									}));

							case GUIElementEnum::forceTeleportRelativeVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, true, 8>>
									(game, ToolTipCollection("How far forward/right/up to teleport the player, relative to their look-direction"), "Teleport: ##forceTeleportRelativeVec3", settings->forceTeleportRelativeVec3, "Forward", "Right", "Up"));


							case GUIElementEnum::forceTeleportForwardIgnoreZ:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection("Will ignore the vertical component of your look direction (ie pretends you're looking at the horizon)"), std::nullopt, "Ignore vertical look angle",  settings->forceTeleportForwardIgnoreZ));


						case GUIElementEnum::forceTeleportManual:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioButton>
								(game, ToolTipCollection("Teleport to absolute world coordinates"), std::nullopt, "Teleport to Manual Coordinates",  settings->forceTeleportManual, headerChildElements
									{
									createNestedElement(GUIElementEnum::forceTeleportAbsoluteVec3),
									createNestedElement(GUIElementEnum::forceTeleportAbsoluteFillCurrent),
									createNestedElement(GUIElementEnum::forceTeleportAbsoluteCopy),
									createNestedElement(GUIElementEnum::forceTeleportAbsolutePaste),
									}));

							case GUIElementEnum::forceTeleportAbsoluteVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, true, 8>>
									(game, ToolTipCollection("The xyz world coordinates to teleport to"), "Teleport: ##forceTeleportAbsoluteVec3", settings->forceTeleportAbsoluteVec3));


							case GUIElementEnum::forceTeleportAbsoluteFillCurrent:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Fill with the current xyz position of the player/object"), std::nullopt, "Fill with current position",  settings->forceTeleportAbsoluteFillCurrent));

							case GUIElementEnum::forceTeleportAbsoluteCopy:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Copy the position above to your clipboard"), std::nullopt, "Copy to Clipboard", settings->forceTeleportAbsoluteCopy));

							case GUIElementEnum::forceTeleportAbsolutePaste:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Paste a position from your clipboard into above"), std::nullopt, "Paste from Clipboard", settings->forceTeleportAbsolutePaste));



				case GUIElementEnum::forceLaunchGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Boosts an objects velocity"), RebindableHotkeyEnum::forceLaunch, "Force Launch", settings->forceLaunchEvent));

				case GUIElementEnum::forceLaunchSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection(""), "Force Launch Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::forceLaunchApplyToPlayer),
							createNestedElement(GUIElementEnum::forceLaunchSettingsRadioGroup),
							}));

					case GUIElementEnum::forceLaunchApplyToPlayer:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenFalse, false>>
							(game, ToolTipCollection("The launch should be applied to the player, as opposed to some other object"), std::nullopt, "Apply to player", settings->forceLaunchApplyToPlayer, headerChildElements
								{
								createNestedElement(GUIElementEnum::forceLaunchCustomObject)
								}));

						case GUIElementEnum::forceLaunchCustomObject:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
								(game, ToolTipCollection("The datum (unique ID) of the object the launch should be applied to"), "Custom object datum", settings->forceLaunchCustomObject));


					case GUIElementEnum::forceLaunchSettingsRadioGroup:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioGroup>
							(game, ToolTipCollection(""), "Force Launch Radio Group", headerChildElements
								{
								createNestedElement(GUIElementEnum::forceLaunchForward),
								createNestedElement(GUIElementEnum::forceLaunchManual)
								}));


						case GUIElementEnum::forceLaunchForward:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioButton>
								(game, ToolTipCollection("Add velocity relative to the players look-direction"), std::nullopt, "Launch relative to player facing",  settings->forceLaunchForward, headerChildElements
									{
									createNestedElement(GUIElementEnum::forceLaunchRelativeVec3),
									createNestedElement(GUIElementEnum::forceLaunchForwardIgnoreZ)
									}));

							case GUIElementEnum::forceLaunchRelativeVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, true, 8>>
									(game, ToolTipCollection("How much velocity to apply in the forward/right/up directions, relative to the players look-direction"), "Launch: ##forceLaunchRelativeVec3", settings->forceLaunchRelativeVec3, "Forward", "Right", "Up"));


							case GUIElementEnum::forceLaunchForwardIgnoreZ:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection("Will ignore the vertical component of your look direction (ie pretends you're looking at the horizon)"), std::nullopt, "Ignore vertical look angle", settings->forceLaunchForwardIgnoreZ));


						case GUIElementEnum::forceLaunchManual:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioButton>
								(game, ToolTipCollection("Add velocity relative to the absolute world-axes"), std::nullopt, "Launch with absolute velocity", settings->forceLaunchManual, headerChildElements
									{
									createNestedElement(GUIElementEnum::forceLaunchAbsoluteVec3),
									}));


							case GUIElementEnum::forceLaunchAbsoluteVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, true, 8>>
									(game, ToolTipCollection("How much velocity to add, in absolute world-axes"), "Launch: ##forceLaunchAbsoluteVec3", settings->forceLaunchAbsoluteVec3));

				case GUIElementEnum::switchBSPGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIButtonAndInt<true>>
						(game, ToolTipCollection("Loads a different part of the map by index"), RebindableHotkeyEnum::switchBSP, 
							((game.operator GameState::Value() == GameState::Value::Halo1 || game.operator GameState::Value() == GameState::Value::Halo2) ? "Switch BSP to" : "Switch Zone Set to"), 
							"Index",
							settings->switchBSPIndex, settings->switchBSPEvent));

				case GUIElementEnum::switchBSPSetGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection(""), "Switch BSP Set", headerChildElements
							{
								createNestedElement(GUIElementEnum::switchBSPSetLoadSet),
								createNestedElement(GUIElementEnum::switchBSPSetFillCurrent),
								createNestedElement(GUIElementEnum::switchBSPSetLoadIndex),
								createNestedElement(GUIElementEnum::switchBSPSetUnloadIndex),
							}));

				case GUIElementEnum::switchBSPSetLoadSet:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIButtonAndBinaryInt<true>>
						(game, ToolTipCollection("Loads a set of BSPs by binary index"), RebindableHotkeyEnum::switchBSPSet,
							 "Switch BSP Binary Set",
							"Set",
							settings->switchBSPSetLoadSet, settings->switchBSPSetLoadSetEvent));

				case GUIElementEnum::switchBSPSetFillCurrent:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, ToolTipCollection("Fills the above field with the binary value of the currently loaded BSPs"), std::nullopt,
							"Fill With Current BSP Set",
							settings->switchBSPSetFillCurrent));

				case GUIElementEnum::switchBSPSetLoadIndex:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIButtonAndInt<false>>
						(game, ToolTipCollection("Loads a BSP by index"), std::nullopt,
							"Load BSP index",
							"Index",
							settings->switchBSPSetLoadIndex, settings->switchBSPLoadIndexEvent));

				case GUIElementEnum::switchBSPSetUnloadIndex:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIButtonAndInt<false>>
						(game, ToolTipCollection("Unloads a BSP by index"), std::nullopt,
							"Unload BSP index",
							"Index",
							settings->switchBSPSetUnloadIndex, settings->switchBSPUnloadIndexEvent));


				case GUIElementEnum::setPlayerHealthSubheadingGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Directly set the players health and shields"), "Set Player Health", headerChildElements
							{
							createNestedElement(GUIElementEnum::setPlayerHealthGUI),
							createNestedElement(GUIElementEnum::setPlayerHealthValueGUI),
							},
							0.f));

				case GUIElementEnum::setPlayerHealthGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Directly set the players health and shields"), RebindableHotkeyEnum::setPlayerHealth,
							"Set Player Health",
							settings->setPlayerHealthEvent));

				case GUIElementEnum::setPlayerHealthValueGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec2<true, false, 3>>
						(game, ToolTipCollection("Directly set the players health and shields"),
							 "setPlayerHealthValue",
							settings->setPlayerHealthVec2,
							"Health ##setPlayerHealthValue",
							"Shields##setPlayerHealthValue"));

				case GUIElementEnum::skullToggleGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISkullToggle<false>>
						(game, ToolTipCollection("Set skull state. Note that skull state is saved/loaded by checkpoints"),
							"Skull Toggles",
							settings));

				case GUIElementEnum::playerPositionToClipboardGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, ToolTipCollection("Directly set the players health and shields"), std::nullopt,
							"Copy Position to Clipboard",
							settings->playerPositionToClipboardEvent));

				case GUIElementEnum::consoleCommandGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, ToolTipCollection("Brings up the command console, allowing you to send engine commands"), RebindableHotkeyEnum::commandConsoleHotkey,
							"Command Console", settings->commandConsoleHotkeyEvent));

				case GUIElementEnum::consoleCommandSettings:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Settings for the command console"), "Command Console Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::consoleCommandPauseGame),
							createNestedElement(GUIElementEnum::consoleCommandBlockInput),
							createNestedElement(GUIElementEnum::consoleCommandFreeCursor),
							createNestedElement(GUIElementEnum::consoleCommandFontSize),
							createNestedElement(GUIElementEnum::consoleCommandExecuteBuffer),
							}));

					case GUIElementEnum::consoleCommandPauseGame:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically pause the game when the command console is open"), std::nullopt, "Pause game when Console open", settings->consoleCommandPauseGame));


					case GUIElementEnum::consoleCommandBlockInput:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically block game input when the command console is open"), std::nullopt, "Block input when Console open", settings->consoleCommandBlockInput));

					case GUIElementEnum::consoleCommandFreeCursor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Automatically free cursor when the command console is open"), std::nullopt, "Free cursor when Console open", settings->consoleCommandFreeCursor));


					case GUIElementEnum::consoleCommandFontSize:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(6.f, 50.f)>>
							(game, ToolTipCollection(""), "Font Size##Console", settings->consoleCommandFontSize));


					case GUIElementEnum::consoleCommandExecuteBuffer:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
							(game, ToolTipCollection("Execute the command currently in the console (without clearing the console)"), RebindableHotkeyEnum::commandConsoleExecuteBuffer,
								"Execute Command",
								settings->commandConsoleExecuteBufferEvent));
	
				case GUIElementEnum::disableBarriersToggle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Disable Barriers (aka Soft Ceilings)"), RebindableHotkeyEnum::disableBarriers, "Disable Barriers", settings->disableBarriersToggle));

			case GUIElementEnum::overlaysHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, ToolTipCollection("Overlays that various information over the top of the game view"), "Overlays", headerChildElements
						{ 
							createNestedElement(GUIElementEnum::renderDistance3DGUI),
							createNestedElement(GUIElementEnum::display2DInfoToggleGUI),
							createNestedElement(GUIElementEnum::display2DInfoSettingsInfoSubheading),
							createNestedElement(GUIElementEnum::display2DInfoSettingsVisualSubheading),
							createNestedElement(GUIElementEnum::waypoint3DGUIToggle),
							createNestedElement(GUIElementEnum::waypoint3DGUIList),
							createNestedElement(GUIElementEnum::waypoint3DGUISettings),
							createNestedElement(GUIElementEnum::triggerOverlayToggle),
							createNestedElement(GUIElementEnum::triggerOverlaySettings),
							createNestedElement(GUIElementEnum::softCeilingOverlayToggle),
							createNestedElement(GUIElementEnum::softCeilingOverlaySettings),
							//createNestedElement(GUIElementEnum::shieldInputPrinterToggle),
							createNestedElement(GUIElementEnum::sensDriftDetectorToggle),
						}));


				case GUIElementEnum::renderDistance3DGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(0.1f, 1000.f, ImGuiSliderFlags_None)>>
						(game, ToolTipCollection("How far objects render when using 3D overlays eg Trigger Overlay"), "3D Render Distance", settings->renderDistance3D));


				case GUIElementEnum::display2DInfoToggleGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Displays various player and game information as text on your screen"), RebindableHotkeyEnum::display2DInfo, "Display 2D Game Info", settings->display2DInfoToggle));

				case GUIElementEnum::display2DInfoSettingsInfoSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Control what information should be displayed"), "Info Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::display2DInfoShowGameTick),
							createNestedElement(GUIElementEnum::display2DInfoShowAggro),
							createNestedElement(GUIElementEnum::display2DInfoShowRNG),
							createNestedElement(GUIElementEnum::display2DInfoShowBSP),
							createNestedElement(GUIElementEnum::display2DInfoShowBSPSet),
							createNestedElement(GUIElementEnum::display2DInfoShowNextObjectDatum),
							createNestedElement(GUIElementEnum::display2DInfoTrackPlayer),
							createNestedElement(GUIElementEnum::display2DInfoTrackCustomObject),
							}));

					case GUIElementEnum::display2DInfoShowGameTick:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection(""), std::nullopt, "Show Game Tickcounter", settings->display2DInfoShowGameTick));




					case GUIElementEnum::display2DInfoShowAggro:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection(""), std::nullopt, "Show Aggro info", settings->display2DInfoShowAggro));

					case GUIElementEnum::display2DInfoShowRNG:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection(""), std::nullopt, "Show RNG seed", settings->display2DInfoShowRNG));


					case GUIElementEnum::display2DInfoShowBSP:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection(""), std::nullopt, 
								("Show BSP Index"),
								settings->display2DInfoShowBSP));

					case GUIElementEnum::display2DInfoShowBSPSet:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection(""), std::nullopt,
								("Show BSP Set"),
								settings->display2DInfoShowBSPSet));


					case GUIElementEnum::display2DInfoShowNextObjectDatum:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Shows the datum (salt + index) of the next object to spawn. Useful for Halo 2's Arbitary-unit-possession trick."), std::nullopt, "Show Next Object Datum", settings->display2DInfoShowNextObjectDatum));

					case GUIElementEnum::display2DInfoTrackPlayer:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, false>>
							(game, ToolTipCollection(""), std::nullopt, "Track Player Entity", settings->display2DInfoTrackPlayer, headerChildElements
								{
									createNestedElement(GUIElementEnum::display2DInfoShowPlayerViewAngle),
									createNestedElement(GUIElementEnum::display2DInfoShowPlayerPosition),
									createNestedElement(GUIElementEnum::display2DInfoShowPlayerVelocity),
									createNestedElement(GUIElementEnum::display2DInfoShowPlayerHealth),
								}
								));

						case GUIElementEnum::display2DInfoShowPlayerViewAngle:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Player View Angle", settings->display2DInfoShowPlayerViewAngle));



						case GUIElementEnum::display2DInfoShowPlayerPosition:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Position##player", settings->display2DInfoShowPlayerPosition));

						case GUIElementEnum::display2DInfoShowPlayerVelocity:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Velocity##player", settings->display2DInfoShowPlayerVelocity, headerChildElements
									{
										createNestedElement(GUIElementEnum::display2DInfoShowPlayerVelocityAbs),
										createNestedElement(GUIElementEnum::display2DInfoShowPlayerVelocityXY),
										createNestedElement(GUIElementEnum::display2DInfoShowPlayerVelocityXYZ),
									}));

							case GUIElementEnum::display2DInfoShowPlayerVelocityAbs:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "As absolute velocity##player", settings->display2DInfoShowPlayerVelocityAbs));

							case GUIElementEnum::display2DInfoShowPlayerVelocityXY:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "As XY magnitude##player", settings->display2DInfoShowPlayerVelocityXY));

							case GUIElementEnum::display2DInfoShowPlayerVelocityXYZ:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "As XYZ magnitude##player", settings->display2DInfoShowPlayerVelocityXYZ));

						case GUIElementEnum::display2DInfoShowPlayerHealth:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Health/Shields##player", settings->display2DInfoShowPlayerHealth, headerChildElements
									{
									createNestedElement(GUIElementEnum::display2DInfoShowPlayerRechargeCooldown),
									createNestedElement(GUIElementEnum::display2DInfoShowPlayerVehicleHealth),
									}
									));

						case GUIElementEnum::display2DInfoShowPlayerRechargeCooldown:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Recharge Cooldown##entity", settings->display2DInfoShowPlayerRechargeCooldown));

							case GUIElementEnum::display2DInfoShowPlayerVehicleHealth:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "Show Vehicle's Health/Shields##player", settings->display2DInfoShowPlayerVehicleHealth));




					case GUIElementEnum::display2DInfoTrackCustomObject:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, false>>
							(game, ToolTipCollection(""), std::nullopt, "Track Custom Entity", settings->display2DInfoTrackCustomObject, headerChildElements
							{
							createNestedElement(GUIElementEnum::display2DInfoCustomObjectDatum),
							createNestedElement(GUIElementEnum::display2DInfoShowEntityObjectType),
							createNestedElement(GUIElementEnum::display2DInfoShowEntityTagName),
							createNestedElement(GUIElementEnum::display2DInfoShowEntityPosition),
							createNestedElement(GUIElementEnum::display2DInfoShowEntityVelocity),
							createNestedElement(GUIElementEnum::display2DInfoShowEntityHealth),
							}
							));

						case GUIElementEnum::display2DInfoCustomObjectDatum:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
								(game, ToolTipCollection("Datum of custom object to track"), "Custom entity datum: ", settings->display2DInfoCustomObjectDatum));


						case GUIElementEnum::display2DInfoShowEntityObjectType:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Object Type##entity", settings->display2DInfoShowEntityObjectType));

						case GUIElementEnum::display2DInfoShowEntityTagName:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Tag Name##entity", settings->display2DInfoShowEntityTagName));

						case GUIElementEnum::display2DInfoShowEntityPosition:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Position##entity", settings->display2DInfoShowEntityPosition));

						case GUIElementEnum::display2DInfoShowEntityVelocity:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Velocity##entity", settings->display2DInfoShowEntityVelocity, headerChildElements
									{
										createNestedElement(GUIElementEnum::display2DInfoShowEntityVelocityAbs),
										createNestedElement(GUIElementEnum::display2DInfoShowEntityVelocityXY),
										createNestedElement(GUIElementEnum::display2DInfoShowEntityVelocityXYZ),
									}));

							case GUIElementEnum::display2DInfoShowEntityVelocityAbs:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "As absolute velocity##entity", settings->display2DInfoShowEntityVelocityAbs));

							case GUIElementEnum::display2DInfoShowEntityVelocityXY:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "As XY magnitude##entity", settings->display2DInfoShowEntityVelocityXY));

							case GUIElementEnum::display2DInfoShowEntityVelocityXYZ:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "As XYZ magnitude##entity", settings->display2DInfoShowEntityVelocityXYZ));

						case GUIElementEnum::display2DInfoShowEntityHealth:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, false>>
								(game, ToolTipCollection(""), std::nullopt, "Show Health/Shields##entity", settings->display2DInfoShowEntityHealth, headerChildElements
							{
								createNestedElement(GUIElementEnum::display2DInfoShowEntityRechargeCooldown),
								createNestedElement(GUIElementEnum::display2DInfoShowEntityVehicleHealth),
							}
							));

							case GUIElementEnum::display2DInfoShowEntityRechargeCooldown:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "Show Recharge Cooldown##entity", settings->display2DInfoShowEntityRechargeCooldown));

							case GUIElementEnum::display2DInfoShowEntityVehicleHealth:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, ToolTipCollection(""), std::nullopt, "Show Vehicle's Health/Shields##entity", settings->display2DInfoShowEntityVehicleHealth));




				case GUIElementEnum::display2DInfoSettingsVisualSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Cosmetic/visual settings for how the info should be displayed"), "Visual Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::display2DInfoAnchorCorner),
							createNestedElement(GUIElementEnum::display2DInfoScreenOffset),
							createNestedElement(GUIElementEnum::display2DInfoFontSize),
							createNestedElement(GUIElementEnum::display2DInfoFontColour),
							createNestedElement(GUIElementEnum::display2DInfoFloatPrecision),
							createNestedElement(GUIElementEnum::display2DInfoOutline),
							}));

					case GUIElementEnum::display2DInfoAnchorCorner:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnum<SettingsEnums::Display2DInfoAnchorEnum, 100.f>>
							(game, ToolTipCollection(""), "Corner to Anchor to", settings->display2DInfoAnchorCorner));

					case GUIElementEnum::display2DInfoScreenOffset:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared < GUIVec2 < false, false, 0, SliderParam<SimpleMath::Vector2>({0.f, 0.f}, {1600.f, 1000.f}, ImGuiSliderFlags_None) >>
							(game, ToolTipCollection(""), "Pixel Offset from Corner", settings->display2DInfoScreenOffset, "Horizontal", "Vertical"));


					case GUIElementEnum::display2DInfoFontSize:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(6.f, 120.f)>>
							(game, ToolTipCollection(""),  "Info Font Size", settings->display2DInfoFontSize));

					case GUIElementEnum::display2DInfoFontColour:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<true>>
							(game, ToolTipCollection(""),  "Colour#2dinfo", settings->display2DInfoFontColour));

					case GUIElementEnum::display2DInfoFloatPrecision:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputInt<SliderParam<int>(0, 10)>>
							(game, ToolTipCollection(""), "Info Decimal Precision", settings->display2DInfoFloatPrecision));

					case GUIElementEnum::display2DInfoOutline:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Adds a black outline to text (has a small negative performance impact)"), std::nullopt, "Info Font Outline", settings->display2DInfoOutline));

				case GUIElementEnum::waypoint3DGUIToggle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Toggles 3D waypoints"), RebindableHotkeyEnum::toggleWaypoint3D, "Custom Waypoints", settings->waypoint3DToggle));

				case GUIElementEnum::waypoint3DGUIList:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIWaypointList<false>>
						(game, ToolTipCollection("List of 3D Waypoints"), settings->waypoint3DList, settings->editWaypointEvent, settings->deleteWaypointEvent, settings->addWaypointEvent, exp));


				case GUIElementEnum::waypoint3DGUISettings:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Custom Waypoint Settings"), "Custom Waypoint Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::waypoint3DGUIClampToggle),
							createNestedElement(GUIElementEnum::waypoint3DGUIGlobalSpriteColor),
							createNestedElement(GUIElementEnum::waypoint3DGUIGlobalSpriteScale),
							createNestedElement(GUIElementEnum::waypoint3DGUIGlobalLabelColor),
							createNestedElement(GUIElementEnum::waypoint3DGUIGlobalLabelScale),
							createNestedElement(GUIElementEnum::waypoint3DGUIGlobalDistanceColor),
							createNestedElement(GUIElementEnum::waypoint3DGUIGlobalDistanceScale),
							createNestedElement(GUIElementEnum::waypoint3DGUIGlobalDistancePrecision),
							}));


					case GUIElementEnum::waypoint3DGUIClampToggle:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Clamps waypoints to the edge of the screen when you turn away from them"), std::nullopt, "Clamp to Screen", settings->waypoint3DClampToggle));


					case GUIElementEnum::waypoint3DGUIGlobalSpriteColor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<false>>
							(game, ToolTipCollection("Color to apply to waypoint sprite"), "Global Sprite Color", settings->waypoint3DGlobalSpriteColor));

					case GUIElementEnum::waypoint3DGUIGlobalSpriteScale:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(0.f, 10.f)>>
							(game, ToolTipCollection("Scaling factor of waypoint sprite"), "Global Sprite Scale", settings->waypoint3DGlobalSpriteScale));

					case GUIElementEnum::waypoint3DGUIGlobalLabelColor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<false>>
							(game, ToolTipCollection("Color to apply to waypoint label"), "Global Label Color", settings->waypoint3DGlobalLabelColor));

					case GUIElementEnum::waypoint3DGUIGlobalLabelScale:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(6.f, 120.f)>>
							(game, ToolTipCollection("Scaling factor of waypoint label"), "Global Label Scale", settings->waypoint3DGlobalLabelScale));

					case GUIElementEnum::waypoint3DGUIGlobalDistanceColor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<false>>
							(game, ToolTipCollection("Color to apply to waypoint distance text"), "Global Distance Color", settings->waypoint3DGlobalDistanceColor));

					case GUIElementEnum::waypoint3DGUIGlobalDistanceScale:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(6.f, 120.f)>>
							(game, ToolTipCollection("Scaling factor of waypoint distance text"), "Global Distance Scale", settings->waypoint3DGlobalDistanceScale));

					case GUIElementEnum::waypoint3DGUIGlobalDistancePrecision:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputInt<SliderParam<int>(0, 10)>>
							(game, ToolTipCollection("Decimal precision of waypoint distance text"), "Global Distance Precision", settings->waypoint3DGlobalDistancePrecision));


						

				case GUIElementEnum::triggerOverlayToggle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection("Overlays trigger volumes onto the screen"), RebindableHotkeyEnum::triggerOverlayToggleHotkey, "Trigger Overlay", settings->triggerOverlayToggle));

				case GUIElementEnum::triggerOverlaySettings:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Settings for the trigger overlay"), "Trigger Overlay Settings", headerChildElements
							{
								createNestedElement(GUIElementEnum::triggerOverlayFilterToggle),
								createNestedElement(GUIElementEnum::triggerOverlayRenderStyle),
								createNestedElement(GUIElementEnum::triggerOverlayInteriorStyle),
								createNestedElement(GUIElementEnum::triggerOverlayLabelStyle),
								createNestedElement(GUIElementEnum::triggerOverlayLabelScale),
								createNestedElement(GUIElementEnum::triggerOverlayNormalColor),
								createNestedElement(GUIElementEnum::triggerOverlayBSPColor),
								createNestedElement(GUIElementEnum::triggerOverlaySectorColor),
								createNestedElement(GUIElementEnum::triggerOverlayAlpha),
								createNestedElement(GUIElementEnum::triggerOverlayWireframeAlpha),
								createNestedElement(GUIElementEnum::triggerOverlayCheckHitToggle),
								createNestedElement(GUIElementEnum::triggerOverlayCheckMissToggle),
								createNestedElement(GUIElementEnum::triggerOverlayMessageOnEnter),
								createNestedElement(GUIElementEnum::triggerOverlayMessageOnExit),
								createNestedElement(GUIElementEnum::triggerOverlayMessageOnCheckHit),
								createNestedElement(GUIElementEnum::triggerOverlayMessageOnCheckMiss),
								createNestedElement(GUIElementEnum::triggerOverlayPositionToggle),
							}));

				case GUIElementEnum::triggerOverlayFilterToggle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::AlwaysShowChildren, false>>
						(game, ToolTipCollection("Filter which triggers to show by name"), std::nullopt, "Filter Triggers by Name", settings->triggerOverlayFilterToggle, headerChildElements
							{
							createNestedElement(GUIElementEnum::triggerOverlayFilterStringDialog),
							createNestedElement(GUIElementEnum::triggerOverlayFilterCountLabel),
							createNestedElement(GUIElementEnum::triggerOverlayFilterStringCopy),
							createNestedElement(GUIElementEnum::triggerOverlayFilterStringPaste),
							}));

				case GUIElementEnum::triggerOverlayFilterStringDialog:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, ToolTipCollection("Which triggers should show up. Names should be semicolon seperated."), std::nullopt, "Edit Filter Name Whitelist", settings->triggerOverlayFilterStringDialogEvent));


				case GUIElementEnum::triggerOverlayFilterCountLabel:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUILabel<int>>
						(game, ToolTipCollection("How many trigger names are in the filter list."), "Filter Name Count: {}", settings->triggerOverlayFilterCountLabel));


				case GUIElementEnum::triggerOverlayFilterStringCopy:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, ToolTipCollection("Copy the filter list to the clipboard"), std::nullopt, "Copy Filter List", settings->triggerOverlayFilterStringCopyEvent));

				case GUIElementEnum::triggerOverlayFilterStringPaste:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, ToolTipCollection("Paste the filter list from the clipboard"), std::nullopt, "Paste Filter List", settings->triggerOverlayFilterStringPasteEvent));

				case GUIElementEnum::triggerOverlayRenderStyle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnum<SettingsEnums::TriggerRenderStyle, 150.f>>
						(game, ToolTipCollection("What style to render trigger volumes as"), "Render trigger volumes as: ", settings->triggerOverlayRenderStyle));

				case GUIElementEnum::triggerOverlayInteriorStyle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnum<SettingsEnums::TriggerInteriorStyle, 100.f>>
						(game, ToolTipCollection("What style to render trigger volumes as when the camera is inside them"), "Render Solid Interior as: ", settings->triggerOverlayInteriorStyle));

				case GUIElementEnum::triggerOverlayLabelStyle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnum<SettingsEnums::TriggerLabelStyle, 100.f>>
						(game, ToolTipCollection("Render Labels (Trigger names) at center of volume, corner, or not at all."), "Render Labels at: ", settings->triggerOverlayLabelStyle));

				case GUIElementEnum::triggerOverlayLabelScale:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(6.f, 120.f)>>
						(game, ToolTipCollection("How large the font size of the label should be, in pts"), "Label Font Size", settings->triggerOverlayLabelScale));


				case GUIElementEnum::triggerOverlayNormalColor:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<true>>
						(game, ToolTipCollection("Color to render triggers when not checked or BSP"), "Trigger Normal Colour", settings->triggerOverlayNormalColor));

				case GUIElementEnum::triggerOverlayBSPColor:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<true>>
						(game, ToolTipCollection("Color to render BSP (loadzone) triggers"), "Trigger BSP Colour", settings->triggerOverlayBSPColor));

				case GUIElementEnum::triggerOverlaySectorColor:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<true>>
						(game, ToolTipCollection("Color to render Sector-type triggers"), "Trigger Sector Colour", settings->triggerOverlaySectorColor));

				case GUIElementEnum::triggerOverlayAlpha:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(0.1, 1.f)>>
						(game, ToolTipCollection("Multiplies how opaque triggers are, from 0 (fully transparent) to 1 (fully opaque)"), "Trigger Opacity Multiplier", settings->triggerOverlayAlpha));

				case GUIElementEnum::triggerOverlayWireframeAlpha:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam<float>(0.1, 1.f)>>
						(game, ToolTipCollection("Multiplies how opaque trigger wireframes are, from 0 (fully transparent) to 1 (fully opaque)"), "Wireframe Opacity Multiplier", settings->triggerOverlayWireframeAlpha));


				case GUIElementEnum::triggerOverlayCheckHitToggle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::AlwaysShowChildren, false>>
						(game, ToolTipCollection("Whether to flash triggers to a certain colour when the script checks them for the players presence"), std::nullopt, "Flash Triggers on Successful Check", settings->triggerOverlayCheckHitToggle, headerChildElements
							{
							createNestedElement(GUIElementEnum::triggerOverlayCheckHitFalloff),
							createNestedElement(GUIElementEnum::triggerOverlayCheckHitColor),
							}));

					case GUIElementEnum::triggerOverlayCheckHitFalloff:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputInt<>>
							(game, ToolTipCollection("How many seconds does the check-flash last"), "Flash for x ticks", settings->triggerOverlayCheckHitFalloff));

					case GUIElementEnum::triggerOverlayCheckHitColor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<true>>
							(game, ToolTipCollection("Color to flash trigger when it successfully checks for the player"), "Colour on Successful Check", settings->triggerOverlayCheckHitColor));

				case GUIElementEnum::triggerOverlayCheckMissToggle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::AlwaysShowChildren, false>>
						(game, ToolTipCollection("Whether to flash triggers to a certain colour when the script checks them for the players presence"), std::nullopt, "Flash Triggers on Failed Check", settings->triggerOverlayCheckMissToggle, headerChildElements
							{
							createNestedElement(GUIElementEnum::triggerOverlayCheckMissFalloff),
							createNestedElement(GUIElementEnum::triggerOverlayCheckMissColor),
							}));

					case GUIElementEnum::triggerOverlayCheckMissFalloff:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputInt<>>
							(game, ToolTipCollection("How many seconds does the check-flash last"), "Flash for x ticks", settings->triggerOverlayCheckMissFalloff));

					case GUIElementEnum::triggerOverlayCheckMissColor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPickerAlpha<true>>
							(game, ToolTipCollection("Color to flash trigger when it successfully checks for the player"), "Colour on Failed Check", settings->triggerOverlayCheckMissColor));



				case GUIElementEnum::triggerOverlayMessageOnEnter:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
						(game, ToolTipCollection("Prints a message to the screen when the player enters a trigger"), std::nullopt, "Message on Entering Trigger", settings->triggerOverlayMessageOnEnter));

				case GUIElementEnum::triggerOverlayMessageOnExit:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
						(game, ToolTipCollection("Prints a message to the screen when the player exits a trigger"), std::nullopt, "Message on Exiting Trigger", settings->triggerOverlayMessageOnExit));

				case GUIElementEnum::triggerOverlayMessageOnCheckHit:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
						(game, ToolTipCollection("Prints a message to the screen when a trigger successfully checks for the player"), std::nullopt, "Message on Successful Trigger Check", settings->triggerOverlayMessageOnCheckHit));


				case GUIElementEnum::triggerOverlayMessageOnCheckMiss:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
						(game, ToolTipCollection("Prints a message to the screen when a trigger failfully checks for the player"), std::nullopt, "Message on Failed Trigger Check", settings->triggerOverlayMessageOnCheckMiss));

				case GUIElementEnum::triggerOverlayPositionToggle:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::AlwaysShowChildren, false>>
						(game, ToolTipCollection("Draws a sphere at the vertex of the player model that activates triggers"), std::nullopt, "Show Player Trigger Vertex", settings->triggerOverlayPositionToggle, headerChildElements
							{
							createNestedElement(GUIElementEnum::triggerOverlayPositionColor),
							createNestedElement(GUIElementEnum::triggerOverlayPositionScale),
							createNestedElement(GUIElementEnum::triggerOverlayPositionWireframe),
							}));

					case GUIElementEnum::triggerOverlayPositionColor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPicker<true>>
							(game, ToolTipCollection("Color of the sphere drawn at the players trigger vertex"), "Trigger Vertex Color", settings->triggerOverlayPositionColor));

					case GUIElementEnum::triggerOverlayPositionScale:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam(0.01f, 4.f)>>
							(game, ToolTipCollection("Size of the sphere drawn at the players trigger vertex"), "Trigger Vertex Scale", settings->triggerOverlayPositionScale));

					case GUIElementEnum::triggerOverlayPositionWireframe:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Draw the player-trigger-vertex sphere as wireframe instead of solid"), std::nullopt, "Trigger Vertex Wireframe", settings->triggerOverlayPositionWireframe));


			case GUIElementEnum::softCeilingOverlayToggle:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
					(game, ToolTipCollection("Renders Soft Ceilings (barriers)"), RebindableHotkeyEnum::softCeilingOverlayToggleHotkey, "Barrier Overlay", settings->softCeilingOverlayToggle));

			case GUIElementEnum::softCeilingOverlaySettings:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
					(game, ToolTipCollection("Settings for the barrier overlay"), "Barrier Overlay Settings", headerChildElements
						{
						createNestedElement(GUIElementEnum::softCeilingOverlayRenderTypes),
						createNestedElement(GUIElementEnum::softCeilingOverlayRenderDirection),
						createNestedElement(GUIElementEnum::softCeilingOverlayColorAccel),
						createNestedElement(GUIElementEnum::softCeilingOverlayColorSlippy),
						createNestedElement(GUIElementEnum::softCeilingOverlayColorKill),
						createNestedElement(GUIElementEnum::softCeilingOverlaySolidTransparency),
						createNestedElement(GUIElementEnum::softCeilingOverlayWireframeTransparency)
						}));

			case GUIElementEnum::softCeilingOverlayRenderTypes:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnum<SettingsEnums::SoftCeilingRenderTypes, 150.f>>
					(game, ToolTipCollection("Whether to render barriers that apply to bipeds, vehicles, or either."), "Render Barriers Affecting:", settings->softCeilingOverlayRenderTypes));

			case GUIElementEnum::softCeilingOverlayRenderDirection:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnum<SettingsEnums::SoftCeilingRenderDirection, 100.f>>
					(game, ToolTipCollection("Whether to render the front of barriers, the back, or both."), "Render Barriers Direction:", settings->softCeilingOverlayRenderDirection));


			case GUIElementEnum::softCeilingOverlayColorAccel:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPicker<true>>
					(game, ToolTipCollection("Color of barriers of the \"Acceleration\" type"), "Accelleration Color", settings->softCeilingOverlayColorAccel));

			case GUIElementEnum::softCeilingOverlayColorSlippy:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPicker<true>>
					(game, ToolTipCollection("Color of barriers of the \"Slip Surface\" type"), "Slippery Color", settings->softCeilingOverlayColorSlippy));

			case GUIElementEnum::softCeilingOverlayColorKill:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIColourPicker<true>>
					(game, ToolTipCollection("Color of barriers of the \"Soft Kill\" type"), "Kill Color", settings->softCeilingOverlayColorKill));

			case GUIElementEnum::softCeilingOverlaySolidTransparency:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam(0.f, 1.f)>>
					(game, ToolTipCollection("Opacity of the filled barrier triangles"), "Solid Opacity", settings->softCeilingOverlaySolidTransparency));

			case GUIElementEnum::softCeilingOverlayWireframeTransparency:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam(0.f, 1.f)>>
					(game, ToolTipCollection("Opacity of the outline of the barrier triangles"), "Outline Opacity", settings->softCeilingOverlayWireframeTransparency));



			case GUIElementEnum::shieldInputPrinterToggle:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
					(game, ToolTipCollection("Locks onto nearest deployable shield; when shield disabled, prints which tick you pressed movement inputs"), std::nullopt, "Shield Input Printer", settings->shieldInputPrinterToggle));

			case GUIElementEnum::sensDriftDetectorToggle:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
					(game, ToolTipCollection("Prints a warning message whenever you move your mouse fast enough to cause a sensitivity manip to drift"), std::nullopt, "Sensitivity Drift Detector", settings->sensDriftDetectorToggle));

			case GUIElementEnum::cameraHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, ToolTipCollection("Tools to manipulate the camera"), "Camera", headerChildElements
						{ 
							createNestedElement(GUIElementEnum::hideHUDToggle),
							createNestedElement(GUIElementEnum::editPlayerViewAngleSubheading),
							createNestedElement(GUIElementEnum::freeCameraToggleGUI),
							createNestedElement(GUIElementEnum::freeCameraSettingsSimpleSubheading),
							createNestedElement(GUIElementEnum::freeCameraSettingsAdvancedSubheading),
						}));

			case GUIElementEnum::hideHUDToggle:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
					(game, ToolTipCollection("Disables the HUD, similiar to the Blind skull but you keep your first person model"), RebindableHotkeyEnum::hideHUDToggle, "Hide HUD", settings->hideHUDToggle));


				case GUIElementEnum::editPlayerViewAngleSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Edit the view angle of the player"), "Edit View Angle", headerChildElements
							{
								createNestedElement(GUIElementEnum::editPlayerViewAngleSet),
								createNestedElement(GUIElementEnum::editPlayerViewAngleVec2),
								createNestedElement(GUIElementEnum::editPlayerViewAngleFillCurrent),
								createNestedElement(GUIElementEnum::editPlayerViewAngleCopy),
								createNestedElement(GUIElementEnum::editPlayerViewAnglePaste),
								createNestedElement(GUIElementEnum::editPlayerViewAngleAdjustHorizontal),
								createNestedElement(GUIElementEnum::editPlayerViewAngleAdjustVertical),
								createNestedElement(GUIElementEnum::editPlayerViewAngleAdjustFactor),
							}));

					case GUIElementEnum::editPlayerViewAngleSet:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
							(game, ToolTipCollection("Set player view direction to the specified angles"), RebindableHotkeyEnum::editPlayerViewAngleSet, "Set view angle to: ##editPlayerViewAngleSet", settings->editPlayerViewAngleSet));

					case GUIElementEnum::editPlayerViewAngleVec2:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared < GUIVec2 < true, false, 8, SliderParam<SimpleMath::Vector2>({0, -std::numbers::pi / 2.f }, {std::numbers::pi * 2.f, std::numbers::pi / 2.f})>>
							(game, ToolTipCollection("Rotation of the player"), "Rotation: ##editPlayerViewAngleVec2", settings->editPlayerViewAngleVec2, "yaw", "pitch"));


					case GUIElementEnum::editPlayerViewAngleFillCurrent:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
							(game, ToolTipCollection("Fill data above with players current look angles"), std::nullopt, "Fill with Current##editPlayerViewAngleFillCurrent", settings->editPlayerViewAngleFillCurrent));


					case GUIElementEnum::editPlayerViewAngleCopy:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
							(game, ToolTipCollection("Copy view angles to the clipboard"), std::nullopt, "Copy to Clipboard##editPlayerViewAngleCopy", settings->editPlayerViewAngleCopy));


					case GUIElementEnum::editPlayerViewAnglePaste:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
							(game, ToolTipCollection("Paste in view angles from the clipboard"), std::nullopt, "Paste from Clipboard##editPlayerViewAnglePaste", settings->editPlayerViewAnglePaste));


					case GUIElementEnum::editPlayerViewAngleAdjustHorizontal:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
							(game, ToolTipCollection("Nudges the horizontal view angle by the factor below"), RebindableHotkeyEnum::editPlayerViewAngleAdjustHorizontal, "Nudge Horizontal Angle##editPlayerViewAngleAdjustHorizontal", settings->editPlayerViewAngleAdjustHorizontal));


					case GUIElementEnum::editPlayerViewAngleAdjustVertical:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
							(game, ToolTipCollection("Nudges the vertical view angle by the factor below"), RebindableHotkeyEnum::editPlayerViewAngleAdjustVertical, "Nudge Vertical Angle##editPlayerViewAngleAdjustVertical", settings->editPlayerViewAngleAdjustVertical));


					case GUIElementEnum::editPlayerViewAngleAdjustFactor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
							(game, ToolTipCollection("Amount to nudge by when using above functions"), "Nudge Amount##editPlayerViewAngleAdjustFactor", settings->editPlayerViewAngleAdjustFactor));


				case GUIElementEnum::freeCameraToggleGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, ToolTipCollection(""), RebindableHotkeyEnum::freeCamera, "Free Camera", settings->freeCameraToggle));

				case GUIElementEnum::freeCameraSettingsSimpleSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Simple settings for Free Camera"), "Free Camera Simple Settings", headerChildElements
							{
								createNestedElement(GUIElementEnum::freeCameraTeleportToCamera),
								createNestedElement(GUIElementEnum::freeCameraGameInputDisable),
								createNestedElement(GUIElementEnum::freeCameraCameraInputDisable),
								createNestedElement(GUIElementEnum::freeCameraUserInputCameraBindingsSubheading),
								createNestedElement(GUIElementEnum::freeCameraUserInputCameraTranslationSpeed),
								createNestedElement(GUIElementEnum::freeCameraUserInputCameraIncreaseTranslationSpeedHotkey),
								createNestedElement(GUIElementEnum::freeCameraUserInputCameraDecreaseTranslationSpeedHotkey),
								createNestedElement(GUIElementEnum::freeCameraUserInputCameraRotationSpeed),
								createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetPosition),
								createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetPositionChildren),

							}));

				case GUIElementEnum::freeCameraSettingsAdvancedSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<false>>
						(game, ToolTipCollection("Advanced settings for Free Camera"), "Free Camera Advanced Settings", headerChildElements
							{
								createNestedElement(GUIElementEnum::freeCameraTeleportToCamera),
								createNestedElement(GUIElementEnum::freeCameraThirdPersonRendering),
								createNestedElement(GUIElementEnum::freeCameraDisableScreenEffects),
								createNestedElement(GUIElementEnum::freeCameraGameInputDisable),
								createNestedElement(GUIElementEnum::freeCameraCameraInputDisable),
								createNestedElement(GUIElementEnum::freeCameraUserInputCameraSettings),
								//createNestedElement(GUIElementEnum::freeCameraAnchorPositionToObjectPosition),
								//createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectPosition),
								//createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectFacing),
								//createNestedElement(GUIElementEnum::freeCameraAnchorFOVToObjectDistance),
							}));

					case GUIElementEnum::freeCameraTeleportToCamera:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
							(game, ToolTipCollection("Teleports the player to the cameras position"), RebindableHotkeyEnum::freeCameraTeleportToCameraHotkey, "Teleport to Camera", settings->freeCameraTeleportToCameraEvent));



					case GUIElementEnum::freeCameraThirdPersonRendering:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Makes the player model render, and hides game UI (ala Blind skull)"), std::nullopt, "Third Person Rendering", settings->freeCameraThirdPersonRendering));

					case GUIElementEnum::freeCameraDisableScreenEffects:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, ToolTipCollection("Disables screen effects like rumble or the screen flashing when player is hit"), std::nullopt, "Disable Screen Effects", settings->freeCameraDisableScreenEffects));


					case GUIElementEnum::freeCameraGameInputDisable:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
							(game, ToolTipCollection("Disables game inputs while freecam active"), RebindableHotkeyEnum::freeCameraGameInputDisable, "Disable Game Inputs", settings->freeCameraGameInputDisable));

					case GUIElementEnum::freeCameraCameraInputDisable:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
							(game, ToolTipCollection("Disables direct user camera inputs (translation, rotation)"), RebindableHotkeyEnum::freeCameraCameraInputDisable, "Disable Camera Inputs", settings->freeCameraCameraInputDisable));

					case GUIElementEnum::freeCameraUserInputCameraSettings:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<true>>
							(game, ToolTipCollection("Settings for user-controlled camera"), "User Input Camera Settings", headerChildElements
								{
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraBindingsSubheading),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraBaseFOV),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraTranslationSpeed),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraIncreaseTranslationSpeedHotkey),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraDecreaseTranslationSpeedHotkey),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraTranslationSpeedChangeFactor),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraTranslationInterpolator),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraRotationSpeed),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraRotationInterpolator),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraFOVSpeed),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraFOVInterpolator),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetPosition),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetPositionChildren),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetRotation),
									createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetRotationChildren),
									//createNestedElement(GUIElementEnum::freeCameraUserInputCameraMaintainVelocity),
								}));

						case GUIElementEnum::freeCameraUserInputCameraBindingsSubheading:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading<true>>
								(game, ToolTipCollection("Input bindings for user-controlled camera"), "Input Bindings", headerChildElements
									{
										createNestedElement(GUIElementEnum::freeCameraUserInputTranslateUpBinding),
										createNestedElement(GUIElementEnum::freeCameraUserInputTranslateDownBinding),
										createNestedElement(GUIElementEnum::freeCameraUserInputRollLeftBinding),
										createNestedElement(GUIElementEnum::freeCameraUserInputRollRightBinding),
										createNestedElement(GUIElementEnum::freeCameraUserInputFOVIncreaseBinding),
										createNestedElement(GUIElementEnum::freeCameraUserInputFOVDecreaseBinding),
									}));

							case GUIElementEnum::freeCameraUserInputTranslateUpBinding:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHotkeyOnly<103>>
									(game, ToolTipCollection("Move camera up"), RebindableHotkeyEnum::cameraTranslateUpBinding, "Translate Up" ));

							case GUIElementEnum::freeCameraUserInputTranslateDownBinding:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHotkeyOnly<103>>
									(game, ToolTipCollection("Move camera down"), RebindableHotkeyEnum::cameraTranslateDownBinding, "Translate Down"));

							case GUIElementEnum::freeCameraUserInputRollLeftBinding:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHotkeyOnly<103>>
									(game, ToolTipCollection("Roll the camera to the left"), RebindableHotkeyEnum::cameraRollLeftBinding, "Roll Left"));

							case GUIElementEnum::freeCameraUserInputRollRightBinding:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHotkeyOnly<103>>
									(game, ToolTipCollection("Roll the camera to the right"), RebindableHotkeyEnum::cameraRollRightBinding, "Roll Right"));

							case GUIElementEnum::freeCameraUserInputFOVIncreaseBinding:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHotkeyOnly<103>>
									(game, ToolTipCollection("Increase the cameras Field of View (zoom out)"), RebindableHotkeyEnum::cameraFOVIncreaseBinding, "Increase FOV"));

							case GUIElementEnum::freeCameraUserInputFOVDecreaseBinding:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHotkeyOnly<103>>
									(game, ToolTipCollection("Decrease the cameras Field of View (zoom in)"), RebindableHotkeyEnum::cameraFOVDecreaseBinding, "Decrease FOV"));



						case GUIElementEnum::freeCameraUserInputCameraBaseFOV:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam(0.f, 179.9999f)>>
								(game, ToolTipCollection("Field of view of the camera, in vertical degrees (assumes 16:9. same as in-game-menu) (before any adjustment by anchorFOVtoObjectDistance automation)"), "Base Field of View", settings->freeCameraUserInputCameraBaseFOV));

						case GUIElementEnum::freeCameraUserInputCameraTranslationSpeed:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
								(game, ToolTipCollection("How fast the camera translates (moves positionally)"), "Translation Speed", settings->freeCameraUserInputCameraTranslationSpeed));


						case GUIElementEnum::freeCameraUserInputCameraIncreaseTranslationSpeedHotkey:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
								(game, ToolTipCollection("Increases camera translation speed by a scale factor"), RebindableHotkeyEnum::freeCameraUserInputCameraIncreaseTranslationSpeedHotkey, "Increase Translation Speed", settings->freeCameraUserInputCameraIncreaseTranslationSpeedHotkey));

						case GUIElementEnum::freeCameraUserInputCameraDecreaseTranslationSpeedHotkey:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
								(game, ToolTipCollection("Decreases camera translation speed by a scale factor"), RebindableHotkeyEnum::freeCameraUserInputCameraDecreaseTranslationSpeedHotkey, "Decrease Translation Speed", settings->freeCameraUserInputCameraDecreaseTranslationSpeedHotkey));

						case GUIElementEnum::freeCameraUserInputCameraTranslationSpeedChangeFactor:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
								(game, ToolTipCollection("How much the camera translation speed changes when you press the above hotkeys"), "Translation Speed Change Factor", settings->freeCameraUserInputCameraTranslationSpeedChangeFactor));



						case GUIElementEnum::freeCameraUserInputCameraTranslationInterpolator:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsEnums::FreeCameraInterpolationTypesEnum, 100.f>>
								(game, ToolTipCollection("Function that controls how smoothly the camera adjusts to input"),  "Translation Interpolation##UserInputCamera", settings->freeCameraUserInputCameraTranslationInterpolator,
									vectorOfHeaderChildElements
							{
								headerChildElements{},
								headerChildElements{ createNestedElement(GUIElementEnum::freeCameraUserInputCameraTranslationInterpolatorLinearFactor) }
							}
							));

						case GUIElementEnum::freeCameraUserInputCameraTranslationInterpolatorLinearFactor:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam(0.f, 1.f, ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Logarithmic)>>
								(game, ToolTipCollection("0 to 1 value controlling smoothness of the input. Low values make the camera sluggish, high values make it fast and snappy."),  "Snap Factor##UserInputCameraTranslation", settings->freeCameraUserInputCameraTranslationInterpolatorLinearFactor));



						case GUIElementEnum::freeCameraUserInputCameraRotationSpeed:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
								(game, ToolTipCollection("How fast the camera rotates (turns about)"), "Rotation Speed", settings->freeCameraUserInputCameraRotationSpeed));

						case GUIElementEnum::freeCameraUserInputCameraRotationInterpolator:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsEnums::FreeCameraInterpolationTypesEnum, 100.f>>
								(game, ToolTipCollection("Function that controls how smoothly the camera adjusts to input"), "Rotation Interpolation##UserInputCamera", settings->freeCameraUserInputCameraRotationInterpolator,
									vectorOfHeaderChildElements
									{
										headerChildElements{},
										headerChildElements{ createNestedElement(GUIElementEnum::freeCameraUserInputCameraRotationInterpolatorLinearFactor) }
									}
							));

						case GUIElementEnum::freeCameraUserInputCameraRotationInterpolatorLinearFactor:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam(0.f, 1.f, ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Logarithmic)>>
								(game, ToolTipCollection("0 to 1 value controlling smoothness of the input. Low values make the camera sluggish, high values make it fast and snappy."), "Snap Factor##UserInputCameraRotation", settings->freeCameraUserInputCameraRotationInterpolatorLinearFactor));



						case GUIElementEnum::freeCameraUserInputCameraFOVSpeed:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
								(game, ToolTipCollection("How fast the camera FOV changes when you hold down the increase/decrease FOV binding"), "FOV change Speed", settings->freeCameraUserInputCameraFOVSpeed));

						case GUIElementEnum::freeCameraUserInputCameraFOVInterpolator:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsEnums::FreeCameraInterpolationTypesEnum, 100.f>>
								(game, ToolTipCollection("Function that controls how smoothly the camera adjusts to input"), "FOV Interpolation##UserInputCamera", settings->freeCameraUserInputCameraFOVInterpolator,
									vectorOfHeaderChildElements
									{
										headerChildElements{},
										headerChildElements{ createNestedElement(GUIElementEnum::freeCameraUserInputCameraFOVInterpolatorLinearFactor) }
									}
							));

						case GUIElementEnum::freeCameraUserInputCameraFOVInterpolatorLinearFactor:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<SliderParam(0.f, 1.f, ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Logarithmic)>>
								(game, ToolTipCollection("0 to 1 value controlling smoothness of the input. Low values make the camera sluggish, high values make it fast and snappy."), "Snap Factor##UserInputCameraFOV", settings->freeCameraUserInputCameraFOVInterpolatorLinearFactor));




						case GUIElementEnum::freeCameraUserInputCameraSetPosition:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
								(game, ToolTipCollection("Set camera Position"), RebindableHotkeyEnum::freeCameraUserInputCameraSetPosition, "Set Position To##freeCameraUserInputCameraSetPosition", settings->freeCameraUserInputCameraSetPosition));


						case GUIElementEnum::freeCameraUserInputCameraSetPositionChildren:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIDummyContainer>
								(game, "freeCameraUserInputCameraSetPositionChildren", headerChildElements
									{
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetPositionVec3),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetPositionFillCurrent),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetPositionCopy),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetPositionPaste),
									}
									));


							case GUIElementEnum::freeCameraUserInputCameraSetPositionVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, false, 8>>
									(game, ToolTipCollection("Position of the camera "), "Position: ##freeCameraUserInputCameraSetPositionVec3", settings->freeCameraUserInputCameraSetPositionVec3));

							case GUIElementEnum::freeCameraUserInputCameraSetPositionFillCurrent:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Fill data with current camera position"), std::nullopt, "Fill with Current##freeCameraUserInputCameraSetPositionCopyCurrent", settings->freeCameraUserInputCameraSetPositionFillCurrent));

							case GUIElementEnum::freeCameraUserInputCameraSetPositionCopy:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Copy Position to the clipboard"), std::nullopt, "Copy to Clipboard##freeCameraUserInputCameraSetPositionPaste", settings->freeCameraUserInputCameraSetPositionCopy));


							case GUIElementEnum::freeCameraUserInputCameraSetPositionPaste:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Paste in a Position from the clipboard"), std::nullopt, "Paste from Clipboard##freeCameraUserInputCameraSetPositionPaste", settings->freeCameraUserInputCameraSetPositionPaste));

						case GUIElementEnum::freeCameraUserInputCameraSetRotation:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
								(game, ToolTipCollection("Set camera Rotation"), RebindableHotkeyEnum::freeCameraUserInputCameraSetRotation, "Set Rotation To##freeCameraUserInputCameraSetRotation", settings->freeCameraUserInputCameraSetRotation));

						case GUIElementEnum::freeCameraUserInputCameraSetRotationChildren:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIDummyContainer>
								(game, "freeCameraUserInputCameraSetRotationChildren", headerChildElements
									{
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetRotationVec3),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetRotationFillCurrent),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetRotationCopy),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetRotationPaste),
									}
							));

							case GUIElementEnum::freeCameraUserInputCameraSetRotationVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared < GUIVec3 < true, false, 8, SliderParam<SimpleMath::Vector3>({ -std::numbers::pi, -std::numbers::pi / 2.f, -std::numbers::pi / 2.f } , { std::numbers::pi, std::numbers::pi / 2.f, std::numbers::pi / 2.f }) >>
									(game, ToolTipCollection("Rotation of the camera (in radians, yaw pitch roll)"), "Rotation: ##freeCameraUserInputCameraSetRotationVec3", settings->freeCameraUserInputCameraSetRotationVec3, "Yaw", "Pitch", "Roll"));

							case GUIElementEnum::freeCameraUserInputCameraSetRotationFillCurrent:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Fill data with current camera rotation"), std::nullopt, "Fill with Current##freeCameraUserInputCameraSetRotationCopyCurrent", settings->freeCameraUserInputCameraSetRotationFillCurrent));

							case GUIElementEnum::freeCameraUserInputCameraSetRotationCopy:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Copy Rotation to the clipboard"), std::nullopt, "Copy to Clipboard##freeCameraUserInputCameraSetRotationPaste", settings->freeCameraUserInputCameraSetRotationCopy));


							case GUIElementEnum::freeCameraUserInputCameraSetRotationPaste:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Paste in a Rotation from the clipboard"), std::nullopt, "Paste from Clipboard##freeCameraUserInputCameraSetRotationPaste", settings->freeCameraUserInputCameraSetRotationPaste));

								/* // Unfinished free camera features

						case GUIElementEnum::freeCameraUserInputCameraMaintainVelocity:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, true>>
								(game, ToolTipCollection("Maintains the current camera velocity while enabled"), RebindableHotkeyEnum::freeCameraUserInputCameraMaintainVelocity, "Maintain Velocity", settings->freeCameraUserInputCameraMaintainVelocity, headerChildElements
									{
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetVelocity),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetVelocityChildren),

									}
							));

						case GUIElementEnum::freeCameraUserInputCameraSetVelocity:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
								(game, ToolTipCollection("Set camera Velocity"), RebindableHotkeyEnum::freeCameraUserInputCameraSetVelocity, "Set Velocity To##freeCameraUserInputCameraSetVelocity", settings->freeCameraUserInputCameraSetVelocity));


						case GUIElementEnum::freeCameraUserInputCameraSetVelocityChildren:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIDummyContainer>
								(game, "freeCameraUserInputCameraSetVelocityChildren", headerChildElements
									{
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetVelocityVec3),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetVelocityFillCurrent),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetVelocityCopy),
										createNestedElement(GUIElementEnum::freeCameraUserInputCameraSetVelocityPaste),
									}
							));


							case GUIElementEnum::freeCameraUserInputCameraSetVelocityVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, false, 8>>
									(game, ToolTipCollection("Velocity of the camera (relative to facing)"), "Velocity ##freeCameraUserInputCameraSetVelocityVec3", settings->freeCameraUserInputCameraSetVelocityVec3, "Forward", "Right", "Up"));

							case GUIElementEnum::freeCameraUserInputCameraSetVelocityFillCurrent:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Fill with the cameras current relative Velocity"), std::nullopt, "Fill with Current##freeCameraUserInputCameraSetVelocityCopyCurrent", settings->freeCameraUserInputCameraSetVelocityFillCurrent));

							case GUIElementEnum::freeCameraUserInputCameraSetVelocityCopy:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Copy a relative Velocity to the clipboard"), std::nullopt, "Copy to Clipboard##freeCameraUserInputCameraSetVelocityPaste", settings->freeCameraUserInputCameraSetVelocityCopy));


							case GUIElementEnum::freeCameraUserInputCameraSetVelocityPaste:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, ToolTipCollection("Paste in a relative Velocity from the clipboard"), std::nullopt, "Paste from Clipboard##freeCameraUserInputCameraSetVelocityPaste", settings->freeCameraUserInputCameraSetVelocityPaste));

					case GUIElementEnum::freeCameraAnchorPositionToObjectPosition:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, true>>
							(game, ToolTipCollection("Anchors the camera position to an objects position, following it around"), RebindableHotkeyEnum::freeCameraAnchorPositionToObjectPosition, "Anchor Camera Position to Object Position", settings->freeCameraAnchorPositionToObjectPosition, headerChildElements
								{
								createNestedElement(GUIElementEnum::freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup),
								createNestedElement(GUIElementEnum::freeCameraAnchorPositionToObjectPositionTranslationInterpolator),
								createNestedElement(GUIElementEnum::freeCameraAnchorPositionToObjectRotation),
								}
							));

						case GUIElementEnum::freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsStateAndEvents::FreeCameraObjectTrackEnum, 100.f>>
								(game, ToolTipCollection(""), "Object to Anchor to##freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup", settings->freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup,
									vectorOfHeaderChildElements
									{
										headerChildElements{},
										headerChildElements{createNestedElement(GUIElementEnum::freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum)},
									}));


							case GUIElementEnum::freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
									(game, ToolTipCollection("The datum (unique ID) of the object the camera should anchor to"), "Custom object datum##freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum", settings->freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum));


						case GUIElementEnum::freeCameraAnchorPositionToObjectPositionTranslationInterpolator:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsStateAndEvents::FreeCameraInterpolationTypesEnum, 100.f>>
								(game, ToolTipCollection("Function that controls how smoothly the camera adjusts to input"), "Position Interpolation##freeCameraAnchorPositionToObjectPositionTranslationInterpolator", settings->freeCameraAnchorPositionToObjectPositionTranslationInterpolator,
									vectorOfHeaderChildElements
									{
										headerChildElements{},
										headerChildElements{ createNestedElement(GUIElementEnum::freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor) }
									}
							));

							case GUIElementEnum::freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
									(game, ToolTipCollection("0 to 1 value controlling smoothness of the input. Low values make the camera sluggish, high values make it fast and snappy."), "Snap Factor##freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor", settings->freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor));


						case GUIElementEnum::freeCameraAnchorPositionToObjectRotation:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
								(game, ToolTipCollection("Anchors the camera position to an objects rotation, staying on the same side of it as it turns around"), RebindableHotkeyEnum::freeCameraAnchorPositionToObjectRotation, "Anchor Camera Position to Object Rotation", settings->freeCameraAnchorPositionToObjectRotation
							));

						

					case GUIElementEnum::freeCameraAnchorRotationToObjectPosition:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, true>>
							(game, ToolTipCollection("Anchors the camera rotation to an objects position, panning to track it as it moves"), RebindableHotkeyEnum::freeCameraAnchorRotationToObjectPosition, "Anchor Camera Rotation to Object Position", settings->freeCameraAnchorRotationToObjectPosition, headerChildElements
								{
								createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup),
								createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectPositionRotationInterpolator),
								}
						));

						case GUIElementEnum::freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsStateAndEvents::FreeCameraObjectTrackEnumPlusAbsolute, 100.f>>
								(game, ToolTipCollection(""), "Object to Anchor to##freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup", settings->freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup,
									vectorOfHeaderChildElements
									{
										headerChildElements{},
										headerChildElements{createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum)},
										headerChildElements{createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3)},
									}));


							case GUIElementEnum::freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
									(game, ToolTipCollection("The datum (unique ID) of the object the camera should anchor to"), "Custom object datum##freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum", settings->freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum));


							case GUIElementEnum::freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, false, 8>>
									(game, ToolTipCollection("Absolute position to anchor to"), "Absolute Position ##freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3", settings->freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3, "x", "y", "z"));



						case GUIElementEnum::freeCameraAnchorRotationToObjectPositionRotationInterpolator:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsStateAndEvents::FreeCameraInterpolationTypesEnum, 100.f>>
								(game, ToolTipCollection("Function that controls how smoothly the camera adjusts to input"), "Rotation Interpolation##freeCameraAnchorRotationToObjectPositionRotationInterpolator", settings->freeCameraAnchorRotationToObjectPositionRotationInterpolator,
									vectorOfHeaderChildElements
									{
										headerChildElements{},
										headerChildElements{ createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor) }
									}
							));

							case GUIElementEnum::freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
									(game, ToolTipCollection("0 to 1 value controlling smoothness of the input. Low values make the camera sluggish, high values make it fast and snappy."), "Snap Factor##freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor", settings->freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor));


					case GUIElementEnum::freeCameraAnchorRotationToObjectFacing:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, true>>
							(game, ToolTipCollection("Anchors the camera rotation to an objects rotation, panning to track it as it moves"), RebindableHotkeyEnum::freeCameraAnchorRotationToObjectFacing, "Anchor Camera Rotation to Object Facing", settings->freeCameraAnchorRotationToObjectFacing, headerChildElements
								{
								createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup),
								createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectFacingRotationInterpolator),
								}
						));

						case GUIElementEnum::freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsStateAndEvents::FreeCameraObjectTrackEnum, 100.f>>
								(game, ToolTipCollection(""), "Object to Anchor to##freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup", settings->freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup,
									vectorOfHeaderChildElements
									{
										headerChildElements{},
										headerChildElements{createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum)},
									}));


							case GUIElementEnum::freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
									(game, ToolTipCollection("The datum (unique ID) of the object the camera should anchor to"), "Custom object datum##freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum", settings->freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum));


							case GUIElementEnum::freeCameraAnchorRotationToObjectFacingRotationInterpolator:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsStateAndEvents::FreeCameraInterpolationTypesEnum, 100.f>>
									(game, ToolTipCollection("Function that controls how smoothly the camera adjusts to input"), "Rotation Interpolation##freeCameraAnchorRotationToObjectFacingRotationInterpolator", settings->freeCameraAnchorRotationToObjectFacingRotationInterpolator,
										vectorOfHeaderChildElements
										{
											headerChildElements{},
											headerChildElements{ createNestedElement(GUIElementEnum::freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor) }
										}
								));

								case GUIElementEnum::freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor:
									return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
										(game, ToolTipCollection("0 to 1 value controlling smoothness of the input. Low values make the camera sluggish, high values make it fast and snappy."), "Snap Factor##freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor", settings->freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor));

					case GUIElementEnum::freeCameraAnchorFOVToObjectDistance:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenTrue, true>>
							(game, ToolTipCollection("Anchors the camera FOV to its distance to an object, zooming in as the object moves further away"), RebindableHotkeyEnum::freeCameraAnchorFOVToObjectDistance, "Anchor Camera FOV to Object Distance", settings->freeCameraAnchorFOVToObjectDistance, headerChildElements
								{
								createNestedElement(GUIElementEnum::freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup),
								createNestedElement(GUIElementEnum::freeCameraAnchorFOVToObjectDistanceFOVInterpolator),
								}
						));

					case GUIElementEnum::freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsStateAndEvents::FreeCameraObjectTrackEnumPlusAbsolute, 100.f>>
							(game, ToolTipCollection(""), "Object to Anchor to##freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup", settings->freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup,
								vectorOfHeaderChildElements
								{
									headerChildElements{},
									headerChildElements{createNestedElement(GUIElementEnum::freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum)},
									headerChildElements{createNestedElement(GUIElementEnum::freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3)},
								}));


					case GUIElementEnum::freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
							(game, ToolTipCollection("The datum (unique ID) of the object the camera should anchor to"), "Custom object datum##freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum", settings->freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum));


					case GUIElementEnum::freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, false, 8>>
							(game, ToolTipCollection("Absolute position to anchor to"), "Absolute Position: ##freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3", settings->freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3, "x", "y", "z"));



						case GUIElementEnum::freeCameraAnchorFOVToObjectDistanceFOVInterpolator:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIComboEnumWithChildren<SettingsStateAndEvents::FreeCameraInterpolationTypesEnum, 100.f>>
								(game, ToolTipCollection("Function that controls how smoothly the camera adjusts to input"), "FOV Interpolation##freeCameraAnchorFOVToObjectDistanceFOVInterpolator", settings->freeCameraAnchorFOVToObjectDistanceFOVInterpolator,
									vectorOfHeaderChildElements
									{
										headerChildElements{},
										headerChildElements{ createNestedElement(GUIElementEnum::freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor) }
									}
							));

							case GUIElementEnum::freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIFloat<>>
									(game, ToolTipCollection("0 to 1 value controlling smoothness of the input. Low values make the camera sluggish, high values make it fast and snappy."), "Snap Factor##freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor", settings->freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor));
*/
				

			case GUIElementEnum::theaterHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, ToolTipCollection("this is where theater stuff would go IF I HAD ANY"), "Camera", headerChildElements{ std::nullopt }));


			

			case GUIElementEnum::debugHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, ToolTipCollection("Debug tools"), "Debug", headerChildElements
						{ 
						createNestedElement(GUIElementEnum::getPlayerDatumGUI),
						createNestedElement(GUIElementEnum::getObjectAddressGUI),
						}));

				case GUIElementEnum::getPlayerDatumGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, ToolTipCollection("Copies the player datum to the clipboard"), std::nullopt, "Get Player Datum", settings->getPlayerDatumEvent));



				case GUIElementEnum::getObjectAddressGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
						(game, ToolTipCollection("Evaluates a main object datums address, copying it to the clipboard"), "Get Object Address: ", settings->getObjectAddressDWORD, settings->getObjectAddressEvent));


#ifdef HCM_DEBUG

				case GUIElementEnum::HCMDebugHeadingGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
						(game, ToolTipCollection("Debug tools for HCM development"), "HCM Debug", headerChildElements
							{
						}));


#endif




			default:
				throw HCMInitException(std::format("You forgot to add a creation case label for GUIElement: {} in {}::{}", magic_enum::enum_name(guielementenum), nameof(GUIElementConstructor), nameof(createGUIElement)));
			}


		}
		catch (HCMInitException ex)
		{
			info->addFailure(guielementenum, game, ex);
			cacheFailedServices.insert(std::make_pair(guielementenum, game));
			return std::nullopt;
		}
	}


	std::shared_ptr<GUIElementStore> mStore;

	MCCProcessType mProcType;

public:
	GUIElementConstructorImpl(std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIElementStore> store, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings, MCCProcessType procType, std::shared_ptr<RuntimeExceptionHandler> exp)
		: mStore(store), mProcType(procType)
	{
		// Create all top level GUI elements. Each one will recursively create it's nested elements, if it has any.
		for (auto& [element, game] : guireq->getToplevelGUIElements())
		{


			auto x = createGUIElementAndStoreResult(element, game, guireq, fail, info, settings, exp);
			if (x.has_value())
			{
				store->getTopLevelGUIElementsMutable().at(game).push_back(x.value());
			}
		}
	}
};






GUIElementConstructor::GUIElementConstructor(std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIElementStore> store, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings, MCCProcessType procType, std::shared_ptr<RuntimeExceptionHandler> exp) : pimpl(std::make_unique<GUIElementConstructorImpl>(guireq, fail, store, info, settings, procType, exp)) {}
GUIElementConstructor::~GUIElementConstructor() = default;
