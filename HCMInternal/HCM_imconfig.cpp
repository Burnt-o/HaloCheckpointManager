#include "pch.h"
#include "HCM_imconfig.h"


// io extension methods
std::ostream& operator<< (std::ostream& out, const SimpleMath::Vector2& vec)
{
    out << vec.x << ", " << vec.y;
    return out;
}



std::ostream& operator<< (std::ostream& out, const SimpleMath::Vector3& vec)
{
    out << vec.x << ", " << vec.y << ", " << vec.z;
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
}