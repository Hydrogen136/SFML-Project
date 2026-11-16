
#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

int textureloadchk(Texture& a, string add) {
    if (!a.loadFromFile(add))
    {
        cout << "Failed to load texture: " << add << endl;
        return -1;
    }
    return 0;
}

int main()
{
    int r = 5, c=10;
    vector<vector<int>> bricks;
	bricks.assign(r, vector<int>(c, 3)); // initialize all bricks to 1 hit left
    //mazeData.assign(rows, vector<char>(cols));

    //for (int r = 0; r < rows; r++)
    //{
    //    for (int c = 0; c < cols; c++)
    //    {
    //        maze >> mazeData[r][c];
    //    }
    //}
	float paddleX = 300.f, ballX = 300.f, ballY = 400.f;
    RectangleShape paddle({ 100.f, 14.f });
    CircleShape ball(6.f); // radius 6, so it fits in a 12 x 12 box
    paddle.setPosition({ (float)paddleX, 650.f });
    ball.setPosition({ (float)ballX, (float)ballY });
	FloatRect paddleBounds = paddle.getGlobalBounds();
    const float BRICK_W = 60.f, BRICK_H = 25.f; // one brick is 60 x 25 pixels
    const float BRICKS_Y = 100.f; // the bricks start at y = 100
    Color colors[4] = {Color(25, 25, 45), // 0: no brick
    Color::Green, // 1: one hit left
    Color::Yellow, // 2: two hits left
    Color::Red }; // 3: three hits left

    RectangleShape brick({ BRICK_W - 2, BRICK_H - 2 }); // smaller, so a gap shows
    
    
    RenderWindow window(VideoMode({ 600,700 }), "My Game");


    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                paddleX = paddleX - 8;
				paddle.setPosition({ (float)paddleX, 650.f });
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                paddleX = paddleX + 8;
                paddle.setPosition({ (float)paddleX, 650.f });
            }
               
        }

        window.clear(); 

        for (int rows = 0; rows < r; rows++)
        {
            for (int cols = 0; cols < c; cols++)
            {
                if (bricks[rows][cols] > 0) 
                {
          
                    brick.setFillColor(colors[bricks[rows][cols]]);
                    brick.setPosition({ cols * BRICK_W + 1, BRICKS_Y + rows * BRICK_H + 1 });
                    window.draw(brick);
                }
            }
        }
		ball.move(Vector2f(0.0f, 0.1f));
        
		window.draw(paddle);
		window.draw(ball);
        window.display(); // 4. Display the newly drawn frame
    }
}

