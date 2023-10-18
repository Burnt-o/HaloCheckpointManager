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

class GUIElementConstructor::GUIElementConstructorImpl {
private:

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
							createNestedElement(GUIElementEnum::pauseAlsoBlocksInputGUI),
							createNestedElement(GUIElementEnum::pauseAlsoFreesCursorGUI),
							}));

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
							createNestedElement(GUIElementEnum::dumpCheckpointGUI),
							createNestedElement(GUIElementEnum::injectCoreGUI),
							createNestedElement(GUIElementEnum::dumpCoreGUI),
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

				case GUIElementEnum::dumpCheckpointGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::dumpCheckpoint, "Dump Checkpoint", settings->dumpCheckpointEvent));

				case GUIElementEnum::injectCoreGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::injectCore, "Inject Core Save", settings->injectCoreEvent));

				case GUIElementEnum::dumpCoreGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton<true>>
						(game, HotkeysEnum::dumpCore, "Dump Core Save", settings->dumpCoreEvent));

			case GUIElementEnum::cheatsHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Useful Cheats", headerChildElements
						{
							createNestedElement(GUIElementEnum::speedhackGUI),
							createNestedElement(GUIElementEnum::invulnGUI),
							createNestedElement(GUIElementEnum::aiFreezeGUI),
							createNestedElement(GUIElementEnum::consoleCommandGUI),
						}));

				case GUIElementEnum::speedhackGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISpeedhack>
						(game, HotkeysEnum::speedhack, settings));

				case GUIElementEnum::invulnGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInvulnerability>
						(game, HotkeysEnum::invuln, "Invulnerability", settings->invulnerabilityToggle,
							createNestedElement(GUIElementEnum::invulnNPCGUI))); // optional nested element, recursively calls this function

				case GUIElementEnum::invulnNPCGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISubHeading>
						(game, "Invulnerability Settings", headerChildElements
							{
							std::make_optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<false>>
							(game, std::nullopt, "NPC's invulnerable too", settings->invulnerabilityNPCToggle))
							}, 20.f));

				case GUIElementEnum::aiFreezeGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle<true>>
						(game, HotkeysEnum::aiFreeze, "Freeze AI", settings->aiFreezeToggle));

				case GUIElementEnum::consoleCommandGUI:
					return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIConsoleCommand>
						(game, std::nullopt, settings));

			case GUIElementEnum::overlaysHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Overlays", headerChildElements{ std::nullopt }));

			case GUIElementEnum::cameraHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Camera", headerChildElements{ std::nullopt }));

			case GUIElementEnum::theaterHeadingGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIHeading>
					(game, "Camera", headerChildElements{ std::nullopt }));


			



			default:
				throw HCMInitException(std::format("You forgot to add a creation case label for GUIElement: {} in {}::{}", magic_enum::enum_name(guielementenum), nameof(GUIElementConstructor), nameof(createGUIElement)));
			}


		}
		catch (HCMInitException ex)
		{
			info->addFailure(guielementenum, game, ex);
			return std::nullopt;
		}
	}


	std::shared_ptr<GUIElementStore> mStore;


public:
	GUIElementConstructorImpl(std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIElementStore> store, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings)
		: mStore(store)
	{
		// problem.. how do we deal with toplevelness? rn all gui elements would be in the guireq thing.. even nested ones.
		// do we need a seperate definition of which are top level?


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
