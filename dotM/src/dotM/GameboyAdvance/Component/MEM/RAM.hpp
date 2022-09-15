#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/MEM/MEMORY.hpp"

namespace dot::gba
{
	template<size_t ADDR0, size_t ADDRF>
	class RAM : public MEMORY
	{
	public:
		RAM() : MEMORY{ Type::Read_Write, ADDR0, ADDRF } {}
		~RAM() override = default;
	};
}
