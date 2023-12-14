#pragma once
#include "HotkeyRenderer.h"
#include "RebindableHotkey.h"
#include "HotkeyDefinitions.h"
#include "MessagesGUI.h"
#include "HotkeyManager.h"
class HotkeyRendererImpl : public IHotkeyRendererImpl
{
private:
	void beginHotkeyRebindDialog(std::shared_ptr<RebindableHotkey> hotkey);
	void renderHotkeyRebindDialog(SimpleMath::Vector2);

	// injected services
	std::shared_ptr<MessagesGUI> messagesGUI;
	std::shared_ptr<HotkeyManager> hotkeyManager;
	std::shared_ptr<HotkeyDefinitions> hotkeyDefinitions;

	//data
	std::shared_ptr<RebindableHotkey> mHotkeyToManipulate;
	int currentSelectedBindingForEditing = 0; // -1 means none selected
	std::vector<std::vector<ImGuiKey>> tempBindingSet{};


	// ImGuiManager Event reference and our handle to the append so we can remove it in destructor
	ScopedCallback<RenderEvent> mImGuiRenderCallbackHandle;

public:
	virtual void renderHotkey(std::optional<RebindableHotkeyEnum>, int pixelWidth) override;

	HotkeyRendererImpl(std::shared_ptr<RenderEvent> renderEvent, std::shared_ptr<MessagesGUI> mes, std::shared_ptr<HotkeyManager> hkm, std::shared_ptr<HotkeyDefinitions> hkd)
		: messagesGUI(mes),
		hotkeyManager(hkm),
		hotkeyDefinitions(hkd),
		mImGuiRenderCallbackHandle(renderEvent, [this](SimpleMath::Vector2 ss) {renderHotkeyRebindDialog(ss); })
	{
		PLOG_DEBUG << "HotkeyRendererImpl constructor";
	}

	~HotkeyRendererImpl()
	{
		PLOG_DEBUG << "~HotkeyRendererImpl";
	}





};

