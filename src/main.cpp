#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

using namespace sf;
using namespace std;

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

void scoresave(int playerscr) {
    int score = 0;
    ifstream scorefile("scores/scores.txt");
    scorefile >> score;
    cout << "Current high score: " << score << endl;
    scorefile.close();
    if (score < playerscr)
    {
        cout << "New high score: " << playerscr << endl;
        ofstream output("scores/scores.txt");
        if (!output.is_open()) {
            cout << "Failed to open scores file" << endl;
            return;
        }
        output << playerscr << endl;
        output.close();
    }

}
bool levelload(int lvl, int& r, int& c, vector<vector<int>>& bricks) {

    string levelFile = "Levels/level" + to_string(lvl) + ".txt";
    ifstream bricksdata(levelFile);

    if (!bricksdata.is_open()) {
        cout << "Failed to load bricks data" << endl;
        return false;
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
    return true;
}
void resetter(int &scoretxt, int &lives, int level, int r, int c, vector<vector<int>>& bricks, float &paddleX,Text &scrtxt,Text &livesText,int &ballstate,RectangleShape &paddle) {
	ballstate = 0;
    // Reset score
    scoretxt = 0;
    // Reset lives
    lives = 3;
    // Reset paddle
    paddleX = 250.f;
    //setting strings 
    scrtxt.setString("Score: " + to_string(scoretxt));
    livesText.setString("Lives: " + to_string(lives));
	paddle.setPosition({ paddleX, 650.f });
    // Reload the selected level
    levelload(level, r, c, bricks);

}
bool wincondition(int r, int c, vector<vector<int>>& bricks) {
	for (int rows = 0; rows < r; rows++)
	{
		for (int cols = 0; cols < c; cols++)
		{
			if (bricks[rows][cols] > 0)
			{
				return false;
			}
		}
	}
	return true;
}
bool loadsavegame(int &lives, int& level, int& scoretxt, int& r, int& c, vector<vector<int>>& bricks) {

	ifstream savefile("data.txt");
	if (!savefile.is_open()) {
		cout << "Failed to load saved game data" << endl;
		return false;
	}
	savefile >> level >> scoretxt >> lives >> r >> c;
	bricks.assign(r, vector<int>(c));
	for (int rows = 0; rows < r; rows++)
	{
		for (int cols = 0; cols < c; cols++)
		{
			savefile >> bricks[rows][cols];
		}
	}
    return true;
}
struct LevelBtn {
    RectangleShape shape;
    Texture texture;
    string levelPath;
};
int main() {
    int r = 0, c = 0;
    vector<vector<int>> bricks;
    int scoretxt = 0,lives = 3,option = 0,level = 1,ballstate = 0;
    levelload(level, r, c, bricks);
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

    // Main window
    RenderWindow window(VideoMode({ 600, 700 }), "Block buster");
    window.setFramerateLimit(60);









    //Game sounds
    // for brick sound
    SoundBuffer brickBuffer;

    if (!brickBuffer.loadFromFile("Sounds/brick.wav"))
    {
        cout << "Brick sound failed to load!" << endl;
    }

    Sound brickSound(brickBuffer);

    // for game over sound
    SoundBuffer gameOverBuffer;

    if (!gameOverBuffer.loadFromFile("Sounds/game over.mp3"))
    {
        cout << "Game Over sound failed to load!" << endl;
    }

    Sound gameOverSound(gameOverBuffer);

    // for paddle sound
    SoundBuffer paddleBuffer;
    if (!paddleBuffer.loadFromFile("Sounds/paddle.wav"))
    {
        cout << "Paddle sound failed to load!" << endl;
    }
    Sound paddleSound(paddleBuffer);

    // for life lost sound
    SoundBuffer lifeLostBuffer;
    if (!lifeLostBuffer.loadFromFile("Sounds/life lost.mp3"))
    {
        cout << "Life lost sound failed to load!" << endl;
    }
    Sound lifeLostSound(lifeLostBuffer);

    // for main menu music
    SoundBuffer menuBuffer;
    if (!menuBuffer.loadFromFile("Sounds/Main menu.mp3"))
    {
        cout << "Main menu sound failed to load!" << endl;
    }
    Sound menuSound(menuBuffer);
    menuSound.setLooping(true);

    float speed = 5.0f;
    Vector2f velocity(0.0f, 0.0f);
    //score and lives text
    Font font;
    if (!font.openFromFile("C:\\Windows\\Fonts\\arial.ttf"))
    {
        cout << "Failed to load font" << endl;
        return -1;
    }
    Text scoreText(font, "Score: 0", 24);
    scoreText.setPosition({ 10.f, 10.f });
    Text livesText(font, "Lives: 3", 24);
    livesText.setPosition({ 10.f, 40.f });







    // Game Menu 
    Texture playbtn, levelbtn, scorebtn, exitbtn, bgTexture, logoTexture, conbtn,wintexture,retrytexture,nxtlvltexture;
    if (!nxtlvltexture.loadFromFile("images/nxtlvlbtn.jpg"))
    {
        cout << "Failed to load play button texture" << endl;
        return -1;
    }    
    if (!retrytexture.loadFromFile("images/retrybtn.jpg"))
    {
        cout << "Failed to load play button texture" << endl;
        return -1;
    }
    if (!playbtn.loadFromFile("images/play.jpg")) {
        cout << "Failed to load play button texture" << endl;
        return -1;
    }
    if (!levelbtn.loadFromFile("images/levels.jpg")) {
        cout << "Failed to load level button texture" << endl;
        return -1;
    }
    if (!scorebtn.loadFromFile("images/scores.jpg")) {
        cout << "Failed to load scores button texture" << endl;
        return -1;
    }
    if (!exitbtn.loadFromFile("images/exit.jpg")) {
        cout << "Failed to load exit button texture" << endl;
        return -1;
    }
    if (!bgTexture.loadFromFile("images/bg.jpg")) {
        cout << "Failed to load background texture" << endl;
        return -1;
    }
    if (!logoTexture.loadFromFile("images/logo.png"))
    {
        cout << "Failed to load logo" << endl;
        return -1;
    }
    if (!conbtn.loadFromFile("images/continue.jpg"))
    {
        cout << "Failed to load continue button texture" << endl;
        return -1;
    }

    Sprite logo(logoTexture);

    logo.setScale({ 0.30f, 0.30f });
    logo.setPosition({ 70.f, 30.f });

    RectangleShape background({ 600.f, 700.f });
    background.setPosition({ 0.f, 0.f });
    background.setTexture(&bgTexture);
    RectangleShape play({ 200.f, 50.f }), continuebtn({ 200.f, 50.f }), scores({ 200.f, 50.f }), levels({ 200.f, 50.f }), exit({ 200.f, 50.f }), retrybtn({ 175.f, 60.f }), nxtlvlbtn({ 175.f, 60.f });
    play.setPosition({ 200.f, 300.f });
    play.setTexture(&playbtn);
    continuebtn.setPosition({ 200.f, 360.f });
    continuebtn.setTexture(&conbtn);
    scores.setPosition({ 200.f, 420.f });
    scores.setTexture(&scorebtn);
    levels.setPosition({ 200.f, 480.f });
    levels.setTexture(&levelbtn);
    exit.setPosition({ 200.f, 540.f });
    exit.setTexture(&exitbtn);
	retrybtn.setPosition({ 220.f, 380.f });
	retrybtn.setTexture(&retrytexture);
    nxtlvlbtn.setPosition({ 220.f, 440.f });
    nxtlvlbtn.setTexture(&nxtlvltexture);
    if (!wintexture.loadFromFile("images/windialog.jpg")) {
        cout << "Failed to load win texture" << endl;
        return -1;
    }
    wintexture.setSmooth(true);
    (void)wintexture.generateMipmap();

    Sprite winSprite(wintexture);

    // Scale uniformly using the height so the aspect ratio never stretches
    Vector2u texSize = wintexture.getSize();
    float targetHeight = 560.f; // Fits as a centered dialog inside 600x700
    float scale = targetHeight / static_cast<float>(texSize.y);
    winSprite.setScale({ scale, scale });

    // Round to whole pixels so OpenGL doesn't blur edges across sub-pixels
    float posX = std::round((600.f - texSize.x * scale) / 2.f);
    float posY = std::round((700.f - texSize.y * scale) / 2.f);
    winSprite.setPosition({ posX, posY });
	Text winscr(font, "000", 36);
	winscr.setPosition({ 275.f, 325.f });


    LevelBtn lvlBtns[5];

    for (int i = 0; i < 5; i++) {
        if (!lvlBtns[i].texture.loadFromFile("images/level" + to_string(i + 1) + ".jpg")) {
            cout << "Failed to load level " << i + 1 << " button texture" << endl;
            return -1;
        }

        lvlBtns[i].shape.setSize({ 200.f, 50.f });
        lvlBtns[i].shape.setPosition({ 200.f, 150.f + i * 90.f });
        lvlBtns[i].shape.setTexture(&lvlBtns[i].texture);
        lvlBtns[i].levelPath = "Levels/level" + to_string(i + 1) + ".txt";
    }








    // Main game loop
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
						resetter(scoretxt, lives, level, r, c, bricks, paddleX,scoreText,livesText,ballstate,paddle);
                        ball.setPosition({ 300.f, 400.f });
                        velocity = { 0.f, 0.f };

                        option = 1;


                    }
                    else if (option == 0 && scores.getGlobalBounds().contains(mousePos))
                    {
                        option = 2;
                    }
                    else if (option == 4 && retrybtn.getGlobalBounds().contains(mousePos))
                    {

                        option = 1;
						resetter(scoretxt, lives, level, r, c, bricks, paddleX, scoreText, livesText,ballstate,paddle);
                    }
                    else if (option == 4 && nxtlvlbtn.getGlobalBounds().contains(mousePos))
                    {
                        if (level<5)
                        {
                            level++;
                        }
                        else {
                            level = 1;
                        }
                        resetter(scoretxt, lives, level, r, c, bricks, paddleX,scoreText,livesText,ballstate,paddle);
                        ball.setPosition({ 300.f, 400.f });
                        velocity = { 0.f, 0.f };
						option = 1;
                    }
                    else if (option == 0 && continuebtn.getGlobalBounds().contains(mousePos))
                    {
                        if (loadsavegame(lives, level, scoretxt, r, c, bricks))
                        {
                            scoreText.setString("Score: " + to_string(scoretxt));
                            livesText.setString("Lives: " + to_string(lives));
                            ball.setPosition({ 300.f, 400.f });
                            velocity = { 0.f, 0.f };
                            ballstate = 0;
                            option = 1;
                        }
                        else
                        {
                            cout << "No saved game found. Please start a new game." << endl;
                        }

                    }
                    else if (option == 0 && levels.getGlobalBounds().contains(mousePos))
                    {
                        option = 3; // Open levels screen
                    }
                    else if ((option == 0 || option == 2) && exit.getGlobalBounds().contains(mousePos))
                    {
                        if (option == 2)
                        {
                            option = 0;
                        }
                        else {
                            window.close();
                        }
                        
                    }
                    else if (option == 3)
                    {
                        for (int i = 0; i < 5; i++)
                        {
                            if (lvlBtns[i].shape.getGlobalBounds().contains(mousePos))
                            {
                                if (levelload(i + 1, r, c, bricks))
                                {
                                    level = i + 1;
                                    cout << "level " << level << " loaded successfully." << endl;
                                    option = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (option == 1 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))) {
            paddleX = paddleX - 8;
            paddle.setPosition({ (float)paddleX, 650.f });
            if (!ballstate)
            {
                ballstate = 1;
                velocity = { 0.f, speed };
            }

        }
        if (option == 1 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))) {
            paddleX = paddleX + 8;
            paddle.setPosition({ (float)paddleX, 650.f });
            if (!ballstate)
            {
                ballstate = 1;
                velocity = { 0.f, speed };
            }

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) && option != 0 && option != 1) {
            option = 0;
        }
        window.clear();
        switch (option) {
        case 0:
        {


            if (menuSound.getStatus() != Sound::Status::Playing)
            {
                menuSound.play();
            }

            window.draw(background);
            window.draw(logo);
            window.draw(levels);
            window.draw(exit);
            window.draw(scores);
            window.draw(play);
            window.draw(continuebtn);
      
            break;
        }

        case 1: {
            window.draw(background);
            menuSound.stop();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                ofstream savefile("data.txt");
                if (savefile.is_open())
                {
                    savefile << level << " " << scoretxt << " " << lives << " " << r << " " << c << endl;
                    for (int i = 0; i < r; i++)
                    {
                        for (int j = 0; j < c; j++)
                        {
                            savefile << bricks[i][j] << " ";
                        }
                        savefile << endl;
                    }
                    velocity = { 0.f, 0.f };
                    option = 0;
                    ballstate = 0;
                }
                savefile.close();
            }
            else if (option == 2 || option == 3)
            {
                option = 0;
            }
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
                            scoreText.setString("Score: " + to_string(scoretxt += 10));
                            if (brickSound.getStatus() != Sound::Status::Playing)
                            {
                                brickSound.play();
                            }
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
            if (ballBounds.position.y >= 700.f)
            {
                if (lives > 1)
                {
                    lifeLostSound.stop();
                    lifeLostSound.play();

                    lives--;
                    livesText.setString("Lives: " + to_string(lives));
                    ball.setPosition({ 300.f, 400.f });
                    velocity = { 0.f, 0.f };
                    paddleX = 250.f;
                    paddle.setPosition({ paddleX, 650.f });
                    ballstate = 0;
                }
                else
                {
                    gameOverSound.play();
                    scoresave(scoretxt);
                    option = 0;
                    scoretxt = 0;
                    lives = 3;
                    levelload(level, r, c, bricks);
                    ballstate = 0;

                }
            }

            if (paddleBounds.findIntersection(ballBounds))
            {
                if (paddleSound.getStatus() != Sound::Status::Playing)
                {
                    paddleSound.play();
                }
                directcalc(paddleBounds, ballBounds, velocity, speed);
            }
            if (wincondition(r, c, bricks))
            {
                option = 4;
            }
            window.draw(paddle);
            window.draw(ball);
            window.draw(scoreText);
            window.draw(livesText);
            break;
        }
        case 2:
        {
            int score = 0;
            ifstream scoresFile("scores/scores.txt");
            if (scoresFile.is_open()) {
                scoresFile >> score;
                scoresFile.close();
            }

            window.draw(background);

            Text scoresTitle(font, "Highest score: ", 36);
            scoresTitle.setPosition({ 80.f, 150.f });

            Text plyrscr(font, to_string(score), 36);
            plyrscr.setPosition({ 360.f, 150.f });

            window.draw(scoresTitle);
            window.draw(plyrscr);
            window.draw(exit);
            break;
        }
        case 3:
        {
            window.draw(background);
            for (int i = 0; i < 5; i++)
            {
                window.draw(lvlBtns[i].shape);
            }
            if (!levelload(level, r, c, bricks))
            {
                cout << "Failed to load level " << level << "." << endl;
            }
            break;
        }
        case 4:
        {
			scoresave(scoretxt);
            window.draw(background);
            window.draw(winSprite);
			winscr.setString("" + to_string(scoretxt));
            window.draw(winscr);
			window.draw(retrybtn);
			window.draw(nxtlvlbtn);
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