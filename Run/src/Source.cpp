#include <iostream>

#include "dotM/Gameboy/GameBoy.hpp"
#include "dotM/GameBoyAdvance/GameBoyAdvance.hpp"

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"

int main(int argc, char** argv)
{
    dot::gb::GameBoy gb{};
    gb.on();





    sf::RenderWindow window(sf::VideoMode(160, 144), "Game Boy");

    std::vector<sf::Uint8> output{};
    output.resize(160 * 144 * 4);


    while (true)
    //for (int i = 0; i < 10000000; ++i)
    {
        gb.m_cpu->cycle();

        if (gb.m_memory.at(0xFF02) == 0x81)
        {
            const auto& c = gb.m_memory.at(0xFF01);
            std::cout << c;
            gb.m_memory.at(0xFF02) = 0x0;
        }
    }

    int a{};

    ////VRAM layout from msb to lsb: [1-, 5B, 5G, 5R]...
    //for (int i = 0; i < 0xFFFF; ++i)
    //{
    //    output.at(i) = gb.m_display->m_screen.at(i / 4);
    //}


    //while (window.isOpen())
    //{
    //    sf::Event event{};
    //    while (window.pollEvent(event))
    //    {
    //        if (event.type == sf::Event::Closed) window.close();
    //    }







    //    sf::Image image{};
    //    image.create(160, 144, reinterpret_cast<sf::Uint8*>(output.data()));

    //    sf::Texture texture{};
    //    texture.create(160, 144);
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
