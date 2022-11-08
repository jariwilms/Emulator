#pragma once

#include <stdafx.hpp>
#include "dotM/GameboyAdvance/Specification/MEMORY_MAP.hpp"

namespace dot::gba
{
	class AGS101 //TODO: not the name of the screen => change
	{
	public:
		AGS101() = default;
		~AGS101() = default;

		void render()
		{
			
		}

	private:
		enum class TileMode
		{
			Mode0 = 0x0, 
			Mode1 = 0x1, 
			Mode2 = 0x2, 
			Mode3 = 0x3, 
			Mode4 = 0x4, 
			Mode5 = 0x5, 
			Mode6 = 0x6, 
			Mode7 = 0x7, 
		};

		static inline const std::pair<const unsigned int, const unsigned int> s_dimensions{ 240, 160 };
	};
}
