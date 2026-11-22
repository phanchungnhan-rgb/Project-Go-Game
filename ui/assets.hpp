#include <SFML/Graphics.hpp>

class Font {
public:
    sf::Font zpix;

    Font() {
        if (!zpix.openFromFile("fonts/zpix.ttf"))
        {
            // error...
        }
    }
private:
};

extern Font font;