#include "main.hpp"

int main() {
    CONSOLE_LOG("this is %d %s \n", 342, "testing 1234");

    while (true) {
        int x, y, side;
        std::cin >> x >> y >> side;

        if (x == -404) {
            char x[69];
            std::cin >> x;
            saveGame(x);

            continue;
        } else if (x == 404) {
            char x[69];
            std::cin >> x;
            loadGame(x);

            continue;
        }

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