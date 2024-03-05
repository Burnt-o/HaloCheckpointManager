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

SimpleMath::Vector2 vec2FromString(const std::string& str)
{
    std::stringstream ss(str);

    SimpleMath::Vector2 out(0.f, 0.f); // default value if serialisation fails
    std::string xString, yString;


    std::getline(ss, xString, ',');
    std::getline(ss, yString);


    // remove whitespace
    std::erase_if(xString, [](const char& c) { return std::isspace(c); });
    std::erase_if(yString, [](const char& c) { return std::isspace(c); });


    // strtof does not throw, just returns 0.f if conversion failed. That's fine.
    out.x = std::strtof(xString.c_str(), nullptr);
    out.y = std::strtof(yString.c_str(), nullptr);

    return out;
}

SimpleMath::Vector3 vec3FromString(const std::string& str)
{
    std::stringstream ss(str);

    SimpleMath::Vector3 out(0.f, 0.f, 0.f); // default value if serialisation fails
    std::string xString, yString, zString;


    std::getline(ss, xString, ',');
    std::getline(ss, yString, ',');
    std::getline(ss, zString);

    // remove whitespace
    std::erase_if(xString, [](const char& c) { return std::isspace(c); });
    std::erase_if(yString, [](const char& c) { return std::isspace(c); });
    std::erase_if(zString, [](const char& c) { return std::isspace(c); });

    // strtof does not throw, just returns 0.f if conversion failed. That's fine.
    out.x = std::strtof(xString.c_str(), nullptr);
    out.y = std::strtof(yString.c_str(), nullptr);
    out.z = std::strtof(zString.c_str(), nullptr);

    return out;
}

SimpleMath::Vector4 vec4FromString(const std::string& str)
{
    std::stringstream ss(str);

    SimpleMath::Vector4 out(0.f, 0.f, 0.f, 0.f); // default value if serialisation fails
    std::string xString, yString, zString, wString;


    std::getline(ss, xString, ',');
    std::getline(ss, yString, ',');
    std::getline(ss, zString, ',');
    std::getline(ss, wString);

    // remove whitespace
    std::erase_if(xString, [](const char& c) { return std::isspace(c); });
    std::erase_if(yString, [](const char& c) { return std::isspace(c); });
    std::erase_if(zString, [](const char& c) { return std::isspace(c); });
    std::erase_if(wString, [](const char& c) { return std::isspace(c); });

    // strtof does not throw, just returns 0.f if conversion failed. That's fine.
    out.x = std::strtof(xString.c_str(), nullptr);
    out.y = std::strtof(yString.c_str(), nullptr);
    out.z = std::strtof(zString.c_str(), nullptr);
    out.w = std::strtof(wString.c_str(), nullptr);

    return out;

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