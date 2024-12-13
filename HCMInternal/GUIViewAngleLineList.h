#pragma once
#include "pch.h"
#include "IGUIElement.h"
#include "RuntimeExceptionHandler.h"

// largely copy of GUIViewAngleLineList



template<bool startsOpen>
class GUIViewAngleLineList : public IGUIElement {

private:
	std::string mHeadingText = "View Angle Overlay List";
	std::weak_ptr<UnarySetting<ViewAngleLineList>> viewAngleLineListWeak;
	std::weak_ptr<ViewAngleLineAndListEvent> mEditEventWeak;
	std::weak_ptr<ViewAngleLineAndListEvent> mDeleteEventWeak;
	std::weak_ptr<ViewAngleLineListEvent> mAddEventWeak;
	std::shared_ptr<RuntimeExceptionHandler> mRuntimeExceptions;
	bool hasElements;
	bool headingOpen = false;
	float mLeftMargin;
	std::vector<std::thread> mFireEventThreads;
public:


	GUIViewAngleLineList(GameState implGame, ToolTipCollection tooltip, std::shared_ptr<UnarySetting<ViewAngleLineList>> viewAngleLineList,
		std::shared_ptr<ViewAngleLineAndListEvent> editEvent,
		std::shared_ptr<ViewAngleLineAndListEvent> deleteEvent,
		std::shared_ptr<ViewAngleLineListEvent> addEvent,
		std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions,
		float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt, tooltip), mLeftMargin(leftMargin), viewAngleLineListWeak(viewAngleLineList),
		mEditEventWeak(editEvent), mDeleteEventWeak(deleteEvent), mAddEventWeak(addEvent), mRuntimeExceptions(runtimeExceptions)
	{
		PLOG_VERBOSE << "Constructing GUIViewAngleLineList, name: " << getName();

		this->currentHeight = 0;

	}



	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		auto viewAngleLineListPtr = viewAngleLineListWeak.lock();
		if (!viewAngleLineListPtr)
		{
			PLOG_ERROR << "bad viewAngleLineList weakptr when rendering " << getName();
			return;
		}

		ImGui::Dummy({ mLeftMargin, GUIFrameHeight }); // left margin
		ImGui::SameLine();

		ImGui::BeginChild(mHeadingText.c_str(), { 500 - mLeftMargin, currentHeight - GUISpacing });


		constexpr ImGuiTreeNodeFlags treeFlags = startsOpen ? ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_FramePadding;
		headingOpen = ImGui::TreeNodeEx(mHeadingText.c_str(), treeFlags);
		renderTooltip();

		currentHeight = GUIFrameHeightWithSpacing;

		if (headingOpen)
		{
			if (viewAngleLineListPtr->GetValue().listInUse)
			{
				viewAngleLineListPtr->GetValue().listInUse.wait(true);
			}
			ScopedAtomicBool lock(viewAngleLineListPtr->GetValue().listInUse);
			auto& viewAngleLineList = viewAngleLineListPtr->GetValue();

			// draw add button
			if (ImGui::Button("Add New View Angle##GUIViewAngleLineList"))
			{
				PLOG_VERBOSE << "GUIViewAngleLineList firing add event";
				auto mEventToFire = mAddEventWeak.lock();
				if (!mEventToFire)
				{
					PLOG_ERROR << "bad mAddEventWeak weakptr when rendering " << getName();
					return;
				}

				auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire, &viewAngleLineList]() {mEvent->operator()(viewAngleLineList); }));
				newThread.detach();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Add a new viewAngleLine to the viewAngleLine list"); }
			currentHeight += GUIFrameHeightWithSpacing;

			// draw copy list to clipboard button
			if (ImGui::Button("Copy View Angle List##GUIViewAngleLineList"))
			{
				PLOG_VERBOSE << "GUIViewAngleLineList copying";
				std::stringstream ss;
				ss << "<viewAngleLine3DList>\n";
				ss << viewAngleLineList;
				ss << "</viewAngleLine3DList>";
				ImGui::SetClipboardText(ss.str().c_str());
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Copy the entire viewAngleLine list to the clipboard"); }
			currentHeight += GUIFrameHeightWithSpacing;

			// draw paste list from clipboard button
			if (ImGui::Button("Paste View Angle List##GUIViewAngleLineList"))
			{
				try
				{
					PLOG_VERBOSE << "GUIViewAngleLineList pasting";
					std::string clipboard = ImGui::GetClipboardText();
					viewAngleLineList.setListFromString(clipboard);
				}
				catch (HCMSerialisationException ex)
				{
					ex.prepend("Failed pasting viewAngleLine list: \n");
					mRuntimeExceptions->handleMessage(ex);
				}

			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Paste a viewAngleLine list from the clipboard"); }
			currentHeight += GUIFrameHeightWithSpacing;

			// draw viewAngleLine list
			ImGui::BeginTable("ViewAngleLineTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV);
			currentHeight += GUISpacing;
			if (viewAngleLineList.list.empty())
			{
				ImGui::TableNextColumn();
				currentHeight += GUIFrameHeightWithSpacing;
				ImGui::Dummy({ mLeftMargin, GUIFrameHeight }); // left margin
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("No View Angles yet!");
			}
			else
			{
				int viewAngleLineIndex = 0;
				for (auto& viewAngleLine : viewAngleLineList.list)
				{
					ImGui::TableNextColumn();
					currentHeight += GUIFrameHeightWithSpacing;
					renderViewAngleLine(viewAngleLine, viewAngleLineList, viewAngleLineIndex);
					viewAngleLineIndex++;
				}
			}

			ImGui::EndTable();

			ImGui::TreePop();
		}

		ImGui::EndChild();


	}

	void renderViewAngleLine(ViewAngleLine& viewAngleLine, ViewAngleLineList& viewAngleLineList, int viewAngleLineIndex)
	{
		std::string uid = std::format("{}", (uintptr_t)&viewAngleLine); // use viewAngleLine pointer as unique imgui id

		ImGui::AlignTextToFramePadding();
		ImGui::Text(std::format("{}", viewAngleLineIndex).c_str());
		ImGui::SameLine();

		// draw "Enabled" textbox
		if (ImGui::Checkbox(std::format("##{}", uid).c_str(), &viewAngleLine.viewAngleLineEnabled))
		{
			PLOG_VERBOSE << "viewAngleLine list toggling enabled flag";
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Whether this viewAngleLine is enabled (visible) or not."); }


		// draw edit button
		ImGui::SameLine();
		if (ImGui::Button(std::format("Edit##{}", uid).c_str()))
		{
			PLOG_VERBOSE << "GUIViewAngleLineList firing edit event";
			auto mEventToFire = mEditEventWeak.lock();
			if (!mEventToFire)
			{
				PLOG_ERROR << "bad mEditEventWeak weakptr when rendering " << getName();
				return;
			}

			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire, &viewAngleLine, &viewAngleLineList]() {mEvent->operator()(viewAngleLine, viewAngleLineList); }));
			newThread.detach();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Open the edit dialog with more settings for this specific viewAngleLine"); }


		// draw delete button
		ImGui::SameLine();
		if (ImGui::Button(std::format("Delete##{}", uid).c_str()))
		{
			PLOG_VERBOSE << "GUIViewAngleLineList firing delete event";
			auto mEventToFire = mDeleteEventWeak.lock();
			if (!mEventToFire)
			{
				PLOG_ERROR << "bad mDeleteEventWeak weakptr when rendering " << getName();
				return;
			}

			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire, &viewAngleLine, &viewAngleLineList]() {mEvent->operator()(viewAngleLine, viewAngleLineList); }));
			newThread.detach();

		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Deletes this viewAngleLine from the viewAngleLine list"); }


		// draw label text
		ImGui::SameLine();
		ImGui::Text(std::format("{}", viewAngleLine.label).c_str());

	}

	std::string_view getName() override { return mHeadingText; }

};