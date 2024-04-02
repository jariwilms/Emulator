#pragma once

#include <stdafx.hpp>

#include "dotM/GameBoyAdvance/Component/MEM/RAM.hpp"
#include "dotM/GameBoyAdvance/Architecture/MEMORY/MAP.hpp"

namespace dot::gba
{
	class DISPLAY
	{
	public:
		DISPLAY() = default;
		~DISPLAY() = default;

		void render()
		{
			
		}

		std::array<byte, 76800> buffer{};                                      //240 * 160 * 2

	private:
		static inline const std::pair<unsigned int, unsigned int> s_dimensions{ 240, 160 };
	};
}
