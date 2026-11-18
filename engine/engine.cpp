#include "engine.hpp"

bool INVALID_MOVE() {
    CONSOLE_LOG("INVALID MOVE\n");
    return false;
}

bool Game::isValidMove(go::Cord2i position, bool side) {
    if (position.x < 1 || position.x > BOARDSIZE || position.y < 1 || position.y > BOARDSIZE) return false;
    if (currentBoard[position.x][position.y] != 0) return false;
    int theirsideminlib = 67;
    int oursidetotallib = 0;
    go::Cord2i sus = {0, 0};
    for (int _ = 0; _ < 4; _++) {
        go::Cord2i adj = {position.x + ADJCORD_X[_], position.y + ADJCORD_Y[_]};
        if (currentBoard[adj.x][adj.y] == 127) continue;
        go::Cord2i adjroot = root(adj);
        if (currentBoard[adj.x][adj.y] != 0) {
            if (currentBoard[adj.x][adj.y] == side + 1) {
                oursidetotallib += rootLiberties[adjroot.x][adjroot.y].size() - 1;
            } else {
                theirsideminlib = std::min(
                    theirsideminlib,
                    rootLiberties[adjroot.x][adjroot.y].size()
                );
                if (
                    rootLiberties[adjroot.x][adjroot.y].size() == 1 &&
                    rootChilds[adjroot.x][adjroot.y].size() == 1 &&
                    sus.x == 0 && sus.y == 0
                ) {
                    sus = adjroot;
                } else if (rootLiberties[adjroot.x][adjroot.y].size() == 1) {
                    sus = {-1, -1};
                }
            }
        } else {
            oursidetotallib++;
        }
    }
    if (theirsideminlib > 1 && oursidetotallib == 0) return false;
    if (
        sus.x > 0 && sus.y > 0 &&
        previous_moves.back().placed.size() == 1 && previous_moves.back().removed.size() == 1 &&
        previous_moves.back().placed.back() == sus && previous_moves.back().removed.back() == position
    ) return false;
    return true;
}

bool Game::placeStone(go::Cord2i position, bool side) {
    if (!isValidMove(position, side)) return INVALID_MOVE();

    future_moves.clear();

    currentBoard[position.x][position.y] = side + 1;

    Move thismove(position, side);
    thismove.placed.push_back(position);

    for (int _ = 0; _ < 4; _++) {
        go::Cord2i adj = {position.x + ADJCORD_X[_], position.y + ADJCORD_Y[_]};
        if (currentBoard[adj.x][adj.y] == 127) continue;
        go::Cord2i adjroot = root(adj);
        if (currentBoard[adj.x][adj.y] != 0) {
            rootLiberties[adjroot.x][adjroot.y].erase(position);
            if (currentBoard[adj.x][adj.y] != side + 1 && rootLiberties[adjroot.x][adjroot.y].size() == 0) {
                removeDSU(adj, &(thismove.removed));
            }
        } else {
            rootLiberties[position.x][position.y].insert(adj);
        }
    }
    for (int _ = 0; _ < 4; _++) {
        go::Cord2i adj = {position.x + ADJCORD_X[_], position.y + ADJCORD_Y[_]};
        if (currentBoard[adj.x][adj.y] == 127) continue;
        if (currentBoard[adj.x][adj.y] == side + 1) {
            mergeDSU(position, adj);
        }
    }
    
    // record move
    previous_moves.push_back(thismove);
    if (previous_moves.size() > UNDOSTSZ) previous_moves.pop_front();

    return true;
}

bool Game::passTurn(bool side) {
    Move thismove({0, 0}, side);
    
    future_moves.clear();

    // record move
    previous_moves.push_back(thismove);
    if (previous_moves.size() > UNDOSTSZ) previous_moves.pop_front();

    return true;
}

bool Game::undoMove(int moves) {
    while (moves-- && previous_moves.size() > 1) {
        int side = previous_moves.back().owner;
        for (go::Cord2i pos : previous_moves.back().placed) {
            currentBoard[pos.x][pos.y] = 0;
        }
        for (go::Cord2i pos : previous_moves.back().removed) {
            currentBoard[pos.x][pos.y] = (!side) + 1;
        }
        future_moves.push_back(previous_moves.back());
        previous_moves.pop_back();
    }
    recalculateDSU();
    return true;
} 

bool Game::redoMove(int moves) {
    while (moves-- && !future_moves.empty()) {
        int side = future_moves.back().owner;
        for (go::Cord2i pos : future_moves.back().placed) {
            currentBoard[pos.x][pos.y] = side + 1;
        }
        for (go::Cord2i pos : future_moves.back().removed) {
            currentBoard[pos.x][pos.y] = 0;
        }
        previous_moves.push_back(future_moves.back());
        future_moves.pop_back();
    }
    recalculateDSU();
    return true;
}

bool Game::save(char *file_name, int gamemode_flag = 0) {    
    char file_path[50] = "./saves/";

    strcat(file_path, file_name);
    FILE *save_file = fopen(file_path, "w");

    if (save_file == NULL) {
        CONSOLE_LOG("open file to save failed\n");
        return 0;
    }

    // gamemode flag
    fprintf(save_file, "%d\n", gamemode_flag);

    // player turn
    fprintf(save_file, "%d\n", turn);

    // board
    for (int i = 1; i < BOARDSIZE + 1; i++) {
        for (int j = 1; j < BOARDSIZE + 1; j++) {
            fprintf(save_file, "%d ", currentBoard[i][j]);
        }
        fprintf(save_file, "\n");
    }
    fclose(save_file);

    CONSOLE_LOG("SAVED\n");

    return true;
}

bool Game::loadGame(char *file_name, int *gamemode_flag = nullptr) {
    char file_path[50] = "./saves/";

    strcat(file_path, file_name);
    FILE *save_file = fopen(file_path, "r");

    if (save_file == NULL) {
        CONSOLE_LOG("open file to load failed\n");
        return 0;
    }

    // gamemode flag
    int gmf;
    fscanf(save_file, "%d", &gmf);
    if (gamemode_flag != nullptr) *gamemode_flag = gmf;
    
    // player turn
    fscanf(save_file, "%d", &turn);

    // board
    for (int i = 1; i < BOARDSIZE + 1; i++) {
        for (int j = 1; j < BOARDSIZE + 1; j++) {
            fscanf(save_file, "%d ", &currentBoard[i][j]);
        }
    }
    fclose(save_file);

    Move defaultMove({0, 0}, 1);
    previous_moves.clear();
    previous_moves.push_back(defaultMove);
    recalculateDSU();

    CONSOLE_LOG("LOADED\n");

    return true;
}