#pragma once

namespace dot::gba
{
	//Memory-Mapped Hardware Registers
    constexpr auto BIT00 = (0x1 <<  0);
    constexpr auto BIT01 = (0x1 <<  1);
    constexpr auto BIT02 = (0x1 <<  2);
    constexpr auto BIT03 = (0x1 <<  3);
    constexpr auto BIT04 = (0x1 <<  4);
    constexpr auto BIT05 = (0x1 <<  5);
    constexpr auto BIT06 = (0x1 <<  6);
    constexpr auto BIT07 = (0x1 <<  7);
    constexpr auto BIT08 = (0x1 <<  8);
    constexpr auto BIT09 = (0x1 <<  9);
    constexpr auto BIT10 = (0x1 << 10);
    constexpr auto BIT11 = (0x1 << 11);
    constexpr auto BIT12 = (0x1 << 12);
    constexpr auto BIT13 = (0x1 << 13);
    constexpr auto BIT14 = (0x1 << 14);
    constexpr auto BIT15 = (0x1 << 15);

    //Joystick Keys
    constexpr auto BUTTON_A             = 0x0001;                              //A Button
    constexpr auto BUTTON_B             = 0x0002;                              //B Button
    constexpr auto BUTTON_SELECT        = 0x0004;                              //SELECT Button
    constexpr auto BUTTON_START	        = 0x0008;                              //START Button
    constexpr auto KEYPAD_RIGHT         = 0x0010;                              //Right Key
    constexpr auto KEYPAD_LEFT          = 0x0020;                              //Left  Key
    constexpr auto KEYPAD_UP            = 0x0040;                              //Up    Key
    constexpr auto KEYPAD_DOWN          = 0x0080;                              //Down  Key
    constexpr auto BUTTON_RIGHT	        = 0x0100;                              //R Shoulder Button
    constexpr auto BUTTON_LEFT          = 0x0200;                              //L Shoulder Button

    constexpr auto TMR_PRESCALER_1CK    = 0x0000;                              //Prescaler    1 Clock
    constexpr auto TMR_PRESCALER_64CK   = 0x0001;                              //Prescaler   64 Clocks
    constexpr auto TMR_PRESCALER_256CK  = 0x0002;                              //Prescaler  256 Clocks
    constexpr auto TMR_PRESCALER_1024CK = 0x0003;                              //Prescaler 1024 Clocks
    constexpr auto TMR_IF_ENABLE        = 0x0040;                              //Interrupt Request Enable
    constexpr auto TMR_ENABLE           = 0x0080;                              //Run Timer

    //Interrupts
    constexpr auto INT_VBLANK        = 0x0001;
    constexpr auto INT_HBLANK        = 0x0002;
    constexpr auto INT_VCOUNT        = 0x0004;                                 //Generate Interrupt on V-Counter Match
    constexpr auto INT_TIMER0        = 0x0008;
    constexpr auto INT_TIMER1        = 0x0010;
    constexpr auto INT_TIMER2        = 0x0020;
    constexpr auto INT_TIMER3        = 0x0040;
	constexpr auto INT_COMMUNICATION = 0x0080;                                 //Serial Communication Interrupt
    constexpr auto INT_DMA0          = 0x0100;
    constexpr auto INT_DMA1          = 0x0200;
    constexpr auto INT_DMA2          = 0x0400;
    constexpr auto INT_DMA3          = 0x0800;
    constexpr auto INT_KEYBOARD      = 0x1000;
    constexpr auto INT_CART          = 0x2000;                                 //Card Interrupt
    constexpr auto INT_ALL           = 0x4000;                                 //Any Interrupt

    //Sound Channels
    constexpr auto SOUNDINIT      = 0x8000;                                    //Restart Sound
	constexpr auto SOUNDDUTY87    = 0x0000;                                    //87.5% Wave Duty Cycle
	constexpr auto SOUNDDUTY75    = 0x0040;                                    //75%   Wave Duty Cycle
	constexpr auto SOUNDDUTY50    = 0x0080;                                    //50%   Wave Duty Cycle
	constexpr auto SOUNDDUTY25    = 0x00C0;                                    //25%   Wave Duty Cycle

    constexpr auto SOUND1PLAYONCE = 0x4000;                                    //Play sound once
    constexpr auto SOUND1PLAYLOOP = 0x0000;                                    //Play sound looped
    constexpr auto SOUND1INIT     = 0x8000;                                    //Makes the sound restart
    //constexpr auto SOUND1SWEEPSHIFTS(n)	n	                               //Number of sweep shifts (0-7)
    constexpr auto SOUND1SWEEPINC = 0x0000;	                                   //Sweep add (freq increase)
    constexpr auto SOUND1SWEEPDEC = 0x0008;	                                   //Sweep dec (freq decrese)
    //constexpr auto SOUND1SWEEPTIME(n)	(n<<4)	                               //Time of sweep (0-7)
    //constexpr auto SOUND1ENVSTEPS(n)	(n<<8)	                               //Envelope steps (0-7)
    constexpr auto SOUND1ENVINC   = 0x0800;                                    //Envelope increase
    constexpr auto SOUND1ENVDEC   = 0x0000;                                    //Envelope decrease
    //constexpr auto SOUND1ENVINIT(n)	(n<<12)                                //Initial envelope volume (0-15)

    constexpr auto SOUND2PLAYONCE = 0x4000;                                    //Play sound once
    constexpr auto SOUND2PLAYLOOP = 0x0000;                                    //Play sound looped
    constexpr auto SOUND2INIT	  = 0x8000;                                    //Makes the sound restart
    //#define SOUND2ENVSTEPS(n)	(n<<8)                                         //Envelope steps (0-7)
    constexpr auto SOUND2ENVINC   = 0x0800;                                    //Envelope increase
    constexpr auto SOUND2ENVDEC   = 0x0000;                                    //Envelope decrease
    //#define SOUND2ENVINIT(n)	(n<<12                                         //Initial envelope volume (0-15)

    constexpr auto SOUND3BANK32	  = 0x0000;                                    //Use two banks of 32 steps each
    constexpr auto SOUND3BANK64	  = 0x0020;                                    //Use one bank of 64 steps
    constexpr auto SOUND3SETBANK0 = 0x0000;                                    //Bank to play 0 or 1 (non set bank is written to)
    constexpr auto SOUND3SETBANK1 = 0x0040;                                    
    constexpr auto SOUND3PLAY     = 0x0080;                                    //Output sound

    constexpr auto SOUND3OUTPUT0  = 0x0000;                                    //Mute output
    constexpr auto SOUND3OUTPUT1  = 0x2000;                                    //Output unmodified
    constexpr auto SOUND3OUTPUT12 = 0x4000;                                    //Output 1/2 
    constexpr auto SOUND3OUTPUT14 = 0x6000;                                    //Output 1/4 
    constexpr auto SOUND3OUTPUT34 = 0x8000;                                    //Output 3/4

    constexpr auto SOUND3PLAYONCE = 0x4000;                                    //Play sound once
    constexpr auto SOUND3PLAYLOOP = 0x0000;                                    //Play sound looped
    constexpr auto SOUND3INIT	  = 0x8000;                                    //Makes the sound restart

    constexpr auto SOUND4PLAYONCE = 0x4000;                                    //Play sound once
    constexpr auto SOUND4PLAYLOOP = 0x0000;                                    //Play sound looped
    constexpr auto SOUND4INIT     = 0x8000;                                    //Makes the sound restart
    //#define SOUND4ENVSTEPS(n)	(n<<8)	// envelope steps (0-7)
    constexpr auto SOUND4ENVINC   = 0x0800;                                    //Envelope increase
    constexpr auto SOUND4ENVDEC   = 0x0000;                                    //Envelope decrease
    //#define SOUND4ENVINIT(n)	(n<<12) // initial envelope volume (0-15)
    constexpr auto SOUND4STEPS7   = 0x0004;
    constexpr auto SOUND4STEPS15  = 0x0000;

    //Registers
    constexpr auto REG_INTERUPT    = 0x03007FFC;                               //Interrupt Register
    constexpr auto REG_DISPCNT     = 0x04000000;                               //Display Control (Mode)
    constexpr auto REG_DISPCNT_L   = 0x04000000;                               //???
    constexpr auto REG_DISPCNT_H   = 0x04000002;                               //???
    constexpr auto REG_DISPSTAT    = 0x04000004;                               //???
    constexpr auto REG_VCOUNT      = 0x04000006;                               //Vertical Control (Sync)
    constexpr auto REG_BG0CNT      = 0x04000008;                               //Background 0
    constexpr auto REG_BG1CNT      = 0x0400000A;                               //Background 1
    constexpr auto REG_BG2CNT      = 0x0400000C;                               //Background 2
    constexpr auto REG_BG3CNT      = 0x0400000E;                               //Background 3
    constexpr auto REG_BG0HOFS     = 0x04000010;                               //Background 0 Horizontal Offset
    constexpr auto REG_BG0VOFS     = 0x04000012;                               //Background 0 Vertical Offset
    constexpr auto REG_BG1HOFS     = 0x04000014;                               //Background 1 Horizontal Offset
    constexpr auto REG_BG1VOFS     = 0x04000016;                               //Background 1 Vertical Offset
    constexpr auto REG_BG2HOFS     = 0x04000018;                               //Background 2 Horizontal Offset
    constexpr auto REG_BG2VOFS     = 0x0400001A;                               //Background 2 Vertical Offset
    constexpr auto REG_BG3HOFS     = 0x0400001C;                               //Background 3 Horizontal Offset
    constexpr auto REG_BG3VOFS     = 0x0400001E;                               //Background 3 Vertical Offset
    constexpr auto REG_BG2PA       = 0x04000020;                               //Background 2 PA Rotation (pa = x_scale * cos(angle);)
    constexpr auto REG_BG2PB       = 0x04000022;                               //Background 2 PB Rotation (pb = y_scale * sin(angle);)
    constexpr auto REG_BG2PC       = 0x04000024;                               //Background 2 PC Rotation (pc = x_scale * -sin(angle);)
    constexpr auto REG_BG2PD       = 0x04000026;                               //Background 2 PD Rotation (pd = y_scale * cos(angle);)
    constexpr auto REG_BG2X        = 0x04000028;                               //Background 2 X Location
    constexpr auto REG_BG2X_L      = 0x04000028;                               //???
    constexpr auto REG_BG2X_H      = 0x0400002A;                               //???
    constexpr auto REG_BG2Y        = 0x0400002C;                               //Background 2 Y Location
    constexpr auto REG_BG2Y_L      = 0x0400002C;                               //???
    constexpr auto REG_BG2Y_H      = 0x0400002E;                               //???
    constexpr auto REG_BG3PA       = 0x04000030;                               //Background 3 PA Rotation (pa = x_scale * cos(angle);)
    constexpr auto REG_BG3PB       = 0x04000032;                               //Background 3 PB Rotation (pb = y_scale * sin(angle);)
    constexpr auto REG_BG3PC       = 0x04000034;                               //Background 3 PC Rotation (pc = x_scale * -sin(angle);)
    constexpr auto REG_BG3PD       = 0x04000036;                               //Background 3 PD Rotation (pd = y_scale * cos(angle);)
    constexpr auto REG_BG3X        = 0x04000038;                               //Background 3 X Location
    constexpr auto REG_BG3X_L      = 0x04000038;                               //???
    constexpr auto REG_BG3X_H      = 0x0400003A;                               //???
    constexpr auto REG_BG3Y        = 0x0400003C;                               //Background 3 Y Location
    constexpr auto REG_BG3Y_L      = 0x0400003C;                               //???
    constexpr auto REG_BG3Y_H      = 0x0400003E;                               //???
    constexpr auto REG_WIN0H       = 0x04000040;                               //Window 0 X coords (bits 0-7 right, bits 8-16 left)
    constexpr auto REG_WIN1H       = 0x04000042;                               //Window 1 X coords (bits 0-7 right, bits 8-16 left)
    constexpr auto REG_WIN0V       = 0x04000044;                               //Window 0 Y coords (bits 0-7 bottom, bits 8-16 top)
    constexpr auto REG_WIN1V       = 0x04000046;                               //Window 1 Y coords (bits 0-7 bottom, bits 8-16 top)
    constexpr auto REG_WININ       = 0x04000048;                               //Inside Window Settings
    constexpr auto REG_WINOUT      = 0x0400004A;                               //Outside Window Settings
    constexpr auto REG_MOSAIC      = 0x0400004C;                               //Mosaic Mode
    constexpr auto REG_MOSAIC_L    = 0x0400004C;                               //???
    constexpr auto REG_MOSAIC_H    = 0x0400004E;                               //???
    constexpr auto REG_BLDMOD      = 0x04000050;                               //Blend Mode
    constexpr auto REG_COLEV       = 0x04000052;                               //???
    constexpr auto REG_COLEY       = 0x04000054;                               //???

    constexpr auto REG_SOUND1CNT   = 0x04000060;                               //Sound 1
    constexpr auto REG_SOUND1CNT_L = 0x04000060;                               //
    constexpr auto REG_SOUND1CNT_H = 0x04000062;                               //
    constexpr auto REG_SOUND1CNT_X = 0x04000064;                               //
                                                                               
    constexpr auto REG_SOUND2CNT_L = 0x04000068;                               //Sound 2 length & wave duty
    constexpr auto REG_SOUND2CNT_H = 0x0400006C;                               //Sound 2 frequency + loop + reset
                                                                               
    constexpr auto REG_SG30        = 0x04000070;                               //???
    constexpr auto REG_SOUND3CNT   = 0x04000070;                               //???
    constexpr auto REG_SG30_L      = 0x04000070;                               //???
    constexpr auto REG_SOUND3CNT_L = 0x04000070;                               //???
    constexpr auto REG_SG30_H      = 0x04000072;                               //???
    constexpr auto REG_SOUND3CNT_H = 0x04000072;                               //???
    constexpr auto REG_SG31        = 0x04000074;                               //???
    constexpr auto REG_SOUND3CNT_X = 0x04000074;                               //???
                                                                               
    constexpr auto REG_SOUND4CNT_L = 0x04000078;                               //???
    constexpr auto REG_SOUND4CNT_H = 0x0400007C;                               //???

    constexpr auto REG_SGCNT0      = 0x04000080;                               
    constexpr auto REG_SGCNT0_L    = 0x04000080;                               
    constexpr auto REG_SOUNDCNT    = 0x04000080;                                 
    constexpr auto REG_SOUNDCNT_L  = 0x04000080;                               //DMG Sound Control
                                                                               
    constexpr auto REG_SGCNT0_H    = 0x04000082;                               
    constexpr auto REG_SOUNDCNT_H  = 0x04000082;                               //Direct Sound Control
                                                                               
    constexpr auto REG_SGCNT1      = 0x04000084;                               
    constexpr auto REG_SOUNDCNT_X  = 0x04000084;                               //Extended Sound Control
                                                                               
    constexpr auto REG_SGBIAS      = 0x04000088;                               
    constexpr auto REG_SOUNDBIAS   = 0x04000088;                               //Bitrate + Sound Bias
                                                                               
    constexpr auto REG_WAVE_RAM0   = 0x04000090;                               //???
    constexpr auto REG_SGWR0_L     = 0x04000090;                               //???
    constexpr auto REG_SGWR0_H     = 0x04000092;                               //???
    constexpr auto REG_WAVE_RAM1   = 0x04000094;                               //???
    constexpr auto REG_SGWR1_L     = 0x04000094;                               //???
    constexpr auto REG_SGWR1_H     = 0x04000096;                               //???
    constexpr auto REG_WAVE_RAM2   = 0x04000098;                               //???
    constexpr auto REG_SGWR2_L     = 0x04000098;                               //???
    constexpr auto REG_SGWR2_H     = 0x0400009A;                               //???
    constexpr auto REG_WAVE_RAM3   = 0x0400009C;                               //???
    constexpr auto REG_SGWR3_L     = 0x0400009C;                               //???
    constexpr auto REG_SGWR3_H     = 0x0400009E;                               //???
                                                                               
    constexpr auto REG_SGFIFOA     = 0x040000A0;                               //???
    constexpr auto REG_SGFIFOA_L   = 0x040000A0;                               //???
    constexpr auto REG_SGFIFOA_H   = 0x040000A2;                               //???
    constexpr auto REG_SGFIFOB     = 0x040000A4;                               //???
    constexpr auto REG_SGFIFOB_L   = 0x040000A4;                               //???
    constexpr auto REG_SGFIFOB_H   = 0x040000A6;                               //???
    constexpr auto REG_DMA0SAD     = 0x040000B0;                               //DMA0 Source Address
    constexpr auto REG_DMA0SAD_L   = 0x040000B0;                               //DMA0 Source Address Low Value
    constexpr auto REG_DMA0SAD_H   = 0x040000B2;                               //DMA0 Source Address High Value
    constexpr auto REG_DMA0DAD     = 0x040000B4;                               //DMA0 Destination Address
    constexpr auto REG_DMA0DAD_L   = 0x040000B4;                               //DMA0 Destination Address Low Value
    constexpr auto REG_DMA0DAD_H   = 0x040000B6;                               //DMA0 Destination Address High Value
    constexpr auto REG_DMA0CNT     = 0x040000B8;                               //DMA0 Control (Amount)
    constexpr auto REG_DMA0CNT_L   = 0x040000B8;                               //DMA0 Control Low Value
    constexpr auto REG_DMA0CNT_H   = 0x040000BA;                               //DMA0 Control High Value
                                                                               
    constexpr auto REG_DMA1SAD     = 0x040000BC;                               //DMA1 Source Address
    constexpr auto REG_DMA1SAD_L   = 0x040000BC;                               //DMA1 Source Address Low Value
    constexpr auto REG_DMA1SAD_H   = 0x040000BE;                               //DMA1 Source Address High Value
    constexpr auto REG_DMA1DAD     = 0x040000C0;                               //DMA1 Desination Address
    constexpr auto REG_DMA1DAD_L   = 0x040000C0;                               //DMA1 Destination Address Low Value
    constexpr auto REG_DMA1DAD_H   = 0x040000C2;                               //DMA1 Destination Address High Value
    constexpr auto REG_DMA1CNT     = 0x040000C4;                               //DMA1 Control (Amount)
    constexpr auto REG_DMA1CNT_L   = 0x040000C4;                               //DMA1 Control Low Value
    constexpr auto REG_DMA1CNT_H   = 0x040000C6;                               //DMA1 Control High Value
                                                                               
    constexpr auto REG_DMA2SAD     = 0x040000C8;                               //DMA2 Source Address
    constexpr auto REG_DMA2SAD_L   = 0x040000C8;                               //DMA2 Source Address Low Value
    constexpr auto REG_DMA2SAD_H   = 0x040000CA;                               //DMA2 Source Address High Value
    constexpr auto REG_DMA2DAD     = 0x040000CC;                               //DMA2 Destination Address
    constexpr auto REG_DMA2DAD_L   = 0x040000CC;                               //DMA2 Destination Address Low Value
    constexpr auto REG_DMA2DAD_H   = 0x040000CE;                               //DMA2 Destination Address High Value
    constexpr auto REG_DMA2CNT     = 0x040000D0;                               //DMA2 Control (Amount)
    constexpr auto REG_DMA2CNT_L   = 0x040000D0;                               //DMA2 Control Low Value
    constexpr auto REG_DMA2CNT_H   = 0x040000D2;                               //DMA2 Control High Value
                                                                               
    constexpr auto REG_DMA3SAD     = 0x040000D4;                               //DMA3 Source Address
    constexpr auto REG_DMA3SAD_L   = 0x040000D4;                               //DMA3 Source Address Low Value
    constexpr auto REG_DMA3SAD_H   = 0x040000D6;                               //DMA3 Source Address High Value
    constexpr auto REG_DMA3DAD     = 0x040000D8;                               //DMA3 Destination Address
    constexpr auto REG_DMA3DAD_L   = 0x040000D8;                               //DMA3 Destination Address Low Value
    constexpr auto REG_DMA3DAD_H   = 0x040000DA;                               //DMA3 Destination Address High Value
    constexpr auto REG_DMA3CNT     = 0x040000DC;                               //DMA3 Control (Amount)
    constexpr auto REG_DMA3CNT_L   = 0x040000DC;                               //DMA3 Control Low Value
    constexpr auto REG_DMA3CNT_H   = 0x040000DE;                               //DMA3 Control High Value

    constexpr auto REG_TM0CNT      = 0x04000100;                               //Timer 0
    constexpr auto REG_TM0CNT_L	   = 0x04000100;                               //Timer 0 count value
    constexpr auto REG_TM0CNT_H    = 0x04000102;                               //Timer 0 Control
                                                                               
    constexpr auto REG_TM1CNT	   = 0x04000104;                               //Timer 2
    constexpr auto REG_TM1CNT_L	   = 0x04000104;                               //Timer 2 count value
    constexpr auto REG_TM1CNT_H	   = 0x04000106;                               //Timer 2 control
                                                                               
    constexpr auto REG_TM2D        = 0x04000108;                               //Timer 3?
    constexpr auto REG_TM2CNT      = 0x0400010A;                               //Timer 3 Control
                                                                               
    constexpr auto REG_TM3D        = 0x0400010C;                               //Timer 4?
    constexpr auto REG_TM3CNT      = 0x0400010E;                               //Timer 4 Control
                                                                               
    constexpr auto REG_SCD0        = 0x04000120;                               //32-bit Normal Serial Communication Data 0 / Multi-play
    constexpr auto REG_SCD1        = 0x04000122;                               //32-bit Normal Serial Communication Data 1 /Multi-play
    constexpr auto REG_SCD2        = 0x04000124;                               //Multi-play Communication Data 2
    constexpr auto REG_SCD3        = 0x04000126;                               //Multi-play Communication Data 3
    constexpr auto REG_SCCNT       = 0x04000128;                               //???
    constexpr auto REG_SCCNT_L     = 0x04000128;                               //???
    constexpr auto REG_SCCNT_H     = 0x0400012A;                               //???
    constexpr auto REG_P1          = 0x04000130;                               //Player 1 Input
    constexpr auto REG_KEYPAD      = 0x04000130;                               //Player 1 Input
    constexpr auto REG_P1CNT       = 0x04000132;                               //Player 1 Input Interrupt Status
    constexpr auto REG_R           = 0x04000134;                               //???
    constexpr auto REG_HS_CTRL     = 0x04000140;                               //???
    constexpr auto REG_JOYRE       = 0x04000150;                               //???
    constexpr auto REG_JOYRE_L     = 0x04000150;                               //???
    constexpr auto REG_JOYRE_H     = 0x04000152;                               //???
    constexpr auto REG_JOYTR       = 0x04000154;                               //???
    constexpr auto REG_JOYTR_L     = 0x04000154;                               //???
    constexpr auto REG_JOYTR_H     = 0x04000156;                               //???
    constexpr auto REG_JSTAT       = 0x04000158;                               //???
    constexpr auto REG_JSTAT_L     = 0x04000158;                               //???
    constexpr auto REG_JSTAT_H     = 0x0400015A;                               //???
    constexpr auto REG_IE          = 0x04000200;                               //Interrupt Enable
    constexpr auto REG_IF          = 0x04000202;                               //Interrupt Flags
    constexpr auto REG_WSCNT       = 0x04000204;                               //???
    constexpr auto REG_IME         = 0x04000208;                               //Interrupt Master Enable
    constexpr auto REG_PAUSE       = 0x04000300;                               //Pause
}
