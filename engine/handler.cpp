#include "handler.hpp"


MCTSBOT mctsR;

int gamemode_flag = 1; 
// 0 : play with human
// 1 : play with MCTS Random

bool moveWithMCTSRandom(int x, int y) {
    if (placeStone(x, y, games.back().turn)) {
        mctsR.opponentMove(x, y);
    }
    return 1;
}

bool AIMoveEvent() {
    if (gamemode_flag == 0 || games.back().turn == 0) return 1;
    else if (gamemode_flag == 1) {
        std::pair<int, int> loca = mctsR.ourTurn();
        placeStone(loca.first, loca.second, games.back().turn);
    }
}

bool stonePlaceEvent(int x, int y) {
    if (gamemode_flag == 0) {
        return placeStone(x, y, games.back().turn);
    } else if (gamemode_flag == 1) {
        return moveWithMCTSRandom(x, y);
    }
}

void turnPassEvent() {
    if (gamemode_flag == 0) {
        passTurn(games.back().turn);
    } else if (gamemode_flag == 1) {
        moveWithMCTSRandom(-1, -1);
    }
}

void undoEvent() {
    if (gamemode_flag == 0) {
        undoMove();
    } else if (gamemode_flag == 1) {
        undoMove();
        undoMove();
        mctsR.init();
    }
}

void redoEvent() {
    if (gamemode_flag == 0) {
        redoMove();
    } else if (gamemode_flag == 1) {
        redoMove();
        redoMove();
        mctsR.init();
    }
}