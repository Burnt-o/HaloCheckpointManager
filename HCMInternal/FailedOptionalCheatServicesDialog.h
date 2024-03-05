#pragma once
#include "IModalDialog.h"
#include "GameState.h"
#include "GUIServiceInfo.h"
#include "GuiElementEnum.h"
#include "imgui.h"
#include "imgui_stdlib.h"
class FailedOptionalCheatServicesDialog : public IModalDialogVoid
{
private:
	std::map<GameState, std::pair<std::vector<std::pair<GUIElementEnum, std::string>>, int>> guiFailureMap;
	bool anyErrorsAtAll = false;
	int totalErrors = 0;
public:
	FailedOptionalCheatServicesDialog(std::shared_ptr<GUIServiceInfo> guiFailures) : IModalDialogVoid("Failed optional cheat services")
	{
		guiFailureMap =
		{
			{GameState::Value::Halo1, {{}, 0} },
			{GameState::Value::Halo2, {{}, 0}  },
			{GameState::Value::Halo3, {{}, 0}  },
			{GameState::Value::Halo3ODST, {{}, 0}  },
			{GameState::Value::HaloReach, {{}, 0}  },
			{GameState::Value::Halo4, {{}, 0}  },
			{GameState::Value::NoGame, {{}, 0}  },
		};

		// fill up guiFailureMap
		anyErrorsAtAll = false;
		totalErrors = 0;

		for (auto& [gameGuiPair, errorMessage] : guiFailures->getFailureMessagesMap())
		{
			guiFailureMap.at(gameGuiPair.first).second++; // increment error count
			totalErrors++;
			anyErrorsAtAll = true;
			guiFailureMap.at(gameGuiPair.first).first.emplace_back(std::pair<GUIElementEnum, std::string>{ gameGuiPair.second, errorMessage });
		}
	}


	virtual void renderInternal(SimpleMath::Vector2 screenSize) override
	{

			ImGui::Text("Services that failed to initialise are shown below, grouped by game. ");

			if (!anyErrorsAtAll)
			{
				ImGui::Text("All optional services successfully initialised!");
			}
			else
			{
				ImGui::Text(std::format("Total service failures: {}", totalErrors).c_str());
				for (auto& [game, enumMessagePairVector] : guiFailureMap)
				{
					if (enumMessagePairVector.first.empty()) continue;

					if (ImGui::TreeNodeEx(std::format("{} : {} service failure{}", game.toString(), enumMessagePairVector.second, enumMessagePairVector.second == 1 ? "" : "s").c_str(), ImGuiTreeNodeFlags_FramePadding))
					{
						for (auto enumMessagePair : enumMessagePairVector.first)
						{
							ImGui::Text(std::format("{} service failed! Error: ", magic_enum::enum_name(enumMessagePair.first)).c_str());
							ImGui::Text(std::format("   {}", enumMessagePair.second).c_str());
							ImGui::Dummy({ 2,2 }); // padding between messages
						}
						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("Ok"))
			{
				PLOG_DEBUG << "closing FailedOptionalCheatServicesDialog with Ok";
				closeDialog();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				PLOG_DEBUG << "closing FailedOptionalCheatServicesDialog with Ok";
				closeDialog();
			}

		
	}
};