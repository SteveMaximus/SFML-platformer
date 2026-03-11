#include "settings.h"

#pragma once
// --- ÊËÀÑÑ ÏËÀÒÔÎÐÌÛ ---
class Platform {
public:
    sf::RectangleShape shape;
    sf::Vector2f vel;
    sf::Vector2f startPos;
    float range;
    bool isVertical;

    Platform(float x, float y, float r_tiles, float s, bool vert) {
        shape.setSize({ 120.f, 20.f });
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color(100, 255, 100));
        startPos = { x, y };
        range = r_tiles * TILE_SIZE;
        isVertical = vert;
        vel = isVertical ? sf::Vector2f(0, s) : sf::Vector2f(s, 0);
    }

    void update() {
        shape.move(vel);
        float dist = isVertical ? (shape.getPosition().y - startPos.y) : (shape.getPosition().x - startPos.x);
        if (std::abs(dist) > range) vel = -vel;
    }
};