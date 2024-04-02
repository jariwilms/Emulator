#pragma once

#include <stdafx.hpp>

namespace dot::arc
{
	constexpr size_t VEC_RESET              = 0x00000000;
	constexpr size_t VEC_UNDEFINED          = 0x00000004;
	constexpr size_t VEC_SOFTWARE_INTERRUPT = 0x00000008;
	constexpr size_t VEC_PREFETCH_ABORT     = 0x0000000C;
	constexpr size_t VEC_DATA_ABORT         = 0x00000010;
	constexpr size_t VEC_RESERVED           = 0x00000014;
	constexpr size_t VEC_IRQ                = 0x00000018;
	constexpr size_t VEC_FIQ                = 0x0000001C;
}
