#pragma once

#include "engine/engine.hpp"
#include "engine/MCTSRandomAI.hpp"
#include "engine/handler.hpp"

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

    void MouseMoveEventHandler(sf::Event::MouseMoved *mouseButtonEvent);
    void MouseButtonPressedEventHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    void MouseButtonReleasedEventHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);

    void processEvents(sf::RenderWindow& window);
    void drawStone(sf::RenderWindow& window, int x, int y, sf::Color color, float radius = ((windowHeight - 2.f * boardMargin) / static_cast<float>(BOARDSIZE - 1)) * 0.45f);
    void drawBoard(sf::RenderWindow& window);
};

class MenuButton {
public:
    static constexpr float fontSize = 24;
    sf::Vector2f position;
    sf::Color fillColor = sf::Color(128, 128, 128);
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size;
    std::string innerText = "";

    sf::FloatRect boundingBox;

    MenuButton(sf::Vector2f sz, std::string str) {
        size = sz;
        innerText = str;
    }

    void (*onClick)();
     
    void setPosition(sf::Vector2f pos);
    void setFillColor(sf::Color clr);
    void setColor(sf::Color clr);
    void drawButton(sf::RenderWindow& window);

    void hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    void pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    void releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};

class TurnIndicator {
public:
    static constexpr float fontSize = 24;
    sf::Vector2f position;
    sf::Color fillColor = sf::Color(128, 128, 128);
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size;
    std::string innerText = "";

    sf::FloatRect boundingBox;

    TurnIndicator(sf::Vector2f sz, std::string str) {
        size = sz;
        innerText = str;
    }

    void (*onClick)();
     
    void setPosition(sf::Vector2f pos);
    void setFillColor(sf::Color clr);
    void setColor(sf::Color clr);
    void drawIndicator(sf::RenderWindow& window);
private:
};

class GameMenu {
public:
    static constexpr float menuMargin = 40.f;
    static constexpr float buttonHeight = 50.f;

    sf::Vector2f position;
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size;
    std::string innerText = "";

    MenuButton undoButton; MenuButton redoButton;
    MenuButton passButton;

    TurnIndicator indicator;

    GameMenu(sf::Vector2f sz, std::string str) :
    undoButton({(sz.x - 3 * menuMargin) / 2, buttonHeight}, "UNDO"), 
    redoButton({(sz.x - 3 * menuMargin) / 2, buttonHeight}, "REDO"),
    passButton({sz.x - 2 * menuMargin, buttonHeight}, "PASS"),
    indicator({sz.x - 2 * menuMargin, 70.f}, "BLACK TURN") {
        size = sz;
        innerText = str;

        undoButton.setPosition({position.x + menuMargin, position.y + menuMargin});
        redoButton.setPosition({position.x + (size.x + menuMargin) / 2, position.y + menuMargin});
        passButton.setPosition({position.x + menuMargin, position.y + buttonHeight + 2 * menuMargin});
        indicator.setPosition({position.x + menuMargin, position.y + size.y - 70.f - menuMargin});
        
        undoButton.onClick = &undoEvent;
        redoButton.onClick = &redoEvent;
        passButton.onClick = &turnPassEvent;
    }
     
    void setPosition(sf::Vector2f pos);
    void setFillColor(sf::Color clr);
    void drawMenu(sf::RenderWindow& window);

    void hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    void pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    void releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};