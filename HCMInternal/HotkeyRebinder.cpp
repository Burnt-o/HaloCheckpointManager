#include "pch.h"
#include "HotkeyRebinder.h"
#include "MessagesGUI.h"
#include "ImGuiManager.h"
#include "imgui_internal.h"
#include "HotkeyManager.h"
void HotkeyRebinder::beginHotkeyRebindDialog(std::shared_ptr<Hotkey> hotkey)
{
	if (!hotkey)
	{
		MessagesGUI::addMessage("Error: null hotkey passed to beginHotkeyRebindDialog");
		return;
	}
	PLOG_INFO << "opening new hotkey rebind dialog for " << hotkey.get()->getName();
	instance->mHotkeyToManipulate = hotkey;

	// since the stack id context is different here (because of HCMInternalGUI BeginWindow call), we need to manually
	// override/calculate the ID so that we can access it from the different context in renderHotkeyRebindDialog
	ImGuiID popup_id = ImHashStr("Rebind Hotkey");
	ImGui::PushOverrideID(popup_id); // since we're calling from a different window context than the OpenPopup
	ImGui::OpenPopup("Rebind Hotkey");
	ImGui::PopID();

	HotkeyManager::setDisableHotkeysForRebinding(true);
	instance->tempBindingSet = hotkey.get()->getBindings();
	instance->currentSelectedBindingForEditing = 0;
}


const std::set<ImGuiKey> mouseKeys = { ImGuiKey_MouseLeft, ImGuiKey_MouseRight, ImGuiKey_MouseMiddle,  };

void HotkeyRebinder::renderHotkeyRebindDialog()
{


	ImGui::SetNextWindowPos(ImGuiManager::getScreenCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGuiID popup_id = ImHashStr("Rebind Hotkey");
	ImGui::PushOverrideID(popup_id); // since we're calling from a different window context than the OpenPopup
	if (ImGui::BeginPopupModal("Rebind Hotkey", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{

		for (int i = 0; i < tempBindingSet.size(); i++)
		{
			auto& binding = tempBindingSet.at(i);
			std::string currentBindingText = Hotkey::generateBindingTextSingle(binding);

			bool notBeingEdited = i != currentSelectedBindingForEditing;


			ImGui::BeginChild(std::format("InputField{}", i).c_str(), {400, 30}, true, ImGuiWindowFlags_NoResize);
			if (notBeingEdited) ImGui::BeginDisabled();
			ImGui::Text(currentBindingText.c_str());
			if (notBeingEdited) ImGui::EndDisabled();
			ImGui::EndChild();

			// used for hit testing mouse hotkeys
			auto inputFieldPosMin = ImGui::GetItemRectMin();
			auto inputFieldPosMax = ImGui::GetItemRectMax();

			if (notBeingEdited)
			{
				ImGui::SameLine();
				if (ImGui::Button(std::format("Edit###RebindEdit{}", i).c_str()))
				{
					currentSelectedBindingForEditing = i;
				}
			}
			else
			{
				ImGui::SameLine();
				if (ImGui::Button(std::format("Stop###RebindStop{}", i).c_str()))
				{
					currentSelectedBindingForEditing = -1;
				}
			}

			ImGui::SameLine();
			if (ImGui::Button(std::format("Clear###RebindClear{}", i).c_str()))
			{
				binding.clear();
			}

			ImGui::SameLine();
			if (ImGui::Button(std::format("Delete###RebindDelete{}", i).c_str()))
			{
				tempBindingSet.erase(tempBindingSet.begin() + i);
				break; // since we're iterating over the set we're deleting from, best to stop iterating now
			}

			if (!notBeingEdited)
			{
				// now to listen for keypresses
				// code is similiar-ish to HotkeyManager::pollInputs, but like in reverse

				for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
				{
					if (ImGui::IsKeyPressed(key))
					{
						// if key is already in the binding set then skip
						bool alreadyExists = false;
						for (auto existingKey : binding)
						{
							if (key == existingKey)
							{
								alreadyExists = true;
								break;
							}
						}
						if (alreadyExists) continue;

						// if the key type is mouse related, we only want to accept it if it's hovered over the input box.
						if (key >= ImGuiKey_Mouse_BEGIN && key < ImGuiKey_Mouse_END)
						{
							if (!ImGui::IsMouseHoveringRect(inputFieldPosMin, inputFieldPosMax, false))
								continue; // ignore this key, check next
						}
						
						binding.push_back(key);
						break; // only bind one key at a time
					}
				}
			}

		}


		// button to add a new binding
		if (ImGui::Button("Add binding"))
		{
			tempBindingSet.push_back({}); // add empty binding
			currentSelectedBindingForEditing = tempBindingSet.size() - 1; // make it the one selected for editing
		}

		if (ImGui::Button("Save"))
		{
			// remove any empty binding sets
			auto new_end = std::remove_if(tempBindingSet.begin(), tempBindingSet.end(), [](auto& binding) { return binding.empty(); });
			tempBindingSet.erase(new_end, tempBindingSet.end());

			mHotkeyToManipulate.get()->setBindings(tempBindingSet);
			HotkeyManager::setDisableHotkeysForRebinding(false);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			HotkeyManager::setDisableHotkeysForRebinding(false);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();
}