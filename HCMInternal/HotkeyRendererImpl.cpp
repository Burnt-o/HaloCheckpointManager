#include "pch.h"
#include "HotkeyRendererImpl.h"




void HotkeyRendererImpl::renderHotkey(std::optional<RebindableHotkeyEnum> hotkeyEnum, int pixelWidth)
{


	if (!hotkeyEnum.has_value())
	{
		ImGui::Dummy({ pixelWidth + 30.f,GUIFrameHeight }); // padding
	}
	else
	{
		if (!hotkeyDefinitions->getAllRebindableHotkeys().contains(hotkeyEnum.value()))
		{
			LOG_ONCE(PLOG_ERROR << "Error: bad hotkey passed to beginHotkeyRebindDialog");
			ImGui::Dummy({ pixelWidth + 30.f,GUIFrameHeight }); // padding
			return;
		}

		auto hotkey = hotkeyDefinitions->getAllRebindableHotkeys().at(hotkeyEnum.value());

		ImGui::PushID(hotkey->getName().data());
		ImGui::BeginChild("HotkeyChild", { (float)pixelWidth, GUIFrameHeight });
		ImGui::AlignTextToFramePadding();
		ImGui::Text(hotkey->getBindingTextShort().data());
		addTooltip(std::format("{} hotkeys: {}", hotkey->getName().data(), hotkey->getBindingText().data()).c_str());
		ImGui::EndChild();


		ImGui::SameLine();

		// button for rebinding hotkey
		if (ImGui::Button("..")) 
		{	
			ImGui::PopID(); // pop id before creating dialog
			PLOG_DEBUG << "creating hotkeyRebindDialog";
			hotkeyRebindDialog = std::make_unique<HotkeyRebindDialog>(hotkey, hotkeyDisabler->makeScopedRequest());
		}
		else
		{
			ImGui::PopID(); // pop id 
			// You'd think we could just put the PopID call before the ImGui::Button, but the button needs the id too.
		}
		addTooltip(std::format("Rebind hotkeys for {}", hotkey->getName().data()).c_str());


	}
}

void HotkeyRendererImpl::renderHotkeyRebindDialog(SimpleMath::Vector2 screenSize)
{
	if (hotkeyRebindDialog)
	{
		// render dialog
		hotkeyRebindDialog->render(screenSize);

		// close if done
		if (hotkeyRebindDialog->isFinished())
		{
			PLOG_DEBUG << "closing Hotkey Rebind Dialog";
			hotkeyRebindDialog.reset();
			hotkeyManager->saveHotkeysToFile();

		}
			


	}
}