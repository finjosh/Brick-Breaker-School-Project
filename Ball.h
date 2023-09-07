#ifndef BALL_H // header guard
#define BALL_H

#include "physics.h"
#include "Paddle.h"
#include <math.h>

struct Ball: public sf::CircleShape // takes all circleshapes properties
{
    // constructor
    // if the number of hits = 0 then it will automaticly have 1 hit till it is destroied
    Ball(b2World &world, float x, float y, float r, bool canDie = true, float DestroyTimer = -1, int numberOfHits = -1, sf::Font* font = nullptr, b2BodyType type = b2_dynamicBody) {
        b2BodyDef bodyDef;
        bodyDef.position.Set(x/pixels_per_meter, y/pixels_per_meter);
        bodyDef.type = type;
        bodyDef.linearDamping = 0;
        b2CircleShape b2shape;

        b2shape.m_radius = r/pixels_per_meter;

        b2FixtureDef fixtureDef;
        fixtureDef.density = 1.0;
        fixtureDef.friction = 0.4;
        fixtureDef.restitution = 1; // how bouncy it is
        fixtureDef.shape = &b2shape;

        body = world.CreateBody(&bodyDef);
        body->CreateFixture(&fixtureDef);

        setRadius(r);
        setOrigin(r,r);
        setPosition(x, y);
        setFillColor(sf::Color::White);

        Ball::destroyTimer = DestroyTimer;
        Ball::hits = --numberOfHits;
        if (numberOfHits >= 0)
        {
            text = sf::Text(to_string(hits+1), *font);
            text.setPosition(sf::Vector2f(x, y));
            UpdateText();
        } else 
        if (destroyTimer > 0)
        {
            text = sf::Text(to_string((int)destroyTimer) + "s", *font);
            text.setPosition(sf::Vector2f(x, y));
            UpdateText();
        }
        Ball::canDie = canDie;
    }

    // returns true if the ball is destroied
    bool Update(Paddle Paddle, float deltaTime, b2World &world, sf::RenderWindow &window)
    {
        sf::Vector2f position = sf::Vector2f(body->GetPosition().x*pixels_per_meter, body->GetPosition().y*pixels_per_meter);
        
        // checking if the ball is to low and should be deleted
        if (canDie)
        {
            if (position.y >= window.getSize().y - 10 - this->getRadius())
            {
                physics::deleteBlock(world, this->body);
                return true;
            } else 
            if (position.x <= 0-getRadius() || position.x >= window.getSize().x + getRadius())
            {
                body->SetTransform(Paddle.body->GetPosition(), 0);
            }
        }

        // checking if the ball timer is being used then if the timer is up
        if (Ball::destroyTimer >= 0) {
            Ball::destroyTimer -= deltaTime;
            if (Ball::destroyTimer <= 0) {
                physics::deleteBlock(world, this->body);
                // returning true as the ball was destroied
                return true;
            }
            text.setString(to_string((int)destroyTimer) + "s");
            UpdateText();
        } 

        // checking for paddle collision
        for (b2ContactEdge* edge = body->GetContactList(); edge; edge = edge->next) {
            if (edge->other == Paddle.body) {
                if (edge->contact->IsTouching()) {
                    float angle = ((body->GetPosition().x - Paddle.body->GetPosition().x) / 2.6) * 65 - 90;
                    angle /= deg_per_rad;

                    body->SetLinearVelocity(b2Vec2(282.84*cos(angle), 282.84*sin(angle)));
                }
            }
        }

        b2Vec2 velocity = body->GetLinearVelocity();
        body->SetLinearVelocity((282.84/pixels_per_meter)/velocity.Length()*velocity);
        velocity = body->GetLinearVelocity();

        // checking that the ball is not stuck going back and forth with a constant y value (stuck)
        if ((int)velocity.y == 0 && physics::checkCollision(body)) {
            body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, 0.5f));
        } else
        if ((int)velocity.x == 0 && physics::checkCollision(body))
        {
            body->SetLinearVelocity(b2Vec2(0.5f,body->GetLinearVelocity().y));
        }

        // setting the postition of the text to the position of the ball
        text.setPosition(position);
        // setting it to the text position as it is already set to the position of the ball
        setPosition(text.getPosition());

        // in the cse that the ball was not destroied
        return false;
    }

    void SetVelocity(sf::Vector2f v)
    {
        body->SetLinearVelocity(b2Vec2(v.x/pixels_per_meter,v.y/pixels_per_meter));
    }

    // returns true if the ball was destroied else false
    bool subHit(b2World &world)
    {
        // checking if this ball has limited hits
        if (hits >= 0) 
        {
            if (--hits < 0) 
            {
                physics::deleteBlock(world, this->body);
                return true;
            } else 
            {
                text.setString(to_string(hits+1));
                UpdateText();
            }
        }
        return false;
    }

    b2Body* body;
    float destroyTimer = -1;
    int hits = -1;
    sf::Text text;
    bool canDie = false;

    private:

        void UpdateText()
        {
            // Set the new text height
            text.setCharacterSize(this->getRadius()*2);

            // The text still has too fit inside the area
            float width = text.getLocalBounds().width;
            if (width > this->getRadius()*2)
            {
                // Adjust the text size
                text.setCharacterSize((this->getRadius()*2 * this->getRadius()*2) / width);
            }

            // reseting origin
            text.setOrigin(sf::Vector2f(round(text.getLocalBounds().left + text.getGlobalBounds().width / 2),
                                    round(text.getLocalBounds().top + text.getGlobalBounds().height / 2)));
        }

};

#endif // BALL_H
