#pragma once
#include "pch.h"
#include "IGUIElement.h"

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
	bool hasElements;
	bool headingOpen = false;
	float mLeftMargin;
	std::vector<std::thread> mFireEventThreads;
public:


	GUIWaypointList(GameState implGame, ToolTipCollection tooltip, std::shared_ptr<UnarySetting<WaypointList>> waypointList,
		std::shared_ptr<WaypointAndListEvent> editEvent,
		std::shared_ptr<WaypointAndListEvent> deleteEvent,
		std::shared_ptr<WaypointListEvent> addEvent,
		float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt, tooltip), mLeftMargin(leftMargin), waypointListWeak(waypointList),
		mEditEventWeak(editEvent), mDeleteEventWeak(deleteEvent), mAddEventWeak(addEvent)
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

		

		ImGui::BeginChild(mHeadingText.c_str(), { 500 - mLeftMargin, currentHeight - GUISpacing });


		constexpr ImGuiTreeNodeFlags treeFlags = startsOpen ? ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_FramePadding;
		headingOpen = ImGui::TreeNodeEx(mHeadingText.c_str(), treeFlags);
		renderTooltip();
		DEBUG_GUI_HEIGHT;
		if (headingOpen)
		{
			if (waypointListPtr->GetValue().listInUse)
			{
				waypointListPtr->GetValue().listInUse.wait(true);
			}
			ScopedAtomicBool lock(waypointListPtr->GetValue().listInUse);
			auto& waypointList = waypointListPtr->GetValue();

			currentHeight = GUIFrameHeightWithSpacing;
			for (auto& waypoint : waypointList.list)
			{
				currentHeight += GUIFrameHeightWithSpacing;
				renderWaypoint(waypoint, waypointList);
			}


			// draw add button
			currentHeight += GUIFrameHeightWithSpacing;
			if (ImGui::Button("Add Waypoint##GUIWaypointList"))
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

			ImGui::TreePop();
		}
		else
		{
			currentHeight = GUIFrameHeightWithSpacing;
		}
		ImGui::EndChild();


	}

	void renderWaypoint(Waypoint& waypoint, WaypointList& waypointList)
	{
		std::string uid = std::format("{}", (uintptr_t)& waypoint); // use waypoint pointer as unique imgui id

		ImGui::Dummy({ mLeftMargin, GUIFrameHeight }); // left margin
		ImGui::SameLine();

		// draw "Enabled" textbox
		if (ImGui::Checkbox(std::format("##{}",uid).c_str(), &waypoint.enabled))
		{
			PLOG_VERBOSE << "waypoint list toggling enabled flag";
		}



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

		// draw label text
		ImGui::SameLine();
		ImGui::Text(std::format("{}", waypoint.label).c_str());

	}

	std::string_view getName() override { return mHeadingText; }

};