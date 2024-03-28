#include <iostream>

#include "dotM/GameboyAdvance/GameboyAdvance.hpp"
#include "dotM/GameboyAdvance/Component/CPU/ARM7TDMI.hpp"



int main(int argc, char** argv)
{
	dot::gba::GameboyAdvance gameboy{};
	gameboy.on();



	std::cout << "Press ENTER to exit..." << std::endl;
	std::cin.get();

	return 0;
}
