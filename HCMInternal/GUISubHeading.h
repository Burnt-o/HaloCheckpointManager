#pragma once
#include "IGUIElement.h"

// Very similiar to GUIHeading but with less visual contrast and a parameter to add a left margin to it and it's child elements
class GUISubHeading : public IGUIElement {

private:
	std::string mHeadingText;
	std::vector<std::shared_ptr<IGUIElement>> mChildElements;
	bool hasElements;
	bool headingOpen = false;
	float mLeftMargin;
public:


	GUISubHeading(GameState implGame, ToolTipCollection tooltip, std::string headingText,  std::vector<std::optional<std::shared_ptr<IGUIElement>>> childElements, float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt, tooltip), mLeftMargin(leftMargin), mHeadingText(headingText)
	{
		PLOG_VERBOSE << "Constructing GUISubHeading, name: " << getName();

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

		ImGui::Dummy({ mLeftMargin, GUIFrameHeight }); // left margin
		ImGui::SameLine();

		ImGui::BeginChild(mHeadingText.c_str(), { 500 - mLeftMargin, currentHeight - GUISpacing });



		headingOpen = ImGui::TreeNodeEx(mHeadingText.c_str(), ImGuiTreeNodeFlags_FramePadding);
		renderTooltip();
		DEBUG_GUI_HEIGHT;
		if (headingOpen)
		{

			currentHeight = GUIFrameHeightWithSpacing;
			for (auto& element : mChildElements)
			{
				auto thisElementHeight = element->getCurrentHeight();
				currentHeight += thisElementHeight;

				//ImGui::Dummy({ mLeftMargin, thisElementHeight < 3 ? 0 : thisElementHeight - GUISpacing }); // left margin
				//ImGui::SameLine();
				element->render(hotkeyRenderer);
			}


			ImGui::TreePop();
		}
		else
		{
			currentHeight = GUIFrameHeightWithSpacing;
		}
		ImGui::EndChild();


	}

	std::string_view getName() override { return mHeadingText; }

};