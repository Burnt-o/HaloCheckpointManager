#pragma once
#define IMGUI_USER_CONFIG "HCM_imconfig.h"
//#include <directxtk\SimpleMath.h>

#define IM_VEC2_CLASS_EXTRA                                                     \
        constexpr ImVec2(const SimpleMath::Vector2& f) : x(f.x), y(f.y) {}                   \
        operator SimpleMath::Vector2() const { return SimpleMath::Vector2(x,y); }


#define IM_VEC3_CLASS_EXTRA                                                     \
        constexpr ImVec3(const SimpleMath::Vector3& f) : x(f.x), y(f.y), z(f.z) {}                   \
        operator SimpleMath::Vector3() const { return SimpleMath::Vector3(x,y,z); }


#define IM_VEC4_CLASS_EXTRA                                                     \
        constexpr ImVec4(const SimpleMath::Vector4& f) : x(f.x), y(f.y), z(f.z), w(f.w) {}    \
        operator SimpleMath::Vector4() const { return SimpleMath::Vector4(x,y,z,w); }


// io extension methods
std::ostream& operator<< (std::ostream& out, const SimpleMath::Vector2& vec);

std::ostream& operator<< (std::ostream& out, const SimpleMath::Vector3& vec);

std::ostream& operator<< (std::ostream& out, const SimpleMath::Vector4& vec);

std::string vec2ToString(const SimpleMath::Vector2& vec, int decimalPrecision = 6);

std::string vec3ToString(const SimpleMath::Vector3& vec, int decimalPrecision = 6);

std::string vec4ToString(const SimpleMath::Vector4& vec, int decimalPrecision = 6);

namespace plog
{
    Record& operator<<(Record& record, const SimpleMath::Vector2& vec);

    Record& operator<<(Record& record, const SimpleMath::Vector3& vec);

    Record& operator<<(Record& record, const SimpleMath::Vector4& vec);
}