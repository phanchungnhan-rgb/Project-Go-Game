#include "handler.hpp"


MCTSBOT mctsR;

int gamemode_flag = 0; 
// 0 : play with human
// 1 : play with MCTS Random

bool moveWithMCTSRandom(int x, int y) {
    if (placeStone(x, y, games.back().turn)) {
        mctsR.opponentMove(x, y);

        std::pair<int, int> loca = mctsR.ourTurn();
        placeStone(loca.first, loca.second, games.back().turn);
    }

    return 1;
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