#include "engine.hpp"

bool INVALID_MOVE() {
    CONSOLE_LOG("INVALID MOVE\n");
    return false;
}

// Game current_game, last_game;
std::vector<Game> games(2);
int current_game_index = 1;

bool cmpGameArray(int8_t next[][BOARDSIZE + 2], int8_t previous[][BOARDSIZE + 2]) {
    for (int i = 1; i < BOARDSIZE + 1; i++) {
        for (int j = 1; j < BOARDSIZE + 1; j++) {
            if (next[i][j] != previous[i][j]) {
                return false;
            }
        }
    }
    return true;
}

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

                        if (newgame.current[adjx][adjy] <= 0) {
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
                } /* else {
                    for (std::pair<int8_t, int8_t> &point : cluster) {
                        newgame.current[point.first][point.second] = side + 1;
                    }
                } */ // wrong code, this will make the game check a cluster multiple time
            }
        }
    }
    
    for (int i = 1; i < BOARDSIZE + 1; i++) {
        for (int j = 1; j < BOARDSIZE + 1; j++) {

            if (newgame.current[i][j] == 67) {
                newgame.current[i][j] = side + 1;
            }
        }
    }

    return deleted;
}

bool placeStone(int x, int y, bool side) {
    if (games[current_game_index].current[x][y]) return INVALID_MOVE();

    Game newgame;
    newgame = games[current_game_index];

    newgame.current[x][y] = 1 + side;

    checkAndClear(newgame, !side);

    if (checkAndClear(newgame, side)) {
        return INVALID_MOVE();
    } else {
        if (cmpGameArray(newgame.current, games[current_game_index - 1].current)) {
            return INVALID_MOVE();
        }
        
        while (games.size() > current_game_index + 1) {
            games.pop_back();
        }

        current_game_index++;
        games.push_back(newgame);
        return true;
    }
    return INVALID_MOVE();
}  

bool undoMove() {
    if (current_game_index < 2) {
        return INVALID_MOVE();
    }
    current_game_index--;
    return true;
}

bool redoMove() {
    if (current_game_index + 1 >= games.size()) {
        return INVALID_MOVE();
    }
    current_game_index++;
    return true;
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
    printGame(games[current_game_index]);
}