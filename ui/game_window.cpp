#include "game_window.hpp"

// 0 = normal
// 1 = show AI simulation (Debug)
int render_mode = 0;

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

void GameWindow::MouseMoveEventHandler(sf::Event::MouseMoved *mouseButtonEvent) {
    std::cout << "new mouse x: " << mouseButtonEvent->position.x << std::endl;
    std::cout << "new mouse y: " << mouseButtonEvent->position.y << std::endl;

    if (pixelToBoardPosition(mouseButtonEvent->position, stoneHoverX, stoneHoverY)) {
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
        int boardX = 0, boardY = 0;

        if (pixelToBoardPosition(mouseButtonEvent->position, boardX, boardY)) {
            // if (placeStone(boardX, boardY, games.back().turn)) {
            // }

            if (boardX != 0 && boardY != 0) stonePlaceEvent(boardX, boardY);
        }
        CONSOLE_LOG("clicked %d and %d\n", boardX, boardY);
    }

    gameMenu.releasedHandler(mouseButtonEvent);
}

void GameWindow::processEvents(sf::RenderWindow& window) {
    while (std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }


        if (auto* KeyDownEvent = event->getIf<sf::Event::KeyPressed>()) {
            if (KeyDownEvent->scancode == sf::Keyboard::Scancode::A) {
                render_mode = 1;
                continue;
            }
            if (KeyDownEvent->scancode == sf::Keyboard::Scancode::D) { // for testing only
                CONSOLE_LOG("simulating\n");

                auto beg = std::chrono::steady_clock::now();

                MoveNode newnode(0, 0);
                newnode.last = *(games.end() - 2);
                newnode.current = games.back();
                CONSOLE_LOG("random play score: %lf\n", simulateRandomPlay(newnode));

                auto en = std::chrono::steady_clock::now();

                CONSOLE_LOG("time simulated %d\n", std::chrono::duration_cast<std::chrono::milliseconds>(en - beg).count());

                continue;
            }
            continue;
        }
        if (auto* KeyUpEvent = event->getIf<sf::Event::KeyReleased>()) {
            if (KeyUpEvent->scancode == sf::Keyboard::Scancode::A) {
                render_mode = 0;
            }
            continue;
        }
        
        if (auto* mouseButtonEvent = event->getIf<sf::Event::MouseMoved>()) {
            MouseMoveEventHandler(mouseButtonEvent);
        }
        if (auto* mouseButtonEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
        }
        if (auto* mouseButtonEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
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

            if (games.back().current[i][j] == 1) {
                drawStone(window, i, j, sf::Color::Black);
            } else if (games.back().current[i][j] == 2) {
                drawStone(window, i, j, sf::Color::White);
            }
        }
    }

    // draw the hover
    if (stoneHoverX != 0 && stoneHoverY != 0) {
        drawStone(window, stoneHoverX, stoneHoverY, sf::Color(games.back().turn * 255, games.back().turn * 255, games.back().turn * 255, 50));
    }
}




void MenuButton::setPosition(sf::Vector2f pos) {
    position = pos;
}

void MenuButton::setFillColor(sf::Color clr) {
    fillColor = clr;
}
void MenuButton::setColor(sf::Color clr) {
    color = clr;
}

void MenuButton::drawButton(sf::RenderWindow& window) {
    sf::RectangleShape button(
        {size.x, size.y}
    );
    button.setPosition({position.x, position.y});
    button.setFillColor(fillColor);
    window.draw(button);
    boundingBox = button.getGlobalBounds();

    sf::Font font;
    if (!font.openFromFile("fonts/zpix.ttf"))
    {
        // error...
    }

    sf::Text text(font);
    text.setString(innerText);
    text.setCharacterSize(fontSize);
    sf::FloatRect textBoundingBox = text.getGlobalBounds();
    text.setPosition({position.x + (size.x - textBoundingBox.size.x) / 2, position.y + (size.y - fontSize) / 2});
    text.setFillColor(color);
    text.setStyle(sf::Text::Regular);
    window.draw(text);
}

void MenuButton::hoverHandler(sf::Event::MouseMoved *mouseButtonEvent) {   
    if (boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        setFillColor(sf::Color(108, 108, 108));
    } else {
        setFillColor(sf::Color(128, 128, 128));
    }
}

void MenuButton::pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {   
}

void MenuButton::releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    if (boundingBox.contains(sf::Vector2f(mouseButtonEvent->position.x, mouseButtonEvent->position.y))) {
        if (onClick != nullptr) onClick();
    }
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
    stone.setFillColor(sf::Color(games.back().turn * 255, games.back().turn * 255, games.back().turn * 255));
    window.draw(stone);

    if (games.back().turn == 0) {
        innerText = "Black turn";
    } else {
        innerText = "White turn";
    }


    sf::Font font;
    if (!font.openFromFile("fonts/zpix.ttf"))
    {
        // error...
    }

    sf::Text text(font);
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
    indicator.drawIndicator(window);
}

void GameMenu::hoverHandler(sf::Event::MouseMoved *mouseButtonEvent) {   
    undoButton.hoverHandler(mouseButtonEvent);
    redoButton.hoverHandler(mouseButtonEvent);
    passButton.hoverHandler(mouseButtonEvent);
}

void GameMenu::pressedHandler(sf::Event::MouseButtonPressed *mouseButtonEvent) {  
    undoButton.pressedHandler(mouseButtonEvent);
    redoButton.pressedHandler(mouseButtonEvent);
    passButton.pressedHandler(mouseButtonEvent); 
}

void GameMenu::releasedHandler(sf::Event::MouseButtonReleased *mouseButtonEvent) {
    undoButton.releasedHandler(mouseButtonEvent);
    redoButton.releasedHandler(mouseButtonEvent);
    passButton.releasedHandler(mouseButtonEvent);
    
}