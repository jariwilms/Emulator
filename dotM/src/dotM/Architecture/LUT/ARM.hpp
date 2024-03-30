#pragma once

#include <stdafx.hpp>

namespace dot::arc
{
	constexpr unsigned int ARM_FORMAT_COUNT = 15;
	constexpr std::array<unsigned int, ARM_FORMAT_COUNT> ARM_INSTRUCTION_FORMAT
	{
		0x0000'0000,                                                           //Data Processing
		0x0000'0090,                                                           //Multiply
		0x0080'0090,                                                           //Multiply Long
		0x0100'0090,                                                           //Single Data Swap
		0x012F'FF10,                                                           //Branch and Exchange
		0x0000'0090,                                                           //Halfword Data Transfer: register offset
		0x0040'0090,                                                           //Halfword Data Transfer: immediate offset
		0x0400'0000,                                                           //Single Data Transfer
		0x0600'0010,                                                           //Undefined
		0x0800'0000,                                                           //Block Data Transfer
		0x0A00'0000,                                                           //Branch
		0x0C00'0000,                                                           //Coprocessor Data Transfer
		0x0E00'0000,                                                           //Coprocessor Data Operation
		0x0E00'0010,                                                           //Coprocessor Register Transfer
		0x0F00'0000,                                                           //Software Interrupt
	};
	constexpr std::array<unsigned int, ARM_FORMAT_COUNT> ARM_INSTRUCTION_MASK
	{
		0x0C00'0000,                                                           //Data Processing
		0x0FC0'00F0,                                                           //Multiply
		0x0F80'0090,                                                           //Multiply Long
		0x0F80'0FF0,                                                           //Single Data Swap
		0x0FFF'FFF0,                                                           //Branch and Exchange
		0x0E40'0F90,                                                           //Halfword Data Transfer: register offset
		0x0E40'0090,                                                           //Halfword Data Transfer: immediate offset
		0x0C00'0000,                                                           //Single Data Transfer
		0x0E00'0010,                                                           //Undefined
		0x0E00'0000,                                                           //Block Data Transfer
		0x0E00'0000,                                                           //Branch
		0x0E00'0000,                                                           //Coprocessor Data Transfer
		0x0F00'0010,                                                           //Coprocessor Data Operation
		0x0F00'0010,                                                           //Coprocessor RegisterTransfer
		0x0F00'0000,                                                           //Software Interrupt
	};
}
