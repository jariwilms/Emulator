#include <iostream>

#include "dotM/GameboyAdvance/GameboyAdvance.hpp"
#include "dotM/GameboyAdvance/Component/CPU/ARM7TDMI.hpp"

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"

int main(int argc, char** argv)
{
    dot::gba::GameboyAdvance gameboy{};
    gameboy.on();



    //sf::RenderWindow window(sf::VideoMode(240, 160), "SFML window");

    //std::vector<sf::Uint8> mem
    //{
    //    255, 255, 255, 255, 0, 0, 0, 0, 
    //    0, 0, 0, 0, 255, 255, 255, 255
    //};

    //sf::Image image{};
    //image.create(2, 2, mem.data());
    ////image.create(2, 2, sf::Color{ 255, 255, 255 });

    //sf::Texture texture{};
    //texture.create(2, 2);
    //texture.update(image);

    //sf::Sprite sprite{ texture };
    //sprite.setScale(120.0f, 80.0f);


    //while (window.isOpen())
    //{
    //    // Process events
    //    sf::Event event{};
    //    while (window.pollEvent(event))
    //    {
    //        // Close window: exit
    //        if (event.type == sf::Event::Closed)
    //            window.close();
    //    }

    //    // Clear screen
    //    window.clear();

    //    // Draw the sprite
    //    window.draw(sprite);

    //    // Draw the string
    //    //window.draw(text);

    //    // Update the window
    //    window.display();
    //}




	std::cout << "Press ENTER to exit..." << std::endl;
	std::cin.get();

	return EXIT_SUCCESS;
}
