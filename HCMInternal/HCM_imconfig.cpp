#include "pch.h"
#include "HCM_imconfig.h"




// io extension methods
std::ostream& operator<< (std::ostream& out, const SimpleMath::Vector2& vec)
{
    return out << vec.x << ", " << vec.y;
}

std::ostream& operator<< (std::ostream& out, const SimpleMath::Vector3& vec)
{
    return out << vec.x << ", " << vec.y << ", " << vec.z;
}

std::ostream& operator<< (std::ostream& out, const SimpleMath::Vector4& vec)
{
    return out << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
}



std::string vec2ToString(const SimpleMath::Vector2& vec, int decimalPrecision)
{
    std::ostringstream ss;
    ss << std::setprecision(6) << std::fixed << std::showpos << vec;
    return ss.str();
}

std::string vec3ToString(const SimpleMath::Vector3& vec, int decimalPrecision)
{
    std::ostringstream ss;
    ss << std::setprecision(6) << std::fixed << std::showpos << vec;
    return ss.str();
}

std::string vec4ToString(const SimpleMath::Vector4& vec, int decimalPrecision)
{
    std::ostringstream ss;
    ss << std::setprecision(6) << std::fixed << std::showpos << vec;
    return ss.str();
}

namespace plog
{
    Record& operator<<(Record& record, const SimpleMath::Vector2& vec)
    {
        return record << vec.x << ", " << vec.y;
    }

    Record& operator<<(Record& record, const SimpleMath::Vector3& vec)
    {
        return record << vec.x << ", " << vec.y << ", " << vec.z;
    }

    Record& operator<<(Record& record, const SimpleMath::Vector4& vec)
    {
        return record << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
    }
}