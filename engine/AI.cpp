#include "AI.hpp"

// use for function SimulateMove() only, cuz all pieces of connected region are marked 67
int eyesCounter(Game game) {
    bool deleted = false;
    int id = 2;
    int cnt = 0;
    for (int i = 1; i < BOARDSIZE + 1; i++) {
        for (int j = 1; j < BOARDSIZE + 1; j++) {

            if (game.current[i][j] == 0) {
                bool legit = 1;
                id++;

                std::queue<std::pair<int8_t, int8_t>> que;
                que.push({i, j});
                game.current[i][j] = id;

                while (que.size()) {
                    int x = que.front().first;
                    int y = que.front().second;
                    que.pop();

                    for (int _ = 0; _ < 4; _++) {
                        int adjx = x + ADJCORD_X[_];
                        int adjy = y + ADJCORD_Y[_];

                        if (game.current[adjx][adjy] == 0) {
                            que.push({adjx, adjy});
                            game.current[adjx][adjy] = id;
                        } else if (game.current[adjx][adjy] != 67 && game.current[adjx][adjy] != 127) {
                            legit = 0;
                        }
                    }
                }
                cnt += legit;
            }
        }
    }
    return cnt;
}

// simulate a move for AI, aside from rule breaking moves, moves that are suicidal are negative, while big capturing moves are large positive
bool SimulateMove(Game &game, Game &previousgame, bool side, int x, int y) {
    if (game.current[x][y]) return INVALID_MOVE();

    Game newgame;
    newgame = game;

    newgame.current[x][y] = 1 + side;

    checkAndClear(newgame, !side);

    if (checkAndClear(newgame, side)) {
        return INVALID_MOVE();
    } else {
        if (cmpGameArray(newgame.current, previousgame.current)) {
            return INVALID_MOVE();
        }

        std::queue<std::pair<int8_t, int8_t>> que;
        que.push({x, y});
        newgame.current[x][y] = 3;

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

        holecnt = eyesCounter(newgame);

        CONSOLE_LOG("holes count: %d\n", holecnt);
    }
    return INVALID_MOVE();
}