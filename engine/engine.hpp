#pragma once

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
#include <fstream>

#include "debug/log.hpp"
// #include "engine/AI.hpp"

#define BOARDSIZE 9
#define UNDOSTSZ 69

const int8_t ADJCORD_X[4] = {1, 0, 0, -1};
const int8_t ADJCORD_Y[4] = {0, -1, 1, 0};

struct Game {
    int8_t current[BOARDSIZE + 2][BOARDSIZE + 2];

    bool turn = 0; // next player turn

    int taken[2];

    Game() {
        taken[0] = taken[1] = 0;
        for (int i = 1; i < BOARDSIZE + 1; i++) {
            for (int j = 1; j < BOARDSIZE + 1; j++) {
                current[i][j] = 0;
            }
        }
        for (int i = 0; i < BOARDSIZE + 2; i++) {
            current[0][i] = current[BOARDSIZE + 1][i] = current[i][0] = current[i][BOARDSIZE + 1] = 127;
        }
    }
};

// Game current_game, last_game;
extern std::deque<Game> games;
extern std::vector<Game> redo_stack;

bool INVALID_MOVE();

bool cmpGameArray(int8_t next[][BOARDSIZE + 2], int8_t previous[][BOARDSIZE + 2]);
bool checkAndClear(Game &newgame, bool side, int x, int y, std::vector<std::pair<int8_t, int8_t>> *clearptr = nullptr);

bool placeStone(int x, int y, bool side);

bool undoMove();
bool redoMove();

bool saveGame(char *file_name);
bool loadGame(char *file_name);

void printGame(Game c);
void printCurrentGame();