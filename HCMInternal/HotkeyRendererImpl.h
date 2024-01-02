#pragma once
#include "HotkeyRenderer.h"
#include "RebindableHotkey.h"
#include "HotkeyDefinitions.h"
#include "MessagesGUI.h"
#include "HotkeyManager.h"
#include "ScopedServiceRequest.h"

class HotkeyRendererImpl : public IHotkeyRendererImpl
{
private:
	void beginHotkeyRebindDialog(std::shared_ptr<RebindableHotkey> hotkey);
	void renderHotkeyRebindDialog(SimpleMath::Vector2);

	// injected services
	std::shared_ptr<MessagesGUI> messagesGUI;
	std::shared_ptr<HotkeyManager> hotkeyManager;
	std::shared_ptr<HotkeyDefinitions> hotkeyDefinitions;
	std::shared_ptr<GenericScopedServiceProvider> hotkeyDisabler;

	//data
	std::shared_ptr<RebindableHotkey> mHotkeyToManipulate;
	int currentSelectedBindingForEditing = 0; // -1 means none selected
	std::vector<std::vector<ImGuiKey>> tempBindingSet{};
	std::optional<std::unique_ptr<ScopedServiceRequest>> hotkeyDisableRequest = std::nullopt;


	// ImGuiManager Event reference and our handle to the append so we can remove it in destructor
	ScopedCallback<RenderEvent> mImGuiRenderCallbackHandle;

public:
	virtual void renderHotkey(std::optional<RebindableHotkeyEnum>, int pixelWidth) override;

	HotkeyRendererImpl(std::shared_ptr<RenderEvent> renderEvent, std::shared_ptr<MessagesGUI> mes, std::shared_ptr<HotkeyManager> hkm, std::shared_ptr<HotkeyDefinitions> hkd, std::shared_ptr<GenericScopedServiceProvider> hkDisabler)
		: messagesGUI(mes),
		hotkeyManager(hkm),
		hotkeyDefinitions(hkd),
		hotkeyDisabler(hkDisabler),
		mImGuiRenderCallbackHandle(renderEvent, [this](SimpleMath::Vector2 ss) {renderHotkeyRebindDialog(ss); })
	{
		PLOG_DEBUG << "HotkeyRendererImpl constructor";
	}

	~HotkeyRendererImpl()
	{
		PLOG_DEBUG << "~HotkeyRendererImpl";
	}





};

