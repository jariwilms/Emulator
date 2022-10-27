#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/MEM/MEMORY.hpp"

namespace dot::gba
{
	template<size_t ADDR0, size_t ADDRF>
	class WOM : public MEMORY
	{
	public:
		WOM() : MEMORY{ Access::Write, ADDR0, ADDRF } {}
	};
}
