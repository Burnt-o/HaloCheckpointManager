#pragma once
#include "IGUIElement.h"

class GUIHeading : public IGUIElement {

private:
	std::string mHeadingText;
	std::vector<std::shared_ptr<IGUIElement>> mChildElements;
	bool hasElements;
	bool headingCollapsed = false;
public:


	GUIHeading(GameState implGame, std::string headingText, std::vector<std::optional<std::shared_ptr<IGUIElement>>> childElements)
		: IGUIElement(implGame, std::nullopt), mHeadingText(headingText)
	{
		PLOG_VERBOSE << "Constructing GUIHeading, name: " << getName();

		for (auto maybeElement : childElements)
		{
			if (maybeElement.has_value()) mChildElements.push_back(maybeElement.value());
		}

		hasElements = !mChildElements.empty();

		this->currentHeight = hasElements ? GUIFrameHeightWithSpacing : 0;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		
		if (!hasElements) return;



		headingCollapsed = ImGui::CollapsingHeader(mHeadingText.c_str(), ImGui::IsItemHovered());
		DEBUG_GUI_HEIGHT;
		currentHeight = GUIFrameHeightWithSpacing;
		if (!headingCollapsed)
		{
			for (auto& element : mChildElements)
			{
				ImGui::Dummy({ 5, element->getCurrentHeight() == 0.f ? 0.f : 1.f }); // very small left margin
				ImGui::SameLine();
				currentHeight += element->getCurrentHeight();
				element->render(hotkeyRenderer);
			}
		}



	}

	std::string_view getName() override { return mHeadingText; }

};