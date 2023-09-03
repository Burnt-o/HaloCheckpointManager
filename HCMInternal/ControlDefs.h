#pragma once

typedef eventpp::CallbackList<void()> ActionEvent;
#define addTooltip(x) if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip(x)