#include "handler.hpp"


MCTSBOT mctsR;

int gamemode_flag = 1; 
// 0 : play with human
// 1 : play with MCTS Random

void switchGamemodeEvent(int index) {
    if (index == 0) {
        gamemode_flag = 0;
    } else if (index == 1) {
        gamemode_flag = 1;
        mctsR.init();
    }
}

bool moveWithMCTSRandom(int x, int y) {
    if (mainGame.placeStone({x, y}, !mainGame.previous_moves.back().owner)) {
        mctsR.opponentMove(x, y);
    }
    return 1;
}

bool AIMoveEvent() {
    if (gamemode_flag == 0 || mainGame.previous_moves.back().owner != 0) return 0;
    else if (gamemode_flag == 1) {
        go::Cord2i loca = mctsR.ourTurn();
        return mainGame.placeStone({loca.x, loca.y}, !mainGame.previous_moves.back().owner);
    }
    return 1;
}

bool stonePlaceEvent(int x, int y) {
    if (gamemode_flag == 0) {
        return mainGame.placeStone({x, y}, !mainGame.previous_moves.back().owner);
    } else if (gamemode_flag == 1) {
        return moveWithMCTSRandom(x, y);
    }
}

void turnPassEvent() {
    if (gamemode_flag == 0) {
        mainGame.passTurn(!mainGame.previous_moves.back().owner);
    } else if (gamemode_flag == 1) {
        mctsR.opponentPass();
        mainGame.passTurn(!mainGame.previous_moves.back().owner);
    }
}

void undoEvent() {
    if (gamemode_flag == 0) {
        mainGame.undoMove(1);
    } else if (gamemode_flag == 1) {
        mainGame.undoMove(2);
        mctsR.init();
    }
}

void redoEvent() {
    if (gamemode_flag == 0) {
        mainGame.redoMove(1);
    } else if (gamemode_flag == 1) {
        mainGame.redoMove(2);
        mctsR.init();
    }
}

bool saveGame(std::string &filename) {
    char filenamewithextension[100];
    strcpy(filenamewithextension, (filename + ".txt").c_str());
    return mainGame.saveGame(filenamewithextension, gamemode_flag);
}