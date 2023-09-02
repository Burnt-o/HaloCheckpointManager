#include "pch.h"
#include "HotkeyManager.h"
#include "OptionsState.h"
#include "imgui.h"

#include "Hotkeys.h"
#include "HCMDirPath.h"
#include "MessagesGUI.h"

class HotkeyManager::HotkeyManagerImpl
{
private:


	// ImGuiManager Event reference and our handle to the append so we can remove it in destructor
	eventpp::CallbackList<void()>& pImGuiRenderEvent;
	eventpp::CallbackList<void()>::Handle mImGuiRenderCallbackHandle = {};



	static bool inline shouldHotkeyActivate(const std::vector<ImGuiKey>& bindingSet)
	{

		bool newPress = false;
		for (auto key : bindingSet)
		{
			if (!ImGui::IsKeyDown(key)) return false;		// check if all keys are held down

			// also check for a key being newly pressed. We need at least one key to be newly pressed to prevent continous firing.
			if (ImGui::IsKeyPressed(key)) newPress = true;
		}

		return newPress;
	}

	static void pollInput()// we poll every frame. Really we could've chosen any time interval but this is convienent
	{
			for (auto& hotkey: Hotkeys::allHotkeys)
			{
				for (auto& bindingSet : hotkey.get()->mBindings)
				{
					if (shouldHotkeyActivate(bindingSet))
					{
						hotkey.get()->invokeEvent();
						break; // continue to next hotkey, we don't need to check the rest of the bindingSets for this hotkey
					}
				}

			}
	}

	


public:

	HotkeyManagerImpl(eventpp::CallbackList<void()>& pRenderEvent) : pImGuiRenderEvent(pRenderEvent)
	{
		// so we can poll inputs every frame
		mImGuiRenderCallbackHandle = pImGuiRenderEvent.append(pollInput);


		// adding test hotkey for forceCheckpoint, where you have to simultaneous hold F1 on keyboard, D-pad left on gamepad, and mouse left click. Yes, this actually works (if you have 3 hands).

		



	}

	~HotkeyManagerImpl()
	{
		if (mImGuiRenderCallbackHandle)
			pImGuiRenderEvent.remove(mImGuiRenderCallbackHandle);
	}
};


void serialiseHotkey(const std::shared_ptr<Hotkey> hotkey, pugi::xml_node parent)
{
	auto mainNode = parent.append_child(hotkey.get()->getName().data());

	for (auto& binding : hotkey.get()->mBindings)
	{
		auto bindingNode = mainNode.append_child("BindingSet");
		for (auto key : binding)
		{
			auto keyNode = bindingNode.append_child("Key");
			keyNode.text().set(std::to_string((int)key).c_str());
		}
	}

}

void deserialiseHotkey(std::shared_ptr<Hotkey> hotkey, pugi::xml_node input)
{
	if (!input)
	{
		PLOG_ERROR << "could not deserialise hotkey " << hotkey.get()->getName() << ", no config data found";
	}

	hotkey.get()->mBindings.clear();
	for (pugi::xml_node bindingSetNode = input.first_child(); bindingSetNode; bindingSetNode = input.next_sibling())
	{
		std::vector<ImGuiKey> thisBindingSet;
		for (pugi::xml_node keyNode = bindingSetNode.first_child(); keyNode; keyNode = bindingSetNode.next_sibling())
		{
			// convert text to int (ImGuiKey)
			int key;
			try
			{
				key = std::stoi(keyNode.text().as_string());
			}
			catch (...)
			{
				PLOG_ERROR << "could not convert hotkey text: " << keyNode.text().as_string() << " to an integer";
				continue;
			}
			PLOG_VERBOSE << "adding key " << key << " to binding set";
			thisBindingSet.push_back((ImGuiKey)key);
		}
		PLOG_VERBOSE << "adding binding set with " << thisBindingSet.size() << " keys to mBindings";
		hotkey.get()->mBindings.push_back(thisBindingSet);
	}
	PLOG_VERBOSE << "hotkey loaded with " << hotkey.get()->mBindings.size() << " binding sets";

}




HotkeyManager::HotkeyManager(eventpp::CallbackList<void()>& pRenderEvent) : pimpl(std::make_unique<HotkeyManagerImpl>(pRenderEvent))
{
	if (instance) throw HCMInitException("Cannot have more than one HotkeyManager");
	instance = this;

	std::string filePath(HCMDirPath::GetHCMDirPath());
	mHotkeyConfigPath = filePath + "HCMHotkeyConfig.xml";
	// deserialise hotkeys
	if (!fileExists(mHotkeyConfigPath))
	{
		MessagesGUI::addMessage(std::format("Could not load hotkeys, HCMHotkeyConfig.xml not found. \nUsing default bindings."));
		PLOG_ERROR << "Could not deserialise hotkeys: Hotkey config file didn't exist at " << mHotkeyConfigPath;
	}
	else
	{
		pugi::xml_document hotkeyConfig;
		pugi::xml_parse_result result = hotkeyConfig.load_file(mHotkeyConfigPath.c_str());
		if (!result)
		{
			MessagesGUI::addMessage(std::format("Error parsing hotkey file: {}. \nUsing default bindings.", result.description()));
			PLOG_ERROR << "Error parsing hotkeyConfig file at " << mHotkeyConfigPath << ": " << result.description();
		}
		else
		{
			for (auto& hotkey : Hotkeys::allHotkeys)
			{
				deserialiseHotkey(hotkey, hotkeyConfig.child(hotkey.get()->getName().data()));
			}
		}

	}


}

HotkeyManager::~HotkeyManager()
{
	// serialise hotkeys

	pugi::xml_document hotkeyConfig;
	for (auto& hotkey : Hotkeys::allHotkeys)
	{
		serialiseHotkey(hotkey, hotkeyConfig);
	}

	if (!hotkeyConfig.save_file(mHotkeyConfigPath.c_str()))
	{
		PLOG_ERROR << "Error saving hotkeyConfig to " << mHotkeyConfigPath;
	}

	pimpl.reset();
	instance = nullptr;
}
