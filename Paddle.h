#ifndef PADDLE_H
#define PADDLE_H

#include "physics.h"

struct Paddle : public sf::RectangleShape
{

    Paddle(b2World &world, float x, float y, float width, float height, b2BodyType type = b2_kinematicBody) {
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
    }

    void UpdatePosition(sf::RenderWindow &render)
    {
        b2Vec2 currentPos = body->GetPosition();
        currentPos = b2Vec2(currentPos.x * pixels_per_meter, currentPos.y * pixels_per_meter);
        if (currentPos.x < -(this->getLocalBounds().width/4)) {
            body->SetTransform(b2Vec2(-this->getLocalBounds().width/4 / pixels_per_meter, body->GetTransform().p.y), 0);
        } else
        if (currentPos.x > render.getSize().x + (this->getLocalBounds().width/4)) {
            body->SetTransform(b2Vec2((render.getSize().x + this->getLocalBounds().width/4) / pixels_per_meter, body->GetTransform().p.y), 0);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            body->SetLinearVelocity(b2Vec2(-10,0));
        } else
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            body->SetLinearVelocity(b2Vec2(10,0));
        }
        else
        {
            body->SetLinearVelocity(b2Vec2(0,0));
        }

        setPosition(sf::Vector2f(body->GetPosition().x*pixels_per_meter, body->GetPosition().y*pixels_per_meter));

        render.draw(*this);
    }

    b2Body* body;

};

#endif // PADDLE
