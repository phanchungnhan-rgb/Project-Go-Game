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

#include <intrin.h>
#pragma intrinsic(_BitScanForward)


#include "debug/log.hpp"
// #include "engine/AI.hpp"

#define BOARDSIZE 19
#define UNDOSTSZ 500

const int8_t ADJCORD_X[4] = {1, 0, 0, -1};
const int8_t ADJCORD_Y[4] = {0, -1, 1, 0};

const int8_t PROCORD_X[8] = {1, 0, 0, -1, -1, 1, -1, 1};
const int8_t PROCORD_Y[8] = {0, -1, 1, 0, -1, -1, 1, 1};

namespace go {
    struct Cord2i {
        int x, y;
        // Cord2i(int xxx, int yyy) {
        //     x = xxx;
        //     y = yyy;
        // }
        bool operator == (Cord2i &a) {
            return (x == a.x && y == a.y);
        }
    };

    struct Set2i {
        unsigned long availability[BOARDSIZE + 2] = {};
        unsigned long rowavai = 0;
        std::vector<int> rowlist;
        int cnt = 0;

        Set2i() {
        }

        unsigned long unsignedone = 1;

        bool find(Cord2i ele) {
            return (availability[ele.x] >> ele.y) & unsignedone;
        } 
        bool insert(Cord2i ele) {
            if (find(ele)) return false;
            if (((rowavai >> ele.x) & unsignedone) == 0) {
                rowlist.push_back(ele.x);
                rowavai = rowavai | (unsignedone << ele.x);
            }
            availability[ele.x] = availability[ele.x] | (unsignedone << ele.y);
            cnt++;
            return true;
        }
        bool erase(Cord2i ele) {
            if (!find(ele)) return false;
            availability[ele.x] &= ~(unsignedone << ele.y);
            cnt--;
            return true;
        }
        int size() {
            return cnt;
        }
        void clear() {
            for (int x : rowlist) {
                availability[x] = 0;
            }
            rowavai = 0;
            rowlist.clear();
            cnt = 0;
        }
        void mergeTo(Set2i &otherset) {
            if (this == &otherset) return;
            for (int x : rowlist) {
                if (availability[x] != 0) {
                    if (((otherset.rowavai >> x) & unsignedone) == 0) {
                        otherset.rowlist.push_back(x);
                        otherset.rowavai |= (unsignedone << x);
                    }
                    otherset.cnt += __popcnt((~otherset.availability[x]) & availability[x]);
                    otherset.availability[x] |= availability[x];
                }
            }
            clear();
        } 
    };
}

struct Move {
    go::Cord2i position;
    int owner = 0;

    std::vector<go::Cord2i> placed;
    std::vector<go::Cord2i> removed;

    Move(go::Cord2i pos, int ow) {
        position = pos;
        owner = ow;
    }
};

struct Game {
    int8_t currentBoard[BOARDSIZE + 2][BOARDSIZE + 2];

    // Game current_game, last_game;
    std::deque<Move> previous_moves;
    std::vector<Move> future_moves;

    bool turn = 0; // next player turn

    // DSU part 
    go::Cord2i ancestor[BOARDSIZE + 2][BOARDSIZE + 2];
    std::vector<go::Cord2i> rootChilds[BOARDSIZE + 2][BOARDSIZE + 2];
    go::Set2i rootLiberties[BOARDSIZE + 2][BOARDSIZE + 2];

    go::Cord2i root(go::Cord2i pos) {
        if (ancestor[pos.x][pos.y] == pos) return pos;
        // else return root(ancestor[pos.x][pos.y]); // without compression
        else return ancestor[pos.x][pos.y] = root(ancestor[pos.x][pos.y]); // with compression
    }

    bool mergeDSU(go::Cord2i u, go::Cord2i v) {
        u = root(u); v = root(v);
        if (u == v) return false;

        if (rootChilds[u.x][u.y].size() < rootChilds[v.x][v.y].size()) std::swap(u, v);

        ancestor[v.x][v.y] = u;
        for (go::Cord2i &pos : rootChilds[v.x][v.y]) {
            rootChilds[u.x][u.y].push_back(pos);
        }
        rootChilds[v.x][v.y].clear();

        rootLiberties[v.x][v.y].mergeTo(rootLiberties[u.x][u.y]);

        return true;
    }

    bool removeDSU(go::Cord2i u, std::vector<go::Cord2i> *removedStones = nullptr) {
        int8_t color = currentBoard[u.x][u.y];
        u = root(u);
        for (go::Cord2i v : rootChilds[u.x][u.y]) {
            for (int _ = 0; _ < 4; _++) {
                go::Cord2i adj = {v.x + ADJCORD_X[_], v.y + ADJCORD_Y[_]};
                if (currentBoard[adj.x][adj.y] == 127) continue;
                go::Cord2i adjroot = root(adj);
                if (currentBoard[adj.x][adj.y] != color && currentBoard[adj.x][adj.y] != 0) {
                    rootLiberties[adjroot.x][adjroot.y].insert(v);
                }
            }
            if (removedStones != nullptr) {
                removedStones->push_back(v);
            }
            if (v == u) continue;
            ancestor[v.x][v.y] = v;
            rootChilds[v.x][v.y].clear(); rootChilds[v.x][v.y].push_back(v);
            rootLiberties[v.x][v.y].clear();
            currentBoard[v.x][v.y] = 0;
        }
        ancestor[u.x][u.y] = u;
        rootChilds[u.x][u.y].clear(); rootChilds[u.x][u.y].push_back(u);
        rootLiberties[u.x][u.y].clear();
        currentBoard[u.x][u.y] = 0;
        return true;
    }

    bool recalculateDSU() {
        for (int i = 1; i < BOARDSIZE + 1; i++) {
            for (int j = 1; j < BOARDSIZE + 1; j++) {
                ancestor[i][j] = {i, j};
                rootChilds[i][j].clear();
                rootChilds[i][j].push_back({i, j});
                
                rootLiberties[i][j].clear();
                if (currentBoard[i][j] != 0) {
                    if (i > 1 && currentBoard[i - 1][j] == 0) rootLiberties[i][j].insert({i - 1, j});
                    if (j > 1 && currentBoard[i][j - 1] == 0) rootLiberties[i][j].insert({i, j - 1});
                    if (i < BOARDSIZE && currentBoard[i + 1][j] == 0) rootLiberties[i][j].insert({i + 1, j});
                    if (j < BOARDSIZE && currentBoard[i][j + 1] == 0) rootLiberties[i][j].insert({i, j + 1});
                }
            }
        }
        
        for (int i = 1; i < BOARDSIZE + 1; i++) {
            for (int j = 1; j < BOARDSIZE + 1; j++) {
                if (i > 1) {
                    if (currentBoard[i][j] == currentBoard[i - 1][j] && currentBoard[i][j] != 0) {
                        mergeDSU({i, j}, {i - 1, j});
                    }
                }
                if (j > 1) {
                    if (currentBoard[i][j] == currentBoard[i][j - 1] && currentBoard[i][j] != 0) {
                        mergeDSU({i, j}, {i, j - 1});
                    }
                }
            }
        }
        return true;
    }

    Game() {
        Move defaultMove({0, 0}, 1);
        previous_moves.push_back(defaultMove);
        for (int i = 1; i < BOARDSIZE + 1; i++) {
            for (int j = 1; j < BOARDSIZE + 1; j++) {
                currentBoard[i][j] = 0;
            }
        }
        for (int i = 0; i < BOARDSIZE + 2; i++) {
            currentBoard[0][i] = currentBoard[BOARDSIZE + 1][i] = currentBoard[i][0] = currentBoard[i][BOARDSIZE + 1] = 127;
        }
        recalculateDSU();
    }

    bool isValidMove(go::Cord2i position, bool side);
    bool placeStone(go::Cord2i position, bool side);
    bool passTurn(bool side);
    bool undoMove(int moves);
    bool redoMove(int moves);
    bool saveGame(char *file_name, int gamemode_flag = 0);
    bool loadGame(char *file_name, int *gamemode_flag = nullptr);
};

bool INVALID_MOVE();

extern Game mainGame;