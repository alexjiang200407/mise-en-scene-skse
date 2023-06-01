#pragma once
#include "REL/Relocation.h"
#include "logger.h"

namespace MES
{
    // Utility Functions

    template <typename T1, typename T2>
    T1 EnumCast(const T2& enumeration)
    {
        return static_cast<std::underlying_type<T2>::type>(enumeration);
    };

    template <typename T>
    int32_t EnumToInt32(const T& enumeration)
    {
        return MES::EnumCast<int32_t>(enumeration);
    }

    template <typename T>
    int16_t EnumToInt16(const T& enumeration)
    {
        return MES::EnumCast<int16_t>(enumeration);
    }

}

