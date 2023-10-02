#pragma once
#include "HotkeyEnum.h"

// Renders hotkeys and rebinds them (by popup dialog)

class IHotkeyRendererImpl {
public:
	virtual void renderHotkey(std::optional<HotkeysEnum>) = 0;
};


class HotkeyRenderer
{
private:
	std::unique_ptr<IHotkeyRendererImpl> pimpl;

public:
	HotkeyRenderer(std::unique_ptr<IHotkeyRendererImpl> impl) : pimpl(std::move(impl)) 
	{
		PLOG_DEBUG << "HotkeyRenderer constructor";
	}
	~HotkeyRenderer()
	{
		PLOG_DEBUG << "~HotkeyRenderer()";
	}

	// renders a hotkey if we have it, or blank padding space if we don't. If you don't want either than just don't call renderHotkey.
	void renderHotkey(std::optional<HotkeysEnum> h) { pimpl->renderHotkey(h); }

};

