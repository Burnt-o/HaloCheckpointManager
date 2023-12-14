#pragma once
#include "HotkeysEnum.h"

// Renders hotkeys and rebinds them (by popup dialog)

class IHotkeyRendererImpl {
public:
	virtual void renderHotkey(std::optional<RebindableHotkeyEnum>, int pixelWidth) = 0;
	virtual ~IHotkeyRendererImpl() = default;
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
	void renderHotkey(std::optional<RebindableHotkeyEnum> h, int pixelWidth = 63) { pimpl->renderHotkey(h, pixelWidth); }

};

