#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/CPU/ARM7TDMI.hpp"
#include "dotM/GameboyAdvance/Component/MEM/RAM.hpp"
#include "dotM/GameboyAdvance/Component/MEM/ROM.hpp"
#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"
#include "dotM/Library/Macro.hpp"

namespace dot::gba
{
	const size_t RESET_VEC{ 0x00000000 };



	//Addressing ranges
	const size_t BIOSADDR0{ 0x00000000 }, BIOSADDRF{ 0x00003FFF };
	const size_t IWRAMADDR0{ 0x03000000 }, IWRAMADDRF{ 0x03007FFF };
	const size_t EWRAMADDR0{ 0x02000000 }, WRAMADDRF{ 0x0203FFFF };
	const size_t VRAMADDR0{ 0x06000000 }, VRAMADDRF{ 0x06017FFF };
	const size_t OAMADDR0{ 0x07000000 }, OAMADDRF{ 0x070003FF };
	const size_t PALADDR0{ 0x05000000 }, PALADDRF{ 0x050003FF };



	//Hardware Specifications
	using BIOS = ROM<BIOSADDR0, BIOSADDRF>;   // 16KB
	using IWRAM = RAM<IWRAMADDR0, IWRAMADDRF>; // 32KB
	using EWRAM = RAM<EWRAMADDR0, WRAMADDRF>;  //256KB
	using VRAM = RAM<VRAMADDR0, VRAMADDRF>;   // 96KB
	using OAM = RAM<OAMADDR0, OAMADDRF>;     //  1KB
	using PAL = RAM<PALADDR0, PALADDRF>;     //  1KB


	
	const unsigned int BUS_WIDTH{ 32 }; //redundant?

	

	//Data locations
	const std::string BIOS_LOC{ R"(C:\dev\C++\Emulator\Emulator\src\Gameboy\Data\gba_bios.bin)" }; //TODO: dont use absolute path
}
