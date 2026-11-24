#pragma once

#include "controller/systemutil.hpp"

#include "engine/engine.hpp"
#include "engine/MCTSRandomAI.hpp"
#include "engine/handler.hpp"

#include "ui/assets.hpp"

#include <cmath>
#include <queue>
#include <string>
#include <utility>
#include <filesystem>

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

class NotificationQueue {
public:
    float fontSize = 24;
    sf::Color fillColor = sf::Color(0, 0, 255);
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size = {500, 100};
    sf::Vector2f position = {30, 30};
    float margin = 10;
    int timeout = 1000;

    std::deque<std::pair<std::string, std::chrono::steady_clock::time_point>> queue;

    void requestNotification(std::string notification);

    void drawNotifications(sf::RenderWindow& window);
private:
};

class Button {
public:
    float fontSize = 24;
    sf::Vector2f position;
    sf::Color fillColor = sf::Color(128, 128, 128);
    sf::Color hoverFillColor = sf::Color(108, 108, 108);
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size;
    std::string innerText = "";

    sf::FloatRect boundingBox;

    bool hovered = false;

    Button(sf::Vector2f sz, std::string str) {
        size = sz;
        innerText = str;
    }

    void (*onClick)() = nullptr;
     
    void setPosition(sf::Vector2f pos);
    void setFillColor(sf::Color clr);
    void setHoverFillColor(sf::Color clr);
    void setColor(sf::Color clr);
    void setFontSize(float sz);
    void drawButton(sf::RenderWindow& window);

    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};

class Input {
public:
    float fontSize = 24;
    sf::Vector2f position;
    sf::Color fillColor = sf::Color(128, 128, 128);
    sf::Color hoverFillColor = sf::Color(108, 108, 108);
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size;
    std::string innerText = "";

    int blinkCycle = 10;
    int blickCycleState = 0;

    sf::FloatRect boundingBox;

    bool hovered = false;

    Input(sf::Vector2f sz, std::string str) {
        size = sz;
        innerText = str;
    }

    void (*onClick)() = nullptr;
     
    void setPosition(sf::Vector2f pos);
    void setFillColor(sf::Color clr);
    void setHoverFillColor(sf::Color clr);
    void setColor(sf::Color clr);
    void setFontSize(float sz);
    void drawInput(sf::RenderWindow& window);

    bool characterInput(char32_t unicode);

    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};



class DropdownSelector {
public:
    float fontSize = 24;
    sf::Vector2f position;
    sf::Color fillColor = sf::Color(128, 128, 128);
    sf::Color hoverFillColor = sf::Color(108, 108, 108);
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size;
    std::string innerText = "";

    sf::FloatRect boundingBox;

    bool droped = false;
    int selected = -1;

    Button activeSelection;
    std::vector<Button *> selectionList;

    DropdownSelector(sf::Vector2f sz, std::string str) :
    activeSelection(sz, str) {
        size = sz;
        innerText = str;

        activeSelection.fontSize = fontSize;
        activeSelection.position = position;
        activeSelection.fillColor = fillColor;
        activeSelection.hoverFillColor = hoverFillColor;
        activeSelection.color = color;
    }

    void (*onSelect)(int index) = nullptr;

    void addSelection(std::string str);
    void addSelectionList(std::vector<std::string> vtrstr);
     
    void setPosition(sf::Vector2f pos);
    void setFillColor(sf::Color clr);
    void setHoverFillColor(sf::Color clr);
    void setColor(sf::Color clr);
    void setFontSize(float sz);
    void drawDropdownSelector(sf::RenderWindow& window);

    void switchSelection(int index) {
        selected = index;
        activeSelection.innerText = selectionList[index]->innerText;
        if (onSelect != nullptr) onSelect(index);
    }

    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
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

    Button undoButton, redoButton;
    Button passButton;

    DropdownSelector gamemodeSelector;

    Button saveButton, loadButton;

    TurnIndicator indicator;

    GameMenu(sf::Vector2f sz, std::string str) :
    undoButton({(sz.x - 3 * menuMargin) / 2, buttonHeight}, "UNDO"), 
    redoButton({(sz.x - 3 * menuMargin) / 2, buttonHeight}, "REDO"),
    passButton({sz.x - 2 * menuMargin, buttonHeight}, "PASS"),
    gamemodeSelector({380 - 40 * 2, 40}, "Select gamemode"),
    saveButton({(sz.x - 3 * menuMargin) / 2, buttonHeight}, "SAVE"), 
    loadButton({(sz.x - 3 * menuMargin) / 2, buttonHeight}, "LOAD"),
    indicator({sz.x - 2 * menuMargin, 70.f}, "BLACK TURN") {
        size = sz;
        innerText = str;

        undoButton.setPosition({position.x + menuMargin, position.y + menuMargin});
        redoButton.setPosition({position.x + (size.x + menuMargin) / 2, position.y + menuMargin});
        passButton.setPosition({position.x + menuMargin, position.y + buttonHeight + 2 * menuMargin});

        gamemodeSelector.addSelection("gamemode 0");
        gamemodeSelector.addSelection("gamemode 1");
        gamemodeSelector.addSelection("gamemode 2");
        gamemodeSelector.onSelect = &switchGamemodeEvent;

        saveButton.setPosition({position.x + menuMargin, 300});
        loadButton.setPosition({position.x + (size.x + menuMargin) / 2, 300});

        indicator.setPosition({position.x + menuMargin, position.y + size.y - 70.f - menuMargin});
        
        undoButton.onClick = &undoEvent;
        redoButton.onClick = &redoEvent;
        passButton.onClick = &turnPassEvent;
    }
     
    void setPosition(sf::Vector2f pos);
    void setFillColor(sf::Color clr);
    void drawMenu(sf::RenderWindow& window);

    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};


// windows part

class SavePopupWindow {
public:
    std::string titleString = "Save";
    float titlebarHeight = 40;
    float borderWidth = 5;
    sf::Vector2f position = {300, 250};
    sf::Color color = sf::Color(0, 0, 0);
    sf::Color inverseColor = sf::Color(255, 255, 255);
    sf::Vector2f size = {500, 222};

    Button closeButton;

    sf::FloatRect boundingBox;

    Input textInput; Button saveButton;

    bool visible = true;

    SavePopupWindow(): closeButton({titlebarHeight , titlebarHeight}, "X"), textInput({300, 30}, "hocphi"), saveButton({80, 30}, "save") {
        closeButton.setFillColor(sf::Color(255, 0, 0));
        closeButton.setHoverFillColor(sf::Color(180, 0, 0));
        closeButton.setColor(sf::Color(255, 255, 255));
        closeButton.setPosition({float(position.x + size.x - titlebarHeight - borderWidth), float(position.y)});
        closeButton.setFontSize(closeButton.size.x - borderWidth * 2);

        textInput.setFillColor(sf::Color(0, 0, 255));
        textInput.setHoverFillColor(sf::Color(0, 0, 200));
        textInput.setColor(sf::Color(255, 255, 255));
        textInput.setPosition({float(position.x + 50), float(position.y + 100)});
        textInput.setFontSize(20);

        saveButton.setHoverFillColor(sf::Color(0, 0, 200));
        saveButton.setFillColor(sf::Color(0, 0, 255));
        saveButton.setColor(sf::Color(255, 255, 255));
        saveButton.setPosition({float(position.x + 50 + 300 + 20), float(position.y + 100)});
        saveButton.setFontSize(20);
    }
    
    void drawWindow(sf::RenderWindow& window);

    bool turnVisible();

    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};

class PopupWindowLayer {
public:
    SavePopupWindow savepopup;
    
    void drawPopupWindows(sf::RenderWindow& window);

    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};

extern Input *activeInput;