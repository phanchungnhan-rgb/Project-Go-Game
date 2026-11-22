#include "game_window.hpp"

// 0 = normal
// 1 = show AI simulation (Debug)
int render_mode = 0;

Input *activeInput = nullptr;

PopupWindowLayer popuplayer;

GameMenu gameMenu({380, 800 - 40}, "sample");

void GameWindow::run() {
    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}),
        "NoGoNoLife",
        sf::Style::Titlebar
    );
    window.setFramerateLimit(60);

    gameMenu.size = {380, windowHeight - boardMargin};
    gameMenu.setPosition({800, boardMargin / 2});
    gameMenu.setFillColor(sf::Color(220, 179, 92));

    while (window.isOpen()) {
        AIMoveEvent();

        processEvents(window);

        window.clear(sf::Color(220, 179, 92));
        window.clear(sf::Color(0, 0, 0));
        drawBoard(window);
        
        gameMenu.drawMenu(window);
        popuplayer.drawPopupWindows(window);
        window.display();
    }
}

float GameWindow::boardSpacing() const {
    return (windowHeight - 2.f * boardMargin) /
           static_cast<float>(BOARDSIZE - 1);
}

bool GameWindow::pixelToBoardPosition(
    sf::Vector2i pixel,
    int& boardX,
    int& boardY
) const {
    const float spacing = boardSpacing();

    const int column = static_cast<int>(
        std::lround((static_cast<float>(pixel.x) - boardMargin) / spacing)
    );
    const int row = static_cast<int>(
        std::lround((static_cast<float>(pixel.y) - boardMargin) / spacing)
    );

    if (column < 0 || column >= BOARDSIZE ||
        row < 0 || row >= BOARDSIZE) {
        return false;
    }

    const float intersectionX = boardMargin + column * spacing;
    const float intersectionY = boardMargin + row * spacing;
    const float clickTolerance = spacing * 0.35f;

    if (std::abs(static_cast<float>(pixel.x) - intersectionX) > clickTolerance ||
        std::abs(static_cast<float>(pixel.y) - intersectionY) > clickTolerance) {
        return false;
    }

    boardX = column + 1;
    boardY = row + 1;

    return true;
}

// this look very bad, gotta change somehow
bool isSomethingHovered; // only check for this if the condition is not a hoverHandler
void GameWindow::MouseMoveEventHandler(sf::Event::MouseMoved *mouseButtonEvent) {
    std::cout << "new mouse x: " << mouseButtonEvent->position.x << std::endl;
    std::cout << "new mouse y: " << mouseButtonEvent->position.y << std::endl;

    isSomethingHovered = false;

    if (popuplayer.hoverHandler(mouseButtonEvent)) return;

    if ((!isSomethingHovered) && pixelToBoardPosition(mouseButtonEvent->position, stoneHoverX, stoneHoverY)) {
    } else {
        stoneHoverX = 0;
        stoneHoverY = 0;
    }

    gameMenu.hoverHandler(mouseButtonEvent);
}

void GameWindow::MouseButtonPressedEventHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {
    gameMenu.pressedHandler(mouseButtonEvent);
}

void GameWindow::MouseButtonReleasedEventHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    if (mouseButtonEvent->button == sf::Mouse::Button::Left) {
        if (popuplayer.releasedHandler(mouseButtonEvent)) return;
        if (gameMenu.releasedHandler(mouseButtonEvent)) return;

        int boardX = 0, boardY = 0;

        if (pixelToBoardPosition(mouseButtonEvent->position, boardX, boardY)) {
            // if (placeStone(boardX, boardY, games.back().turn)) {
            // }

            if (boardX != 0 && boardY != 0) stonePlaceEvent(boardX, boardY);
        }
        CONSOLE_LOG("clicked %d and %d\n", boardX, boardY);
    }
}

void GameWindow::processEvents(sf::RenderWindow& window) {
    while (std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }


        if(auto* textEnteredEvent = event->getIf<sf::Event::TextEntered>()){
            CONSOLE_LOG("TYPED %s and active %d", std::to_string(textEnteredEvent->unicode), (int) (activeInput != nullptr));
            if (activeInput != nullptr) {
                char32_t unicode = textEnteredEvent->unicode;
                if (unicode != 8) {
                    if (('a' <= unicode && unicode <= 'z') ||
                        ('A' <= unicode && unicode <= 'Z') || 
                        ('0' <= unicode && unicode <= '9') ||
                        (unicode == '_' || unicode == '-')
                    ) activeInput->innerText.push_back(char(textEnteredEvent->unicode));
                } else {
                    if (!activeInput->innerText.empty()) activeInput->innerText.pop_back();
                } 
            }
        }
        
        if (auto* mouseButtonEvent = event->getIf<sf::Event::MouseMoved>()) {
            MouseMoveEventHandler(mouseButtonEvent);
        }
        if (auto* mouseButtonEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            MouseButtonPressedEventHandler(mouseButtonEvent);
        }
        if (auto* mouseButtonEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
            activeInput = nullptr;
            MouseButtonReleasedEventHandler(mouseButtonEvent);
        }
    }
}

void GameWindow::drawStone(sf::RenderWindow& window, int x, int y, sf::Color color, float radius) {
    const float spacing = boardSpacing();
    const float pixelX = boardMargin + (x - 1) * spacing + 0.2 * ((((x * y + 67) * 131) % 24) - 24 / 2);
    const float pixelY = boardMargin + (y - 1) * spacing + 0.2 * ((((y * x + 131) * 67) % 24) - 24 / 2);
    sf::CircleShape stone(radius);
    stone.setOrigin({radius, radius});
    stone.setPosition({pixelX, pixelY});
    stone.setFillColor(color);
    window.draw(stone);
}

void GameWindow::drawBoard(sf::RenderWindow& window) {
    const float spacing = boardSpacing();

    sf::RectangleShape backboard(
        {windowHeight - boardMargin, windowHeight - boardMargin}
    );
    backboard.setPosition({boardMargin / 2, boardMargin / 2});
    backboard.setFillColor(sf::Color(220, 179, 92));
    window.draw(backboard);

    for (int i = 0; i < BOARDSIZE; ++i) {
        const float offset = boardMargin + static_cast<float>(i) * spacing;

        sf::RectangleShape horizontal(
            {windowHeight - 2.f * boardMargin, 1.f}
        );
        horizontal.setPosition({boardMargin, offset});
        horizontal.setFillColor(sf::Color::Black);
        window.draw(horizontal);

        sf::RectangleShape vertical(
            {1.f, windowHeight - 2.f * boardMargin}
        );
        vertical.setPosition({offset, boardMargin});
        vertical.setFillColor(sf::Color::Black);
        window.draw(vertical);
    }
    for (int i = 1; i < BOARDSIZE + 1; ++i) {
        for (int j = 1; j < BOARDSIZE + 1; ++j) {
            const float radius = spacing * 0.45f;
            const float pixelX = boardMargin + (i - 1) * spacing;
            const float pixelY = boardMargin + (j - 1) * spacing;

            if (mainGame.currentBoard[i][j] == 1) {
                drawStone(window, i, j, sf::Color::Black);
            } else if (mainGame.currentBoard[i][j] == 2) {
                drawStone(window, i, j, sf::Color::White);
            }
        }
    }

    // draw the hover
    if (stoneHoverX != 0 && stoneHoverY != 0) {
        bool color = !mainGame.previous_moves.back().owner;
        drawStone(window, stoneHoverX, stoneHoverY, sf::Color(color * 255, color * 255, color * 255, 50));
    }
}




void Button::setPosition(sf::Vector2f pos) {
    position = pos;
}

void Button::setFillColor(sf::Color clr) {
    fillColor = clr;
}
void Button::setHoverFillColor(sf::Color clr) {
    hoverFillColor = clr;
}
void Button::setColor(sf::Color clr) {
    color = clr;
}
void Button::setFontSize(float sz) {
    fontSize = sz;
}

void Button::drawButton(sf::RenderWindow& window) {
    sf::RectangleShape button(
        {size.x, size.y}
    );
    button.setPosition({position.x, position.y});
    if (hovered) button.setFillColor(hoverFillColor); else button.setFillColor(fillColor);
    window.draw(button);
    boundingBox = button.getGlobalBounds();

    sf::Text text(font.zpix);
    text.setString(innerText);
    text.setCharacterSize(fontSize);
    sf::FloatRect textBoundingBox = text.getGlobalBounds();
    text.setPosition({position.x + (size.x - textBoundingBox.size.x) / 2, position.y + (size.y - fontSize) / 2});
    text.setFillColor(color);
    text.setStyle(sf::Text::Regular);
    window.draw(text);
}

bool Button::hoverHandler(sf::Event::MouseMoved *mouseButtonEvent) { 
    if ((!isSomethingHovered) && boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        CONSOLE_LOG("HOVING ");
        hovered = true;
        isSomethingHovered = true;
    } else {
        hovered = false;
    }
    return false;
}

bool Button::pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {   
    return false;
}

bool Button::releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    if (boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        if (onClick != nullptr) onClick();
        return true;
    }
    return false;
}




void Input::setPosition(sf::Vector2f pos) {
    position = pos;
}

void Input::setFillColor(sf::Color clr) {
    fillColor = clr;
}
void Input::setHoverFillColor(sf::Color clr) {
    hoverFillColor = clr;
}
void Input::setColor(sf::Color clr) {
    color = clr;
}
void Input::setFontSize(float sz) {
    fontSize = sz;
}

void Input::drawInput(sf::RenderWindow& window) {
    sf::RectangleShape button(
        {size.x, size.y}
    );
    button.setPosition({position.x, position.y});
    if (hovered || activeInput == this) button.setFillColor(hoverFillColor); else button.setFillColor(fillColor);
    window.draw(button);
    boundingBox = button.getGlobalBounds();

    sf::Text text(font.zpix);
    if (blickCycleState++ % (blinkCycle * 2) < blinkCycle && this == activeInput) text.setString(innerText + "_"); else text.setString(innerText);
    text.setCharacterSize(fontSize);
    text.setPosition({position.x + (size.y - fontSize), position.y + (size.y - fontSize) / 2});
    text.setFillColor(color);
    text.setStyle(sf::Text::Regular);
    window.draw(text);
}

bool Input::hoverHandler(sf::Event::MouseMoved *mouseButtonEvent) { 
    if ((!isSomethingHovered) && boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        CONSOLE_LOG("HOVING ");
        hovered = true;
        isSomethingHovered = true;
    } else {
        hovered = false;
    }
    return false;
}

bool Input::pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {   
    return false;
}

bool Input::releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    if (boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        activeInput = this;
        if (onClick != nullptr) onClick();
        return true;
    }
    return false;
}


void DropdownSelector::addSelection(std::string str) {
    Button *selection = new Button(size, str);
    selectionList.push_back(selection); 

    selection->fontSize = fontSize;
    selection->position = {position.x, position.y + selectionList.size() * size.y};
    selection->fillColor = fillColor;
    selection->hoverFillColor = hoverFillColor;
    selection->color = color;
    selection->size = size;
}
void DropdownSelector::addSelectionList(std::vector<std::string> vtrstr) {

}

void DropdownSelector::setPosition(sf::Vector2f pos) {
    position = pos;
    activeSelection.position = position;
    for (int i = 0; i < selectionList.size(); i++) {
        Button * selection = selectionList[i];
        selection->position = {position.x, position.y + (i + 1) * size.y};
    }
}

void DropdownSelector::setFillColor(sf::Color clr) {
    fillColor = clr;
}
void DropdownSelector::setHoverFillColor(sf::Color clr) {
    hoverFillColor = clr;
}
void DropdownSelector::setColor(sf::Color clr) {
    color = clr;
}
void DropdownSelector::setFontSize(float sz) {
    fontSize = sz;
}

void DropdownSelector::drawDropdownSelector(sf::RenderWindow& window) {
    activeSelection.drawButton(window);
    if (droped) for (Button * selection : selectionList) {
        CONSOLE_LOG("pos : %d %d", selection->position.x, selection->position.y);
        selection->drawButton(window);
    }
}

bool DropdownSelector::hoverHandler(sf::Event::MouseMoved *mouseButtonEvent) { 
    if (activeSelection.hoverHandler(mouseButtonEvent)) {
    }
    if (droped) for (Button *selection : selectionList) {
        if (selection->hoverHandler(mouseButtonEvent)) {
        }
    }
    return false;
}

bool DropdownSelector::pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {   
    return false;
}

bool DropdownSelector::releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    if (activeSelection.releasedHandler(mouseButtonEvent)) {
        CONSOLE_LOG("I CLIKEDDDD");
        droped = !droped;
        return true;
    }
    if (droped) for (int i = 0; i < selectionList.size(); i++) {
        if (selectionList[i]->releasedHandler(mouseButtonEvent)) {
            switchSelection(i);
            droped = false;
            return true;
        }
    }
    droped = false;
    return false;
}


void TurnIndicator::setPosition(sf::Vector2f pos) {
    position = pos;
}
void TurnIndicator::setFillColor(sf::Color clr) {
    fillColor = clr;
}
void TurnIndicator::setColor(sf::Color clr) {
    color = clr;
}

void TurnIndicator::drawIndicator(sf::RenderWindow& window) {
    sf::RectangleShape background(
        {size.x, size.y}
    );
    background.setPosition({position.x, position.y});
    background.setFillColor(fillColor);
    window.draw(background);
    boundingBox = background.getGlobalBounds();

    const float radius = 25.f;

    sf::CircleShape stone(radius);
    stone.setOrigin({radius, radius});
    stone.setPosition({position.x + size.y / 2, position.y + size.y / 2});
    bool side = !mainGame.previous_moves.back().owner;
    stone.setFillColor(sf::Color(side * 255, side * 255, side * 255, 50));
    window.draw(stone);

    if (mainGame.previous_moves.back().owner == 1) {
        innerText = "Black turn";
    } else {
        innerText = "White turn";
    }


    sf::Text text(font.zpix);
    text.setString(innerText);
    text.setCharacterSize(fontSize);
    sf::FloatRect textBoundingBox = text.getGlobalBounds();
    text.setPosition({position.x + 100, position.y + (size.y - fontSize) / 2});
    text.setFillColor(color);
    text.setStyle(sf::Text::Regular);
    window.draw(text);
}





void GameMenu::setPosition(sf::Vector2f pos) {
    position = pos;

    undoButton.setPosition({position.x + menuMargin, position.y + menuMargin});
    redoButton.setPosition({position.x + (size.x + menuMargin) / 2, position.y + menuMargin});
    passButton.setPosition({position.x + menuMargin, position.y + buttonHeight + 2 * menuMargin});

    gamemodeSelector.setPosition({position.x + menuMargin, 220});
    
    saveButton.setPosition({position.x + menuMargin, 310});
    loadButton.setPosition({position.x + (size.x + menuMargin) / 2, 310});

    indicator.setPosition({position.x + menuMargin, position.y + size.y - buttonHeight - menuMargin});
}

void GameMenu::setFillColor(sf::Color clr) {
    color = clr;
}

void GameMenu::drawMenu(sf::RenderWindow& window) {
    sf::RectangleShape background(
        {size.x, size.y}
    );
    background.setPosition({position.x, position.y});
    background.setFillColor(color);
    window.draw(background);

    undoButton.drawButton(window);
    redoButton.drawButton(window);
    passButton.drawButton(window);
    
    saveButton.drawButton(window);
    loadButton.drawButton(window);

    gamemodeSelector.drawDropdownSelector(window);

    indicator.drawIndicator(window);
}

bool GameMenu::hoverHandler(sf::Event::MouseMoved *mouseButtonEvent) { 
    if (undoButton.hoverHandler(mouseButtonEvent)) {
    }
    if (redoButton.hoverHandler(mouseButtonEvent)) {
    }
    if (passButton.hoverHandler(mouseButtonEvent)) {
    }
    if (gamemodeSelector.hoverHandler(mouseButtonEvent)) {
    }
    if (saveButton.hoverHandler(mouseButtonEvent)) {
    }
    if (loadButton.hoverHandler(mouseButtonEvent)) {
    }
    return false;
}

bool GameMenu::pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {  
    if (undoButton.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    if (redoButton.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    if (passButton.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    if (gamemodeSelector.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    if (saveButton.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    if (loadButton.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    return false;
}

bool GameMenu::releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    if (undoButton.releasedHandler(mouseButtonEvent)) {
        return true;
    }
    if (redoButton.releasedHandler(mouseButtonEvent)) {
        return true;
    }
    if (passButton.releasedHandler(mouseButtonEvent)) {
        return true;
    }
    if (gamemodeSelector.releasedHandler(mouseButtonEvent)) {
        return true;
    }
    if (saveButton.releasedHandler(mouseButtonEvent)) {
        return true;
    }
    if (loadButton.releasedHandler(mouseButtonEvent)) {
        return true;
    }
    return false;
}




void SavePopupWindow::drawWindow(sf::RenderWindow& window) {
    if (!visible) return;

    sf::RectangleShape background(
        {size.x, size.y}
    );
    background.setPosition({position.x, position.y}); boundingBox = background.getGlobalBounds();
    background.setFillColor(sf::Color(180, 180, 180));
    window.draw(background);

    sf::RectangleShape titlebarBackground(
        {size.x, titlebarHeight}
    );
    titlebarBackground.setPosition({position.x, position.y});
    titlebarBackground.setFillColor(sf::Color(0, 0, 180));
    window.draw(titlebarBackground);

    sf::Text title(font.zpix);
    title.setString(titleString);
    title.setFillColor(inverseColor);
    title.setCharacterSize(titlebarHeight - borderWidth * 3);
    title.setPosition({position.x + borderWidth * 2, position.y + borderWidth});
    window.draw(title);

    closeButton.drawButton(window);

    textinput.drawInput(window);

    sf::RectangleShape border(
        {size.x, size.y}
    );
    border.setPosition({position.x, position.y});
    border.setOutlineColor(sf::Color(120, 120, 120));
    border.setOutlineThickness(-borderWidth);
    border.setFillColor(sf::Color(0, 0, 0, 0));
    window.draw(border);
}

bool SavePopupWindow::hoverHandler(sf::Event::MouseMoved *mouseButtonEvent) {
    if (!visible) return false;
    if (closeButton.hoverHandler(mouseButtonEvent)) {
    }
    if (textinput.hoverHandler(mouseButtonEvent)) {
    }
    if ((!isSomethingHovered) && boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        isSomethingHovered = true;
    }
    return false;
}
bool SavePopupWindow::pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {
    if (!visible) return false;
    if (closeButton.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    if (textinput.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    if (boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        return true;
    }
    return false;
}
bool SavePopupWindow::releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    if (!visible) return false;
    if (closeButton.releasedHandler(mouseButtonEvent)) {
        visible = false;
        return true;
    }
    if (textinput.releasedHandler(mouseButtonEvent)) {
        return true;
    }
    if (boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        return true;
    } else {
        visible = false;
        return true;
    }
    return false;
}





void PopupWindowLayer::drawPopupWindows(sf::RenderWindow& window) {
    savepopup.drawWindow(window);
}

bool PopupWindowLayer::hoverHandler(sf::Event::MouseMoved *mouseButtonEvent) {
    if (savepopup.hoverHandler(mouseButtonEvent)) {
    }
    return false;
}
bool PopupWindowLayer::pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {
    if (savepopup.pressedHandler(mouseButtonEvent)) {
        return true;
    }
    return false;
}
bool PopupWindowLayer::releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    if (savepopup.releasedHandler(mouseButtonEvent)) {
        return true;
    }
    return false;
}