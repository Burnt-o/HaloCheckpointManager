#pragma once
// imgui.h defines ImVec3, but I would like to use it without having to include that whole thing
// plus add some extension menthods
// an implicit conversion from Vec3 to ImVec3 is defined in HCM_imconfig
#include "HCM_imconfig.h"



/// do we just replace this with the SimpleMath.h things? 
// just would add a conversion from ImVec2/3 to SimpleMath::Vector2/3
https://github.com/microsoft/DirectXTK/wiki/SimpleMath

struct Vec3
{
    float x, y, z;
    constexpr Vec3() : x(0.0f), y(0.0f), z(0.0f) { }
    constexpr Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }

    auto operator==(Vec3 const& other) const
    {
        return (x == other.x && y == other.y && z == other.z);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec3& vec)
    {
        os << vec.x << ", " << vec.y << ", " << vec.z;
        return os;
    }
};



//SCALAR TO Vec3 OPERATIONS
inline Vec3 operator + (const Vec3& v1, float s) { return Vec3(v1.x + s, v1.y + s, v1.z + s); }
inline Vec3 operator - (const Vec3& v1, float s) { return Vec3(v1.x - s, v1.y - s, v1.z - s); }
inline Vec3 operator * (const Vec3& v1, float s) { return Vec3(v1.x * s, v1.y * s, v1.z * s); }
inline Vec3 operator / (const Vec3& v1, float s) { return Vec3(v1.x / s, v1.y / s, v1.z / s); }


//Vec3 TO Vec3 OPERATIONS
inline Vec3 operator + (const Vec3& v1, const Vec3& v2) { return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
inline Vec3 operator - (const Vec3& v1, const Vec3& v2) { return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
inline Vec3 operator * (const Vec3& v1, const Vec3& v2) { return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
inline Vec3 operator / (const Vec3& v1, const Vec3& v2) { return Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z); }

inline Vec3 operator += (const Vec3& v1, const Vec3& v2) { return v1 + v2; }




