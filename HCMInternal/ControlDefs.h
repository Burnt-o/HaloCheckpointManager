#pragma once
typedef eventpp::CallbackList<void()> ActionEvent;
typedef eventpp::CallbackList<void(SimpleMath::Vector2)> RenderEvent;
typedef eventpp::CallbackList<void(bool&)> ToggleEvent;
#define addTooltip(x) if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip(x)

#define nameof(x) #x

#define safe_release(p) if (p) { p->Release(); p = nullptr; } 


#ifdef HCM_DEBUG
#define FORCEFULLY_DISABLE_GUIELEMENT currentHeight = 0; return;
#define DEBUG_GUI_HEIGHT addTooltip(std::format("Height: {}", currentHeight).c_str());
#else
#define FORCEFULLY_DISABLE_GUIELEMENT
#define DEBUG_GUI_HEIGHT 
#endif

#define lockOrThrow(weak, shared) auto shared = weak.lock(); if (!shared) throw HCMRuntimeException(std::format("Bad weak ptr: {}", nameof(weak)));
#define lockOrUnexpected(weak, shared) auto shared = weak.lock(); if (!shared) return std::unexpected(std::make_shared<BadWeakPtrError>(nameof(weak)));



#ifdef HCM_DEBUG
#define DEBUG_KEY_LOG(key, log) if (GetKeyState(key) & 0x8000) { log }
#else
#define DEBUG_KEY_LOG(key, log) 
#endif