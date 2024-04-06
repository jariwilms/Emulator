#pragma once

#include "dotM/Gameboy/Component/CPU/SharpSM83.hpp"
#include "dotM/Gameboy/Component/PPU/PPU.hpp"
#include "dotM/Gameboy/Component/DISPLAY/Display.hpp"

namespace dot::gb
{
    using CPU = SharpSM83;



    constexpr auto BIOS_LOC = "C:/dev/C++/Emulator/dotM/src/dotM/Gameboy/Data/bios.bin";
    //constexpr auto PAK_LOC  = "C:/dev/C++/Emulator/dotM/src/dotM/Gameboy/Data/cpu_instrs.gb";
    constexpr auto PAK_LOC  = "C:/dev/C++/Emulator/dotM/src/dotM/Gameboy/Data/09-op r,r.gb";
}
