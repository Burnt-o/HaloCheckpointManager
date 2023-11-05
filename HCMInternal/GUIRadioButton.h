#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

class GUIRadioButton : public IGUIElement {

private:
	std::string mHeadingText;
	std::vector<std::shared_ptr<IGUIElement>> mChildElements;
public:

	std::weak_ptr<Setting<bool>> mBoundOption;


	GUIRadioButton(GameState implGame, ToolTipCollection tooltip, std::optional<HotkeysEnum> mHotkey, std::string headingText,  std::weak_ptr<Setting<bool>> boundOption, std::vector<std::optional<std::shared_ptr<IGUIElement>>> childElements)
		: IGUIElement(implGame, mHotkey, tooltip), mHeadingText(headingText), mBoundOption(boundOption)
	{
		PLOG_VERBOSE << "Constructing GUIRadioButton, name: " << getName();

		for (auto maybeElement : childElements)
		{
			if (maybeElement.has_value()) mChildElements.push_back(maybeElement.value());
		}


		this->currentHeight = 3 ;
	}



	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		
		currentHeight = 0;

		if (mChildElements.empty()) return;

		for (auto& element : mChildElements)
		{
			currentHeight += element->getCurrentHeight();
		}
		
		//

		DEBUG_GUI_HEIGHT;
			for (auto& element : mChildElements)
			{
				ImGui::Dummy({ 20, 2 }); // left margin
				ImGui::SameLine();
				element->render(hotkeyRenderer);
			}





	}

	std::string_view getName() override { return mHeadingText; }

};