#pragma once
#include "IModalDialog.h"
#include "WaypointList.h"
#include "RuntimeExceptionHandler.h"

class EditOrAddWaypointDialog : public IModalDialogReturner<std::optional<Waypoint>> // returns std::nullopt if user cancels
{
private:
	std::optional<SimpleMath::Vector3> mPlayerPosition;
	bool mCanMeasureDistance;
	std::shared_ptr<RuntimeExceptionHandler> mRuntimeExceptionHandler;
	const ImGuiColorEditFlags editFlags = ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoInputs;

public:
	EditOrAddWaypointDialog(std::shared_ptr<RuntimeExceptionHandler> runtimeExceptionHandler, std::string dialogTitle, Waypoint defaultWaypoint, std::optional<SimpleMath::Vector3> playerPosition, bool canMeasureDistance)
		: IModalDialogReturner(dialogTitle, defaultWaypoint),
		mPlayerPosition(playerPosition),
		mCanMeasureDistance(canMeasureDistance),
		mRuntimeExceptionHandler(runtimeExceptionHandler)
	{
	}


	void renderGlobalOption(bool& useGlobal, SimpleMath::Vector4& customColor, std::string optionName)
	{
		ImGui::Checkbox(std::format("Use Global {}", optionName).c_str(), &useGlobal);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

		{
			ImGui::BeginDisabled(useGlobal);

			ImGui::ColorEdit4(std::format("{}", optionName).c_str(), &customColor.x, editFlags);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			ImGui::EndDisabled();
		}
	}

	void renderGlobalOption(bool& useGlobal, float customScale, std::string optionName)
	{
		ImGui::Checkbox(std::format("Use Global {}", optionName).c_str(), &useGlobal);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

		{
			ImGui::BeginDisabled(useGlobal);

			ImGui::SetNextItemWidth(100.f);
			ImGui::InputFloat(std::format("{}", optionName).c_str(), &customScale);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			ImGui::EndDisabled();
		}
	}

	void renderGlobalOption(bool& useGlobal, int customPrecision, std::string optionName)
	{
		ImGui::Checkbox(std::format("Use Global {}", optionName).c_str(), &useGlobal);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

		{
			ImGui::BeginDisabled(useGlobal);

			ImGui::SetNextItemWidth(100.f);
			ImGui::InputInt(std::format("{}", optionName).c_str(), &customPrecision);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			ImGui::EndDisabled();
		}
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

		ImGui::InputText("Label", &workingWaypoint.label);

		ImGui::Checkbox("Enabled", &workingWaypoint.waypointEnabled);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

		{
			ImGui::Checkbox("Show Sprite", &workingWaypoint.showSprite);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			ImGui::BeginDisabled(workingWaypoint.showSprite == false);
			renderGlobalOption(workingWaypoint.spriteColorUseGlobal, workingWaypoint.spriteColor, "Sprite Color");
			renderGlobalOption(workingWaypoint.spriteScaleUseGlobal, workingWaypoint.spriteScale, "Sprite Scale");
			ImGui::EndDisabled();
		}

		{
			ImGui::Checkbox("Show Label", &workingWaypoint.showLabel);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			ImGui::BeginDisabled(workingWaypoint.showLabel == false);
			renderGlobalOption(workingWaypoint.labelColorUseGlobal, workingWaypoint.labelColor, "Label Text Color");
			renderGlobalOption(workingWaypoint.labelScaleUseGlobal, workingWaypoint.labelScale, "Label Text Scale");
			ImGui::EndDisabled();
		}

		if (mCanMeasureDistance) // requires optional services that might fail
		{
			ImGui::Checkbox("Show Distance Measure", &workingWaypoint.showDistance);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("todo"); }

			ImGui::BeginDisabled(workingWaypoint.showDistance == false);
			ImGui::Checkbox("Measure Horizontal Only", &workingWaypoint.measureHorizontalOnly);
			renderGlobalOption(workingWaypoint.distanceColorUseGlobal, workingWaypoint.distanceColor, "Distance Text Color");
			renderGlobalOption(workingWaypoint.distanceScaleUseGlobal, workingWaypoint.distanceScale, "Distance Text Scale");
			renderGlobalOption(workingWaypoint.distancePrecisionUseGlobal, workingWaypoint.distancePrecision, "Distance Text Precision");
			ImGui::EndDisabled();
		}



		// Copy to clipboard button
		if (ImGui::Button("Copy to Clipboard"))
		{
			pugi::xml_document doc;
			pugi::xml_node node = doc.append_child("");
			workingWaypoint.serialise(node);
			std::ostringstream ss;
			node.first_child().print(ss);
			ImGui::SetClipboardText(ss.str().c_str());
		}
	
		// Paste from clipboard button
		if (ImGui::Button("Paste from Clipboard"))
		{
			try
			{
				pugi::xml_document doc;
				auto parseResult = doc.load_string(ImGui::GetClipboardText()); // todo check parse result
				if (!parseResult)
				{
					throw HCMSerialisationException(std::format("Error parsing clipboard text \"{}\": {}", ImGui::GetClipboardText(), parseResult.description()));
				}

				Waypoint in(doc.first_child()); // constructor can throw HCMSerialisation
				workingWaypoint = in;
			}
			catch (HCMSerialisationException ex)
			{
				mRuntimeExceptionHandler->handleMessage(ex);
			}

		}

			

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