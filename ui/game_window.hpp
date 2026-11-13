#pragma once

#include "engine/engine.hpp"
#include "engine/MCTSRandomAI.hpp"

#include <cmath>

#include <SFML/Graphics.hpp>

class GameWindow {
public:
    void run();

private:
    static constexpr unsigned int windowWidth = 1200;
    static constexpr unsigned int windowHeight = 800;
    static constexpr int boardSize = BOARDSIZE;
    static constexpr float boardMargin = 40.f;

    int stoneHoverX = 0;
    int stoneHoverY = 0;

    float boardSpacing() const;
    bool pixelToBoardPosition(
        sf::Vector2i pixel,
        int& boardX,
        int& boardY
    ) const;

    void processEvents(sf::RenderWindow& window);
    void drawStone(sf::RenderWindow& window, int x, int y, sf::Color color);
    void drawBoard(sf::RenderWindow& window);
};

class MenuButton {
public:
    void run();

    sf::Vector2f position;
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size;
    std::string innerText = "";

    MenuButton(sf::Vector2f sz, std::string str) {
        size = sz;
        innerText = str;
    }
     
    void setPosition(sf::Vector2f &pos);
    void setFillColor(sf::Color &clr);
    void drawButton(sf::RenderWindow& window);
private:
};


