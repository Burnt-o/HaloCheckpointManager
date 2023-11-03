#pragma once
// imgui.h defines ImVec2, but I would like to use it without having to include that whole thing
// plus add some extension menthods
// an implicit conversion from Vec2 to ImVec2 is defined in HCM_imconfig
#include "HCM_imconfig.h"
struct Vec2
{
    float x, y;
    constexpr Vec2() : x(0.0f), y(0.0f) { }
    constexpr Vec2(float _x, float _y) : x(_x), y(_y) { }

    auto operator==(Vec2 const& other) const
    {
        return (x == other.x && y == other.y);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2& vec)
    {
        os << vec.x << ", " << vec.y;
        return os;
    }
};



//SCALAR TO Vec2 OPERATIONS
inline Vec2 operator + (const Vec2& v1, float s) { return Vec2(v1.x + s, v1.y + s); }
inline Vec2 operator - (const Vec2& v1, float s) { return Vec2(v1.x - s, v1.y - s); }
inline Vec2 operator * (const Vec2& v1, float s) { return Vec2(v1.x * s, v1.y * s); }
inline Vec2 operator / (const Vec2& v1, float s) { return Vec2(v1.x / s, v1.y / s); }


//Vec2 TO Vec2 OPERATIONS
inline Vec2 operator + (const Vec2& v1, const Vec2& v2) { return Vec2(v1.x + v2.x, v1.y + v2.y); }
inline Vec2 operator - (const Vec2& v1, const Vec2& v2) { return Vec2(v1.x - v2.x, v1.y - v2.y); }
inline Vec2 operator * (const Vec2& v1, const Vec2& v2) { return Vec2(v1.x * v2.x, v1.y * v2.y); }
inline Vec2 operator / (const Vec2& v1, const Vec2& v2) { return Vec2(v1.x / v2.x, v1.y / v2.y); }




