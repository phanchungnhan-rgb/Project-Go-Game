#include "engine.hpp"

Game current_game, last_game;

bool checkAndClear(Game &newgame, bool side) {
    bool deleted = false;
    for (int i = 1; i < BOARDSIZE + 1; i++) {
        for (int j = 1; j < BOARDSIZE + 1; j++) {

            if (newgame.current[i][j] == side + 1) {
                std::queue<std::pair<int8_t, int8_t>> que;
                que.push({i, j});
                newgame.current[i][j] = 67;

                bool cleared = false;

                std::vector<std::pair<int8_t, int8_t>> cluster;

                while (que.size()) {
                    int x = que.front().first;
                    int y = que.front().second;
                    que.pop();

                    cluster.push_back({x, y});

                    for (int _ = 0; _ < 4; _++) {
                        int adjx = x + ADJCORD_X[_];
                        int adjy = y + ADJCORD_Y[_];

                        if (newgame.current[adjx][adjy] == 0) {
                            cleared = true;
                        }

                        if (newgame.current[adjx][adjy] == side + 1) {
                            que.push({adjx, adjy});
                            newgame.current[adjx][adjy] = 67;
                        }
                    }
                }

                if (!cleared) {
                    for (std::pair<int8_t, int8_t> &point : cluster) {
                        newgame.current[point.first][point.second] = 0;
                    }
                    deleted = true;
                } else {
                    for (std::pair<int8_t, int8_t> &point : cluster) {
                        newgame.current[point.first][point.second] = side + 1;
                    }
                }
            }
        }
    }
    return deleted;
}

bool placeStone(int x, int y, bool side) {
    if (current_game.current[x][y]) return 0;

    Game newgame;
    newgame = current_game;

    newgame.current[x][y] = 1 + side;

    checkAndClear(newgame, !side);

    if (checkAndClear(newgame, side)) {
        return false;
    } else {
        current_game = newgame;
        return true;
    }
    return false;
}  






void printGame(Game c) {
    for (int i = 1; i < BOARDSIZE + 1; i++) {
        for (int j = 1; j < BOARDSIZE + 1; j++) {
            printf("%d ", c.current[i][j]);
        }
        printf("\n");
    }
}

void printCurrentGame() {
    printGame(current_game);
}