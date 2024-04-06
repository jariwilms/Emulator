#pragma once

#include <stdafx.hpp>

namespace dot::gb::arc
{
    constexpr word P1_JOYP   = 0xFF00;
    constexpr word SB        = 0xFF01;                                         //Serial transfer data                                                 R / W    All
    constexpr word SC        = 0xFF02;                                         //Serial transfer control                                              R / W    Mixed
    constexpr word DIV       = 0xFF04;                                         //Divider register                                                     R / W    All
    constexpr word TIMA      = 0xFF05;                                         //Timer counter                                                        R / W    All
    constexpr word TMA       = 0xFF06;                                         //Timer modulo                                                         R / W    All
    constexpr word TAC       = 0xFF07;                                         //Timer control                                                        R / W    All
    constexpr word IF        = 0xFF0F;                                         //Interrupt flag                                                       R / W    All
    constexpr word NR10      = 0xFF10;                                         //Sound channel 1 sweep                                                R / W    All
    constexpr word NR11      = 0xFF11;                                         //Sound channel 1 length timer & duty cycle                            Mixed    All
    constexpr word NR12      = 0xFF12;                                         //Sound channel 1 volume & envelope                                    R / W    All
    constexpr word NR13      = 0xFF13;                                         //Sound channel 1 period low                                               W    All
    constexpr word NR14      = 0xFF14;                                         //Sound channel 1 period high & control                                Mixed    All
    constexpr word NR21      = 0xFF16;                                         //Sound channel 2 length timer & duty cycle                            Mixed    All
    constexpr word NR22      = 0xFF17;                                         //Sound channel 2 volume & envelope                                    R / W    All
    constexpr word NR23      = 0xFF18;                                         //Sound channel 2 period low                                               W    All
    constexpr word NR24      = 0xFF19;                                         //Sound channel 2 period high & control                                Mixed    All
    constexpr word NR30      = 0xFF1A;                                         //Sound channel 3 DAC enable                                           R / W    All
    constexpr word NR31      = 0xFF1B;                                         //Sound channel 3 length timer                                             W    All
    constexpr word NR32      = 0xFF1C;                                         //Sound channel 3 output level                                         R / W    All
    constexpr word NR33      = 0xFF1D;                                         //Sound channel 3 period low                                               W    All
    constexpr word NR34      = 0xFF1E;                                         //Sound channel 3 period high & control                                Mixed    All
    constexpr word NR41      = 0xFF20;                                         //Sound channel 4 length timer                                             W    All
    constexpr word NR42      = 0xFF21;                                         //Sound channel 4 volume & envelope                                    R / W    All
    constexpr word NR43      = 0xFF22;                                         //Sound channel 4 frequency & randomness                               R / W    All
    constexpr word NR44      = 0xFF23;                                         //Sound channel 4 control                                              Mixed    All
    constexpr word NR50      = 0xFF24;                                         //Master volume & VIN panning                                          R / W    All
    constexpr word NR51      = 0xFF25;                                         //Sound panning                                                        R / W    All
    constexpr word NR52      = 0xFF26;                                         //Sound on / off                                                       Mixed    All
    constexpr word WAVERAM0  = 0xFF30;                                         //Storage for one of the sound channels’ waveform                      R / W    All
    constexpr word WAVERAMF  = 0xFF3F;                                         //Storage for one of the sound channels’ waveform                      R / W    All
    constexpr word LCDC      = 0xFF40;                                         //LCD control                                                          R / W    All
    constexpr word STAT      = 0xFF41;                                         //LCD status                                                           Mixed    All
    constexpr word SCY       = 0xFF42;                                         //Viewport Y position                                                  R / W    All
    constexpr word SCX       = 0xFF43;                                         //Viewport X position                                                  R / W    All
    constexpr word LY        = 0xFF44;                                         //LCD Y coordinate                                                     R        All
    constexpr word LYC       = 0xFF45;                                         //LY compare                                                           R / W    All
    constexpr word DMA       = 0xFF46;                                         //OAM DMA source address & start                                       R / W    All
    constexpr word BGP       = 0xFF47;                                         //BG palette data                                                      R / W    DMG
    constexpr word OBP0      = 0xFF48;                                         //OBJ palette 0 data                                                   R / W    DMG
    constexpr word OBP1      = 0xFF49;                                         //OBJ palette 1 data                                                   R / W    DMG
    constexpr word WY        = 0xFF4A;                                         //Window Y position                                                    R / W    All
    constexpr word WX        = 0xFF4B;                                         //Window X position plus 7                                             R / W    All
    constexpr word KEY1      = 0xFF4D;                                         //Prepare speed switch                                                 Mixed    CGB
    constexpr word VBK       = 0xFF4F;                                         //VRAM bank                                                            R / W    CGB
    constexpr word HDMA1     = 0xFF51;                                         //VRAM DMA source high                                                     W    CGB
    constexpr word HDMA2     = 0xFF52;                                         //VRAM DMA source low                                                      W    CGB
    constexpr word HDMA4     = 0xFF54;                                         //VRAM DMA destination low                                                 W    CGB
    constexpr word HDMA3     = 0xFF53;                                         //VRAM DMA destination high                                                W    CGB
    constexpr word HDMA5     = 0xFF55;                                         //VRAM DMA length / mode / start                                       R / W    CGB
    constexpr word RP        = 0xFF56;                                         //Infrared communications port                                         Mixed    CGB
    constexpr word BCPS_BGPI = 0xFF68;                                         //Background color palette specification / Background palette index    R / W    CGB
    constexpr word BCPD_BGPD = 0xFF69;                                         //Background color palette data / Background palette data              R / W    CGB
    constexpr word OCPS_OBPI = 0xFF6A;                                         //OBJ color palette specification / OBJ palette index                  R / W    CGB
    constexpr word OCPD_OBPD = 0xFF6B;                                         //OBJ color palette data / OBJ palette data                            R / W    CGB
    constexpr word OPRI      = 0xFF6C;                                         //Object priority mode                                                 R / W    CGB
    constexpr word SVBK      = 0xFF70;                                         //WRAM bank                                                            R / W    CGB
    constexpr word PCM12     = 0xFF76;                                         //Audio digital outputs 1 & 2                                          R        CGB
    constexpr word PCM34     = 0xFF77;                                         //Audio digital outputs 3 & 4                                          R        CGB
    constexpr word IE        = 0xFFFF;                                         //Interrupt enable                                                     R / W    All
}
