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


	GUISubHeading(GameState implGame, std::string headingText,  std::vector<std::optional<std::shared_ptr<IGUIElement>>> childElements, float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt), mLeftMargin(leftMargin), mHeadingText(headingText)
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

		ImGui::Dummy({ mLeftMargin, 20 }); // left margin
		ImGui::SameLine();

		ImGui::BeginChild(mHeadingText.c_str(), { 500 - mLeftMargin, currentHeight });



		headingOpen = ImGui::TreeNodeEx(mHeadingText.c_str(), ImGuiTreeNodeFlags_FramePadding);

		if (headingOpen)
		{

			currentHeight = 23; // extra 3 pixels for frame padding between the tree node and the contained elements
			for (auto& element : mChildElements)
			{
				currentHeight += element->getCurrentHeight() + 3; // extra 3 pixels for frame padding between each element

				ImGui::Dummy({ mLeftMargin, 20 }); // left margin
				ImGui::SameLine();
				element->render(hotkeyRenderer);
			}

			ImGui::TreePop();
		}
		else
		{
			currentHeight = 20;
		}
		ImGui::EndChild();

	}

	std::string_view getName() override { return mHeadingText; }

};