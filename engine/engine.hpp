#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <queue>
#include <deque>
#include <stack>
#include <utility>
#include <debug/log.hpp>

#define BOARDSIZE 19
#define UNDOSTSZ 69

const int8_t ADJCORD_X[4] = {1, 0, 0, -1};
const int8_t ADJCORD_Y[4] = {0, -1, 1, 0};

struct Game {
    int8_t current[BOARDSIZE + 2][BOARDSIZE + 2];

    bool turn = 0;

    Game() {
        for (int i = 1; i < BOARDSIZE + 1; i++) {
            for (int j = 1; j < BOARDSIZE + 1; j++) {
                current[i][j] = 0;
            }
        }
        for (int i = 0; i < BOARDSIZE + 2; i++) {
            current[0][i] = current[20][i] = current[i][0] = current[i][20] = 69;
        }
    }
};

bool placeStone(int x, int y, bool side);

bool undoMove();
bool redoMove();

void printGame(Game c);
void printCurrentGame();