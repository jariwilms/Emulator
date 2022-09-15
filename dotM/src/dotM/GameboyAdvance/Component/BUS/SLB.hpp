#pragma once

#include "BUS.hpp"

namespace dot::gba
{
	template<size_t BITS>
	class SLB : public BUS
	{
	public:
		SLB() : BUS(BITS) {}
		~SLB() override = default;
	};
}
