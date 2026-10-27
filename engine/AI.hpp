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
#include <set>
#include <math.h>

#include "debug/log.hpp"
#include "engine/engine.hpp"

#define heat sqrt(2)

struct MoveNode {
    Game last;
    Game current;

    // we dont have to worry about which item in the set is the current child because its in the first of the set
    // so we just have to recalculate the first of the set each time we simulate
    std::set<std::pair<std::array<int8_t, 3>, MoveNode *>> remaining; // (weight; x; y); child node

    double w, n;
};

bool SimulateMove(Game &game, Game &previousgame, bool side, int x, int y);