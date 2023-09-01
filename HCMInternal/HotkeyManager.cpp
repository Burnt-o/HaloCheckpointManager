#include "pch.h"
#include "HotkeyManager.h"
#include "OptionsState.h"
#include "imgui.h"



class Hotkey //represents hotkeys for keyboard, gamepad, and mouse
{
private:
	std::shared_ptr<ActionEvent> mEventToFire;
public:


	bool mBound = false; // whether the hotkey is currently bound to anything or not
	// how to represent bindings? depends on sdl defs right? might have to move to pimpl.. and HotkeyManager just exposes methods for getting string collections of their names for GUI

	std::vector<ImGuiKey> bindings;



	Hotkey(std::shared_ptr<ActionEvent> eventToFire) : mEventToFire(eventToFire) {}

	void invokeEvent() 
	{ 
		PLOG_DEBUG << "Firing event via hotkey";
		mEventToFire.get()->operator()(); 
	}
};


class HotkeyManager::HotkeyManagerImpl
{
private:


	// ImGuiManager Event reference and our handle to the append so we can remove it in destructor
	eventpp::CallbackList<void()>& pImGuiRenderEvent;
	eventpp::CallbackList<void()>::Handle mImGuiRenderCallbackHandle = {};

	//static void pollInputOnNewThread() // polling needs to happen on seperate thread so we don't delay rendering
	//{
	//	std::thread newThread(pollInput);
	//}



	static void inline processHotkey(Hotkey& hotkey)
	{
		// check if any of the keys are newly pressed
		for (auto keyPress : hotkey.bindings)
		{
			if (ImGui::IsKeyPressed(keyPress, false))
			{
				// a hotkey key was pressed. Now check all other keys to make sure they are being held down
				for (auto keyDown : hotkey.bindings)
				{
					if (ImGui::IsKeyDown(keyDown) == false) return; // if a key isn't being held down, return early
				}

				// execution only reaches here if all keys are held down and at least one is newly pressed. Fire the event.
				PLOG_DEBUG << "invoking hotkey event";
				hotkey.invokeEvent();
			}
		}

	}

	static void pollInput()// we poll every frame. Really we could've chosen any time interval but this is convienent
	{
			for (auto& hotkey : allHotkeys)
			{
				processHotkey(hotkey);
			}
	}
	// could either ref optionState here and construct all hotkeys appropiately, or the GUIElements-
	static inline std::vector<Hotkey> allHotkeys{}; 

public:

	HotkeyManagerImpl(eventpp::CallbackList<void()>& pRenderEvent) : pImGuiRenderEvent(pRenderEvent)
	{
		// so we can poll inputs every frame
		mImGuiRenderCallbackHandle = pImGuiRenderEvent.append(pollInput);


		// adding test hotkey for forceCheckpoint, where you have to simultaneous hold F1 on keyboard, D-pad left on gamepad, and mouse left click. Yes, this actually works (if you have 3 hands).
		Hotkey testHotkey(OptionsState::forceCheckpointEvent);
		testHotkey.bindings.push_back(ImGuiKey_F1);
		testHotkey.bindings.push_back(ImGuiKey_GamepadDpadLeft);
		testHotkey.bindings.push_back(ImGuiKey_MouseLeft);
		allHotkeys.push_back(testHotkey);


	}

	~HotkeyManagerImpl()
	{
		if (mImGuiRenderCallbackHandle)
			pImGuiRenderEvent.remove(mImGuiRenderCallbackHandle);
	}
};


HotkeyManager::HotkeyManager(eventpp::CallbackList<void()>& pRenderEvent) : pimpl(std::make_unique<HotkeyManagerImpl>(pRenderEvent))
{
	if (instance) throw HCMInitException("Cannot have more than one HotkeyManager");
	instance = this;
}

HotkeyManager::~HotkeyManager()
{
	pimpl.reset();
	instance = nullptr;
}