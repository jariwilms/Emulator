#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"
#include "dotM/GameboyAdvance/Component/SCREEN/AGS101.hpp"

namespace dot::gba
{
	class PPU
	{
	public:
		PPU() = default;
		~PPU() = default;

		void cycle()
		{
			const auto displayControl = m_BUS->read<word>(REG_DISPCNT);
			const auto mode = get_bits(displayControl, 0, 3);

			switch (mode)
			{
				case 0b000: break;
				case 0b001: break;
				case 0b010: break;
				case 0b011: break;
				case 0b100: break;
				case 0b101: break;

				default: throw std::runtime_error("Invalid display mode");
			}
		}

	protected:
		void mode3()
		{
			
		}
		
	private:
		std::shared_ptr<AGS101> m_SCREEN{};

		std::shared_ptr<BUS<32>> m_BUS{};

		
	};
}
