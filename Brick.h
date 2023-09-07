#ifndef BRICK_H
#define BRICK_H

#include "physics.h"
#include "Ball.h"
#include <math.h>

struct Brick : public sf::RectangleShape
{

    Brick(b2World &world, float x, float y, float width, float height, b2BodyType type = b2_kinematicBody, int numberOfBricks = 1, sf::Font* font = nullptr) {
        b2BodyDef bodyDef;
        bodyDef.position.Set(x/pixels_per_meter, y/pixels_per_meter);
        bodyDef.type = type;
        bodyDef.linearDamping = 0.00;
        b2PolygonShape b2shape;
        b2shape.SetAsBox(width/pixels_per_meter/2.0, height/pixels_per_meter/2.0);
        b2FixtureDef fixtureDef;
        fixtureDef.density = 1.0;
        fixtureDef.friction = 0.4;
        fixtureDef.restitution = 1.0;
        fixtureDef.shape = &b2shape;

        body = world.CreateBody(&bodyDef);
        body->CreateFixture(&fixtureDef);

        setSize(sf::Vector2f(width, height));
        setOrigin(width/2.0, height/2.0);
        setPosition(x, y);
        setFillColor(sf::Color::White);

        if (numberOfBricks > 1) {
            Brick::amount = numberOfBricks;
            text = sf::Text(to_string(numberOfBricks), *font);
            text.setFillColor(sf::Color::Black);
            
            UpdateText();

            text.setPosition(x, y);
        }
    }

    // if ball is negative then the brick was not deleted if positive that is the index for the ball that hit it
    // also draws this obj
    int Update(sf::RenderWindow &window, b2World &world, vector<Ball>& balls)
    {
        window.draw(*this);
        window.draw(text);
        
        for (int i = 0; i < balls.size(); i++) 
        {
            if (physics::checkCollision(body, balls[i].body))
            {
                // if the brick cant break then push the ball up 
                if (!canBreak)
                {
                    if (balls[i].body->GetPosition().x <= body->GetPosition().x - getSize().x/pixels_per_meter || 
                        balls[i].body->GetPosition().x >= body->GetPosition().x + getSize().x/pixels_per_meter)
                        {
                            balls[i].SetVelocity(sf::Vector2f(balls[i].body->GetLinearVelocity().x*pixels_per_meter, -150.f));
                        }
                    return -10;
                }

                if (--amount <= 0) {
                    physics::deleteBlock(world, body);
                    body = nullptr;
                    return i;
                } else {
                    text.setString(to_string(amount));
                    UpdateText();
                    
                    return i;
                }
            }
        }
        
        return -10;
    }

    void UpdateText()
    {
        // Set the new text height
        text.setCharacterSize(this->getSize().y);

        // The text still has too fit inside the area
        float width = text.getLocalBounds().width;
        if (width > this->getSize().x)
        {
            // Adjust the text size
            text.setCharacterSize((this->getSize().y * this->getSize().x) / width);
        }

        // reseting origin
        text.setOrigin(sf::Vector2f(round(text.getLocalBounds().left + text.getGlobalBounds().width / 2),
                                round(text.getLocalBounds().top + text.getGlobalBounds().height / 2)));
    }

    b2Body* body;
    int amount = 1;
    sf::Text text;
    bool canBreak = true;

};

#endif // BRICK_H
