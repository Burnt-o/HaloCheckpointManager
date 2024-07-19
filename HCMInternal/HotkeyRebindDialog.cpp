#include "pch.h"
#include "HotkeyRebindDialog.h"
#include "imgui.h"
#include "imgui_internal.h"

HotkeyRebindDialog::HotkeyRebindDialog(std::shared_ptr<RebindableHotkey> hotkeyToChange, std::unique_ptr<ScopedServiceRequest> hotkeyDisableRequest)
	: mHotkeyToChange(hotkeyToChange), mHotkeyDisableRequest(std::move(hotkeyDisableRequest)), workingBindingSet(hotkeyToChange->getBindings())
{
	PLOG_DEBUG << "opening popup for Rebind Hotkey";
	// since the stack id context is different here (because of HCMInternalGUI BeginWindow call), we need to manually
	// override/calculate the ID so that we can access it from the different context in render
	ImGuiID popup_id = ImHashStr("Rebind Hotkey");
	ImGui::PushOverrideID(popup_id); // since we're calling from a different window context than the OpenPopup
	ImGui::OpenPopup("Rebind Hotkey");
	ImGui::PopID();
}




void HotkeyRebindDialog::render(SimpleMath::Vector2 screenSize)
{
	if (mIsFinished)
		return;


	ImGui::SetNextWindowPos(screenSize / 2, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGuiID popup_id = ImHashStr("Rebind Hotkey");
	ImGui::PushOverrideID(popup_id); // since we're calling from a different window context than the OpenPopup
	if (ImGui::BeginPopupModal("Rebind Hotkey", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{

		{
			std::optional<std::vector<BindingCombo>::iterator> queuedBindingErasure;

			int idCount = 0; // imgui elements need unique ids
			for (auto bindingComboIter = workingBindingSet.begin(); bindingComboIter != workingBindingSet.end(); ++bindingComboIter)
			{
				idCount++;
				std::string currentBindingText = RebindableHotkey::generateBindingTextSingle(*bindingComboIter);

				bool isActiveEditSelection = currentSelectedBindingForEditing.has_value() && currentSelectedBindingForEditing.value() == bindingComboIter;

				ImGui::BeginChild(std::format("InputField{}", idCount).c_str(), { 400, 30 }, true, ImGuiWindowFlags_NoResize);
				if (!isActiveEditSelection) ImGui::BeginDisabled();
				ImGui::Text(currentBindingText.c_str());
				if (!isActiveEditSelection) ImGui::EndDisabled();
				ImGui::EndChild();

				// used for hit testing mouse hotkeys. Call must be here as it's getting the rect of the InputField window.
				auto inputFieldPosMin = ImGui::GetItemRectMin();
				auto inputFieldPosMax = ImGui::GetItemRectMax();

				if (!isActiveEditSelection)
				{
					ImGui::SameLine();
					if (ImGui::Button(std::format("Edit###RebindEdit{}", idCount).c_str()))
					{
						currentSelectedBindingForEditing = bindingComboIter;
					}
				}
				else
				{
					ImGui::SameLine();
					if (ImGui::Button(std::format("Stop###RebindStop{}", idCount).c_str()))
					{
						currentSelectedBindingForEditing = std::nullopt;
					}
				}

				ImGui::SameLine();
				if (ImGui::Button(std::format("Clear###RebindClear{}", idCount).c_str()))
				{
					bindingComboIter->clear();
				}

				ImGui::SameLine();
				if (ImGui::Button(std::format("Delete###RebindDelete{}", idCount).c_str()))
				{
					// queue up to erase after the for loop
					queuedBindingErasure = bindingComboIter;
				}

				if (isActiveEditSelection)
				{
					// now to listen for keypresses
					// code is similiar-ish to HotkeyManager::pollInputs, but like in reverse

					for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
					{
						if (ImGui::IsKeyPressed(key))
						{
							// if the key type is mouse related, we only want to accept it if it's hovered over the input box.
							if (key >= ImGuiKey_Mouse_BEGIN && key < ImGuiKey_Mouse_END)
							{
								if (!ImGui::IsMouseHoveringRect(inputFieldPosMin, inputFieldPosMax, false))
									continue; // ignore this key, check next
							}

							currentSelectedBindingForEditing.value()->insert(key);
							break; // only bind one key at a time
						}
					}
				}



			}

			if (queuedBindingErasure)
			{
				workingBindingSet.erase(queuedBindingErasure.value());
				currentSelectedBindingForEditing = std::nullopt; // iterator now invalid
			}
				
			// previous iterators may be invalid! therefore added a scope to enforce this
		}
		


		// button to add a new binding
		if (ImGui::Button("Add binding"))
		{
			workingBindingSet.push_back({}); // add empty binding
			currentSelectedBindingForEditing = workingBindingSet.end() - 1; // make it the one selected for editing
		}

		if (ImGui::Button("Save"))
		{
			// remove any empty binding sets
			std::erase_if(workingBindingSet, [](const BindingCombo& binding) { return binding.empty(); });
			mHotkeyToChange->setBindings(workingBindingSet);

			mIsFinished = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			mIsFinished = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();

}