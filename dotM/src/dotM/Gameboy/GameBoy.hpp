#pragma once

#include <stdafx.hpp>

#include "dotM/Gameboy/Specification/GB.hpp"

namespace dot::gb
{
    class GameBoy
    {
    public:
        enum class Key
        {
            A, 
            B, 

            Left, 
            Right, 
            Up, 
            Down, 

            Start, 
            Select, 
        };

        GameBoy();
        ~GameBoy();

        void on();
        void off();

        void press(Key key);

    //private:
        void boot();

        std::unique_ptr<SharpSM83> m_cpu{};
        std::unique_ptr<PPU>       m_ppu{};
        std::unique_ptr<Display>   m_display{};

        std::array<byte, 0x10000> m_memory{};
    };
}
