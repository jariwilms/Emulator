#pragma once

#include <stdafx.hpp>

namespace dot::gb
{
    constexpr byte RESOLUTION_H = 160;
    constexpr byte RESOLUTION_V = 144;

    class Display
    {
    public:
        Display() = default;

    //private:
        std::array<byte, RESOLUTION_H * RESOLUTION_V> m_screen{};
    };
}
