#include "AI.hpp"

double normlizeScore(double score) {
    return tanh(score / 2);
}

// use for function SimulateMove() only, cuz all pieces of connected region are marked 67
int eyesCounter(Game &game, std::vector<std::pair<int8_t, int8_t>> &group) {
    bool deleted = false;
    int cnt = 0;
    std::vector<std::pair<int8_t, int8_t>> clearing;
    for (std::pair<int8_t, int8_t> &point : group) {
        for (int _ = 0; _ < 4; _++) {
            int adjx = point.first + ADJCORD_X[_];
            int adjy = point.second + ADJCORD_Y[_];

            if (game.current[adjx][adjy] == 0) {
                bool legit = 1;

                std::queue<std::pair<int8_t, int8_t>> que;
                que.push({adjx, adjy});
                game.current[adjx][adjy] = 45;

                while (que.size()) {
                    int x = que.front().first;
                    int y = que.front().second;
                    que.pop();

                    clearing.push_back({x, y});

                    for (int _ = 0; _ < 4; _++) {
                        int adjx = x + ADJCORD_X[_];
                        int adjy = y + ADJCORD_Y[_];

                        if (game.current[adjx][adjy] == 0) {
                            que.push({adjx, adjy});
                            game.current[adjx][adjy] = 45;
                        } else if (game.current[adjx][adjy] != 67 && game.current[adjx][adjy] != 127) {
                            legit = 0;
                        }
                    }
                }
                cnt += legit;
            }
        }
    }
    for (std::pair<int8_t, int8_t> &point : clearing) {
        game.current[point.first][point.second] = 0;
    }
    return cnt;
}

// simulate a move for AI, aside from rule breaking moves, moves that are suicidal are negative, while big capturing moves are large positive
bool SimulateMove(Game &game, Game &previousgame, bool side, int x, int y, std::vector<std::pair<int8_t, int8_t>> *clearptr, bool allow_eye_filling) {
    if (game.current[x][y]) return false;

    Game newgame = game;
    newgame.turn = !side;

    newgame.current[x][y] = 1 + side;

    checkAndClear(newgame, !side, x, y, clearptr);

    if (checkAndClear(newgame, side, x, y)) {
        return false;
    } else {
        if (cmpGameArray(newgame.current, previousgame.current)) {
            return false;
        }

        std::queue<std::pair<int8_t, int8_t>> que;
        que.push({x, y});
        newgame.current[x][y] = 67;

        std::vector<std::pair<int8_t, int8_t>> cluster;

        int holecnt = 0;

        while (que.size()) {
            int x = que.front().first;
            int y = que.front().second;
            que.pop();

            cluster.push_back({x, y});

            for (int _ = 0; _ < 4; _++) {
                int adjx = x + ADJCORD_X[_];
                int adjy = y + ADJCORD_Y[_];

                if (newgame.current[adjx][adjy] == side + 1) {
                    que.push({adjx, adjy});
                    newgame.current[adjx][adjy] = 67;
                }
            }
        }

        if (!allow_eye_filling) holecnt = eyesCounter(newgame, cluster);

        for (std::pair<int8_t, int8_t> &point : cluster) {
            newgame.current[point.first][point.second] = side + 1;
        }

        if (holecnt == 1) {
            for (int _ = 0; _ < 4; _++) {
                int adjx = x + ADJCORD_X[_];
                int adjy = y + ADJCORD_Y[_];

                if (newgame.current[adjx][adjy] == 0 || newgame.current[adjx][adjy] == (!side) + 1) {
                } else {
                    return 0;
                }
            }
        }
        previousgame = game;
        game = newgame;
        return 1;
    }
    return false;
}
bool simulatePass(Game &game, Game &previousgame, bool side) {
    Game newgame;
    newgame = game;
    newgame.turn = !side;

    previousgame = game;
    game = newgame;
    return 1;
}


// point for not black (0);
double roughScoring(Game &game) {
    double score = 0;
    for (int i = 1; i <= BOARDSIZE; i++) {
        for (int j = 1; j <= BOARDSIZE; j++) {
            if (game.current[i][j] == 1) {
                score += 1;
            } else if (game.current[i][j] == 2) {
                score -= 1;
            } else if (game.current[i][j] == 0) {
                int b = 0, w = 0;
                for (int _ = 0; _ < 4; _++) {
                    int adjx = i + ADJCORD_X[_];
                    int adjy = j + ADJCORD_Y[_];

                    if (game.current[adjx][adjy] == 0 || game.current[adjx][adjy] == 127) continue;
                    if (game.current[adjx][adjy] == 1) b++;
                    else if (game.current[adjx][adjy] == 2) w++;
                }
                if (b > 0 && w == 0) score += 1;
                else if (b == 0 && w > 0) score -= 1;
            }
        }
    }
    return -score;
}

double simulateRandomPlay(MoveNode node) {
    bool passedlasttime = 0;
    
    std::deque<std::pair<int8_t, int8_t>> moves;
    for (int i = 1; i <= BOARDSIZE; i++) {
        for (int j = 1; j <= BOARDSIZE; j++) {
            if (node.current.current[i][j] == 0) moves.push_back({i, j});
        }
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(moves.begin(), moves.end(), std::default_random_engine(seed));

    std::vector<std::pair<int8_t, int8_t>> newintersections;
    
    for (int plies = 0; plies < 4 * BOARDSIZE * BOARDSIZE; plies++) {
        bool moved = false;
        int i;
        for (i = 0; i < moves.size(); i++) {
            newintersections.clear();
            if (SimulateMove(node.current, node.last, node.current.turn, moves.front().first, moves.front().second, &newintersections)) {
                moved = 1;
                for (std::pair<int8_t, int8_t> &intersection : newintersections) moves.push_back(intersection);
                moves.pop_front();
                break;
            }
            moves.push_back(moves.front());
            moves.pop_front();
        }

        if (!moved) {
            if (!passedlasttime) {
                passedlasttime = 1;
                simulatePass(node.current, node.last, node.current.turn);
                continue;
            }

            // printGame(node.current); // delete this pls
        

            return roughScoring(node.current);
        } else {
            passedlasttime = 0;
        }
    }

    return 0;
}