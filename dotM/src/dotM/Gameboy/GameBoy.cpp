#include "stdafx.hpp"

#include "GameBoy.hpp"

namespace dot::gb
{
    GameBoy::GameBoy()
    {
        m_cpu     = std::make_unique<SharpSM83>(m_memory);
        m_display = std::make_unique<Display>();
        m_ppu     = std::make_unique<PPU>(*m_cpu.get(), *m_display.get(), m_memory);
    }
    GameBoy::~GameBoy()
    {

    }

    void GameBoy::on()
    {
        boot();
    }
    void GameBoy::off()
    {

    }

    void GameBoy::press(Key key)
    {

    }

    void GameBoy::boot()
    {
        const auto& bios = read_file(BIOS_LOC);
        std::memcpy(m_memory.data(), bios.data(), bios.size());

        const auto& pak = read_file(PAK_LOC);
        //std::memcpy(m_memory.data() + 0x100, pak.data(), pak.size());
        std::memcpy(m_memory.data(), pak.data(), pak.size());
    }
}
