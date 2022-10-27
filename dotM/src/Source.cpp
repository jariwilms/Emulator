#include <stdafx.hpp>

#include "dotM/GameboyAdvance/GameboyAdvance.hpp"
#include "dotM/GameboyAdvance/Component/CPU/ARM7TDMI.hpp"

#include "Test.hpp"

int main(int argc, char** argv)
{
	using namespace dot::gba;
	GameboyAdvance gba{};
	gba.on();

	
	
	//auto& arm = gba.m_CPU;
	//arm->m_registers.r0 = 12;
	//arm->m_registers.r1 = 0xDEAD;
	//arm->m_registers.r2 = 0xBEEF;
	//
	////P0 = POST, P1 = PRE
	////                           COND 100 P U S W L   RN   REGISTERLIST
	//ARM7TDMI::Instruction bda{ 0b1110'100'1'0'0'1'1'0000'00000000000110 };     //AL, 100, post, up, false, false, load, 1, 2
	//bda.cond = 0b1110;
	//bda.p = 1;
	//bda.u = 0;
	//bda.s = 0;
	//bda.w = 1;
	//bda.l = 1;
	//bda.rn = 0;
	//bda.registerlist = 0b0110;
	//
	//arm->block_data_transfer(bda);


	
	std::cout << "Press ENTER to exit..." << std::endl;
	std::cin.get();

	return 0;
}
