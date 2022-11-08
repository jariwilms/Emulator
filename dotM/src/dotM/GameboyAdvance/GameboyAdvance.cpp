#include <stdafx.hpp>

#include "GameboyAdvance.hpp"
#include "dotM/Library/File.hpp"

namespace dot::gba
{
	GameboyAdvance::GameboyAdvance()
	{
		m_CPU   = std::make_shared<ARM7TDMI>();
		m_BIOS  = std::make_shared<BIOS>();
		m_IWRAM = std::make_shared<IWRAM>();
		m_EWRAM = std::make_shared<EWRAM>();
		m_VRAM  = std::make_shared<VRAM>();
		m_OAM   = std::make_shared<OAM>();
		m_OPAL  = std::make_shared<OPAL>();
		m_BUS   = std::make_shared<BUS<BUS_WIDTH>>();


		
		m_CPU->connect(m_BUS);
		m_BUS->connect(m_BIOS);
		m_BUS->connect(m_IWRAM);
		m_BUS->connect(m_EWRAM);
		m_BUS->connect(m_VRAM);
		m_BUS->connect(m_OAM);
		m_BUS->connect(m_OPAL);
	}
	void GameboyAdvance::boot()
	{
		auto bios = read_file(BIOS_LOC);
		m_BIOS->set(0x0, bios.data(), bios.size());

		m_running = true;
	}
	void GameboyAdvance::run()
	{
		static size_t cycles{};
		
		while (m_running)
		{
			m_CPU->cycle();
			//++cycles;
		}
	}
}
