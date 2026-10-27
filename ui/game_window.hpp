#pragma once

#include <SFML/Graphics.hpp>

class GameWindow {
public:
    void run();

private:
    static constexpr unsigned int windowWidth = 800;
    static constexpr unsigned int windowHeight = 800;
    static constexpr int boardSize = 19;
    static constexpr float boardMargin = 40.f;

    bool currentSide = false; // false for black, true for white

    float boardSpacing() const;
    bool pixelToBoardPosition(
        sf::Vector2i pixel,
        int& boardX,
        int& boardY
    ) const;

    void processEvents(sf::RenderWindow& window);
    void drawBoard(sf::RenderWindow& window);
};
