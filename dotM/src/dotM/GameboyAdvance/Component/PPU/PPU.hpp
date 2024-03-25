#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Specification/GBA.hpp"
#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"
#include "dotM/GameboyAdvance/Component/DISPLAY/DISPLAY.hpp"
#include "dotM/Architecture/MEMORY/MAP.hpp"
#include "dotM/GameboyAdvance/Component/INTERRUPT/IPeripheral.hpp"

namespace dot::gba
{
    class PPU : public IPeripheral, public IConnectable<32>
    {
	public:
		struct DisplayControl
		{
		public:
			DisplayControl(word value)
			{
				mode          = get_bits(value,  0, 2);
				gbc           = get_bits(value,  2, 1);
				pageSelect    = get_bits(value,  3, 1);
				hBlank        = get_bits(value,  4, 1);
				objectMapping = get_bits(value,  5, 1);
				forceBlank    = get_bits(value,  6, 1);
				background    = get_bits(value,  7, 5);
				window        = get_bits(value, 12, 4);
			}
			~DisplayControl() = default;


			
			bool is_background_enabled(unsigned int index) const
			{
				if (index > 3) return false;

				return get_bit(background, index);
			}
			

			
			byte mode          : 2{};
			byte gbc           : 1{};
			byte pageSelect    : 1{};
			byte hBlank        : 1{};
			byte objectMapping : 1{};
			byte forceBlank    : 1{};
			byte background    : 5{};
			byte window        : 4{};
		};
		struct DisplayStat
		{
		public:
			explicit DisplayStat(word value)
			{
				vBlankStatus    = get_bits(value, 0, 1);
				hBlankStatus    = get_bits(value, 1, 1);
				vCountStatus    = get_bits(value, 2, 1);
				vBlankInterrupt = get_bits(value, 3, 1);
				hBlankInterrupt = get_bits(value, 4, 1);
				vCountInterrupt = get_bits(value, 5, 1);
				_undefined      = get_bits(value, 6, 2);
				vCountTrigger   = get_bits(value, 8, 8);
			}
			~DisplayStat() = default;


			
			byte vBlankStatus     : 1{};
			byte hBlankStatus     : 1{};
			byte vCountStatus     : 1{};
			byte vBlankInterrupt  : 1{};
			byte hBlankInterrupt  : 1{};
			byte vCountInterrupt  : 1{};
			byte _undefined       : 2{};
			byte vCountTrigger    : 8{};
		};
		struct BackgroundControl
		{
		public:
			BackgroundControl(word value)
			{
				priority           = get_bits(value,  0, 2);
				characterBaseBlock = get_bits(value,  2, 2);
				_undefined         = get_bits(value,  4, 2);
				mosaic             = get_bits(value,  6, 1);
				colorMode          = get_bits(value,  7, 1);
				screenBaseBlock    = get_bits(value,  8, 5);
				affineWrapping     = get_bits(value, 13, 1);
				backgroundSize     = get_bits(value, 14, 2);
			}
			~BackgroundControl() = default;


			
			std::pair<unsigned int, unsigned int> tiles(bool isAffine = false) const //TODO: affine
			{
				if (isAffine)
				{
					switch (backgroundSize)
					{
						case 0b00: return std::make_pair( 16,  16);
						case 0b01: return std::make_pair( 32,  32);
						case 0b10: return std::make_pair( 64,  64);
						case 0b11: return std::make_pair(128, 128);
					}
				}
				else
				{
					switch (backgroundSize)
					{
						case 0b00: return std::make_pair(32, 32);
						case 0b01: return std::make_pair(64, 32);
						case 0b10: return std::make_pair(32, 64);
						case 0b11: return std::make_pair(64, 64);
					}
				}
			}
			std::pair<unsigned int, unsigned int> pixels(bool isAffine = false) const
			{
				auto [w, h] = tiles(false);
				return std::make_pair(w * 8, h * 8);
			}

			

			byte priority           : 2{};
			byte characterBaseBlock : 2{};
			byte _undefined         : 2{};
			byte mosaic             : 1{};
			byte colorMode          : 1{};
			byte screenBaseBlock    : 5{};
			byte affineWrapping     : 1{};
			byte backgroundSize     : 2{};
		};

		PPU() = default;
		~PPU() = default;

		void cycle()
		{
			using namespace dot::arc;
			
			DisplayControl displayControl = bus->read<word>(REG_DISPCNT);
			auto backgrounds = get_backgrounds(displayControl);
			
			if (backgrounds.empty()) return;
			
			std::sort(backgrounds.begin(), backgrounds.end(), [](const auto& first, const auto& second) { return first.priority < second.priority; });

			

			switch (displayControl.mode)
			{
				case 0b000: mode0(displayControl); break;
				case 0b001: mode1(displayControl); break;
				case 0b010: mode2(displayControl); break;
				case 0b011: mode3(displayControl); break;
				case 0b100: mode4(displayControl); break;
				case 0b101: mode5(displayControl); break;

				default:             throw std::runtime_error("Invalid display mode");
			}
		}

		void connect2(std::shared_ptr<VRAM> vram)
		{
			m_VRAM = vram;
		}

	protected:
		void mode0(const DisplayControl& displayControl)
		{
			
		}
		void mode1(const DisplayControl& displayControl)
		{

		}
		void mode2(const DisplayControl& displayControl)
		{

		}
		void mode3(const DisplayControl& displayControl)
		{
			const auto size = 76800;
			//std::memcpy(m_SCREEN->buffer.data(), m_VRAM->data(VRAMADDR0), size); //TODO: Dont do this => DMA
		}
		void mode4(const DisplayControl& displayControl)
		{
			const std::pair<unsigned int, unsigned int> dimensions{ 240, 160 };
			const auto bpp = 8;

			
		}
		void mode5(const DisplayControl& displayControl)
		{
			const std::pair<unsigned int, unsigned int> dimensions{ 160, 128 };
			const auto bpp = 16;

			
		}
		
		std::vector<BackgroundControl> get_backgrounds(const DisplayControl& displayControl)
		{
			using namespace dot::arc;
			
			std::vector<BackgroundControl> backgrounds;

			for (unsigned int i = 0; i < 4; i++)
			{
				if (displayControl.is_background_enabled(i))
				{
					backgrounds.emplace_back(bus->read<word>(REG_BG0CNT + (i * sizeof(word))));
				}
			}

			return backgrounds;
		}

	private:
		std::shared_ptr<DISPLAY> m_SCREEN{};
		std::shared_ptr<VRAM>    m_VRAM{};
	};
}
