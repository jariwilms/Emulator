#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/MEM/MEMORY.hpp"

namespace dot::gba
{
	template<size_t ADDR0, size_t ADDRF>
	class ROM : public MEMORY
	{
	public:
		ROM() : MEMORY{ Type::Read, ADDR0, ADDRF } {}
		~ROM() override = default;
	};
}
