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
        sf::Texture wallpaper;

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
            if (!wallpaper.loadFromFile("assets/textures/background_wallpaper_" + std::to_string(index) + ".png")) {
                return false;
            } 
            CONSOLE_LOG("LOADED BACKGROUND TEXTURES");
            return true;
        }
    }
}

namespace audio {
    namespace Music {
        sf::Music background;

        bool setVolume(float value) {
            background.setVolume(value * 100);
            return true;
        }

        bool loadMusic(int index) {
            if (!background.openFromFile("assets/audio/music/background_" + std::to_string(index) + ".mp3")) {
                return false;
            } else {
                background.setLooping(true);
                background.play();
            }
            return true;
        }
    }
    namespace SFX {
        sf::SoundBuffer stoneBuffer;
        sf::SoundBuffer buttonBuffer;
        sf::SoundBuffer slideBuffer;
        sf::SoundBuffer notificationBuffer;

        sf::Sound stone(stoneBuffer);
        sf::Sound button(buttonBuffer);
        sf::Sound slide(slideBuffer);
        sf::Sound notification(notificationBuffer);

        bool placeStone() {
            stone.play();
            return true;
        }
        bool pressButton() {
            button.play();
            return true;
        }
        bool moveSlider() {
            slide.play();
            return true;
        }
        bool newNotification() {
            notification.play();
            return true;
        }

        bool setVolume(float value) {
            stone.setVolume(value * 100);
            button.setVolume(value * 100);
            slide.setVolume(value * 100);
            return true;
        }

        bool loadSFX(int index) {
            if (!stoneBuffer.loadFromFile("assets/audio/sfx/stone_" + std::to_string(index) + ".wav")) {
                return false;
            } 
            if (!buttonBuffer.loadFromFile("assets/audio/sfx/button_" + std::to_string(index) + ".wav")) {
                return false;
            } 
            if (!slideBuffer.loadFromFile("assets/audio/sfx/slide_" + std::to_string(index) + ".wav")) {
                return false;
            } 
            if (!notificationBuffer.loadFromFile("assets/audio/sfx/notification_" + std::to_string(index) + ".wav")) {
                return false;
            } 
            CONSOLE_LOG("LOADED FSXs");
            return true;
        }
    }
    bool loadSound(int index) {
        Music::loadMusic(index);
        SFX::loadSFX(index);
        return true;
    }
}