#include "game_window.hpp"

// 0 = normal
// 1 = show AI simulation (Debug)
int render_mode = 0;

MCTSBOT lv1bot;

void GameWindow::run() {
    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}),
        "NoGoNoLife",
        sf::Style::Titlebar
    );
    window.setFramerateLimit(60);

    lv1bot.init();

    MenuButton testButton({1000, 200}, "This is nut");

    while (window.isOpen()) {
        processEvents(window);

        window.clear(sf::Color(220, 179, 92));
        window.clear(sf::Color(0, 0, 0));
        drawBoard(window);
        // testButton.drawButton(window);
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

bool movewithai(int x, int y) {
    if (placeStone(x, y, games.back().turn)) {
        lv1bot.opponentMove(x, y);

        lv1bot.ourTurn();
    }

    return 1;
}

void GameWindow::processEvents(sf::RenderWindow& window) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
        {
            std::cout << "new mouse x: " << mouseMoved->position.x << std::endl;
            std::cout << "new mouse y: " << mouseMoved->position.y << std::endl;

            if (pixelToBoardPosition(mouseMoved->position, stoneHoverX, stoneHoverY)) {

            } else {
                stoneHoverX = 0;
                stoneHoverY = 0;
            }
        }

        if (const auto* KeyDownEvent = event->getIf<sf::Event::KeyPressed>()) {
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
        if (const auto* KeyUpEvent = event->getIf<sf::Event::KeyReleased>()) {
            if (KeyUpEvent->scancode == sf::Keyboard::Scancode::A) {
                render_mode = 0;
            }
            continue;
        }
        if (const auto* mouseButtonEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonEvent->button != sf::Mouse::Button::Left) {
                continue;
            }
            int boardX = 0, boardY = 0;

            if (pixelToBoardPosition(mouseButtonEvent->position, boardX, boardY)) {
                // if (placeStone(boardX, boardY, games.back().turn)) {
                // }

                movewithai(boardX, boardY);
            }
            CONSOLE_LOG("clicked %d and %d\n", boardX, boardY);
            continue;
        }
    }
}

void GameWindow::drawStone(sf::RenderWindow& window, int x, int y, sf::Color color) {
    const float spacing = boardSpacing();
    const float radius = spacing * 0.45f;
    const float pixelX = boardMargin + (x - 1) * spacing;
    const float pixelY = boardMargin + (y - 1) * spacing;
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
        drawStone(window, stoneHoverX, stoneHoverY, sf::Color(0, 0, 0, 50));
    }
}




void MenuButton::setPosition(sf::Vector2f &pos) {
    position = pos;
}

void MenuButton::setFillColor(sf::Color &clr) {
    color = clr;
}

void MenuButton::drawButton(sf::RenderWindow& window) {
    sf::RectangleShape button(
        {size.x, size.y}
    );
    button.setPosition({position.x, position.y});
    button.setFillColor(sf::Color(128, 128, 128));
    window.draw(button);

    sf::Font font;
    if (!font.openFromFile("fonts/zpix.ttf"))
    {
        // error...
    }

    sf::Text text(font);
    text.setString(innerText);
    text.setCharacterSize(24);
    sf::FloatRect textBoundingBox = text.getGlobalBounds();
    text.setPosition({position.x + (size.x - textBoundingBox.size.x) / 2, position.y + (size.y - textBoundingBox.size.y) / 2});
    text.setFillColor(color);
    text.setStyle(sf::Text::Regular);
    window.draw(text);
}