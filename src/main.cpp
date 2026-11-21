
#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

using namespace sf;
using namespace std;
int option = 0;
void directcalc(FloatRect& objectBounds, FloatRect& ballBounds, Vector2f& velocity, float speed, int brick = 0) {
    float objectCenter = objectBounds.position.x + objectBounds.size.x / 2.f;
    float ballCenter = ballBounds.position.x + ballBounds.size.x / 2.f;

    float hitOffset = (ballCenter - objectCenter) / (objectBounds.size.x / 2.f);
    hitOffset = clamp(hitOffset, -1.f, 1.f);

    float maxAngle = 60.f * (3.14159265f / 180.f);
    float angle = hitOffset * maxAngle;

    velocity.x = speed * sin(angle);
    if (velocity.y < 0.f)
    {
        velocity.y = speed * cos(angle);
    }
    else
    {
        
        velocity.y = -speed * cos(angle);
    }
}

int main() {
    int r, c;
    vector<vector<int>> bricks;

    ifstream bricksdata("Levels/level5.txt");

    if (!bricksdata.is_open()) {
        cout << "Failed to load bricks data" << endl;
        return -1;
    }

    bricksdata >> r >> c;

    bricks.assign(r, vector<int>(c));

    for (int rows = 0; rows < r; rows++)
    {
        for (int cols = 0; cols < c; cols++)
        {
            bricksdata >> bricks[rows][cols];
        }
    }

    float paddleX = 250.f, ballX = 300.f, ballY = 400.f;
    RectangleShape paddle({ 100.f, 14.f });
    CircleShape ball(6.f);
    paddle.setPosition({ paddleX, 650.f });
    ball.setPosition({ ballX, ballY });

    const float BRICK_W = 60.f, BRICK_H = 25.f;
    const float BRICKS_Y = 100.f;
    Color colors[4] = {
        Color(25, 25, 45), // 0: no brick
        Color::Green,      // 1: one hit left
        Color::Yellow,     // 2: two hits left
        Color::Red         // 3: three hits left
    };
    
    RectangleShape brick({ BRICK_W - 2, BRICK_H - 2 });

    RenderWindow window(VideoMode({ 600, 700 }), "Block buster");
    window.setFramerateLimit(60);

    float speed = 5.0f;
    Vector2f velocity(0.0f, speed);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == Mouse::Button::Left)
                {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (option == 0 && play.getGlobalBounds().contains(mousePos))
                    {
                        option = 1;
                    }
                    if (option == 0 && scores.getGlobalBounds().contains(mousePos))
                    {
						cout << "Scores button clicked!" << endl;
                        option = 2;
                    }
                }
            }
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
        switch (option) {
        case 0:
        {
			window.draw(background);
			window.draw(levels);
			window.draw(exit);
			window.draw(scores);
            window.draw(play);
            break;
        }
        case 1:{
         
            for (int rows = 0; rows < r; rows++)
            {
                for (int cols = 0; cols < c; cols++)
                {
                    if (bricks[rows][cols] > 0)
                    {
                        brick.setFillColor(colors[bricks[rows][cols]]);
                        brick.setOutlineThickness(1.5f);      // White border thickness
                        brick.setOutlineColor(Color::White);
                        brick.setPosition({ cols * BRICK_W + 1, BRICKS_Y + rows * BRICK_H + 1 });

                        FloatRect brickBounds = brick.getGlobalBounds();
                        FloatRect ballBounds = ball.getGlobalBounds();

                        if (brickBounds.findIntersection(ballBounds))
                        {
                            bricks[rows][cols]--;
                            directcalc(brickBounds, ballBounds, velocity, speed, 1); // brick = 1
                        }

                        if (bricks[rows][cols] > 0)
                        {
                            window.draw(brick);
                        }
                    }
                }
            }

            ball.move(velocity);

            FloatRect paddleBounds = paddle.getGlobalBounds();
            FloatRect ballBounds = ball.getGlobalBounds();

            // Window boundary checks for paddle
            if (paddleBounds.position.x < 0.f)
            {
                paddleX = 0.f;
                paddle.setPosition({ paddleX, 650.f });
            }
            if (paddleBounds.position.x + paddleBounds.size.x > 600.f)
            {
                paddleX = 600.f - paddleBounds.size.x;
                paddle.setPosition({ paddleX, 650.f });
            }
            // Window boundary checks for ball
            if (ballBounds.position.x <= 0.f)
                velocity.x = abs(velocity.x);
            if (ballBounds.position.x + ballBounds.size.x >= 600.f)
                velocity.x = -abs(velocity.x);
            if (ballBounds.position.y <= 0.f)
                velocity.y = abs(velocity.y);

            if (paddleBounds.findIntersection(ballBounds))
            {
                directcalc(paddleBounds, ballBounds, velocity, speed);
            }

            window.draw(paddle);
            window.draw(ball);
            break;
        }
        default:
        {
            break;
        }
        }

        window.display();
    }
}