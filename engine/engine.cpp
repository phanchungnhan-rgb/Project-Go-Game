#include "engine.hpp"

bool INVALID_MOVE() {
    CONSOLE_LOG("INVALID MOVE\n");
    return false;
}

// Game current_game, last_game;
std::deque<Game> games(2);
std::vector<Game> redo_stack;

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
    if (games.back().current[x][y]) return INVALID_MOVE();

    Game newgame;
    newgame = games.back();

    newgame.current[x][y] = 1 + side;

    checkAndClear(newgame, !side);

    if (checkAndClear(newgame, side)) {
        return INVALID_MOVE();
    } else {
        if (cmpGameArray(newgame.current, (games.end() - 2)->current)) {
            return INVALID_MOVE();
        }
        
        while (!redo_stack.empty()) redo_stack.pop_back();

        games.push_back(newgame);
        if (games.size() > UNDOSTSZ) games.pop_front();
        return true;
    }
    return INVALID_MOVE();
}  

bool undoMove() {
    if (games.size() <= 2) {
        return INVALID_MOVE();
    }
    redo_stack.push_back(games.back());
    games.pop_back();
    return true;
}

bool redoMove() {
    if (redo_stack.empty()) {
        return INVALID_MOVE();
    }
    games.push_back(redo_stack.back());
    redo_stack.pop_back();
    if (games.size() > UNDOSTSZ) games.pop_front();
    return true;
}

bool saveGame(char *file_name) {
    char file_path[50] = "./saves/";

    strcat(file_path, file_name);
    FILE *save_file = fopen(file_path, "w");

    if (save_file == NULL) {
        CONSOLE_LOG("open file to save failed\n");
        return 0;
    }

    for (Game &game : games) {
        fprintf(save_file, "%d\n", game.turn);

        for (int i = 1; i < BOARDSIZE; i++) {
            for (int j = 1; j < BOARDSIZE; j++) {
                fprintf(save_file, "%d ", game.current[i][j]);
            }
            fprintf(save_file, "\n");
        }
        fprintf(save_file, "\n");
    }

    fprintf(save_file, "-44\n\n");

    for (Game &game : redo_stack) {
        fprintf(save_file, "%d\n", game.turn);

        for (int i = 1; i < BOARDSIZE; i++) {
            for (int j = 1; j < BOARDSIZE; j++) {
                fprintf(save_file, "%d ", game.current[i][j]);
            }
            fprintf(save_file, "\n");
        }
        fprintf(save_file, "\n");
    }
    
    fprintf(save_file, "-44\n\n");

    fclose(save_file);

    CONSOLE_LOG("SAVED\n");

    return true;
}




bool loadGame(char *file_name) {
    char file_path[50] = "./saves/";

    strcat(file_path, file_name);
    FILE *save_file = fopen(file_path, "r");

    if (save_file == NULL) {
        CONSOLE_LOG("open file to load failed\n");
        return 0;
    }

    games.clear();
    while (true) {
        Game game;
        int x;
        fscanf(save_file, "%d", &x);
        game.turn = x;
        if (x == -44) break;

        CONSOLE_LOG("this is %d\n", game.turn);

        for (int i = 1; i < BOARDSIZE; i++) {
            for (int j = 1; j < BOARDSIZE; j++) {
                
                fscanf(save_file, "%d ", &game.current[i][j]);
            }
        }
        games.push_back(game);
    }

    redo_stack.clear();
    while (true) {
        Game game;
        int x;
        fscanf(save_file, "%d", &x);
        game.turn = x;
        if (x == -44) break;

        for (int i = 1; i < BOARDSIZE; i++) {
            for (int j = 1; j < BOARDSIZE; j++) {
                
                fscanf(save_file, "%d ", &game.current[i][j]);
            }
        }
        redo_stack.push_back(game);
    }

    fclose(save_file);

    CONSOLE_LOG("LOADED\n");

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
    printGame(games.back());
}