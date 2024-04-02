#include <stdafx.hpp>

#include "GameBoyAdvance.hpp"

namespace dot::gba
{
	GameBoyAdvance::GameBoyAdvance()
	{
		m_cpu   = std::make_shared<ARM7TDMI>();                                //TODO: make unique for all?
		m_ppu   = std::make_shared<PPU>();
		//m_dma   = std::make_shared<DMAController>();
		//
		m_bios  = std::make_shared<BIOS>();
		//m_iwram = std::make_shared<IWRAM>();
		//m_ewram = std::make_shared<EWRAM>();
		m_vram  = std::make_shared<VRAM>();
		//m_oam   = std::make_shared<OAM>();
		//m_opal  = std::make_shared<OPAL>();

		m_bus   = std::make_shared<BUS<32>>();


		//
		m_cpu->connect(m_bus);
		m_ppu->connect(m_bus);
		//m_ppu->connect2(m_vram); //TODO
		//m_dma->connect(m_bus);

		m_bus->connect(m_bios);
		//m_bus->connect(m_iwram);
		//m_bus->connect(m_ewram);
		m_bus->connect(m_vram);
		//m_bus->connect(m_oam);
		//m_bus->connect(m_opal);

		//m_bus->connect(m_cartridge.cart0);
		//m_bus->connect(m_cartridge.cart1);
		//m_bus->connect(m_cartridge.cart2);
		//m_bus->connect(m_cartridge.cartS);



		//unsigned int index{};
		//for (auto& timer : m_timers)                                           //Create 4 timers and link each one to its predecessor, excluding the first timer
		//{
		//	if (index) timer = std::make_shared<Timer>(index, m_timers[index - 1]);
		//	else       timer = std::make_shared<Timer>(index);

		//	timer->connect(m_bus);
		//	++index;
		//}
	}



	void GameBoyAdvance::on()
	{
		boot();
		run();
	}
	void GameBoyAdvance::off()
	{
		m_running = false;
	}
	void GameBoyAdvance::press(Key key)
	{
		using namespace dot::arc;
		
		const auto& io    = m_ioreg;
		auto* const input = io->data<word>(REG_KEYPAD);

		set_bit((*input), static_cast<unsigned int>(key), false);
	}
	
	void GameBoyAdvance::slot(const Cartridge& cartridge)
	{
		m_cartridge = cartridge;
		m_cartridgeSlotted = true;
	}
	void GameBoyAdvance::unslot()
	{
		m_cartridgeSlotted = false;
	}

	void GameBoyAdvance::boot()
	{
		const auto& biosFile = read_file(PAK_LOC);
		//const auto& biosFile = read_file(BIOS_LOC);
		if (biosFile.size() > m_bios->size()) throw std::invalid_argument{ "The given BIOS file is too large!" };

		std::memcpy(m_bios->data(BIOSADDR0), biosFile.data(), biosFile.size());

		m_running = true;
	}
	void GameBoyAdvance::update()
	{
		using Clock     = std::chrono::steady_clock;
		using DeltaTime = std::chrono::duration<double, std::milli>;

		static DeltaTime deltaTime{ 0 };
		static Clock::time_point t0{};
		static Clock::time_point t1{};
		constexpr auto scanlineDelta = 59.73f / 60.0f;


		
		t1 = Clock::now();
		deltaTime = t1 - t0;
		t0 = t1;
		
		m_cpu->cycle();
		//if (deltaTime.count() > scanlineDelta) m_ppu->cycle();
	}
	void GameBoyAdvance::run()
	{
		using clock = std::chrono::steady_clock;
		using DeltaTime = std::chrono::duration<double, std::milli>;
		
		DeltaTime deltaTime{ 0 };
		clock::time_point t0{};
		clock::time_point t1{};
		const double scanlineDelta = 59.73 / 60.0;



		while (m_running)
		{
			m_cpu->cycle();
			m_ppu->cycle();
			m_dma->cycle();
		}
	}
}
