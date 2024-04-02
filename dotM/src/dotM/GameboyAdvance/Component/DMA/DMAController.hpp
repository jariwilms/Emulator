#pragma once

#include <stdafx.hpp>

#include "dotM/GameBoyAdvance/Architecture/MEMORY/MAP.hpp"
#include "dotM/GameBoyAdvance/Component/BUS/BUS.hpp"
#include "dotM/GameBoyAdvance/Component/DMA/DMAChannel.hpp"
#include "dotM/GameBoyAdvance/Component/INTERRUPT/IPeripheral.hpp"

namespace dot::gba
{
	class DMAController : public IPeripheral, public IConnectable<32>
	{
	public:
		DMAController()
		{
			std::fill(m_delay.begin(), m_delay.end(), 2);
		}
		~DMAController() = default;

		void cycle()
		{
			using namespace dot::arc;

            constexpr auto CNT_OFS = 12;                                       //Each control register is offset by 12 bytes
            constexpr auto SRC_MSB = 28;                                       //Source address has 28 significant bits
            constexpr auto DST_MSB = 27;                                       //Destination address has 27 significant bits
			
			for (auto i = 0; i < 4; ++i)
			{
				const auto SRC_ADR = REG_DMA0SAD + (i * CNT_OFS);
				const auto DST_ADR = REG_DMA0DAD + (i * CNT_OFS);
				const auto CNT_ADR = REG_DMA0CNT + (i * CNT_OFS);
				
				const auto control = bus->read<word>(CNT_ADR);
				const auto enabled = get_bit(control, 15);

				if (enabled)
				{
					if (m_delay[i])
					{
						--m_delay[i];
						continue;
					}

					const auto source      = get_bits(bus->read<dword>(SRC_ADR), 0, SRC_MSB);
					const auto destination = get_bits(bus->read<dword>(DST_ADR), 0, DST_MSB);

					const auto count                 = get_bits(control,  0, 16);
					const auto destinationAdjustment = get_bits(control, 21,  2);
					const auto sourceAdjustment      = get_bits(control, 23,  2);
					const auto repeat                = get_bit (control, 25);
					const auto chunkSize             = get_bit (control, 26);
					const auto timing                = get_bits(control, 28,  2);
					const auto interrupt             = get_bit (control, 30);

					switch (timing)
					{
						case 0b00:
						{
							if (count)
							{
								const auto offset = chunkSize ? 4 : 2;
								
								//transfer data from source to dest
								if (chunkSize)
								{
									const auto data = bus->read<dword>(source);
									bus->write<dword>(destination, data);
								}
								else
								{
									const auto data = bus->read<word>(0);
									bus->write<word>(destination, data);
								}



								//adjust source and destination according to control register
								switch (sourceAdjustment)
								{
									case 0b00: break;
									case 0b01: break;
									case 0b10: break;
									case 0b11: throw std::runtime_error("Invalid source adjustment");
								}
								switch (destinationAdjustment)
								{
									case 0b00: bus->write<dword>(DST_ADR, destination + offset); break;
									case 0b01: bus->write<dword>(DST_ADR, destination - offset); break;
									case 0b10: break;
									case 0b11: bus->write<dword>(DST_ADR, destination + offset); break;
								}
								

								
								//decrement count
								bus->write<word>(REG_DMA0CNT + (i * CNT_OFS), count - 1);
							}
							else
							{
								const auto irq = get_bit(control, 30);
								if (irq) raise_interrupt();

								//reset destination register if DMA_DST_RELOAD is set
								if (destinationAdjustment == 0b11)
								{
									//put original dst in variable
								}

								//disable the channel
								bus->write<word>(REG_DMA0CNT + (i * CNT_OFS), 0);
							}


							break;
						}
						case 0b01: break;
						case 0b10: break;
						case 0b11: break;

						default:   throw std::runtime_error("Invalid DMA timing mode");
					}
				}
				else
				{
					m_delay[i] = 2;
				}
			}
		}

		template<typename T>
		void copy_data(size_t src, size_t dst)
		{
			throw std::runtime_error("Method has not been implemented!");
		}
		
	private:
		std::array<unsigned int, 4> m_delay{};
		std::array<size_t, 4> m_destinations{};
	};
}
