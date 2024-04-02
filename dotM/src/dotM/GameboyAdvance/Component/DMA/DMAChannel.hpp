#pragma once

#include <stdafx.hpp>

#include "dotM/GameBoyAdvance/Component/BUS/BUS.hpp"
#include "dotM/GameBoyAdvance/Component/BUS/IConnectable.hpp"
#include "dotM/GameBoyAdvance/Architecture/MEMORY/MAP.hpp"

namespace dot::gba
{
	class DMAChannel : public IConnectable<32>
	{
	public:
		DMAChannel(unsigned int index)
			: m_index { index } {}
		~DMAChannel() = default;

		void cycle()
		{
			using namespace dot::arc;

			const auto addr = address();
			m_count   = bus->read<word>(addr);
			m_control = bus->read<word>(addr + 2);



			const auto source      = bus->read<dword>(REG_DMA0SAD + (m_index * 12));
			const auto destination = bus->read<dword>(REG_DMA0DAD + (m_index * 12));

			const auto enabled = get_bit (m_control, 15);
			const auto da      = get_bits(m_control,  5,  2);
			const auto sa      = get_bits(m_control,  7, 10);
			const auto r       = get_bit (m_control,  9);
			const auto cs      = get_bit (m_control, 10);			
			const auto tm      = get_bits(m_control, 12,  2);

			const auto i       = get_bit (m_control, 14);
			const auto en      = get_bit (m_control, 15);


			
			if (!enabled) return;
		}

		void transfer()
		{
			
		}
		
	private:
		unsigned int address() const
		{
			using namespace dot::arc;
			return REG_DMA0CNT + (m_index * 4);
		}

		unsigned int m_delay{};
		
		word m_count{};
		word m_control{};

		const unsigned int m_index{};
	};
}
