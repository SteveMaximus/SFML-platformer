#include "settings.h"


#pragma once
struct Particle {
    sf::CircleShape shape;
    sf::Vector2f vel;
    float lifetime;
};

void spawnParticles(std::vector<Particle>& particles, sf::Vector2f pos, sf::Color color) {
    for (int i = 0; i < 10; ++i) {
        Particle p;
        p.shape.setRadius(3.f);
        p.shape.setFillColor(color);
        p.shape.setPosition(pos);
        p.vel = { (float)(rand() % 10 - 5), (float)(rand() % 10 - 5) };
        p.lifetime = 1.0f;
        particles.push_back(p);
    }
}

// --- ÂÐÀÃÈ È ÏÐÅÏßÒÑÒÂÈß ---



