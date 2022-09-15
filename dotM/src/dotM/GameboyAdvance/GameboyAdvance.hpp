#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Specification/GBA.hpp"
#include "dotM/Library/File.hpp"

namespace dot::gba
{
	class GameboyAdvance
	{
	public:
		enum Key
		{
			A,
			B,

			Up,
			Down,
			Left,
			Right,

			L,
			R,

			Start,
			Select,
		};

		GameboyAdvance();
		~GameboyAdvance() = default;

		inline void on()
		{
			m_running = true;
			run();
		}
		inline void off()
		{
			m_running = false;
		}

	private:
		void setup();
		void boot();
		void run();



		std::shared_ptr<ARM7TDMI> m_cpu{};

		std::shared_ptr<BIOS> m_bios{};

		std::shared_ptr<IWRAM> m_iwRam{};
		std::shared_ptr<EWRAM> m_ewRam{};
		std::shared_ptr<VRAM> m_vRAM{};

		std::shared_ptr<OAM> m_oam{};
		std::shared_ptr<PAL> m_pal{};

		std::shared_ptr<SLB<BUS_WIDTH>> m_bus{};



		unsigned int m_cycles{};

		bool m_running{};
	};
}
