#include "main.hpp"

int main() {
    CONSOLE_LOG("this is %d %s \n", 342, "testing 1234");

    while (true) {
        int x, y, side;
        std::cin >> x >> y >> side;

        if (x < 0) {
            undoMove();
        } else if (y < 0) {
            redoMove();
        } else {
            placeStone(x, y, side);
        }

        printCurrentGame();
    }

    return 0;
}