#include "stdafx.hpp"

#include "PPU.hpp"

namespace dot::gb
{
    PPU::PPU(SharpSM83& cpu, Display& display, std::array<byte, 0x10000>& memory)
        : m_cpu{ cpu }, m_memory{ memory }, m_display{ display }
    {

    }

    void PPU::cycle()
    {
        m_lcdControl = m_memory.at(arc::LCDC);

        //Also: "When re-enabling the LCD, the PPU will immediately start drawing again, but the screen will stay blank during the first frame."
        //if (m_lcdControl.lcdEnable && !m_lcdControl.backgroundWindowEnablePriority)
        //{
        //    //Display is not enabled and not overridden by priority
        //}


        const auto& addressingMode = m_lcdControl.backgroundWindowTileDataArea;
        const word  backgroundmap  = m_lcdControl.windowTilemapArea ? 0x9C00 : 0x9800;



        const auto MODE_2_DOTS   = 80;
        const auto MODE_3_DOTS   = 172;
        const auto MODE_0_DOTS   = 204;
        const auto SCANLINE_DOTS = 456;

        if (m_scanlineDots <  MODE_2_DOTS)                                   mode_2(); //OAM scan
        if (m_scanlineDots >= MODE_0_DOTS && m_scanlineDots < MODE_3_DOTS)   mode_3(); //Drawing pixels
        if (m_scanlineDots >= MODE_3_DOTS && m_scanlineDots < SCANLINE_DOTS) mode_0(); //Horizontal blank
        if (m_scanline     >  143)                                           mode_1(); //Vertical blank
        if (m_scanlineDots >= SCANLINE_DOTS) m_scanlineDots = 0;

        m_scanlineDots += 4; //1 cycle = 4 dots
    }

    void PPU::mode_0()
    {
        //Waiting until the end of the scanline, (376 - mode 3 duration) dots, VRAM and OAM available
    }
    void PPU::mode_1()
    {
        const auto& flags = m_memory.at(arc::IF);
        m_memory.at(arc::IF) = set_bit(flags, 0, true);

        m_cpu.signal_interrupt();

        //Waiting until {}the next frame, 4560 dots (10 scanlines), VRAM and OAM available
    }
    void PPU::mode_2()
    {
        //Searching for OBJs which overlap this line, 80 dots, VRAM available
    }
    void PPU::mode_3()
    {
        //Sending pixels to the LCD, 172-289 dots, None available
        //1 pixel per dot from left to right, 4 dots per cycle
        //TODO: "At the very beginning of Mode 3, rendering is paused for SCX % 8 dots while the same number of pixels are discarded from the leftmost tile."
        //    => 12 dots wasted
        //After the last non-window pixel is emitted, a 6-dot penalty is incurred while the BG fetcher is being set up for the window
        //Each object drawn during the scanline (even partially) incurs a 6-to 11-dot penalty

        const auto& backgroundWindowAddressingMode = m_lcdControl.backgroundWindowTileDataArea ? true : false; //VRAM address 0x9000 (signed) or 0x8000 (unsigned)
        const auto& backgroundTilemapAreaAddress   = m_lcdControl.backgroundTilemapArea ? 0x9C00 : 0x9800;


        byte posX{}; //Current X level of pixel
        byte posY{}; //Current Y level of pixel

        for (int i = 0; i < 20; ++i) //for each tilemap to draw
        {
            const auto& tilemap      = m_memory.at(0x9800 + i + (m_scanline * 32)); //Tile map is 1 byte
            const auto& tileDataLow  = m_memory.at(tilemap + m_scanline * 2);
            const auto& tileDataHigh = m_memory.at(tilemap + m_scanline * 2 + 1);

            for (int i = 7; i >= 0; --i)
            {
                const auto& lsb = get_bit(tileDataLow,  i);
                const auto& msb = get_bit(tileDataHigh, i);

                const byte pixel = (msb << 1) | lsb;
                m_display.m_screen.at(m_scanline * 160 + posX) = pixel;

                ++posX;
            }
        }

        ++m_scanline;
    }
}
