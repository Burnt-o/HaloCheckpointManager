#include "pch.h"
#include "GUIElementConstructor.h"
#include "SettingsStateAndEvents.h"
#include "HotkeyEnum.h"
#include "IGUIElement.h"
#include "GUIElementEnum.h"

#include "GUISimpleButton.h"
#include "GUISimpleToggle.h"
#include "GUISpeedhack.h"
#include "GUIInvulnerability.h"
#include "GUIConsoleCommand.h"
#include "GUIHeading.h"
#include "GUISubHeading.h"
#include "GUIHotkeyOnly.h"
#include "GUIAdvanceTicks.h"
#include "GUIRadioButton.h"
#include "GUIRadioGroup.h"
#include "GUIFloat.h"
#include "GUIVec3.h"
#include "GUIVec3RelativeOrAbsolute.h"
#include "GUIToggleWithChildren.h"
#include "GUIInputString.h"
#include "GUIInputDWORD.h"

class GUIElementConstructor::GUIElementConstructorImpl {
private:
	std::set<std::pair<GUIElementEnum, GameState::Value>> cacheFailedServices;

	std::optional<std::shared_ptr<IGUIElement>> createGUIElementAndStoreResult(GUIElementEnum guielementenum, GameState game, std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings)
	{
		auto res = createGUIElement(guielementenum, game, guireq, fail, info, settings);
		if (res.has_value()) mStore->mapOfSuccessfullyConstructedGUIElements.at(game).insert(guielementenum);
		return res;
	}

	std::optional<std::shared_ptr<IGUIElement>> createGUIElement(GUIElementEnum guielementenum, GameState game, std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings)
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
#define createNestedElement(elementEnum) createGUIElementAndStoreResult(elementEnum, game, guireq, fail, info, settings) // optional nested element, recursively calls this function

			// ALL GUI ELEMENTS MUST HAVE A CASE HERE, TOP LEVEL OR NOT
			std::optional<std::shared_ptr<IGUIElement>> x;
			switch (guielementenum)
			{
			case GUIElementEnum::controlHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Control", headerChildElements
						{
						createNestedElement(GUIElementEnum::toggleGUIHotkeyGUI),
						createNestedElement(GUIElementEnum::GUISettingsSubheading),
						createNestedElement(GUIElementEnum::togglePauseGUI),
						createNestedElement(GUIElementEnum::togglePauseSettingsSubheading),
						createNestedElement(GUIElementEnum::showGUIFailuresGUI),
						}));

				case GUIElementEnum::toggleGUIHotkeyGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHotkeyOnly>
						(game, HotkeysEnum::toggleGUI, "Toggle this GUI"));

				case GUIElementEnum::GUISettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "GUI settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::GUIShowingFreesCursor),
							createNestedElement(GUIElementEnum::GUIShowingBlocksInput),
							createNestedElement(GUIElementEnum::GUIShowingPausesGame),
							}));

					case GUIElementEnum::GUIShowingFreesCursor:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Free cursor when GUI open", settings->GUIShowingFreesCursor));

					case GUIElementEnum::GUIShowingBlocksInput:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Block input when GUI open", settings->GUIShowingBlocksInput));

					case GUIElementEnum::GUIShowingPausesGame:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Pause game when GUI open", settings->GUIShowingPausesGame));

				case GUIElementEnum::togglePauseGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, HotkeysEnum::togglePause, "Pause game", settings->togglePause));

				case GUIElementEnum::togglePauseSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "Pause settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::advanceTicksGUI),
							createNestedElement(GUIElementEnum::pauseAlsoBlocksInputGUI),
							createNestedElement(GUIElementEnum::pauseAlsoFreesCursorGUI),
							}));


				case GUIElementEnum::advanceTicksGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIAdvanceTicks<true>>
						(game, HotkeysEnum::advanceTicks, settings->advanceTicksEvent, settings->advanceTicksCount));

					case GUIElementEnum::pauseAlsoBlocksInputGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Pause also blocks game input", settings->pauseAlsoBlocksInput));

					case GUIElementEnum::pauseAlsoFreesCursorGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Pause also frees cursor", settings->pauseAlsoFreesCursor));

				case GUIElementEnum::showGUIFailuresGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
						(game, std::nullopt, "Show optional cheat service failures", settings->showGUIFailures));

			case GUIElementEnum::saveManagementHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Save Management", headerChildElements
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
						}));

				case GUIElementEnum::forceCheckpointGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::forceCheckpoint, "Force Checkpoint", settings->forceCheckpointEvent));

				case GUIElementEnum::forceRevertGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::forceRevert, "Force Revert", settings->forceRevertEvent));

				case GUIElementEnum::forceDoubleRevertGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::forceDoubleRevert, "Force Double Revert", settings->forceDoubleRevertEvent));

				case GUIElementEnum::forceCoreSaveGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::forceCoreSave, "Force Core Save", settings->forceCoreSaveEvent));

				case GUIElementEnum::forceCoreLoadGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::forceCoreLoad, "Force Core Load", settings->forceCoreLoadEvent));

				case GUIElementEnum::injectCheckpointGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::injectCheckpoint, "Inject Checkpoint", settings->injectCheckpointEvent));

				case GUIElementEnum::injectCheckpointSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "Inject Checkpoint Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::injectCheckpointForcesRevert),
							createNestedElement(GUIElementEnum::injectCheckpointLevelCheck),
							createNestedElement(GUIElementEnum::injectCheckpointVersionCheck),
							createNestedElement(GUIElementEnum::injectCheckpointDifficultyCheck),
							createNestedElement(GUIElementEnum::injectCheckpointIgnoresChecksum)
							}));

					case GUIElementEnum::injectCheckpointForcesRevert:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Force revert after injecting", settings->injectCheckpointForcesRevert));

					case GUIElementEnum::injectCheckpointLevelCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Warn on injecting to wrong level", settings->injectCheckpointLevelCheck));

					case GUIElementEnum::injectCheckpointVersionCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Warn on injecting to wrong game version", settings->injectCheckpointVersionCheck));

					case GUIElementEnum::injectCheckpointDifficultyCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Warn on injecting to wrong difficulty", settings->injectCheckpointDifficultyCheck));

					case GUIElementEnum::injectCheckpointIgnoresChecksum:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Ignore checkpoint checksum on revert", settings->injectionIgnoresChecksum));

				case GUIElementEnum::dumpCheckpointGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::dumpCheckpoint, "Dump Checkpoint", settings->dumpCheckpointEvent));

				case GUIElementEnum::dumpCheckpointSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "Dump Checkpoint Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::dumpCheckpointAutonameGUI),
							createNestedElement(GUIElementEnum::dumpCheckpointForcesSave)
							}));

					case GUIElementEnum::dumpCheckpointAutonameGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Autoname checkpoints", settings->autonameCheckpoints));

					case GUIElementEnum::dumpCheckpointForcesSave:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Force checkpoint before dumping", settings->dumpCheckpointForcesSave));

				case GUIElementEnum::injectCoreGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::injectCore, "Inject Core Save", settings->injectCoreEvent));

				case GUIElementEnum::injectCoreSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "Inject Core Save Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::injectCoreForcesRevert),
							createNestedElement(GUIElementEnum::injectCoreLevelCheck),
							createNestedElement(GUIElementEnum::injectCoreVersionCheck),
							createNestedElement(GUIElementEnum::injectCoreDifficultyCheck),
							}));

					case GUIElementEnum::injectCoreForcesRevert:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Force core load after injecting", settings->injectCoreForcesRevert));

					case GUIElementEnum::injectCoreLevelCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Warn on injecting to wrong level", settings->injectCoreLevelCheck));

					case GUIElementEnum::injectCoreVersionCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Warn on injecting to wrong game version", settings->injectCoreVersionCheck));

					case GUIElementEnum::injectCoreDifficultyCheck:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Warn on injecting to wrong difficulty", settings->injectCoreDifficultyCheck));


				case GUIElementEnum::dumpCoreGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::dumpCore, "Dump Core Save", settings->dumpCoreEvent));

				case GUIElementEnum::dumpCoreSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "Dump Core Save Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::dumpCoreAutonameGUI),
							createNestedElement(GUIElementEnum::dumpCoreForcesSave)
							}));

					case GUIElementEnum::dumpCoreAutonameGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Autoname core saves", settings->autonameCoresaves));

					case GUIElementEnum::dumpCoreForcesSave:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "Force core save before dumping", settings->dumpCoreForcesSave));

			case GUIElementEnum::cheatsHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Useful Cheats", headerChildElements
						{
							createNestedElement(GUIElementEnum::speedhackGUI),
							createNestedElement(GUIElementEnum::invulnGUI),
							createNestedElement(GUIElementEnum::invulnerabilitySettingsSubheading),
							createNestedElement(GUIElementEnum::aiFreezeGUI),
							createNestedElement(GUIElementEnum::medusaGUI),
							createNestedElement(GUIElementEnum::forceTeleportGUI),
							createNestedElement(GUIElementEnum::forceTeleportSettingsSubheading),
							createNestedElement(GUIElementEnum::forceLaunchGUI),
							createNestedElement(GUIElementEnum::forceLaunchSettingsSubheading)
						}));

				case GUIElementEnum::speedhackGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISpeedhack>
						(game, HotkeysEnum::speedhack, settings));

				case GUIElementEnum::invulnGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInvulnerability>
						(game, HotkeysEnum::invuln, "Invulnerability", settings->invulnerabilityToggle
							)); 


				case GUIElementEnum::invulnerabilitySettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "InvulnerabilitySettings", headerChildElements
							{
								createNestedElement(GUIElementEnum::invulnNPCGUI),
							}));

					case GUIElementEnum::invulnNPCGUI:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
								(game, std::nullopt, "NPC's invulnerable too", settings->invulnerabilityNPCToggle
								));

				case GUIElementEnum::aiFreezeGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, HotkeysEnum::aiFreeze, "Freeze AI", settings->aiFreezeToggle));

				case GUIElementEnum::medusaGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, HotkeysEnum::medusa, "Medusa", settings->medusaToggle));


				case GUIElementEnum::forceTeleportGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::forceTeleport, "Force Teleport", settings->forceTeleportEvent));

				case GUIElementEnum::forceTeleportSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "Force Teleport Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::forceTeleportApplyToPlayer),
							createNestedElement(GUIElementEnum::forceTeleportSettingsRadioGroup),
							}));

					case GUIElementEnum::forceTeleportApplyToPlayer:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenFalse, false>>
							(game, std::nullopt, "Apply to player", settings->forceTeleportApplyToPlayer, headerChildElements
								{
								createNestedElement(GUIElementEnum::forceTeleportCustomObject),
								}));

						case GUIElementEnum::forceTeleportCustomObject:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
								(game, "Custom object datum", settings->forceTeleportCustomObject));



					case GUIElementEnum::forceTeleportSettingsRadioGroup:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioGroup>
							(game, "Force Teleport Radio Group", headerChildElements
								{
								createNestedElement(GUIElementEnum::forceTeleportForward),
								createNestedElement(GUIElementEnum::forceTeleportManual)
								}));


						case GUIElementEnum::forceTeleportForward:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioButton>
								(game, std::nullopt, "Teleport Relative to Player",   settings->forceTeleportForward, headerChildElements
									{
									createNestedElement(GUIElementEnum::forceTeleportRelativeVec3),
									createNestedElement(GUIElementEnum::forceTeleportForwardIgnoreZ)
									}));

							case GUIElementEnum::forceTeleportRelativeVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, false>>
									(game, "Teleport: ", settings->forceTeleportRelativeVec3));


							case GUIElementEnum::forceTeleportForwardIgnoreZ:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, std::nullopt, "Ignore vertical look angle",  settings->forceTeleportForwardIgnoreZ));


						case GUIElementEnum::forceTeleportManual:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioButton>
								(game, std::nullopt, "Teleport to Manual Coordinates",  settings->forceTeleportManual, headerChildElements
									{
									createNestedElement(GUIElementEnum::forceTeleportAbsoluteVec3),
									createNestedElement(GUIElementEnum::forceTeleportFillWithCurrentPositionEvent)
									}));

							case GUIElementEnum::forceTeleportAbsoluteVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, true>>
									(game, "Teleport: ", settings->forceTeleportAbsoluteVec3));


							case GUIElementEnum::forceTeleportFillWithCurrentPositionEvent:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<false>>
									(game, std::nullopt, "Fill with current position",  settings->forceTeleportFillWithCurrentPositionEvent));

				case GUIElementEnum::forceLaunchGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::forceLaunch, "Force Launch", settings->forceLaunchEvent));

				case GUIElementEnum::forceLaunchSettingsSubheading:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "Force Launch Settings", headerChildElements
							{
							createNestedElement(GUIElementEnum::forceLaunchApplyToPlayer),
							createNestedElement(GUIElementEnum::forceLaunchSettingsRadioGroup),
							}));

					case GUIElementEnum::forceLaunchApplyToPlayer:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIToggleWithChildren<GUIToggleWithChildrenParameters::ShowWhenFalse, false>>
							(game, std::nullopt, "Apply to player", settings->forceLaunchApplyToPlayer, headerChildElements
								{
								createNestedElement(GUIElementEnum::forceLaunchCustomObject)
								}));

						case GUIElementEnum::forceLaunchCustomObject:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
								(game, "Custom object datum", settings->forceLaunchCustomObject));


					case GUIElementEnum::forceLaunchSettingsRadioGroup:
						return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioGroup>
							(game, "Force Launch Radio Group", headerChildElements
								{
								createNestedElement(GUIElementEnum::forceLaunchForward),
								createNestedElement(GUIElementEnum::forceLaunchManual)
								}));


						case GUIElementEnum::forceLaunchForward:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioButton>
								(game, std::nullopt, "Launch relative to player facing",  settings->forceLaunchForward, headerChildElements
									{
									createNestedElement(GUIElementEnum::forceLaunchRelativeVec3),
									createNestedElement(GUIElementEnum::forceLaunchForwardIgnoreZ)
									}));

							case GUIElementEnum::forceLaunchRelativeVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, false>>
									(game, "Launch: ", settings->forceLaunchRelativeVec3));


							case GUIElementEnum::forceLaunchForwardIgnoreZ:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
									(game, std::nullopt, "Ignore vertical look angle", settings->forceLaunchForwardIgnoreZ));


						case GUIElementEnum::forceLaunchManual:
							return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIRadioButton>
								(game, std::nullopt, "Launch with absolute velocity",  settings->forceLaunchManual, headerChildElements
									{
									createNestedElement(GUIElementEnum::forceLaunchAbsoluteVec3),
									}));


							case GUIElementEnum::forceLaunchAbsoluteVec3:
								return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIVec3<true, true>>
									(game, "Launch: ", settings->forceLaunchAbsoluteVec3));



			case GUIElementEnum::overlaysHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Overlays", headerChildElements{ std::nullopt }));

			case GUIElementEnum::cameraHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Camera", headerChildElements{ std::nullopt }));

			case GUIElementEnum::theaterHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Camera", headerChildElements{ std::nullopt }));


			
#ifdef HCM_DEBUG


			case GUIElementEnum::debugHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Debug", headerChildElements
						{ 
						createNestedElement(GUIElementEnum::consoleCommandGUI),
						createNestedElement(GUIElementEnum::getObjectAddressGUI),
						}));


				case GUIElementEnum::consoleCommandGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputString>
						(game, "Console command: ", settings->consoleCommandString, settings->consoleCommandEvent));


				case GUIElementEnum::getObjectAddressGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInputDWORD<true>>
						(game, "Get Object Address: ", settings->getObjectAddressDWORD, settings->getObjectAddressEvent));


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



public:
	GUIElementConstructorImpl(std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIElementStore> store, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings)
		: mStore(store)
	{
		// Create all top level GUI elements. Each one will recursively create it's nested elements, if it has any.
		for (auto& [element, game] : guireq->getToplevelGUIElements())
		{


			auto x = createGUIElementAndStoreResult(element, game, guireq, fail, info, settings);
			if (x.has_value())
			{
				store->getTopLevelGUIElementsMutable().at(game).push_back(x.value());
			}
		}
	}
};






GUIElementConstructor::GUIElementConstructor(std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIElementStore> store, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings) : pimpl(std::make_unique<GUIElementConstructorImpl>(guireq, fail, store, info, settings)) {}
GUIElementConstructor::~GUIElementConstructor() = default;
