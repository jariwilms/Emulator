#include "GameboyAdvanceSP.hpp"

#include "Gameboy/Component/CPU/ARM7TDMI.hpp"

GameboyAdvanceSP::GameboyAdvanceSP()
{
	m_cpu = std::make_unique<ARM7TDMI>();
}

void GameboyAdvanceSP::on()
{

}

void GameboyAdvanceSP::off()
{
	
}
