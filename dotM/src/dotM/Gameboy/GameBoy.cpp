#include "stdafx.hpp"

#include "GameBoy.hpp"

namespace dot::gb
{
    void GameBoy::on()
    {
        boot();

        m_cpu = std::make_unique<SharpSM83>(m_memory);

        for (int i = 0; i < 10; ++i)
            m_cpu->cycle();
    }

    void GameBoy::off()
    {

    }

    void GameBoy::boot()
    {
        const auto& bios = read_file(BIOS_LOC);
        std::memcpy(m_memory.data(), bios.data(), bios.size());

        const auto& pak = read_file(PAK_LOC);
        std::memcpy(m_memory.data() + 0x100, pak.data(), pak.size());
    }
}
