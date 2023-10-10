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



class GUIElementConstructor::GUIElementConstructorImpl {
private:
	std::optional<std::shared_ptr<IGUIElement>> createGUIElement(GUIElementEnum guielementenum, GameState game, std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings)
	{
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

			// ALL GUI ELEMENTS MUST HAVE A CASE HERE, TOP LEVEL OR NOT
			std::optional<std::shared_ptr<IGUIElement>> x;
			switch (guielementenum)
			{
			case GUIElementEnum::forceCheckpointGUI:
				x = std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::forceCheckpoint, "Force Checkpoint", settings->forceCheckpointEvent));
					PLOG_DEBUG << "successfully made forceCheckpointGUI";
				return x;

			case GUIElementEnum::forceRevertGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::forceRevert, "Force Revert", settings->forceRevertEvent));

			case GUIElementEnum::forceDoubleRevertGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::forceDoubleRevert, "Force Double Revert", settings->forceDoubleRevertEvent));

			case GUIElementEnum::forceCoreSaveGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::forceCoreSave, "Force Core Save", settings->forceCoreSaveEvent));

			case GUIElementEnum::forceCoreLoadGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::forceCoreLoad, "Force Core Load", settings->forceCoreLoadEvent));

			case GUIElementEnum::injectCheckpointGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::injectCheckpoint, "Inject Checkpoint", settings->injectCheckpointEvent));

			case GUIElementEnum::dumpCheckpointGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::dumpCheckpoint, "Dump Checkpoint", settings->dumpCheckpointEvent));

			case GUIElementEnum::injectCoreGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::injectCore, "Inject Core Save", settings->injectCoreEvent));

			case GUIElementEnum::dumpCoreGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleButton>
					(game, HotkeysEnum::dumpCore, "Dump Core Save", settings->dumpCoreEvent));

			case GUIElementEnum::speedhackGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISpeedhack>
					(game, HotkeysEnum::speedhack, settings));

			case GUIElementEnum::invulnGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUIInvulnerability>
					(game, HotkeysEnum::invuln, "Invulnerability", settings->invulnerabilityToggle,
						createGUIElement(GUIElementEnum::invulnNPCGUI, game, guireq, fail, info, settings))); // optional nested element, recursively calls this function

			case GUIElementEnum::invulnNPCGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle>
					(game, std::nullopt, "NPC Invulnerability", settings->invulnerabilityNPCToggle));

			case GUIElementEnum::aiFreezeGUI:
				return std::optional<std::shared_ptr<IGUIElement>>(std::make_shared<GUISimpleToggle>
					(game, HotkeysEnum::aiFreeze, "Freeze AI", settings->aiFreezeToggle));
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





public:
	GUIElementConstructorImpl(std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIElementStore> store, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings)
	{
		// problem.. how do we deal with toplevelness? rn all gui elements would be in the guireq thing.. even nested ones.
		// do we need a seperate definition of which are top level?


		for (auto& [element, game] : guireq->getToplevelGUIElements())
		{
			auto x = createGUIElement(element, game, guireq, fail, info, settings);
			if (x.has_value())
			{
				store->getTopLevelGUIElementsMutable().at(game).push_back(x.value());
				store->mapOfSuccessfullyConstructedGUIElements.at(game).insert(element);
			}
		}

	}
};






GUIElementConstructor::GUIElementConstructor(std::shared_ptr<IGUIRequiredServices> guireq, std::shared_ptr<OptionalCheatInfo> fail, std::shared_ptr<GUIElementStore> store, std::shared_ptr<GUIServiceInfo> info, std::shared_ptr<SettingsStateAndEvents> settings) : pimpl(std::make_unique<GUIElementConstructorImpl>(guireq, fail, store, info, settings)) {}
GUIElementConstructor::~GUIElementConstructor() = default;
