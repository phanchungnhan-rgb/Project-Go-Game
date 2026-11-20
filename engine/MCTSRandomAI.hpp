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
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <algorithm>    // std::shuffle
#include <array>

#include "debug/log.hpp"
#include "engine/engine.hpp"

#define ln(x) log(x)
#define MCTSHEAT sqrt(2)
#define MCTSBIAS 67

#define INVALID_SIM std::nan("1")


double normlizeScore(double score);

double moveWithScore(Game &gameSession, go::Cord2i position, bool side);
double roughScoring(Game &game);
double simulateRandomPlay(Game &gameSession, std::mt19937 &rng, int extramoves = 0);

struct MoveNode {
    Move movement;
    
    // i was wrong, this will have to be recalculated every tree traversal
    std::vector<MoveNode *> childs; // (weight; x; y); child node

    MoveNode *parent = nullptr;

    double w = 0, n = 0;

    double bonus = 0;

    MoveNode() : movement({0, 0}, 1) {

    }

    static bool cmpBonus(MoveNode *a, MoveNode *b) {
        return a->bonus < b->bonus;
    }

    double calculateWeight(bool side = 1) {
        if (movement.owner == side) return (w / n) + MCTSHEAT * sqrt(ln(parent->n)/n) + (MCTSBIAS * bonus / (n + 1)); 
        else return (-1 * (w / n)) + MCTSHEAT * sqrt(ln(parent->n)/n) + (MCTSBIAS * bonus / (n + 1));
    }

    // the movement at this node has to be done in order to call this
    void expandNode(Game &gameSession, std::mt19937 &rng, double *win = nullptr) {
        childs.clear();
        if (win != nullptr) (*win) = 0;

        for (int8_t i = 1; i <= BOARDSIZE; i++) {
            for (int8_t j = 1; j <= BOARDSIZE; j++) {
                if (gameSession.currentBoard[i][j] == 0) {
                    bool is_valid_move = 0; // only place moves next to some stone and 
                    if ((i % 5 == 0) && (j % 5 == 0)) is_valid_move = 1;
                    for (int _ = 0; _ < 8; _++) {
                        int x = i + PROCORD_X[_], y = j + PROCORD_Y[_];
                        if ((int) gameSession.currentBoard[x][y] == 1 || (int) gameSession.currentBoard[x][y] == 2) is_valid_move = 1;
                    } 
                    if (!is_valid_move) continue; 
                    childs.push_back(new MoveNode);
                    childs.back()->parent = this;
                    childs.back()->bonus = moveWithScore(gameSession, {i, j}, !movement.owner);
                    
                    if (std::isfinite(childs.back()->bonus)) {

                        // do stuff to child here
                        childs.back()->movement = gameSession.previous_moves.back();

                        double simscore = simulateRandomPlay(gameSession, rng, 1);
                        if (simscore < 0) childs.back()->w = 0;
                        else {
                            childs.back()->w = 1;
                            w++; (*win)++;
                        }

                        childs.back()->n = 1;
                        n++;
                    } else {
                        // child invalid, delete
                        delete childs.back();
                        childs.pop_back();
                    }
                }
            }
        }
        std::sort(childs.begin(), childs.end(), cmpBonus /*, std::greater<std::pair<std::pair<double, std::array<int8_t, 2>>, MoveNode *>>()*/);
    }
};

struct MCTSBOT {
    Game gameSession;

    MoveNode *root = new MoveNode;

    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng{seed};

    int max_thinking_time;

    bool side;

    MCTSBOT(bool ourside = 1, int maxtimethink = 2200) {
        side = ourside;
        max_thinking_time = maxtimethink;
    }

    bool init() {
        gameSession = mainGame;

        root = new MoveNode;
        root->movement = gameSession.previous_moves.back();
        
        root->w = 0, root->n = 0;
        root->bonus = 0;

        return 1;
    }

    go::Cord2i ourTurn() {
        auto time_begin = std::chrono::steady_clock::now();

        int simcnt = 0; // performance testing
        while (true) {
            MoveNode *currentNode = root;

            std::stack<int> idstack;
            int depth = 0;

            while (!currentNode->childs.empty()) {
                int id = 0;
                for (int i = 0; i < currentNode->childs.size(); i++) {
                    if (currentNode->childs[id]->calculateWeight(side) < currentNode->childs[i]->calculateWeight(side)) {
                        id = i;
                    }
                }
                currentNode = currentNode->childs[id];
                gameSession.placeStone(currentNode->movement.position, currentNode->movement.owner);
                depth++;
                idstack.push(id);
            }

            double wincnt, deltasim;
            currentNode->expandNode(gameSession, rng, &wincnt);
            deltasim = currentNode->childs.size();
            simcnt += deltasim;

            while (currentNode->parent != nullptr) {
                currentNode->parent->n += deltasim;
                currentNode->parent->w += wincnt;

                idstack.pop();

                currentNode = currentNode->parent;
            }

            gameSession.undoMove(depth);

            auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_begin).count();

            if (time_elapsed > max_thinking_time) {
                CONSOLE_LOG("time elapsed: %d with %d simulations\n", (int) (time_elapsed), simcnt);
                break;
            }
        }

        // calc done, give move
        MoveNode * candidate_child = *(--(root->childs.end()));
        for (MoveNode * achild : root->childs) {
            if (achild != nullptr) if (achild->n > candidate_child->n) {
                candidate_child = achild;
            }
        }

        // placeStone(candidate_child->x, candidate_child->y, side); // move in engine
        root = candidate_child; // move in AI
        gameSession.placeStone(root->movement.position, root->movement.owner); // move in AI's gameSession
        root->parent = nullptr;
        CONSOLE_LOG("CANDIDATE MOVE : %d %d\n", candidate_child->movement.position.x, candidate_child->movement.position.y);

        return candidate_child->movement.position;
    }
    void opponentMove(int8_t x, int8_t y) {
        bool moved = false;
        for (MoveNode * achild : root->childs) {
            if (achild->movement.position.x == x && achild->movement.position.y == y) {
                root = achild;
                gameSession.placeStone(root->movement.position, root->movement.owner); // move in AI's gameSession
                root->parent = nullptr;
                moved = 1;
            }
        }
        if (moved) return;

        root->childs.push_back(new MoveNode);
        root->childs.back()->parent = root;
        root->childs.back()->bonus = moveWithScore(gameSession, {x, y}, !root->movement.owner);
        
        if (std::isfinite(root->childs.back()->bonus)) {
            // do stuff to child here
            root->childs.back()->movement = gameSession.previous_moves.back();

            // because win rate is not needed for root node
            // double simscore = simulateRandomPlay(gameSession, rng, 1);
            // if (simscore < 0) childs.back()->w = 0;
            // else {
            //     childs.back()->w = 1;
            //     w++; win++;
            // }

            root = root->childs.back();
            root->parent = nullptr;
            moved = 1;
        } else {
            // child invalid, delete
            gameSession.undoMove(1);
            delete root->childs.back();
            root->childs.pop_back();
        }
    }
    void opponentPass() {
        root->childs.push_back(new MoveNode);
        root->childs.back()->parent = root;
        // root->childs.back()->bonus = moveWithScore(gameSession, {x, y}, !root->movement.owner);
        gameSession.passTurn(!side);
        root->childs.back()->bonus = 0;

        if (std::isfinite(root->childs.back()->bonus)) {
            // do stuff to child here
            root->childs.back()->movement = gameSession.previous_moves.back();

            // because win rate is not needed for root node
            // double simscore = simulateRandomPlay(gameSession, rng, 1);
            // if (simscore < 0) childs.back()->w = 0;
            // else {
            //     childs.back()->w = 1;
            //     w++; win++;
            // }

            root = root->childs.back();
            root->parent = nullptr;
        } else {
            // child invalid, delete
            gameSession.undoMove(1);
            delete root->childs.back();
            root->childs.pop_back();
        }
    }
};