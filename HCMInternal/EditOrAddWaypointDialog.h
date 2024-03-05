#pragma once
#include "IModalDialog.h"
#include "WaypointList.h"
#include "RuntimeExceptionHandler.h"

class EditOrAddWaypointDialog : public IModalDialogReturner<std::optional<Waypoint>> // returns std::nullopt if user cancels
{
private:
	std::optional<SimpleMath::Vector3> mPlayerPosition;
	bool mCanMeasureDistance;

public:
	EditOrAddWaypointDialog(std::string dialogTitle, Waypoint defaultWaypoint, std::optional<SimpleMath::Vector3> playerPosition, bool canMeasureDistance)
		: IModalDialogReturner(dialogTitle, defaultWaypoint),
		mPlayerPosition(playerPosition),
		mCanMeasureDistance(canMeasureDistance)
	{
	}

	virtual void renderInternal(SimpleMath::Vector2 screenSize) override
	{
		if (currentReturnValue.has_value() == false)
		{
			PLOG_ERROR << "Something went real bad if EditOrAddWaypointDialog.currentReturnValue is null here";
			return;
		}
		auto& workingWaypoint = currentReturnValue.value();
			
			// Inputs to allow user to edit various aspects of the workingWaypoint


			ImGui::InputText("Label: ", &workingWaypoint.label);

			ImGui::Checkbox("Enabled", &workingWaypoint.enabled);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			if (mCanMeasureDistance)
			{
				ImGui::Checkbox("Show Distance", &workingWaypoint.showDistance);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }
			}


			ImGui::SetNextItemWidth(100);
			ImGui::InputFloat("x position", &workingWaypoint.position.x, 0, 0, "%.6f");
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			ImGui::SetNextItemWidth(100);
			ImGui::InputFloat("y position", &workingWaypoint.position.y, 0, 0, "%.6f");
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			ImGui::SetNextItemWidth(100);
			ImGui::InputFloat("z position", &workingWaypoint.position.z, 0, 0, "%.6f");
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			if (mPlayerPosition.has_value())
			{
					if (ImGui::Button("Fill with player position"))
					{
						PLOG_DEBUG << "Filling waypoint with player position";
						workingWaypoint.position = mPlayerPosition.value();
					}
			}
			
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			// Ok button (return workingWaypoint)
			if (ImGui::Button("Ok") || ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				PLOG_DEBUG << "closing EditOrAddWaypointDialog with OK";


				closeDialog();
			}

			// Cancel button (return std::nullopt)
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				PLOG_DEBUG << "closing EditOrAddWaypointDialog with Cancel";
				currentReturnValue = std::nullopt;
				closeDialog();
			}



	}
};