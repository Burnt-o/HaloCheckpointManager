#pragma once
#include "pch.h"
#include "IGUIElement.h"
#include "RuntimeExceptionHandler.h"
/* Lists the waypoints of the 3D waypoint utility.
Each waypoint shows:
* Enabled toggle
* Label Name
* Edit button (spawns modal)
* Delete button (just removes waypoint from the list)
 
 */
template<bool startsOpen>
class GUIWaypointList : public IGUIElement {

private:
	std::string mHeadingText = "Waypoint List";
	std::weak_ptr<UnarySetting<WaypointList>> waypointListWeak;
	std::weak_ptr<WaypointAndListEvent> mEditEventWeak;
	std::weak_ptr<WaypointAndListEvent> mDeleteEventWeak;
	std::weak_ptr<WaypointListEvent> mAddEventWeak;
	std::shared_ptr<RuntimeExceptionHandler> mRuntimeExceptions;
	bool hasElements;
	bool headingOpen = false;
	float mLeftMargin;
	std::vector<std::thread> mFireEventThreads;
public:


	GUIWaypointList(GameState implGame, ToolTipCollection tooltip, std::shared_ptr<UnarySetting<WaypointList>> waypointList,
		std::shared_ptr<WaypointAndListEvent> editEvent,
		std::shared_ptr<WaypointAndListEvent> deleteEvent,
		std::shared_ptr<WaypointListEvent> addEvent,
		std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions,
		float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt, tooltip), mLeftMargin(leftMargin), waypointListWeak(waypointList),
		mEditEventWeak(editEvent), mDeleteEventWeak(deleteEvent), mAddEventWeak(addEvent), mRuntimeExceptions(runtimeExceptions)
	{
		PLOG_VERBOSE << "Constructing GUIWaypointList, name: " << getName();

		this->currentHeight = 0;

	}



	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		auto waypointListPtr = waypointListWeak.lock();
		if (!waypointListPtr)
		{
			PLOG_ERROR << "bad waypointList weakptr when rendering " << getName();
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
			if (waypointListPtr->GetValue().listInUse)
			{
				waypointListPtr->GetValue().listInUse.wait(true);
			}
			ScopedAtomicBool lock(waypointListPtr->GetValue().listInUse);
			auto& waypointList = waypointListPtr->GetValue();

			// draw add button
			if (ImGui::Button("Add New Waypoint##GUIWaypointList"))
			{
				PLOG_VERBOSE << "GUIWaypointList firing add event";
				auto mEventToFire = mAddEventWeak.lock();
				if (!mEventToFire)
				{
					PLOG_ERROR << "bad mAddEventWeak weakptr when rendering " << getName();
					return;
				}

				auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire, &waypointList]() {mEvent->operator()(waypointList); }));
				newThread.detach();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Add a new waypoint to the waypoint list"); }
			currentHeight += GUIFrameHeightWithSpacing;

			// draw copy list to clipboard button
			if (ImGui::Button("Copy Waypoint List##GUIWaypointList"))
			{
				PLOG_VERBOSE << "GUIWaypointList copying";
				std::stringstream ss;
				ss << "<waypoint3DList>\n";
				ss << waypointList;
				ss << "</waypoint3DList>";
				ImGui::SetClipboardText(ss.str().c_str());
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Copy the entire waypoint list to the clipboard"); }
			currentHeight += GUIFrameHeightWithSpacing;

			// draw paste list from clipboard button
			if (ImGui::Button("Paste Waypoint List##GUIWaypointList"))
			{
				try
				{
					PLOG_VERBOSE << "GUIWaypointList pasting";
					std::string clipboard = ImGui::GetClipboardText();
					waypointList.setListFromString(clipboard);
				}
				catch (HCMSerialisationException ex)
				{
					ex.prepend("Failed pasting waypoint list: \n");
					mRuntimeExceptions->handleMessage(ex);
				}

			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Paste a waypoint list from the clipboard"); }
			currentHeight += GUIFrameHeightWithSpacing;

			// draw waypoint list
			ImGui::BeginTable("WaypointTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV);
			currentHeight += GUISpacing;
			if (waypointList.list.empty())
			{
				ImGui::TableNextColumn();
				currentHeight += GUIFrameHeightWithSpacing;
				ImGui::Dummy({ mLeftMargin, GUIFrameHeight }); // left margin
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("No waypoints yet!");
			}
			else
			{
				int waypointIndex = 0;
				for (auto& waypoint : waypointList.list)
				{
					ImGui::TableNextColumn();
					currentHeight += GUIFrameHeightWithSpacing;
					renderWaypoint(waypoint, waypointList, waypointIndex);
					waypointIndex++;
				}
			}

			ImGui::EndTable();

			ImGui::TreePop();
		}

		ImGui::EndChild();


	}

	void renderWaypoint(Waypoint& waypoint, WaypointList& waypointList, int waypointIndex)
	{
		std::string uid = std::format("{}", (uintptr_t)& waypoint); // use waypoint pointer as unique imgui id

		ImGui::AlignTextToFramePadding();
		ImGui::Text(std::format("{}", waypointIndex).c_str());
		ImGui::SameLine();

		// draw "Enabled" textbox
		if (ImGui::Checkbox(std::format("##{}",uid).c_str(), &waypoint.waypointEnabled))
		{
			PLOG_VERBOSE << "waypoint list toggling enabled flag";
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Whether this waypoint is enabled (visible) or not."); }


		// draw edit button
		ImGui::SameLine();
		if (ImGui::Button(std::format("Edit##{}", uid).c_str()))
		{
			PLOG_VERBOSE << "GUIWaypointList firing edit event";
			auto mEventToFire = mEditEventWeak.lock();
			if (!mEventToFire)
			{
				PLOG_ERROR << "bad mEditEventWeak weakptr when rendering " << getName();
				return;
			}

			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire, &waypoint,&waypointList]() {mEvent->operator()(waypoint, waypointList); }));
			newThread.detach();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Open the edit dialog with more settings for this specific waypoint"); }


		// draw delete button
		ImGui::SameLine();
		if (ImGui::Button(std::format("Delete##{}", uid).c_str()))
		{
			PLOG_VERBOSE << "GUIWaypointList firing delete event";
			auto mEventToFire = mDeleteEventWeak.lock();
			if (!mEventToFire)
			{
				PLOG_ERROR << "bad mDeleteEventWeak weakptr when rendering " << getName();
				return;
			}

			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire, &waypoint, &waypointList]() {mEvent->operator()(waypoint, waypointList); }));
			newThread.detach();

		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Deletes this waypoint from the waypoint list"); }


		// draw label text
		ImGui::SameLine();
		ImGui::Text(std::format("{}", waypoint.label).c_str());

	}

	std::string_view getName() override { return mHeadingText; }

};