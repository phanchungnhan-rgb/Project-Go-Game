#include "debug/log.hpp"

#include <SFML/Graphics.hpp>


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

        bool loadTexture(int index);
    }
}