#pragma once

#include "dotM/Gameboy/Component/CPU/SharpSM83.hpp"

namespace dot::gb
{
    using CPU = SharpSM83;



    constexpr auto BIOS_LOC = "C:/dev/C++/Emulator/dotM/src/dotM/Gameboy/Data/bios.bin";
    //constexpr auto PAK_LOC  = "C:/dev/C++/Emulator/dotM/src/dotM/Gameboy/Data/cpu_instrs.gb";
    constexpr auto PAK_LOC  = "C:/dev/C++/Emulator/dotM/src/dotM/Gameboy/Data/Tetris.gb";
}
