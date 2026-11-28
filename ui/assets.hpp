#include "debug/log.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Font {
public:
    sf::Font zpix;

    Font() {
        if (!zpix.openFromFile("assets/fonts/zpix.ttf"))
        {
            // error...
        }
    }
private:
};

extern Font font;

namespace texture {
    namespace Stone {
        extern sf::Texture black;
        extern sf::Texture white;

        bool loadTexture(int index);
    }
    namespace Background {
        extern sf::Texture board;
        extern sf::Texture sidebar;
        extern sf::Texture window;
        extern sf::Texture wallpaper;

        bool loadTexture(int index);
    }
}

namespace audio {
    namespace Music {
        extern sf::Music background;

        bool setVolume(float value);
        bool loadMusic(int index);
    }
    namespace SFX {
        extern float volume;

        extern sf::SoundBuffer stoneBuffer;
        extern sf::SoundBuffer buttonBuffer;
        extern sf::SoundBuffer slideBuffer;
        extern sf::SoundBuffer notificationBuffer;

        extern sf::Sound stone;
        extern sf::Sound button;
        extern sf::Sound slide;
        extern sf::Sound notification;

        bool placeStone();
        bool pressButton();
        bool moveSlider();
        bool newNotification();

        bool setVolume(float value);

        bool loadSFX(int index);
    }
    bool loadSound(int index);
}