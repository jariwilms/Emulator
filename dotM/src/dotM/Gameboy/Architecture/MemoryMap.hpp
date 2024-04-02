#pragma once

#include "dotM/Library/Type.hpp"

namespace dot::gb::arc
{
    constexpr addr16_t ROM00ADDR0 { 0x0000 }, ROM00ADDRF { 0x3FFF };           // 16KB
    constexpr addr16_t ROM01ADDR0 { 0x4000 }, ROM01ADDRF { 0x7FFF };           // 16KB
    constexpr addr16_t VRAMADDR0  { 0x8000 }, VRAMADDRF  { 0x9FFF };           //  8KB
    constexpr addr16_t EXTRAMADDR0{ 0xA000 }, EXTRAMADDRF{ 0xBFFF };           //  8KB
    constexpr addr16_t WRAMADDR0  { 0xC000 }, WRAMADDRF  { 0xCFFF };           //  4KB
    constexpr addr16_t SWRAMADDR0 { 0xD000 }, SWRAMADDRF { 0xDFFF };           //  4KB
    constexpr addr16_t MRAMADDR0  { 0xE000 }, MRAMADDR   { 0xFDFF };           //  7KB
    constexpr addr16_t OAMADDR0   { 0xFE00 }, OAMADDRF   { 0xFE9F };           //160 B
    constexpr addr16_t UNUSEDADDR0{ 0xFEA0 }, UNUSEDADDRF{ 0xFEFF };           // 60 B
    constexpr addr16_t IOADDR0    { 0xFF00 }, IOADDRF    { 0xFF7F };           //128 B
    constexpr addr16_t HRAMADDR0  { 0xFF80 }, HRAMADDRF  { 0xFFFE };           //127 B
    constexpr addr16_t IEREGADDR0 { 0xFFFF }, IEREGADDRF { 0xFFFF };           //  1 B
}
