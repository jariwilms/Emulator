#include <stdafx.hpp>

#include "GameboyAdvance.hpp"

namespace dot::gba
{
	GameboyAdvance::GameboyAdvance()
	{
		m_cpu = std::make_shared<ARM7TDMI>();
		m_bios = std::make_shared<BIOS>();
		m_iwRam = std::make_shared<IWRAM>();
		m_ewRam = std::make_shared<EWRAM>();
		m_vRAM = std::make_shared<VRAM>();
		m_oam = std::make_shared<OAM>();
		m_pal = std::make_shared<PAL>();
		m_bus = std::make_shared<SLB<BUS_WIDTH>>();

		setup();
		boot();
	}

	void GameboyAdvance::setup()
	{
		m_cpu->bus = m_bus;

		m_bus->attach(m_bios);
		m_bus->attach(m_iwRam);
		m_bus->attach(m_ewRam);
		m_bus->attach(m_vRAM);
		m_bus->attach(m_oam);
		m_bus->attach(m_pal);
	}
	void GameboyAdvance::boot()
	{
		auto bios = read_file(BIOS_LOC);
		m_bios->set(0x0, bios.size(), bios.data());
	}
	void GameboyAdvance::run()
	{
		while (m_running)
		{
			m_cpu->cycle();
		}
	}
}
