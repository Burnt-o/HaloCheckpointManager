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

		this->currentHeight = hasElements ? 20 : 0;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		if (!hasElements) return;

		headingCollapsed = ImGui::CollapsingHeader(mHeadingText.c_str(), ImGui::IsItemHovered());
		if (!headingCollapsed)
		{
			currentHeight = 0;
			for (auto& element : mChildElements)
			{
				ImGui::Dummy({ 5, 20 }); // very small left margin
				ImGui::SameLine();
				currentHeight += element->getCurrentHeight();
				element->render(hotkeyRenderer);
			}
		}
		else
		{
			currentHeight = 20;
		}
	}

	std::string_view getName() override { return mHeadingText; }

};