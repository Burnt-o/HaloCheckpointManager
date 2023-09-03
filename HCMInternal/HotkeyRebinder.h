#pragma once
#include "Hotkey.h"
class HotkeyRebinder
{
private:
	static inline HotkeyRebinder* instance = nullptr;
	// ImGuiManager Event reference and our handle to the append so we can remove it in destructor
	eventpp::CallbackList<void()>& pImGuiRenderEvent;
	eventpp::CallbackList<void()>::Handle mImGuiRenderCallbackHandle = {};

	std::vector<std::vector<ImGuiKey>> tempBindingSet{};
	int currentSelectedBindingForEditing = 0; // -1 means none selected
	std::shared_ptr<Hotkey> mHotkeyToManipulate{};
	void renderHotkeyRebindDialog();

public:
	HotkeyRebinder(ActionEvent& renderEvent) : pImGuiRenderEvent(renderEvent)
	{
		if (instance) throw HCMInitException("Cannot have more than one ModalDialogManager");
		instance = this;
		mImGuiRenderCallbackHandle = pImGuiRenderEvent.append([this]() {renderHotkeyRebindDialog(); });
	}
	~HotkeyRebinder()
	{
		if (mImGuiRenderCallbackHandle)
			pImGuiRenderEvent.remove(mImGuiRenderCallbackHandle);
	}
	static void beginHotkeyRebindDialog(std::shared_ptr<Hotkey> hotkey);
};

