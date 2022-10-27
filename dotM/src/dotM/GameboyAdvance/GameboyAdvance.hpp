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
			boot();
			run();
		}
		inline void off()
		{
			m_running = false;
		}

	//private:
		void boot();
		void run();



		std::shared_ptr<ARM7TDMI> m_CPU{};
		std::shared_ptr<BUS<32>>  m_BUS{};
		
		std::shared_ptr<BIOS>     m_BIOS{};
		std::shared_ptr<EWRAM>    m_EWRAM{};
		std::shared_ptr<IWRAM>    m_IWRAM{};
		std::shared_ptr<IOREG>    m_IOREG{};
		
		std::shared_ptr<OPAL>     m_OPAL{};
		std::shared_ptr<VRAM>     m_VRAM{};
		std::shared_ptr<OAM>      m_OAM{};
		


		
		bool m_running{};
	};
}
