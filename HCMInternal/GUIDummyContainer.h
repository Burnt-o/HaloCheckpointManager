#pragma once
#include "IGUIElement.h"

// Just adds a left margin to all its child elements
class GUIDummyContainer : public IGUIElement {

private:
	std::string mHeadingText;
	std::vector<std::shared_ptr<IGUIElement>> mChildElements;
	bool hasElements;
	bool headingOpen = false;
	float mLeftMargin;
public:


	GUIDummyContainer(GameState implGame, std::string headingText, std::vector<std::optional<std::shared_ptr<IGUIElement>>> childElements, float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt, ToolTipCollection("")), mLeftMargin(leftMargin), mHeadingText(headingText)
	{
		PLOG_VERBOSE << "Constructing GUIDummyContainer, name: " << getName();

		for (auto maybeElement : childElements)
		{
			if (maybeElement.has_value()) mChildElements.push_back(maybeElement.value());
		}

		hasElements = !mChildElements.empty();

		this->currentHeight = hasElements ? GUIFrameHeightWithSpacing : 0;

	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		//FORCEFULLY_DISABLE_GUIELEMENT;

		if (!hasElements) return;

		ImGui::Dummy({ mLeftMargin, 1 }); // left margin
		ImGui::SameLine();

		ImGui::BeginChild(mHeadingText.c_str(), { 500 - mLeftMargin, currentHeight - GUISpacing });

		currentHeight = 0;
		for (auto& element : mChildElements)
		{
			auto thisElementHeight = element->getCurrentHeight();
			currentHeight += thisElementHeight;

			//ImGui::Dummy({ mLeftMargin, thisElementHeight < 3 ? 0 : thisElementHeight - GUISpacing }); // left margin
			//ImGui::SameLine();
			element->render(hotkeyRenderer);
		}

		ImGui::EndChild();


	}

	std::string_view getName() override { return mHeadingText; }

};