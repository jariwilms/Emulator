#include <iostream>

#include "dotM/GameBoyAdvance/GameBoyAdvance.hpp"
#include "dotM/GameBoyAdvance/Component/CPU/ARM7TDMI.hpp"

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"

int main(int argc, char** argv)
{
    //dot::gba::GameBoyAdvance gameboy{};
    //gameboy.boot();
    //
    //auto& vram = gameboy.m_vram;
    //const auto& mem  = vram->m_memory.get();
    //constexpr auto VRAM_SIZE = 76800; //GBA resolution: 240 * 160 * 2 (2 bytes per pixel)
    //std::vector<dword> result{};
    //result.resize(VRAM_SIZE * 2); //Go from 16bpp to 32bpp (GBA -> SFML format)
    //sf::RenderWindow window(sf::VideoMode(240, 160), "Game Boy Advance");
    //
    //while (window.isOpen())
    //{
    //    sf::Event event{};
    //    while (window.pollEvent(event))
    //    {
    //        if (event.type == sf::Event::Closed) window.close();
    //    }
    //    gameboy.update();
    //    //VRAM layout from msb to lsb: [1-, 5B, 5G, 5R]...
    //    for (int i = 0; i < VRAM_SIZE; i += 2)
    //    {
    //        const auto& colorLow = mem[i];
    //        const auto& colorHigh = mem[i + 1];
    //        const word& compositeColor = static_cast<word>(colorHigh) << 16 | colorLow;
    //        const auto& redValue = static_cast<byte>(get_bits(compositeColor, 0, 5));
    //        const auto& greenValue = static_cast<byte>(get_bits(compositeColor, 5, 5));
    //        const auto& blueValue = static_cast<byte>(get_bits(compositeColor, 10, 5));
    //        const dword& resultingColor = redValue << 24 | greenValue << 16 | blueValue << 8 | 255;
    //        result[i / 2] = resultingColor;
    //    }
    //    sf::Image image{};
    //    image.create(240, 160, reinterpret_cast<sf::Uint8*>(result.data()));
    //    sf::Texture texture{};
    //    texture.create(240, 160);
    //    texture.update(image);
    //    sf::Sprite sprite{ texture };
    //    window.clear();
    //    window.draw(sprite);
    //    window.display();
    //}











	std::cout << "Press ENTER to exit..." << std::endl;
	std::cin.get();

	return EXIT_SUCCESS;
}
