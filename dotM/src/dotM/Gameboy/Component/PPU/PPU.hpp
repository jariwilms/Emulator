#pragma once

#include <stdafx.hpp>

#include "dotM/Gameboy/Architecture/IO.hpp"
#include "dotM/Gameboy/Component/DISPLAY/Display.hpp"
#include "dotm/Gameboy/Component/CPU/SharpSM83.hpp"

namespace dot::gb
{
    class PPU
    {
    public:
        PPU(SharpSM83& cpu, Display& display, std::array<byte, 0x10000>& memory);

        void cycle();

    private:
        struct LCDControl
        {
            LCDControl() = default;
            LCDControl(byte value)
            {
                lcdEnable                      = get_bit(value, 7);
                windowTilemapArea              = get_bit(value, 6);
                windowEnable                   = get_bit(value, 5);
                backgroundWindowTileDataArea   = get_bit(value, 4);
                backgroundTilemapArea          = get_bit(value, 3);
                objectSize                     = get_bit(value, 2);
                objectEnable                   = get_bit(value, 1);
                backgroundWindowEnablePriority = get_bit(value, 0);
            }

            bool lcdEnable                      : 1{};
            bool windowTilemapArea              : 1{};
            bool windowEnable                   : 1{};
            bool backgroundWindowTileDataArea   : 1{};
            bool backgroundTilemapArea          : 1{};
            bool objectSize                     : 1{};
            bool objectEnable                   : 1{};
            bool backgroundWindowEnablePriority : 1{};
        };

        void mode_0();
        void mode_1();
        void mode_2();
        void mode_3();

        std::array<byte, 0x10000>& m_memory;
        LCDControl m_lcdControl{};

        word m_scanline{};
        word m_scanlineDots{};

        bool m_hBlankSignaled{};
        bool m_vBlankSignaled{};

        SharpSM83& m_cpu;
        Display& m_display;
    };
}
