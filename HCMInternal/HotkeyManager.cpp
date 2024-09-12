#include "pch.h"
#include "HotkeyManager.h"
#include "SettingsStateAndEvents.h"
#include "IMessagesGUI.h"
#include "pugixml.hpp"

//bool shouldHotkeyActivate(const std::vector<ImGuiKey>& bindingSet);
//void serialiseHotkey(const std::shared_ptr<Hotkey> hotkey, pugi::xml_node parent);
//void deserialiseHotkey(std::shared_ptr<Hotkey> hotkey, pugi::xml_node input);



class HotkeyManagerImpl {
public:
	static bool shouldHotkeyActivate(const BindingCombo& bindingSet);
	static void serialiseHotkey(const std::shared_ptr<RebindableHotkey> hotkey, pugi::xml_node parent);
	static std::optional<std::vector<BindingCombo>> deserialiseHotkey(std::shared_ptr<RebindableHotkey> hotkey, pugi::xml_node input);
};



HotkeyManager::HotkeyManager(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr<HotkeyDefinitions> pHotkeyDefinitions, std::shared_ptr<IMessagesGUI> messagesGUI, std::string dirPath, std::shared_ptr<TokenSharedRequestProvider> hotkeyDisabler)
	: mHotkeyDefinitions(pHotkeyDefinitions),
	mImGuiRenderCallbackHandle(pRenderEvent, [this](SimpleMath::Vector2) { pollInput(); }), 	// setup render callback to poll hotkeys every frame
	mHotkeyDisabler(hotkeyDisabler)
{
	// set hotkey config file path
	mHotkeyConfigPath = dirPath + "HCMHotkeyConfig.xml";

	if (!fileExists(mHotkeyConfigPath)) // use default bindings
	{
		messagesGUI->addMessage(std::format("Could not load hotkeys, HCMHotkeyConfig.xml not found. \nUsing default bindings."));
		PLOG_ERROR << "Could not deserialise hotkeys: Hotkey config file didn't exist at " << mHotkeyConfigPath;
	}
	else // deseralise saved bindings
	{
		PLOG_DEBUG << "Found hotkey file at path: " << mHotkeyConfigPath << ", parsing xml document";
		pugi::xml_document hotkeyConfig;
		pugi::xml_parse_result result = hotkeyConfig.load_file(mHotkeyConfigPath.c_str());



		if (!result)
		{
			messagesGUI->addMessage(std::format("Error parsing hotkey file: {}. \nUsing default bindings.", result.description()));
			PLOG_ERROR << "Error parsing hotkeyConfig file at " << mHotkeyConfigPath << ": " << result.description();
		}
		else
		{
			PLOG_INFO << "Successfully parsed hotkeyConfig xml document";
			std::stringstream ss;
			hotkeyConfig.print(ss);
			PLOG_VERBOSE << "Hotkey file contents: " << std::endl << ss.str();

			for (auto& [hotkeyEnum, hotkey] : mHotkeyDefinitions->getAllRebindableHotkeys())
			{
				auto deserialisedBindings = HotkeyManagerImpl::deserialiseHotkey(hotkey, hotkeyConfig.child(hotkey->getName().data()));
				if (deserialisedBindings.has_value())
					hotkey->setBindings(deserialisedBindings.value());
			}
		}
	}



}

HotkeyManager::~HotkeyManager()
{
	PLOG_VERBOSE << "~HotkeyManager() serialising " << mHotkeyDefinitions->getAllRebindableHotkeys().size() << " hotkeys";
	// serialise hotkeys
	pugi::xml_document hotkeyConfig;
	
	for (auto& [hotkeyEnum, hotkey] : mHotkeyDefinitions->getAllRebindableHotkeys())
	{
		HotkeyManagerImpl::serialiseHotkey(hotkey, hotkeyConfig);
	}

	std::stringstream ss;
	hotkeyConfig.print(ss);
	PLOG_VERBOSE << "hotkeyConfig document contents: " << std::endl << ss.str();

	PLOG_DEBUG << "Saving hotkeys to path: " << mHotkeyConfigPath;
	if (!hotkeyConfig.save_file(mHotkeyConfigPath.c_str()))
	{
		PLOG_ERROR << "Error saving hotkeyConfig to " << mHotkeyConfigPath;
	}
	else
	{
		PLOG_INFO << "Successfully saved hotkey file!";
	}

	for (auto& thread : mFireEventThreads)
	{
		if (thread.joinable())
		{
			PLOG_DEBUG << "~HotkeyManager " << "joining mFireEventThread";
			thread.join();
			PLOG_DEBUG << "~HotkeyManager " << "FireEventThread finished";
		}
	}

}

void HotkeyManager::pollInput()// we poll every frame. Really we could've chosen any time interval but this is convienent
{
	bool hotkeysShouldBeDisabled = mHotkeyDisabler->serviceIsRequested();


	for (auto& [hotkeyEnum, hotkey] : mHotkeyDefinitions->getAllEventOnPressHotkeys())
	{
		if (hotkeysShouldBeDisabled && !hotkey->ignoresHotkeyDisabler)
			continue;


		for (auto& bindingSet : hotkey->getBindings())
		{
			if (HotkeyManagerImpl::shouldHotkeyActivate(bindingSet))
			{
				auto& newThread = mFireEventThreads.emplace_back(std::thread([hk = hotkey]() {hk->invokeEvent(); }));
				newThread.detach();
				break; // continue to next hotkey, we don't need to check the rest of the bindingSets for this hotkey
			}
		}

	}
}


bool inline HotkeyManagerImpl::shouldHotkeyActivate(const BindingCombo& bindingSet)
{

	bool newPress = false;
	for (auto key : bindingSet)
	{
		if (!ImGui::IsKeyDown(key)) return false;		// check if all keys are held down

		// also check for a key being newly pressed. We need at least one key to be newly pressed to prevent continous firing.
		if (ImGui::IsKeyPressed(key, false)) newPress = true;

	}

	return newPress;
}

	


void HotkeyManagerImpl::serialiseHotkey(const std::shared_ptr<RebindableHotkey> hotkey, pugi::xml_node parent)
{
	auto mainNode = parent.append_child(hotkey->getName().data());

	for (auto& binding : hotkey->getBindings())
	{
		auto bindingNode = mainNode.append_child("BindingSet");
		for (auto key : binding)
		{
			auto keyNode = bindingNode.append_child("Key");
			keyNode.text().set(std::to_string((int)key).c_str());
		}
	}

}


std::optional<std::vector<BindingCombo>> HotkeyManagerImpl::deserialiseHotkey(std::shared_ptr<RebindableHotkey> hotkey, pugi::xml_node input)
{

	if (!input)
	{
		PLOG_ERROR << "could not deserialise hotkey " << hotkey->getName() << ", no config data found";
		return std::nullopt;
	}

	PLOG_VERBOSE << "Deserialising hotkey: " << magic_enum::enum_name(hotkey->mHotkeyEnum);

	std::vector<BindingCombo> newBindings;
	if (input.first_child())
	{
		for (pugi::xml_node bindingSetNode = input.first_child(); bindingSetNode; bindingSetNode = bindingSetNode.next_sibling())
		{

			BindingCombo thisBindingSet;

			if (bindingSetNode.first_child())
			{
				for (pugi::xml_node keyNode = bindingSetNode.first_child(); keyNode; keyNode = keyNode.next_sibling())
				{
					// convert text to int (ImGuiKey)
					int key = keyNode.text().as_int(-1);
					if (key == -1)
					{
						PLOG_ERROR << "could not convert hotkey text: " << keyNode.text().as_string() << " to an integer. E";
					}
					else
					{
						// does GetKeyName throw if fed a bad int?
						PLOG_VERBOSE << "adding key " << ImGui::GetKeyName((ImGuiKey)key) << " to binding set";
						thisBindingSet.insert((ImGuiKey)key);
					}



				}
			}


			if (thisBindingSet.empty())
			{
				PLOG_VERBOSE << "empty deserialised binding set";
				continue;
			}
			PLOG_VERBOSE << "adding binding set with " << thisBindingSet.size() << " keys to mBindings";
			newBindings.push_back(thisBindingSet);
		}
	}
	


	if (newBindings.empty())
	{
		PLOG_VERBOSE << "empty newBindings";
		return std::vector<BindingCombo>{}; // empty
	}

	if (newBindings.size() > 20)
	{
		PLOG_ERROR << "newBindings size was over 20 - probably from glitched command console hotkey bug";
		return std::vector<BindingCombo>{}; // empty
	}

	return newBindings;

	PLOG_VERBOSE << "hotkey loaded with " << newBindings.size() << " binding sets";

}


