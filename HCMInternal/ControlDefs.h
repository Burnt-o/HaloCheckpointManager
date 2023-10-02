#pragma once
#include "Vec2.h"
typedef eventpp::CallbackList<void()> ActionEvent;
typedef eventpp::CallbackList<void(Vec2)> RenderEvent;
#define addTooltip(x) if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip(x)

#define nameof(x) #x

#define safe_release(p) if (p) { p->Release(); p = nullptr; } 


