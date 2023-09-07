#ifndef POWERUP_H // header guard
#define POWERUP_H

#include "physics.h"
#include "Paddle.h"
#include <math.h>

struct PowerUp: public sf::CircleShape // takes all circleshapes properties
{
    // constructor
    // if the number of hits = 0 then it will automaticly have 1 hit till it is destroied
    PowerUp(float x, float y, float r, sf::Color color, int powerUpInt) {

        setRadius(r);
        setOrigin(r,r);
        setPosition(x, y);
        setFillColor(color);
        setOutlineColor(sf::Color(255 - color.r, 255 - color.g, 255 - color.b, 255));
        setOutlineThickness(2);

        this->powerUpInt = powerUpInt;
    }

    // returns the number of the power up if the paddle collected the power up
    // if the power up was not collected then it will return a negitive number
    // you should also delete this object if the returned number is a positive
    int Update(Paddle Paddle, sf::RenderWindow &window)
    {
        this->setPosition(sf::Vector2f(this->getPosition().x, this->getPosition().y + 2));

        if (this->getGlobalBounds().intersects(Paddle.getGlobalBounds()))
        {
            return powerUpInt;
        } else
        if (this->getPosition().y >= window.getSize().y)
        {
            return -10;
        }

        window.draw(*this);

        return -10;
    }

    int powerUpInt = -1;

};

#endif // POWERUP_H
