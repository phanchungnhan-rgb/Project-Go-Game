#include "MCTSRandomAI.hpp"

double normlizeScore(double score) {
    if (score < -1) return -1;
    if (score > 1) return 1;
    return score;
    return tanh(score / 2);
}

const double reward_capture = 0.3;
const double reward_eyefill = -0.3;
const double reward_selfatari = -0.2;
const double reward_atari_esc = 0.3;
const double reward_move_nextto_lastmove = 0.1;

// simulate a move for AI, aside from rule breaking moves, moves that are suicidal are negative, while big capturing moves are large positive
double moveWithScore(Game &gameSession, go::Cord2i position, bool side) {
    if (!gameSession.isValidMove(position, side)) return INVALID_SIM; // I think I can just delete this for extra performance

    double score = 0;

    // reward move that are close to opponent last move
    if (abs(gameSession.previous_moves.back().position.x - position.x) < 2 && abs(gameSession.previous_moves.back().position.y - position.y) < 2) score += reward_move_nextto_lastmove; 

    double atari_count = 0;

    for (int _ = 0; _ < 4; _++) {
        go::Cord2i adj = {position.x + ADJCORD_X[_], position.y + ADJCORD_Y[_]};
        if (gameSession.currentBoard[adj.x][adj.y] == 127) continue;
        go::Cord2i adjroot = gameSession.root(adj);

        if (gameSession.currentBoard[adj.x][adj.y] == side + 1 && gameSession.rootLiberties[adjroot.x][adjroot.y].size() == 1) {
            atari_count += gameSession.rootChilds[adjroot.x][adjroot.y].size();
        }        
    }

    if (!gameSession.placeStone(position, side)) return INVALID_SIM;

    go::Cord2i rootpos = gameSession.root(position);
    if (gameSession.rootLiberties[rootpos.x][rootpos.y].size() <= 1) {
        // punish for self atari
        score += reward_selfatari * gameSession.rootChilds[rootpos.x][rootpos.y].size();
    } else {
        // reward escaping atari
        score += reward_atari_esc * atari_count;
    }

    score += reward_capture * gameSession.previous_moves.back().removed.size(); // reward capturing

    return normlizeScore(score);
    return INVALID_SIM;
}

// point for white (1);
double roughScoring(Game &gameSession) {
    double score = 0;
    for (int i = 1; i <= BOARDSIZE; i++) {
        for (int j = 1; j <= BOARDSIZE; j++) {
            if (gameSession.currentBoard[i][j] == 1) score++;
            else if (gameSession.currentBoard[i][j] == 2) score--;
            if (gameSession.currentBoard[i][j] != 0) continue;

            bool neigh[2] = {};
            int count = 0;

            std::stack<go::Cord2i> stk;
            stk.push({i, j});
            gameSession.currentBoard[i][j] = 3;

            while (!stk.empty()) {
                go::Cord2i currentPosition = stk.top(); stk.pop();

                count++;

                for (int _ = 0; _ < 4; _++) {
                    go::Cord2i adj = {currentPosition.x + ADJCORD_X[_], currentPosition.y + ADJCORD_Y[_]};
                    if (gameSession.currentBoard[adj.x][adj.y] > 2) continue;

                    if (gameSession.currentBoard[adj.x][adj.y] == 0) {
                        gameSession.currentBoard[adj.x][adj.y] = 3;
                        stk.push(adj);
                    } else {
                        neigh[gameSession.currentBoard[adj.x][adj.y] - 1] = 1;
                    }
                }
            }
            if (neigh[0] && !neigh[1]) {
                score += count;
            } else if (!neigh[0] && neigh[1]) {
                score -= count;
            }
        }
    }
    for (int i = 1; i <= BOARDSIZE; i++) {
        for (int j = 1; j <= BOARDSIZE; j++) {
            if (gameSession.currentBoard[i][j] == 3) gameSession.currentBoard[i][j] = 0;
        }
    }
    return -score;
}

double simulateRandomPlay(Game &gameSession, std::mt19937 &rng, int extramoves) {
    double score = 0;
    
    std::deque<go::Cord2i> moves[2];
    for (int i = 1; i <= BOARDSIZE; i++) {
        for (int j = 1; j <= BOARDSIZE; j++) {
            if (gameSession.currentBoard[i][j] != 0) continue;
            // bool is_valid_move = 0; // only place moves next to some stone and 
            // if ((i % 5 == 0) && (j % 5 == 0)) is_valid_move = 1;
            // for (int _ = 0; _ < 8; _++) {
            //     int x = i + PROCORD_X[_], y = j + PROCORD_Y[_];
            //     if (gameSession.currentBoard[x][y] == 1 || gameSession.currentBoard[x][y] == 2) is_valid_move = 1;
            // } 
            // if (!is_valid_move) continue; 
            moves[0].push_back({i, j}); 
            moves[1].push_back({i, j});
        }
    }
    shuffle(moves[0].begin(), moves[0].end(), rng);
    shuffle(moves[1].begin(), moves[1].end(), rng);

    bool passed = false;;
    bool moved;
    
    // tune the maximum for plies
    int moveMade = 0;
    int plies;
    for (plies = 0; plies < 0.08 * BOARDSIZE * BOARDSIZE; plies++) {
        moved = false;
        int owner = !gameSession.previous_moves.back().owner;
        int cyclecnt = 0;
        while (moves[owner].size()) {
            if (cyclecnt >= moves[owner].size()) break;
            int nxtcnt = 0;
            for (int _ = 0; _ < 8; _++) {
                int x = moves[owner].front().x + PROCORD_X[_], y = moves[owner].front().y + PROCORD_Y[_];
                if (gameSession.currentBoard[x][y] == 127) continue;
                if (gameSession.currentBoard[x][y] != owner + 1) nxtcnt++; 
            } 
            if (nxtcnt < 2) {
                moves[owner].push_back(moves[owner].front());
                moves[owner].pop_front();
                cyclecnt++;
                continue;
            }
            if (std::isfinite(moveWithScore(gameSession, moves[owner].front(), owner))) {
                // if can move
                moves[owner].pop_front();
                shuffle(gameSession.previous_moves.back().removed.begin(), gameSession.previous_moves.back().removed.end(), rng);
                for (go::Cord2i pos : gameSession.previous_moves.back().removed) {
                    moves[owner].push_back(pos);
                    moves[!owner].push_back(pos);
                } 
                moved = true;
                moveMade++;
                cyclecnt = 0;
                break;
            } else {
                // if cant move
                moves[owner].push_back(moves[owner].front());
                moves[owner].pop_front();
                cyclecnt++;
                continue;
            }
        }

        if (!moved) {
            if (passed) break;
            gameSession.passTurn(owner);
            moveMade++;
            passed = true;
        } else {
            passed = false;
        }
    }

    score += roughScoring(gameSession);

    gameSession.undoMove(moveMade);

    return score;
}