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
#define MCTSBIAS 367

#define INVALID_SIM std::nan("1")


double normlizeScore(double score);

double SimulateMove(Game &game, Game &previousgame, bool side, int x, int y, std::vector<std::pair<int8_t, int8_t>> *clearptr = nullptr, bool allow_eye_filling = 0, int lx = -1, int ly = -1);
bool simulatePass(Game &game, Game &previousgame, bool side);
double roughScoring(Game &game);

struct MoveNode {
    Game last;
    Game current;

    int x, y;

    MoveNode(int xx = 0, int yy = 0) {
        x = xx;
        y = yy;
    }

    std::vector<MoveNode *> unexplored_childs;

    // i was wrong, this will have to be recalculated every tree traversal
    std::vector<MoveNode *> childs; // (weight; x; y); child node

    MoveNode *parent = nullptr;

    double w = 0, n = 0;

    double bonus = 0;

    static bool cmpBonus(MoveNode *a, MoveNode *b) {
        return a->bonus < b->bonus;
    }

    double calculateWeight(bool side = 1) {
        if (parent->current.turn == side) return (w / n) + MCTSHEAT * sqrt(ln(parent->n)/n) + (MCTSBIAS * bonus / (n + 1));
        else return (-1 * (w / n)) + MCTSHEAT * sqrt(ln(parent->n)/n) + (MCTSBIAS * bonus / (n + 1));
    }

    void resetChilds() {
        unexplored_childs.clear();
        childs.clear();
        for (int8_t i = 1; i <= BOARDSIZE; i++) {
            for (int8_t j = 1; j <= BOARDSIZE; j++) {
                if (current.current[i][j] == 0) {
                    bool is_valid_move = 0; // only place moves next to some stone and 
                    if ((i % 5 == 0) && (j % 5 == 0)) is_valid_move = 1;
                    for (int _ = 0; _ < 8; _++) {
                        int x = i + PROCORD_X[_], y = j + PROCORD_Y[_];
                        if (current.current[x][y] == 1 || current.current[x][y] == 2) is_valid_move = 1;
                    } 
                    if (!is_valid_move) continue; 
                    unexplored_childs.push_back(new MoveNode(i, j));
                    unexplored_childs.back()->last = last;
                    unexplored_childs.back()->current = current;
                    unexplored_childs.back()->parent = this;
                    double move_score = SimulateMove(unexplored_childs.back()->current, unexplored_childs.back()->last, unexplored_childs.back()->current.turn, i, j, nullptr, false, x, y);
                    if (std::isfinite(move_score)) {
                        unexplored_childs.back()->bonus = move_score; // must normalize the score in SimulateMove first
                        // unexplored_childs.back().first.first = move_score;
                    } else {
                        delete unexplored_childs.back();
                        unexplored_childs.pop_back();
                    }
                }
            }
        }
        std::sort(unexplored_childs.begin(), unexplored_childs.end(), cmpBonus /*, std::greater<std::pair<std::pair<double, std::array<int8_t, 2>>, MoveNode *>>()*/);
    }
};

double simulateRandomPlay(MoveNode node);

struct MCTSBOT {
    MoveNode *root = new MoveNode(0, 0);

    int max_thinking_time;

    bool side;

    MCTSBOT(bool ourside = 1, int maxtimethink = 2200) {
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

        int simcnt = 0; // performance testing
        while (true) {
            simcnt++;
            MoveNode *currentNode = root;

            std::stack<int> idstack;

            while (currentNode->unexplored_childs.empty() && !currentNode->childs.empty()) {
                int id = 0;
                for (int i = 0; i < currentNode->childs.size(); i++) {
                    if (currentNode->childs[id]->calculateWeight(side) < currentNode->childs[i]->calculateWeight(side)) {
                        id = i;
                    }
                }
                currentNode = currentNode->childs[id];
                idstack.push(id);
            }

            MoveNode *achild;

            bool can_move = 0;

            while (!currentNode->unexplored_childs.empty()) {
                achild = currentNode->unexplored_childs.back(); currentNode->unexplored_childs.pop_back();

                if (true) {
                    achild->resetChilds();

                    // currentNode->childs.insert(achild);

                    double score = simulateRandomPlay(*(achild));

                    score = normlizeScore(score);

                    achild->w = score;
                    achild->n = 1;

                    bool isleaf = 1;

                    idstack.push(currentNode->childs.size());
                    currentNode->childs.push_back(achild);
                    
                    while (currentNode != nullptr) {
                        currentNode->n += 1;
                        currentNode->w += score;

                        MoveNode * &curchild = currentNode->childs[idstack.top()]; idstack.pop();

                        /* old code
                        if (currentNode->current.turn == side) {
                            curchild.first.first = (curchild.second->w / curchild.second->n) + MCTSHEAT * sqrt(ln(currentNode->n)/curchild.second->n) + (MCTSBIAS * curchild.second->bonus / (curchild.second->n + 1)); // calc
                        } else {
                            curchild.first.first = -(curchild.second->w / curchild.second->n) + MCTSHEAT * sqrt(ln(currentNode->n)/curchild.second->n) + (MCTSBIAS * curchild.second->bonus / (curchild.second->n + 1)); // calc
                        }
                        */

                        currentNode = currentNode->parent;
                    }

                    can_move = 1;

                    break;
                } else {
                    delete achild;
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

                    MoveNode * &curchild = currentNode->childs[idstack.top()]; idstack.pop();

                    /* old code
                    if (currentNode->current.turn == side) {
                        curchild.first.first = (curchild.second->w / curchild.second->n) + MCTSHEAT * sqrt(ln(currentNode->n)/curchild.second->n) + (MCTSBIAS * curchild.second->bonus / (curchild.second->n + 1)); // calc
                    } else {
                        curchild.first.first = -(curchild.second->w / curchild.second->n) + MCTSHEAT * sqrt(ln(currentNode->n)/curchild.second->n) + (MCTSBIAS * curchild.second->bonus / (curchild.second->n + 1)); // calc
                    }
                    */

                    currentNode = currentNode->parent;
                }
            }

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

        placeStone(candidate_child->x, candidate_child->y, side); // move in engine
        root = candidate_child; // move in AI
        root->parent = nullptr;
        CONSOLE_LOG("MOVED : %d %d\n", candidate_child->x, candidate_child->y);
    }
    void opponentMove(int8_t x, int8_t y) {
        bool moved = false;
        for (MoveNode * achild : root->unexplored_childs) {
            if (achild->x == x && achild->y == y) {
                achild->resetChilds();
                root = achild;
                root->parent = nullptr;
                moved = 1;
            }
        }
        if (moved) return;
        for (MoveNode * achild : root->childs) {
            if (achild->x == x && achild->y == y) {
                root = achild;
                root->parent = nullptr;
                moved = 1;
            }
        }
        if (moved) return;

        root->unexplored_childs.push_back(new MoveNode(x, y));
        root->unexplored_childs.back()->last = root->last;
        root->unexplored_childs.back()->current = root->current;
        root->unexplored_childs.back()->parent = root;
        double move_score = SimulateMove(root->unexplored_childs.back()->current, root->unexplored_childs.back()->last, root->unexplored_childs.back()->current.turn, x, y, nullptr, false, root->x, root->y);
        if (std::isfinite(move_score)) {
            root->unexplored_childs.back()->bonus = move_score; // must normalize the score in SimulateMove first
            // unexplored_childs.back().first.first = move_score;
        } else {
            delete root->unexplored_childs.back();
            root->unexplored_childs.pop_back();
        }
         
        root->unexplored_childs.back()->resetChilds();
        root = root->unexplored_childs.back();
        root->parent = nullptr;
        moved = 1;
    }
};