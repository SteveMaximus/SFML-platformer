#include "settings.h"

#pragma once
class Enemy {
public:
    sf::RectangleShape shape, weak;
    sf::Image image;
    float speed, dir = 1.0f;
#if DEBUG
    Enemy(float x, float y, float s) : speed(s) {
        shape.setSize({ 45, 45 }); shape.setPosition(x, y); shape.setFillColor(sf::Color(200, 50, 50));
        weak.setSize({ 45, 10 }); weak.setFillColor(sf::Color::Yellow);
    }
#else
    Enemy(float x, float y, float s, string FileName) : speed(s) {
        shape.setSize({ 45, 45 }); shape.setPosition(x, y); shape.setFillColor(sf::Color(200, 50, 50));
        weak.setSize({ 45, 10 }); weak.setFillColor(sf::Color::Yellow);
        this->image.
        this->image.loadFromFile(FileName);

    }
#endif
    void update(const std::vector<sf::RectangleShape>& walls) {
        shape.move(speed * dir, 0);
        for (auto& w : walls) {
            if (shape.getGlobalBounds().intersects(w.getGlobalBounds())) {
                dir *= -1;
                shape.move(speed * dir, 0); // Чтобы не застревал в стене
                break;
            }
        }
        weak.setPosition(shape.getPosition().x, shape.getPosition().y - 10);
    }
#if DEBUG
    void draw(sf::RenderWindow window) {
        window.draw(shape);
        window.draw(weak);
    }
#else
    void draw(sf::RenderWindow window) {

    }
#endif
};

class Spike {
public:
    sf::RectangleShape shape;// Сделаем шипы треугольными (3 точки)
    Spike(float x, float y) {
        shape.setSize({ 45, 45 }); shape.setPosition(x, y); shape.setFillColor(sf::Color(200, 50, 50));
    }
};