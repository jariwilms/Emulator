#pragma once

#include <stdafx.hpp>

#include "dotM/Gameboy/Specification/GB.hpp"

namespace dot::gb
{
    class GameBoy
    {
    public:

        void on();
        void off();

    private:
        void boot();

        std::unique_ptr<SharpSM83> m_cpu{};
        std::array<byte, 0xFFFF> m_memory{};
    };
}
