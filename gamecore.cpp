

#include "settings.h"
#include "Enemy.h"
#include "Particle.h"
#include "Platform.h"

#define WIDTH 1200
#define HIGHT 800

// --- СИСТЕМА ЧАСТИЦ ---





int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HIGHT), "SFML Pro Platformer");


    
    window.setFramerateLimit(60);

    State gameState = MENU;
    sf::Font font;
    font.loadFromFile("resourses/tuffy.ttf");
    sf::Text uiText; uiText.setFont(font); uiText.setCharacterSize(24); uiText.setFillColor(sf::Color::White);

    std::vector<sf::RectangleShape> walls;
    std::vector<sf::CircleShape> coins;
    std::vector<Enemy> enemies;
    std::vector<Spike> spikes;
    std::vector<Particle> particles;
    std::vector<Platform> platforms;
    sf::RectangleShape portal(sf::Vector2f(60, 60)); portal.setFillColor(sf::Color::Magenta);

    sf::CircleShape player(18.f); player.setFillColor(sf::Color::Cyan);
    sf::Vector2f startPos(120, 120), pos = startPos, vel(0, 0);
    // --- НОВОЕ ДЛЯ РЫВКА ---
    float dashTimer = 0;       // Длительность самого рывка
    float dashCooldown = 0;    // Перезарядка
    float dashDir = 1.0f;      // Направление последнего движения (чтобы знать куда рывком бить)

    int score = 0;
    float shakeTimer = 0;

    int mode=0;
    auto load = [&](int lvl) {
        walls.clear(); coins.clear(); enemies.clear(); spikes.clear();
        for (int i = 0; i < levels[lvl].size(); ++i)
            for (int j = 0; j < levels[lvl][i]. size(); ++j) {
                float px = j * 60.f; float py = i * 60.f;
                if (levels[lvl][i][j] == '1') { sf::RectangleShape r({ 60,60 }); r.setPosition(px, py); r.setFillColor(sf::Color(50, 50, 60)); walls.push_back(r); }
                if (levels[lvl][i][j] == '2') { spikes.push_back(Spike(px, py)); }
                if (levels[lvl][i][j] == '3') { portal.setPosition(px, py); }
                if (levels[lvl][i][j] == '4') { enemies.push_back(Enemy(px, py, 3.0f)); }
                if (levels[lvl][i][j] == '5') { sf::CircleShape c(10); c.setPosition(px + 20, py + 20); c.setFillColor(sf::Color::Yellow); coins.push_back(c); }
                else if (levels[lvl][i][j] == '6') {
                    platforms.push_back(Platform(px, py, 4, 3.0f, false));
                }
                else if (levels[lvl][i][j] == '7') {
                    platforms.push_back(Platform(px, py, 5, 2.0f, true));
                }
            }
        };

    load(mode);
    sf::View view(sf::FloatRect(0, 0, WIDTH, HIGHT));

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) if (e.type == sf::Event::Closed) window.close();

        if (gameState == MENU) {
            window.clear(sf::Color(20, 20, 25));
            uiText.setString("PRO PLATFORMER\nPRESS ENTER TO START");
            uiText.setPosition(WIDTH / 2 - 150, HIGHT / 2 - 50);
            window.draw(uiText);
            window.display();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) gameState = PLAYING;
            continue;
        }

        // --- ОБНОВЛЕНИЕ ТАЙМЕРОВ ---
        if (dashTimer > 0) dashTimer -= 0.016f;
        if (dashCooldown > 0) dashCooldown -= 0.016f;

        // Определение направления взгляда
        if (vel.x > 0) dashDir = 1.0f;
        if (vel.x < 0) dashDir = -1.0f;

        // --- ЛОГИКА РЫВКА ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && dashCooldown <= 0) {
            dashTimer = 0.2f;      // Рывок длится 0.2 секунды
            dashCooldown = 1.0f;   // Перезарядка 1 секунда
            vel.y = 0;             // Во время рывка не падаем
        }

        // --- ДВИЖЕНИЕ ---
        vel.x = 0;
        if (dashTimer > 0) {
            vel.x = dashDir * 20;  // Скорость рывка (гораздо выше обычной)
            vel.y = 0;             // Отключаем гравитацию во время рывка
            spawnParticles(particles, pos, sf::Color::Cyan); // Эффект следа
        }
        else {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) vel.x = 9;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) vel.x = -9;
        }

        // Гравитацию применяем только если НЕ делаем рывок
        if (dashTimer <= 0) {
            vel.y += 0.8f;
        }


        bool onGround = false;
        bool touchingWall = false;
        int wallDir = 0; // -1 слева, 1 справа
        // Проверка "на земле ли я" перед прыжком
        player.setPosition(pos.x, pos.y + 2);
        for (auto& w : walls) if (player.getGlobalBounds().intersects(w.getGlobalBounds())) onGround = true;

       

        // 1. Движение платформ и "липкость"
        for (auto& plat : platforms) {
            sf::Vector2f oldPlatPos = plat.shape.getPosition();
            plat.update();
            sf::Vector2f delta = plat.shape.getPosition() - oldPlatPos;

            sf::FloatRect footStep = player.getGlobalBounds();
            footStep.top += 2; // Проверка земли чуть ниже игрока

            if (footStep.intersects(plat.shape.getGlobalBounds()) && vel.y >= 0) {
                pos += delta; // Двигаем игрока вместе с платформой
                pos.y = plat.shape.getPosition().y - player.getRadius() * 2;
                vel.y = 0;
                onGround = true;
            }
        }
        
        

        // Движение по X
        pos.x += vel.x; player.setPosition(pos);
       
        for (auto& w : walls) {

            if (player.getGlobalBounds().intersects(w.getGlobalBounds())) {
                if (vel.x > 0) pos.x = w.getPosition().x - player.getRadius() * 2;
                if (vel.x < 0) pos.x = w.getPosition().x + 60;
            }
        }
        // Движение по Y
        pos.y += vel.y; player.setPosition(pos);
        for (auto& w : walls) if (player.getGlobalBounds().intersects(w.getGlobalBounds())) {
            if (vel.y > 0) { pos.y = w.getPosition().y - player.getRadius() * 2; vel.y = 0; }
            if (vel.y < 0) { pos.y = w.getPosition().y + 60; vel.y = 0; }
        }


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround) {          
             vel.y = -20;
        }
        vel.y += 0.8f; // Гравитация

        // --- ОБРАБОТКА ОБЪЕКТОВ ---
        // Смерть от шипов
        for (auto& s : spikes) if (player.getGlobalBounds().intersects(s.shape.getGlobalBounds())) {
            pos = startPos; vel = { 0,0 };
        }

        // Сбор монет
        for (auto it = coins.begin(); it != coins.end();) {
            if (player.getGlobalBounds().intersects(it->getGlobalBounds())) {
                spawnParticles(particles, it->getPosition(), sf::Color::Yellow);
                it = coins.erase(it); score++;
            }
            else ++it;
        }

        // Враги
        for (auto it = enemies.begin(); it != enemies.end();) {
            it->update(walls);
            if (player.getGlobalBounds().intersects(it->weak.getGlobalBounds()) && vel.y > 0) {
                spawnParticles(particles, it->shape.getPosition(), sf::Color::Red);
                it = enemies.erase(it); vel.y = -12;
            }
            else if (player.getGlobalBounds().intersects(it->shape.getGlobalBounds())) {
                shakeTimer = 0.3f; pos = startPos; vel = { 0,0 }; ++it;
            }
            else ++it;
        }

        if (pos.y > levels[0].size() * 60) {
            shakeTimer = 0.5f; // Тряска от падения
            pos = startPos;
            vel = { 0, 0 };
            spawnParticles(particles, pos, sf::Color::Red); // Частицы при смерти
        }

        // 2. Логика портала (Переход на следующий уровень)
        if (player.getGlobalBounds().intersects(portal.getGlobalBounds())) {
            mode++;
            if (mode < levels.size()) {
                load(mode);
                pos = startPos; // Возвращаем игрока в начало нового уровня
            }
            else {
                // Если уровни кончились, можно просто перезапустить или выйти в меню
                mode = 0;
                load(mode);
                gameState = MENU;
            }
        }
        
        // Частицы
        for (auto it = particles.begin(); it != particles.end();) {
            it->shape.move(it->vel);
            it->lifetime -= 0.02f;
            if (it->lifetime <= 0) it = particles.erase(it); else ++it;
        }

        // Камера
        view.setCenter(pos.x, pos.y);
        if (shakeTimer > 0) {
            view.move(rand() % 10 - 5, rand() % 10 - 5);
            shakeTimer -= 0.016f;
        }

        window.setView(view);
        window.clear(sf::Color(25, 25, 30));
        for (auto& w : walls) window.draw(w);
        for (auto& s : spikes) window.draw(s.shape);
        for (auto& c : coins) window.draw(c);
        for (auto& e : enemies) { window.draw(e.shape); window.draw(e.weak); }
        for (auto& p : particles) window.draw(p.shape);
        for (auto& p : platforms) window.draw(p.shape);
        window.draw(portal);
        window.draw(player);

        // Отрисовка UI относительно камеры
        uiText.setString("Score: " + std::to_string(score));
        uiText.setPosition(pos.x - 580, pos.y - 380);
        window.draw(uiText);

        window.display();
    }
    return 0;
}





/*
#include <SFML/Graphics.hpp>
#include <vector>
#include <Windows.h>
#include <map>
#include <iostream>

class Picture {
public:
	std::string FileDirectory;
	std::vector<int[2]> pos;
	
	Picture(
		std::string FileDirectory,
		std::vector<int[2]> pos
	)
	{
		this->FileDirectory = FileDirectory;
    }
};



int main() {

	sf::RenderWindow window(sf::VideoMode(555, 555), "SFML Works");
	sf::Texture texture;
	if (!texture.loadFromFile("images/Bob.png")) {
		std::cout << 1;
	}

	sf::Sprite Player;
	Player.setTexture(texture);
	
	Player.setPosition(100, 100);

	sf::CircleShape sc;
	sc.setRadius(50);
	sc.setPosition(50, 50);
	sc.setFillColor(sf::Color(255, 255, 255));
	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
	
		window.clear();
		window.draw(sc);
		window.display();
	}
}
*/