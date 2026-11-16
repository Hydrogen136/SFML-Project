#include <SFML/Graphics.hpp>
using namespace sf;
int main()
{
	RenderWindow window(VideoMode({ 1600,800 }), "My Game");
    CircleShape circle(50.f);
    circle.setFillColor(Color::Blue);
	while (window.isOpen()) {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear();
        window.display();

	}
    

}
