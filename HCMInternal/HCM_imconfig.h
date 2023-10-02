#pragma once
#define IMGUI_USER_CONFIG "HCM_imconfig.h"

#define IM_VEC2_CLASS_EXTRA                                                     \
        constexpr ImVec2(const Vec2& f) : x(f.x), y(f.y) {}                   \
        operator Vec2() const { return Vec2(x,y); }
