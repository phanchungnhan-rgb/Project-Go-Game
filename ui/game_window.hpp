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
#include <SFML/Window/ContextSettings.hpp>

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
    void MouseScrollEventHandler(sf::Event::MouseWheelScrolled *mouseButtonEvent);
    void MouseButtonPressedEventHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    void MouseButtonReleasedEventHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);

    void processEvents(sf::RenderWindow& window);
    void drawStone(sf::RenderWindow& window, int x, int y, bool side, bool isGhost, float radius = ((windowHeight - 2.f * boardMargin) / static_cast<float>(BOARDSIZE - 1)) * 0.45f);
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


class Slider {
public:
    float fontSize = 24;
    sf::Vector2f position;
    sf::Color fillColor = sf::Color(128, 128, 128);
    sf::Color backgroundFillColor = sf::Color(108, 108, 108);
    sf::Color color = sf::Color(255, 255, 255);
    sf::Vector2f size;

    sf::FloatRect boundingBox;

    bool hold = false;

    float value = 0.5;

    Slider(sf::Vector2f sz) {
        size = sz;
    }

    bool (*onChange)(float value) = nullptr;
     
    void setPosition(sf::Vector2f pos);
    void setFillColor(sf::Color clr);
    void setBackgroundFillColor(sf::Color clr);
    void setColor(sf::Color clr);
    void setFontSize(float sz);
    void drawSlider(sf::RenderWindow& window);

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

    bool (*onSelect)(int index) = nullptr;

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
    gamemodeSelector({380 - 40 * 2, 40}, "Player vs Player"),
    saveButton({(sz.x - 3 * menuMargin) / 2, buttonHeight}, "SAVE"), 
    loadButton({(sz.x - 3 * menuMargin) / 2, buttonHeight}, "LOAD"),
    indicator({sz.x - 2 * menuMargin, 70.f}, "BLACK TURN") {
        size = sz;
        innerText = str;

        undoButton.setPosition({position.x + menuMargin, position.y + menuMargin});
        redoButton.setPosition({position.x + (size.x + menuMargin) / 2, position.y + menuMargin});
        passButton.setPosition({position.x + menuMargin, position.y + buttonHeight + 2 * menuMargin});

        gamemodeSelector.addSelection("Player vs Player");
        gamemodeSelector.addSelection("MCTS RandomSim");
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

    bool visible = false;

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

    bool scrollHandler(sf::Event::MouseWheelScrolled *mouseButtonEvent);
    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};

class LoadPopupWindow {
public:
    std::string titleString = "Load";
    float titlebarHeight = 40;
    float borderWidth = 5;
    float margin = 40;
    float optionHeight = 40;
    sf::Vector2f position = {300, 250}; 
    sf::Vector2f scrollOffset = {0, 0}; 
    float scrollSpeed = 6.7;
    sf::Color color = sf::Color(0, 0, 0);
    sf::Color inverseColor = sf::Color(255, 255, 255);
    sf::Vector2f size = {500, 222};

    sf::Vector2i cursorPosition = {0, 0};

    Button closeButton;

    sf::FloatRect boundingBox;

    sf::RectangleShape namelistMask;

    std::vector<std::string> namelist;

    bool visible = false;

    LoadPopupWindow(): closeButton({titlebarHeight , titlebarHeight}, "X"), namelistMask({size.x - margin * 2, size.y - margin * 2 - titlebarHeight}) {
        closeButton.setFillColor(sf::Color(255, 0, 0));
        closeButton.setHoverFillColor(sf::Color(180, 0, 0));
        closeButton.setColor(sf::Color(255, 255, 255));
        closeButton.setPosition({float(position.x + size.x - titlebarHeight - borderWidth), float(position.y)});
        closeButton.setFontSize(closeButton.size.x - borderWidth * 2);

        namelistMask.setPosition({position.x + margin, position.y + titlebarHeight + margin});
    }
    
    void drawWindow(sf::RenderWindow& window);

    bool turnVisible();

    bool scrollHandler(sf::Event::MouseWheelScrolled *mouseButtonEvent);
    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};

class ConfigurationPopupWindow {
public:
    std::string titleString = "Load";
    float titlebarHeight = 40;
    float borderWidth = 5;
    float margin = 40;
    float optionHeight = 40;
    sf::Vector2f position = {300, 250}; 
    sf::Vector2f scrollOffset = {0, 0}; 
    float scrollSpeed = 6.7;
    sf::Color color = sf::Color(0, 0, 0);
    sf::Color inverseColor = sf::Color(255, 255, 255);
    sf::Vector2f size = {700, 444};

    sf::Vector2i cursorPosition = {0, 0};

    Button closeButton;

    sf::FloatRect boundingBox;

    DropdownSelector stoneTextureSelector;
    DropdownSelector backgroundTextureSelector;

    Slider musicSlider;
    Slider SFXSlider;

    bool visible = true;

    ConfigurationPopupWindow(): closeButton({titlebarHeight , titlebarHeight}, "X"), 
    stoneTextureSelector({200, 40}, "Marble"), 
    backgroundTextureSelector({200, 40}, "Sandy"),
    musicSlider({200, 40}),
    SFXSlider({200, 40}) {
        closeButton.setFillColor(sf::Color(255, 0, 0));
        closeButton.setHoverFillColor(sf::Color(180, 0, 0));
        closeButton.setColor(sf::Color(255, 255, 255));
        closeButton.setPosition({float(position.x + size.x - titlebarHeight - borderWidth), float(position.y)});
        closeButton.setFontSize(closeButton.size.x - borderWidth * 2);

        stoneTextureSelector.addSelection("Marble");
        stoneTextureSelector.addSelection("Gliter");
        stoneTextureSelector.addSelection("type 2");
        stoneTextureSelector.setPosition({position.x + 80, position.y + titlebarHeight + 80});
        stoneTextureSelector.setFillColor(sf::Color(0, 0, 255));
        stoneTextureSelector.setHoverFillColor(sf::Color(0, 0, 180));
        stoneTextureSelector.setColor(inverseColor);
        stoneTextureSelector.onSelect = &texture::Stone::loadTexture;

        backgroundTextureSelector.addSelection("Sandy");
        backgroundTextureSelector.addSelection("Stone");
        backgroundTextureSelector.addSelection("type 2");
        backgroundTextureSelector.setPosition({position.x + 80 + 200 + 100, position.y + titlebarHeight + 80});
        backgroundTextureSelector.setFillColor(sf::Color(0, 0, 255));
        backgroundTextureSelector.setHoverFillColor(sf::Color(0, 0, 180));
        backgroundTextureSelector.setColor(inverseColor);
        backgroundTextureSelector.onSelect = &texture::Background::loadTexture;
        
        musicSlider.setFillColor(sf::Color(0, 0, 255));
        musicSlider.setBackgroundFillColor(sf::Color(180, 180, 180));
        musicSlider.setPosition({position.x + 80, position.y + titlebarHeight + 80 + 120});
        musicSlider.onChange = &audio::Music::setVolume;
        
        SFXSlider.setFillColor(sf::Color(0, 0, 255));
        SFXSlider.setBackgroundFillColor(sf::Color(180, 180, 180));
        SFXSlider.setPosition({position.x + 80 + 200 + 100, position.y + titlebarHeight + 80 + 120});
        SFXSlider.onChange = &audio::SFX::setVolume;
    }
    
    void drawWindow(sf::RenderWindow& window);

    bool turnVisible();

    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool scrollHandler(sf::Event::MouseWheelScrolled *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};

class PopupWindowLayer {
public:
    SavePopupWindow savepopup;
    LoadPopupWindow loadpopup;
    ConfigurationPopupWindow configpopup;
    
    void drawPopupWindows(sf::RenderWindow& window);

    bool scrollHandler(sf::Event::MouseWheelScrolled *mouseButtonEvent);
    bool hoverHandler(sf::Event::MouseMoved *mouseButtonEvent);
    bool pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent);
    bool releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent);
private:
};

extern Input *activeInput;