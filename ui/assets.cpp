#include "assets.hpp"

Font font;

namespace texture {
    namespace Stone {
        sf::Texture black;
        sf::Texture white;

        bool loadTexture(int index) {
            if (!black.loadFromFile("assets/textures/stone_black_" + std::to_string(index) + ".png")) {
                black.setSmooth(true);
                return false;
            }
            if (!white.loadFromFile("assets/textures/stone_white_" + std::to_string(index) + ".png")) {
                white.setSmooth(true);
                return false;
            }
            CONSOLE_LOG("LOADED STONE TEXTURES");
            return true;
        }
    }
    namespace Background {
        sf::Texture board;
        sf::Texture sidebar;
        sf::Texture window;

        bool loadTexture(int index) {
            if (!board.loadFromFile("assets/textures/background_board_" + std::to_string(index) + ".png")) {
                return false;
            }
            if (!sidebar.loadFromFile("assets/textures/background_sidebar_" + std::to_string(index) + ".png")) {
                return false;
            }
            if (!window.loadFromFile("assets/textures/background_window_" + std::to_string(index) + ".png")) {
                return false;
            } else {
                window.setRepeated(true);
            }
            CONSOLE_LOG("LOADED STONE TEXTURES");
            return true;
        }
    }
}