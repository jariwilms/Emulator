#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"
#include "dotM/GameboyAdvance/Component/BUS/IConnectable.hpp"
#include "dotM/Architecture/MEMORY/MAP.hpp"

namespace dot::gba
{
	class Timer : public IConnectable<32>
	{
	public:
		Timer(unsigned int index, std::shared_ptr<Timer> cascade = nullptr)
			: m_index{ index }, m_frequency{ m_index ? (2 ^ (m_index * 2 + 4)) : 1 }, m_cascade{ cascade } {}
		~Timer() = default;

		void cycle()
		{
			using namespace dot::arc;
			
			const auto addr = address();
			m_counter = bus->read<word>(addr);                                 //Lower 16 bits
			m_control = bus->read<word>(addr + 2);					           //Upper 16 bits


			
			const auto enabled = get_bit(m_control, 7);
			if (!enabled) return;


			
			const auto cascade = get_bit(m_control, 2);                        //In Cascade mode the timer is incremented by the previous timer
			if (cascade && m_index > 0)
			{
				const auto counter = m_cascade->read();
				if (counter == std::numeric_limits<word>::max())
				{
					++m_counter;
					bus->write<word>(addr, m_counter);
				}
			}
			else                                                               //In Free mode the timer is incremented by the frequency
			{
				++m_cycles;

				if (m_cycles >= m_frequency)
				{
					m_cycles -= m_frequency;

					if (m_counter == std::numeric_limits<word>::max())
					{
						m_counter = m_reload;

						const auto irq = get_bit(m_control, 6);
						if (irq) raise_interrupt();
					}
					else
					{
						++m_counter;
					}

					bus->write<word>(addr, m_counter);
				}
			}
		}

		word read() const
		{
			return m_counter;
		}
		void write(word reload)
		{
			m_reload = reload;
		}

		void enable()
		{
			const auto addr = address();
			m_control = set_bit(m_control, 7, true);

			bus->write(addr + 2, m_control);
		}
		void disable()
		{
			const auto addr = address();
			m_control = set_bit(m_control, 7, false);

			bus->write(addr + 2, m_control);
		}

		unsigned int index() const
		{
			return m_index;
		}
		unsigned int frequency() const
		{
			return m_frequency;
		}
		
	private:
		unsigned int address() const
		{
			using namespace dot::arc;
			return REG_TM0CNT + (m_index * 4);
		}
		void raise_interrupt() const
		{
			using namespace dot::arc;
			
			const auto IF = bus->read<word>(REG_IF);
			const auto interrupt = IF | (0x8 << m_index);

			bus->write<word>(REG_IF, interrupt);
		}
		
		unsigned int m_cycles{};

		word m_counter{};
		word m_control{};
		word m_reload{};
		
		const unsigned int m_index{};
		const unsigned int m_frequency{};
		const std::shared_ptr<Timer> m_cascade{};
	};
}
