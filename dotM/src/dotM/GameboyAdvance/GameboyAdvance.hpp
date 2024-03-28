#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/CPU/ARM7TDMI.hpp"
#include "dotM/GameboyAdvance/Component/DMA/DMAController.hpp"
#include "dotM/GameboyAdvance/Component/PPU/PPU.hpp"
#include "dotM/GameboyAdvance/Component/TIMER/Timer.hpp"
#include "dotM/GameboyAdvance/Specification/GBA.hpp"

namespace dot::gba
{
	class GameboyAdvance
	{
	public:
		enum Mode
		{
			On, 
			Off
		};
		enum Key
		{
			A      = 0,
			B      = 1,

			Select = 2,
			Start  = 3,
			
			Right  = 4,
			Left   = 5,
			Up     = 6,
			Down   = 7,

			R      = 8,
			L      = 9,
		};
		class Cartridge
		{
		public:
			Cartridge()
			{
				cart0 = std::make_shared<CART0>();
				cart1 = std::make_shared<CART1>();
				cart2 = std::make_shared<CART2>();
				cartS = std::make_shared<CARTS>();
			}
			~Cartridge() = default;

			static Cartridge create(const std::vector<byte>& data)
			{
				Cartridge cartridge{};
				const auto cartridgeCapacity = cartridge.capacity();
				const auto dataSize = data.size();
				auto sizeLeft = dataSize;

				if (dataSize > cartridgeCapacity) throw std::runtime_error("Cartridge data is too large.");



				std::memcpy(cartridge.cart0->data(CART0ADDR0), data.data() + (dataSize - sizeLeft), std::min(sizeLeft, cartridge.cart0->size()));
				sizeLeft -= std::min(sizeLeft, cartridge.cart0->size());
				
				std::memcpy(cartridge.cart1->data(CART1ADDR0), data.data() + (dataSize - sizeLeft), std::min(sizeLeft, cartridge.cart1->size()));
				sizeLeft -= std::min(sizeLeft, cartridge.cart1->size());
				
				std::memcpy(cartridge.cart2->data(CART2ADDR0), data.data() + (dataSize - sizeLeft), std::min(sizeLeft, cartridge.cart2->size()));
				sizeLeft -= std::min(sizeLeft, cartridge.cart2->size());
				
				std::memcpy(cartridge.cartS->data(CARTSADDR0), data.data() + (dataSize - sizeLeft), std::min(sizeLeft, cartridge.cartS->size()));
				sizeLeft -= std::min(sizeLeft, cartridge.cartS->size());

				

				return cartridge;
			}

			size_t capacity() const
			{
				return cart0->size() + cart1->size() + cart2->size() + cartS->size();
			}

			std::shared_ptr<CART0> cart0{};
			std::shared_ptr<CART1> cart1{};
			std::shared_ptr<CART2> cart2{};
			std::shared_ptr<CARTS> cartS{};
		};

		GameboyAdvance();
		virtual ~GameboyAdvance() = default;

		virtual void on();
		virtual void off();
		virtual void press(Key key);

		virtual void slot(const Cartridge& cartridge);
		virtual void unslot();

    protected:
		virtual void boot();
		virtual void update();
		virtual void run();



		std::shared_ptr<ARM7TDMI> m_cpu{};
		std::shared_ptr<PPU>      m_ppu{};
		std::shared_ptr<BUS<32>>  m_bus{};
								       
		std::shared_ptr<BIOS>  m_bios{};
		std::shared_ptr<EWRAM> m_ewram{};
		std::shared_ptr<IWRAM> m_iwram{};
		std::shared_ptr<IOREG> m_ioreg{};
							
		std::shared_ptr<OPAL>  m_opal{};
		std::shared_ptr<VRAM>  m_vram{};
		std::shared_ptr<OAM>   m_oam{};
								       
		std::shared_ptr<DISPLAY> m_display{};
		
		std::shared_ptr<DMAController> m_dma{};
		
		std::array<std::shared_ptr<Timer>, TIMER_C> m_timers{};

		Cartridge m_cartridge{};



		bool m_running{};
		bool m_cartridgeSlotted{};
	};
}
