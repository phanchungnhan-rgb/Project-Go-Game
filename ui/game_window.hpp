#pragma once

#include "engine/engine.hpp"
#include "engine/AI.hpp"

#include <cmath>

#include <SFML/Graphics.hpp>

class GameWindow {
public:
    void run();

private:
    static constexpr unsigned int windowWidth = 800;
    static constexpr unsigned int windowHeight = 800;
    static constexpr int boardSize = BOARDSIZE;
    static constexpr float boardMargin = 40.f;

    float boardSpacing() const;
    bool pixelToBoardPosition(
        sf::Vector2i pixel,
        int& boardX,
        int& boardY
    ) const;

    void processEvents(sf::RenderWindow& window);
    void drawBoard(sf::RenderWindow& window);
    void drawBoardAIDebug(sf::RenderWindow& window);
};
