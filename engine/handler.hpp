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
#include "engine/engine.hpp"
#include "MinimaxAI.hpp"
#include "MCTSRandomAI.hpp"


extern MCTSBOT mctsR;

extern int gamemode_flag; 
// 0 : play with human
// 1 : play with MCTS Random

bool switchGamemodeEvent(int index);

bool moveWithMCTSRandom(int x, int y);

bool AIMoveEvent();

bool stonePlaceEvent(int x, int y);

void turnPassEvent();

void undoEvent();

void redoEvent();

bool saveGame(std::string &filename);

bool loadGame(std::string &filename);