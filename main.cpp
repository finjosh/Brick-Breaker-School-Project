#include "physics.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "PowerUp.h"
#include <vector>
#include <string>
#include <math.h>
#include <thread>

using namespace std;
using namespace sf; // for graphics

void BuildLevel(string levelSetup, int &breakableBricksLeft, vector<Brick> &bricks, vector<PowerUp> &powerUps, b2World &world, Font *font);
string MakeRandomLevel();

int main()
{
    RenderWindow window(VideoMode(800, 600), "Brick Braker"); // creates a window on the screen that is 800 by 600
    window.setFramerateLimit(60);                             // sets the game loop to run 60 times per second
    b2World world(b2Vec2(0.f, 0.0f));

    Paddle p1(world, window.getSize().x / 2, window.getSize().y - 100, 150, 20, b2_kinematicBody);

    vector<Ball> balls;

    RectangleShape Lazer(Vector2f(10, 800));
    Lazer.setOrigin(Vector2f(5, 800 + p1.getLocalBounds().height/2));
    Lazer.setFillColor(Color::Red);
    float lazerTimer = 0.f;
    bool showLazer = false;
    float attackTimer = 0.f;

    vector<Brick> bricks;
    int BreakableBricksLeft = 0;
    int level = -1; // start as -1 for logical reasons
    vector<string> LevelSetup;
    // level 1
    LevelSetup.push_back("UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "UUUUUUUUEEEEEEEEEEUUUUUUUU"
                         "##########################");
    // level 2
    LevelSetup.push_back("##########################"
                         "33333333333333333333333333"
                         "##########################"
                         "2222222222222222222222222e"
                         "#########################e"
                         "eeeeeeeeeeeeeeeeeeeeeeeeee"
                         "9999999999999#UeU999999999"
                         "8888888888888#UeU888888888"
                         "7777777777777#UeU777777777"
                         "6666666666666#UeU666666666"
                         "5555555555555#UeU555555555"
                         "4444444444444#UeU444444444"
                         "3333333333333#UeU333333333"
                         "2222222222222#UeU222222222"
                         "##############UeU#########");
    // level 3
    LevelSetup.push_back("4444444444444444444444444e"
                         "4444444444444444444444444e"
                         "2222222222222222222222222e"
                         "2222222222222222222222222e"
                         "#########################e"
                         "999999999999999eeeeeeeeeee"
                         "99999999999999UeU999999999"
                         "88888888888888UeU888888888"
                         "77777777777777UeU777777777"
                         "66666666666666UeU666666666"
                         "55555555555555UeU555555555"
                         "44444444444444UeU444444444"
                         "33333333333333UeU333333333"
                         "22222222222222UeU222222222"
                         "##############UeU#########");
    // level 4
    LevelSetup.push_back("eeeeeeeeeeeeeeeeeeeeeeeeee"
                         "eeeeeeeeeeeeeeeeeeeeeeeeee"
                         "eeeee2222222222222222eeeee"
                         "eeeee2222222222222222eeeee"
                         "eeeee################eeeee"
                         "eeeeeeeeeeeeeeeeeeeeeeeeee"
                         "99999999999999UeU999999999"
                         "88888888888888UeU888888888"
                         "77777777777777UeU777777777"
                         "66666666666666UeU666666666"
                         "55555555555555UeU555555555"
                         "44444444444444UeU444444444"
                         "33333333333333UeU333333333"
                         "22222222222222UeU222222222"
                         "UUUUUUUUUUUUUUUeUUUUUUUUUU");
    // level 5
    LevelSetup.push_back("11122233344455566677788899"
                         "11122233344455566677788899"
                         "11122233344455566677788899"
                         "11122233344455566677788899"
                         "11122233344455566677788899"
                         "11122233344455566677788899"
                         "11122233344455566677788899"
                         "11122233344455566677788899"
                         "12312312312312312312312312"
                         "12312312312312312312312312"
                         "12312312312312312312312312"
                         "123123123123UEU12312312312"
                         "123123123123UEU12312312312"
                         "123123123123UEU12312312312"
                         "UUUUUUUUUUUUUEUUUUUUUUUUUU");

    vector<PowerUp> powerUps;

    Font font;
    font.loadFromFile("font.ttf");

    Block floor = physics::createBox(world, 0, window.getSize().y, window.getSize().x, 10, b2_staticBody);
    floor->GetFixtureList()->SetRestitution(1);

    Block rWall = physics::createBox(world, window.getSize().x, 0, 10, window.getSize().y, b2_staticBody);
    floor->GetFixtureList()->SetRestitution(1);

    Block lWall = physics::createBox(world, -10, 0, 10, window.getSize().y, b2_staticBody);
    floor->GetFixtureList()->SetRestitution(1);

    Block top = physics::createBox(world, 0, -10, window.getSize().x, 10, b2_staticBody);
    floor->GetFixtureList()->SetRestitution(1);

    Clock deltaClock;
    float deltaTime;

    Text score;
    score.setFont(font);
    score.setString("0");
    unsigned long int scoreInt = 0;

    Text info;
    info.setFont(font);
    info.setString("Press 'C' to skip a level\nPress enter to play");
    info.setPosition(window.getSize().x / 2 - info.getGlobalBounds().width / 2, window.getSize().y / 2 - info.getGlobalBounds().height / 2);
    string temp = "";

    while (!Keyboard::isKeyPressed(Keyboard::Enter))
    {
        Event event;                    // creates an event object, events include mouse clicks, mouse movement, keyboard presses, etc..
        while (window.pollEvent(event)) // loop that checks for events
        {
            if (event.type == Event::Closed) // checks if window is closed
                window.close();
        }

        window.clear();
        window.draw(info);
        window.display();
    }

    while (window.isOpen() && !Keyboard::isKeyPressed(Keyboard::Escape)) // the main game loop, exits if someone closes the window
    {
        deltaTime = deltaClock.getElapsedTime().asSeconds();
        deltaClock.restart();

        Event event;                    // creates an event object, events include mouse clicks, mouse movement, keyboard presses, etc..
        while (window.pollEvent(event)) // loop that checks for events
        {
            if (event.type == Event::Closed) // checks if window is closed
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::C)
            {
                BreakableBricksLeft = 0;
                scoreInt = 0;
            }
        } // ends the event loop

        world.Step(1.0 / 60.0, int32(8), int32(3));
        window.clear(); // clears the screen

        // used for testing
        // if (Keyboard::isKeyPressed(Keyboard::E))
        // {
        //     showLazer = true;
        //     attackTimer = 50;
        //     lazerTimer = 1;
        // }

        if (lazerTimer <= 0)
            showLazer = false;
        else {
            lazerTimer -= deltaTime;
            attackTimer += deltaTime;
        }
        if (showLazer)
            Lazer.setPosition(p1.getPosition());

        if (BreakableBricksLeft == 0)
        {
            lazerTimer = 0;
            showLazer = false;

            level++;
            string t_levelSetup = "";
            // setting t_levelSetup to the next premade level if there is one
            if (LevelSetup.size() - 1 >= level)
            {
                t_levelSetup = LevelSetup[level];
            }
            BuildLevel(t_levelSetup, BreakableBricksLeft, bricks, powerUps, world, &font);
            for (int i = balls.size() - 1; i >= 0; i--)
            {
                physics::deleteBlock(world, balls[i].body);
            }
            balls.clear();

            p1.body->SetTransform(b2Vec2(window.getSize().x / 2 / pixels_per_meter, p1.body->GetPosition().y), 0);
            p1.setPosition(Vector2f(p1.body->GetPosition().x * pixels_per_meter, p1.body->GetPosition().y * pixels_per_meter));

            balls.push_back(Ball(world, p1.getPosition().x, p1.getPosition().y, 10));
            balls.back().SetVelocity(Vector2f(0, -200));
        }
        else if (balls.size() == 0)
        {
            lazerTimer = 0;
            showLazer = false;

            string t_levelSetup = "";
            // setting t_levelSetup to the next premade level if there is one
            if (LevelSetup.size() - 1 >= level)
            {
                t_levelSetup = LevelSetup[level];
            }
            BuildLevel(t_levelSetup, BreakableBricksLeft, bricks, powerUps, world, &font);

            p1.body->SetTransform(b2Vec2(window.getSize().x / 2 / pixels_per_meter, p1.body->GetPosition().y), 0);
            p1.setPosition(Vector2f(p1.body->GetPosition().x * pixels_per_meter, p1.body->GetPosition().y * pixels_per_meter));

            // adding a new ball
            balls.push_back(Ball(world, p1.getPosition().x, p1.getPosition().y, 10));
            balls.back().SetVelocity(Vector2f(0, -200));

            scoreInt = 0;
        }

        bool attacked = false;
        // updates bricks while drawing and checking if they have been hit
        for (int i = bricks.size() - 1; i >= 0; i--)
        {
            int index = bricks[i].Update(window, world, balls);
            if (showLazer && index < 0 && attackTimer >= 0.15f && Lazer.getGlobalBounds().intersects(bricks[i].getGlobalBounds())) 
            {
                attacked = true;
                index = -1;
                if (--bricks[i].amount <= 0 && bricks[i].canBreak) {
                    physics::deleteBlock(world, bricks[i].body);
                    bricks.erase(bricks.begin() + i);
                    BreakableBricksLeft--;
                    scoreInt += 100;
                } 
                else {
                    bricks[i].text.setString(to_string(bricks[i].amount));
                    bricks[i].UpdateText();
                }
                
            }
            if (index >= 0)
            {
                float randNum = rand() % 100;

                // first checking if the brick is destroyed then if it is erasing it from the vector
                if (bricks[i].body == nullptr)
                {
                    bricks.erase(bricks.begin() + i);
                    // if the brick was destroied there is up to a 15% higher chance for a power up
                    randNum -= rand() % 4 + 1;

                    // getting rid of a brick from the bricks that are left
                    BreakableBricksLeft--;

                    // adding to score
                    scoreInt += 100;
                }

                // any of the chances to spawn something after this comment can be either that chance or
                // that chance plus a possible 5% chance if the brick was broken

                // "1"% chance to spawn a lazer
                if (randNum <= 1)
                {
                    powerUps.push_back(PowerUp(balls[index].getPosition().x, balls[index].getPosition().y, 10, Color::Red, 0));
                }
                // "2"% chance to spawn a ball that does not get destroied by ANYTHING but after a time of 60 to 120 seconds it dies
                else if (randNum <= 2)
                {
                    powerUps.push_back(PowerUp(balls[index].getPosition().x, balls[index].getPosition().y, 10, Color::Green, 1));
                }
                // "3"% chance to spawn a normal ball
                else if (randNum <= 3)
                {
                    powerUps.push_back(PowerUp(balls[index].getPosition().x, balls[index].getPosition().y, 10, Color::White, 2));
                }
                // "5"% chance to spawn a ball that gets destroied over number of hits (limit at 25 hits)
                else if (randNum <= 5)
                {
                    powerUps.push_back(PowerUp(balls[index].getPosition().x, balls[index].getPosition().y, 10, Color::Magenta, 3));
                }
                // "7"% chance to spawn ball power up that gets destroied over time (max 30 secconds)
                else if (randNum <= 7)
                {
                    powerUps.push_back(PowerUp(balls[index].getPosition().x, balls[index].getPosition().y, 10, Color::Blue, 4));
                }

                if (balls[index].subHit(world))
                {
                    balls.erase(balls.begin() + index);
                }
            }
        }
        if (attacked)
        {
            attackTimer = 0;
        }

        for (int i = powerUps.size() - 1; i >= 0; i--)
        {
            int temp = powerUps[i].Update(p1, window);
            if (temp >= 0)
            {
                powerUps.erase(powerUps.begin() + i);
            }
            else
            {
                continue;
            }

            int index = rand() % balls.size() - 1;
            // checking if the index was 0 and went negitive
            index = index < 0 ? 0 : index;
            Vector2f StartPosition = balls[index].getPosition();
            Vector2f StartVelocity = Vector2f(balls[index].body->GetLinearVelocity().x * pixels_per_meter, balls[index].body->GetLinearVelocity().y * pixels_per_meter);

            // spawn a lazer for 1 secconds
            if (temp == 0)
            {
                showLazer = true;
                lazerTimer = lazerTimer <= 0 ? 1 : lazerTimer+1;
            }
            // spawn a ball that does not get destroied by ANYTHING but after a time of 60 to 120 seconds it dies
            else if (temp == 1)
            {
                balls.push_back(Ball(world, StartPosition.x, StartPosition.y, 10, false, rand() % 60 + 60, -1, &font));
                balls.back().setFillColor(Color::Green);
                balls.back().text.setFillColor(Color::Black);
                balls.back().SetVelocity(StartVelocity);
            }
            // spawn a normal ball
            else if (temp == 2)
            {
                balls.push_back(Ball(world, StartPosition.x, StartPosition.y, 10));
                balls.back().SetVelocity(StartVelocity);
            }
            // spawn a ball that gets destroied over number of hits (limit at 25 hits)
            else if (temp == 3)
            {
                balls.push_back(Ball(world, StartPosition.x, StartPosition.y, 10, true, -1, rand() % 24 + 1, &font));
                balls.back().setFillColor(Color::Magenta);
                balls.back().SetVelocity(StartVelocity);
            }
            // spawn ball power up that gets destroied over time (max 30 secconds)
            else if (temp == 4)
            {
                balls.push_back(Ball(world, StartPosition.x, StartPosition.y, 10, true, rand() % 29 + 1, -1, &font));
                balls.back().setFillColor(Color::Blue);
                balls.back().SetVelocity(StartVelocity);
            }
        }

        // updates balls and draws them
        for (int i = balls.size() - 1; i >= 0; i--)
        {
            if (balls[i].Update(p1, deltaTime, world, window))
            {
                balls.erase(balls.begin() + i);
                continue;
            }
            window.draw(balls[i]);
            window.draw(balls[i].text);
        }

        p1.UpdatePosition(window);

        score.setString("Score: " + to_string(scoreInt));
        score.setPosition(window.getSize().x - score.getGlobalBounds().width, window.getSize().y - score.getGlobalBounds().height * 2);
        window.draw(score);

        if (showLazer)
        {
            window.draw(Lazer);
        }

        window.display(); // displays everything on the video card to the monitor
    }                     // ends the game loop

    return 0;
}

void BuildLevel(string levelSetup, int &breakableBricksLeft, vector<Brick> &bricks, vector<PowerUp> &powerUps, b2World &world, Font *font)
{
    breakableBricksLeft = 0;
    for (int i = bricks.size() - 1; i >= 0; i--)
    {
        physics::deleteBlock(world, bricks[i].body);
    }
    bricks.clear();

    powerUps.clear();

    if (levelSetup == "")
    {
        levelSetup = MakeRandomLevel();
    }

    // y for loop
    for (int i = 0; i < levelSetup.length() / 26; i++)
    {
        // x for loop
        for (int n = 0; n < 26; n++)
        {
            breakableBricksLeft++;

            // creating normal brick
            if (levelSetup[(i * 26) + n] == '#')
            {
                bricks.push_back(Brick(world, 25 + n * 30, 15 + i * 20, 25, 15));
            }
            else
                // creating brick with extra amount of bricks
                if (isdigit(levelSetup[(i * 26) + n]))
                {
                    bricks.push_back(Brick(world, 25 + n * 30, 15 + i * 20, 25, 15, b2_kinematicBody, levelSetup[(i * 26) + n] - 48, font));
                }
                else
                    // creating unbreakable brick
                    if (toupper(levelSetup[(i * 26) + n]) == 'U')
                    {
                        bricks.push_back(Brick(world, 25 + n * 30, 15 + i * 20, 25, 15, b2_kinematicBody));
                        bricks.back().canBreak = false;
                        bricks.back().setFillColor(Color::Transparent);
                        bricks.back().setOutlineColor(Color::White);
                        bricks.back().setOutlineThickness(2);

                        breakableBricksLeft--;
                    }
                    else
                    // for any open spaces
                    {
                        breakableBricksLeft--;
                    }
        }
    }
}

string MakeRandomLevel()
{
    string output;

    for (int i = 0; i < 390; i++)
    {
        output += (rand() % 11 + 48);
    }

    // creating random "open" space
    int t_x = rand() % 25;
    // creating random height for this "open" space
    int t_y = rand() % 12 + 2;
    // rand number for if this "open" space will have a boarder
    bool boarder = rand() % 5;

    for (int i = 0; i < t_y; i++)
    {
        if (boarder)
        {
            if (t_x != 25)
                output[output.size() - i * 26 - t_x - 2] = 'U';
            if (t_x != 0)
                output[output.size() - i * 26 - t_x] = 'U';
        }
        output[output.size() - i * 26 - t_x - 1] = 'E';
    }

    // giving a chance to make a unbreakable "floor" at the bottom of the level
    bool floor = rand() % 2;

    if (floor)
    {
        string temp = "";
        for (int i = 0; i < 26; i++)
        {
            temp += output[output.size() - 26 + i];
        }

        int floor_dir = rand() % 3;
        switch (floor_dir)
        {
        case 0:
        {
            for (int i = 0; i < temp.size(); i++)
            {
                if (temp[i] == 'E')
                    break;
                else
                    temp[i] = 'U';
            }
            break;
        }

        case 1:
        {
            for (int i = 0; i < temp.size(); i++)
            {
                if (temp[i] == 'E')
                    continue;
                else
                    temp[i] = 'U';
            }
            break;
        }

        case 2:
        {
            bool place = false;
            for (int i = 0; i < temp.size(); i++)
            {
                if (place)
                    temp[i] = 'U';
                else if (temp[i] == 'E')
                    place = true;
            }
            break;
        }

        default:
            cout << "ERROR making floor to blocks" << endl;
            break;
        }

        for (int i = 0; i < 26; i++)
        {
            output[output.size() - 26 + i] = temp[i];
        }
    }

    return output;
}
