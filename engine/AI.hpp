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

struct MoveNode {
    Game last;
    Game current;

    // we dont have to worry about which item in the set is the current child because its in the first of the set
    // so we just have to recalculate the first of the set each time we simulate
    std::set<std::pair<std::pair<double, std::array<int8_t, 2>>, MoveNode *>, std::greater<std::pair<std::pair<double, std::array<int8_t, 2>>, MoveNode *>>> childs; // (weight; x; y); child node

    MoveNode *parent = nullptr;

    double w = 0, n = 0;

    void resetChilds() {
        childs.clear();
        for (int8_t i = 1; i <= BOARDSIZE; i++) {
            for (int8_t j = 1; j <= BOARDSIZE; j++) {
                if (current.current[i][j] == 0) {
                    childs.insert({{INFINITY, {i, j}}, nullptr}); // the weight is -1 so the untouched will end up at the end of the set, and it will be checked first in the code bellow
                }
            }
        }
    }
};

double normlizeScore(double score);

bool SimulateMove(Game &game, Game &previousgame, bool side, int x, int y, std::vector<std::pair<int8_t, int8_t>> *clearptr = nullptr, bool allow_eye_filling = 0);
bool simulatePass(Game &game, Game &previousgame, bool side);
double roughScoring(Game &game);

double simulateRandomPlay(MoveNode node);

struct MCTSBOT {
    MoveNode *root = new MoveNode;

    int max_thinking_time;

    bool side;

    MCTSBOT(bool ourside = 1, int maxtimethink = 3000) {
        side = ourside;
        max_thinking_time = maxtimethink;
    }

    bool init() {
        root->current = games.back();
        root->last = *(games.end() - 2);
        root->resetChilds();

        return 1;
    }

    void ourTurn() {
        auto time_begin = std::chrono::steady_clock::now();

        while (true) {
            MoveNode *currentNode = root;

            while (currentNode->childs.begin()->second != nullptr) {
                currentNode = currentNode->childs.begin()->second;
            }

            std::pair<std::pair<double, std::array<int8_t, 2>>, MoveNode *> achild;

            bool can_move = 0;

            while (!currentNode->childs.empty()) {
                achild = *currentNode->childs.begin();
                currentNode->childs.erase(currentNode->childs.begin());

                achild.second = new MoveNode; // this is the newly created node
                achild.second->parent = currentNode;
                achild.second->last = currentNode->last;
                achild.second->current = currentNode->current;

                if (SimulateMove(achild.second->current, achild.second->last, achild.second->current.turn, achild.first.second[0], achild.first.second[1])) {
                    achild.second->resetChilds();

                    currentNode->childs.insert(achild);

                    CONSOLE_LOG("simulating\n");
                    double score = simulateRandomPlay(*(achild.second));
                    CONSOLE_LOG("simulating done\n");

                    score = normlizeScore(score);

                    achild.second->w = score;
                    achild.second->n = 1;
                    
                    while (currentNode != nullptr) {
                        currentNode->n += 1;
                        currentNode->w += score;

                        achild = *currentNode->childs.begin(); currentNode->childs.erase(currentNode->childs.begin());

                        if (currentNode->current.turn == side) {
                            achild.first.first = (achild.second->w / achild.second->n) + MCTSHEAT * sqrt(ln(currentNode->n)/achild.second->n); // calc
                        } else {
                            achild.first.first = -(achild.second->w / achild.second->n) + MCTSHEAT * sqrt(ln(currentNode->n)/achild.second->n); // calc
                        }

                        currentNode->childs.insert(achild);

                        currentNode = currentNode->parent;
                    }

                    can_move = 1;

                    break;
                } else {
                    delete achild.second;
                }
            }

            if (!can_move) {
                double score = simulateRandomPlay(*currentNode);

                score = normlizeScore(score);

                currentNode->w += score;
                currentNode->n += 1;

                currentNode = currentNode->parent;
                
                while (currentNode != nullptr) {
                    currentNode->n += 1;
                    currentNode->w += score;

                    achild = *currentNode->childs.begin(); currentNode->childs.erase(currentNode->childs.begin());

                    if (currentNode->current.turn == side) {
                        achild.first.first = (achild.second->w / achild.second->n) + MCTSHEAT * sqrt(ln(currentNode->n)/achild.second->n); // calc
                    } else {
                        achild.first.first = -(achild.second->w / achild.second->n) + MCTSHEAT * sqrt(ln(currentNode->n)/achild.second->n); // calc
                    }

                    currentNode->childs.insert(achild);

                    currentNode = currentNode->parent;
                }
            }

            auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_begin).count();

            CONSOLE_LOG("time elapsed: %d\n", (int) (time_elapsed - max_thinking_time));

            if (time_elapsed > max_thinking_time) break;
        }

        // calc done, give move
        std::pair<std::pair<double, std::array<int8_t, 2>>, MoveNode *> candidate_child = *(--(root->childs.end()));
        for (std::pair<std::pair<double, std::array<int8_t, 2>>, MoveNode *> achild : root->childs) {
            if (achild.second != nullptr) if (achild.second->n > candidate_child.second->n) {
                candidate_child = achild;
            }
        }

        placeStone(candidate_child.first.second[0], candidate_child.first.second[1], side); // move in engine
        root = candidate_child.second; // move in AI
        root->parent = nullptr;
        CONSOLE_LOG("MOVED : %d %d\n", candidate_child.first.second[0], candidate_child.first.second[1]);
    }
    void opponentMove(int8_t x, int8_t y) {
        for (std::pair<std::pair<double, std::array<int8_t, 2>>, MoveNode *> achild : root->childs) {
            if (achild.first.second[0] == x && achild.first.second[1] == y) {
                if (achild.second == nullptr) {
                    achild.second = new MoveNode;
                    achild.second->current = root->current;
                    achild.second->last = root->last;
                    achild.second->resetChilds();
                }
                SimulateMove(achild.second->current, achild.second->last, achild.second->current.turn, achild.first.second[0], achild.first.second[1], nullptr, true);
                root = achild.second;
                root->parent = nullptr;
            }
        }
    }
};