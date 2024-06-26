#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/MEM/RAM.hpp"
#include "dotM/GameboyAdvance/Component/MEM/ROM.hpp"
#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"

namespace dot::gba
{
    //Memory Mapping
    constexpr size_t BIOSADDR0 { 0x00000000 }, BIOSADDRF { 0x00003FFF };       // 16 KB 
    constexpr size_t EWRAMADDR0{ 0x02000000 }, WRAMADDRF { 0x0203FFFF };	   //256 KB - WS2
    constexpr size_t IWRAMADDR0{ 0x03000000 }, IWRAMADDRF{ 0x03007FFF };	   // 32 KB
    constexpr size_t IOREGADDR0{ 0x04000000 }, IOREGADDRF{ 0x040003FE };	   //  1 KB

    constexpr size_t OPALADDR0 { 0x05000000 }, OPALADDRF { 0x050003FF };	   //  1 KB
    constexpr size_t VRAMADDR0 { 0x06000000 }, VRAMADDRF { 0x06017FFF };	   // 96 KB
    constexpr size_t OAMADDR0  { 0x07000000 }, OAMADDRF  { 0x070003FF };	   //  1 KB

    constexpr size_t CART0ADDR0{ 0x08000000 }, CART0ADDRF{ 0x09FFFFFF };	   // 32 MB - WS0
    constexpr size_t CART1ADDR0{ 0x0A000000 }, CART1ADDRF{ 0x0BFFFFFF };	   // 32 MB - WS1
    constexpr size_t CART2ADDR0{ 0x0C000000 }, CART2ADDRF{ 0x0DFFFFFF };	   // 32 MB - WS2
    constexpr size_t CARTSADDR0{ 0x0E000000 }, CARTSADDRF{ 0x0E00FFFF };	   // 64 KB



    //Components
    using BIOS  = ROM<BIOSADDR0,  BIOSADDRF>;                                  
    using EWRAM = RAM<EWRAMADDR0, WRAMADDRF>;                                  
    using IWRAM = RAM<IWRAMADDR0, IWRAMADDRF>;                                 
    using IOREG = RAM<IOREGADDR0, IOREGADDRF>;

    using OPAL  = RAM<OPALADDR0,  OPALADDRF>;                                   
    using VRAM  = RAM<VRAMADDR0,  VRAMADDRF>;                                  
    using OAM   = RAM<OAMADDR0,   OAMADDRF>;                                   

    using CART0 = ROM<CART0ADDR0, CART0ADDRF>;
    using CART1 = ROM<CART1ADDR0, CART1ADDRF>;
    using CART2 = ROM<CART2ADDR0, CART2ADDRF>;
    using CARTS = RAM<CARTSADDR0, CARTSADDRF>;



    constexpr unsigned int TIMER_C{ 4 };

	

    const std::string BIOS_LOC{ R"(C:\dev\C++\Emulator\dotM\src\dotM\GameboyAdvance\Data\BIOS\BIOS.bin)" }; //TODO: dont use absolute path
    const std::string PAK_LOC{ R"(C:\dev\C++\Emulator\dotM\src\dotM\GameboyAdvance\Data\Test\arm.gba)" };
}
