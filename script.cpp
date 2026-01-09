#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

class Board {
public:
    static constexpr char EMPTY = ' ';
    
private:
    int size;
    vector<char> cells;
    vector<vector<int>> winLines;

    void generateWinLines() {
        int n = size;
        // Rows
        for (int r = 0; r < n; r++) {
            vector<int> line;
            for (int c = 0; c < n; c++) {
                line.push_back(r * n + c);
            }
            winLines.push_back(line);
        }
        // Columns
        for (int c = 0; c < n; c++) {
            vector<int> line;
            for (int r = 0; r < n; r++) {
                line.push_back(r * n + c);
            }
            winLines.push_back(line);
        }
        // Diagonals
        vector<int> diag1, diag2;
        for (int i = 0; i < n; i++) {
            diag1.push_back(i * n + i);
            diag2.push_back(i * n + (n - 1 - i));
        }
        winLines.push_back(diag1);
        winLines.push_back(diag2);
    }

public:
    Board(int boardSize) : size(boardSize) {
        if (size < 3 || size > 6) {
            throw invalid_argument("Board size must be between 3 and 6");
        }
        cells.resize(size * size, EMPTY);
        generateWinLines();
    }

    int getSize() const { return size; }
    
    const vector<vector<int>>& getWinLines() const { return winLines; }

    char get(int index) const {
        return cells[index];
    }

    void set(int index, char symbol) {
        cells[index] = symbol;
    }

    bool isEmpty(int index) const {
        return cells[index] == EMPTY;
    }

    bool isFull() const {
        for (char c : cells) {
            if (c == EMPTY) return false;
        }
        return true;
    }

    vector<int> getEmptyCells() const {
        vector<int> empty;
        for (int i = 0; i < (int)cells.size(); i++) {
            if (cells[i] == EMPTY) {
                empty.push_back(i);
            }
        }
        return empty;
    }

    // Returns: 'X', 'O', 'D' for draw, or '\0' for no winner
    char checkWinner() const {
        for (const auto& line : winLines) {
            char first = cells[line[0]];
            if (first == EMPTY) continue;
            
            bool allSame = true;
            for (int idx : line) {
                if (cells[idx] != first) {
                    allSame = false;
                    break;
                }
            }
            if (allSame) return first;
        }
        if (isFull()) return 'D'; // Draw
        return '\0'; // No winner
    }

    Board copy() const {
        Board newBoard(size);
        newBoard.cells = cells;
        return newBoard;
    }

    void display() const {
        int n = size;
        string separator(n * 5 + 1, '-');
        cout << "\n" << separator << endl;
        
        for (int r = 0; r < n; r++) {
            cout << "|";
            for (int c = 0; c < n; c++) {
                int idx = r * n + c;
                if (cells[idx] != EMPTY) {
                    cout << " " << cells[idx] << "  |";
                } else {
                    cout << " " << setw(2) << setfill('0') << idx << " |";
                }
            }
            cout << endl << separator << endl;
        }
    }
};

// Abstract Player class
class Player {
protected:
    char symbol;
    
public:
    Player(char sym) : symbol(sym) {}
    virtual ~Player() = default;
    
    char getSymbol() const { return symbol; }
    virtual int getMove(Board& board) = 0;
};

// Human Player
class HumanPlayer : public Player {
public:
    HumanPlayer(char sym) : Player(sym) {}
    
    int getMove(Board& board) override {
        int maxIndex = board.getSize() * board.getSize() - 1;
        int move;
        
        while (true) {
            cout << "Enter your move (0-" << maxIndex << "): ";
            
            if (!(cin >> move)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a valid number (0-" << maxIndex << ")." << endl;
                continue;
            }
            
            if (move < 0 || move > maxIndex) {
                cout << "Please enter a number between 0 and " << maxIndex << "." << endl;
                continue;
            }
            
            if (!board.isEmpty(move)) {
                cout << "Invalid move! Spot taken." << endl;
                continue;
            }
            
            return move;
        }
    }
};

// AI Engine with Minimax
class AIEngine {
private:
    static const int WIN_SCORE = 1000000;
    static const int LOSS_SCORE = -1000000;
    
    char aiSymbol;
    char humanSymbol;
    int maxDepth;

public:
    AIEngine(char aiSym, char humanSym, int depth) 
        : aiSymbol(aiSym), humanSymbol(humanSym), maxDepth(depth) {}

    int evaluateBoard(const Board& board) const {
        int score = 0;
        int n = board.getSize();

        for (const auto& line : board.getWinLines()) {
            int myCount = 0, oppCount = 0;
            
            for (int idx : line) {
                char val = board.get(idx);
                if (val == aiSymbol) myCount++;
                else if (val == humanSymbol) oppCount++;
            }

            if (myCount > 0 && oppCount > 0) continue;

            if (myCount > 0) {
                if (myCount == n) score += WIN_SCORE;
                else if (myCount == n - 1) score += 50000;
                else if (myCount == n - 2) score += 1000;
                else if (myCount >= 2) score += 10;
            }

            if (oppCount > 0) {
                if (oppCount == n) score -= WIN_SCORE;
                else if (oppCount == n - 1) score -= 55000;
                else if (oppCount == n - 2) score -= 2000;
                else if (oppCount >= 2) score -= 20;
            }
        }
        return score;
    }

    int minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing) {
        char winner = board.checkWinner();
        if (winner == aiSymbol) return WIN_SCORE;
        if (winner == humanSymbol) return LOSS_SCORE;
        if (winner == 'D') return 0;

        if (depth == 0) return evaluateBoard(board);

        vector<int> emptyCells = board.getEmptyCells();

        if (isMaximizing) {
            int maxEval = numeric_limits<int>::min();
            for (int i : emptyCells) {
                board.set(i, aiSymbol);
                int evalScore = minimax(board, depth - 1, alpha, beta, false);
                board.set(i, Board::EMPTY);
                maxEval = max(maxEval, evalScore);
                alpha = max(alpha, evalScore);
                if (beta <= alpha) break;
            }
            return maxEval;
        } else {
            int minEval = numeric_limits<int>::max();
            for (int i : emptyCells) {
                board.set(i, humanSymbol);
                int evalScore = minimax(board, depth - 1, alpha, beta, true);
                board.set(i, Board::EMPTY);
                minEval = min(minEval, evalScore);
                beta = min(beta, evalScore);
                if (beta <= alpha) break;
            }
            return minEval;
        }
    }

    int getBestMove(Board& board) {
        int bestScore = numeric_limits<int>::min();
        int bestMove = -1;
        
        for (int i : board.getEmptyCells()) {
            board.set(i, aiSymbol);
            int score = minimax(board, maxDepth, numeric_limits<int>::min(), 
                               numeric_limits<int>::max(), false);
            board.set(i, Board::EMPTY);
            if (score > bestScore) {
                bestScore = score;
                bestMove = i;
            }
        }
        return bestMove;
    }
};

// AI Player
class AIPlayer : public Player {
private:
    AIEngine engine;

public:
    AIPlayer(char sym, char humanSym, int boardSize) 
        : Player(sym), engine(sym, humanSym, getMaxDepth(boardSize)) {}

    static int getMaxDepth(int boardSize) {
        switch (boardSize) {
            case 3: return 100; // Effectively infinite for 3x3
            case 4: return 6;
            case 5: return 5;
            case 6: return 4;
            default: return 4;
        }
    }

    int getMove(Board& board) override {
        cout << "AI is thinking..." << endl;
        return engine.getBestMove(board);
    }
};

// Game class
class Game {
private:
    Board board;
    HumanPlayer human;
    AIPlayer ai;

public:
    Game(int boardSize) 
        : board(boardSize), human('O'), ai('X', 'O', boardSize) {}

    void play() {
        cout << "Welcome to " << board.getSize() << "x" << board.getSize() 
             << " Tic-Tac-Toe!" << endl;
        cout << "You are '" << human.getSymbol() << "', AI is '" 
             << ai.getSymbol() << "'" << endl;
        board.display();

        // Randomly choose who starts first
        srand(static_cast<unsigned>(time(nullptr)));
        Player* currentPlayer = (rand() % 2 == 0) ? 
            static_cast<Player*>(&human) : static_cast<Player*>(&ai);
        
        if (currentPlayer == &human) {
            cout << "\n>> You go first!" << endl;
        } else {
            cout << "\n>> AI goes first!" << endl;
        }

        while (true) {
            if (board.isFull()) {
                char result = board.checkWinner();
                if (result == ai.getSymbol()) {
                    cout << "AI Wins!" << endl;
                } else if (result == human.getSymbol()) {
                    cout << "You Win!" << endl;
                } else {
                    cout << "It's a Draw! Game Over." << endl;
                }
                break;
            }

            int move = currentPlayer->getMove(board);
            board.set(move, currentPlayer->getSymbol());
            board.display();

            char winner = board.checkWinner();
            if (winner != '\0') {
                if (winner == human.getSymbol()) {
                    cout << "You Win!" << endl;
                } else if (winner == ai.getSymbol()) {
                    cout << "AI Wins!" << endl;
                } else {
                    cout << "It's a Draw!" << endl;
                }
                break;
            }

            // Switch player
            currentPlayer = (currentPlayer == &human) ? 
                static_cast<Player*>(&ai) : static_cast<Player*>(&human);
        }
    }
};

int main() {
    cout << string(40, '=') << endl;
    cout << "      TIC-TAC-TOE" << endl;
    cout << string(40, '=') << endl;

    int size;
    while (true) {
        cout << "Choose board size (3-6): ";
        
        if (!(cin >> size)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a valid number." << endl;
            continue;
        }
        
        if (size >= 3 && size <= 6) break;
        cout << "Please enter a number between 3 and 6." << endl;
    }

    try {
        Game game(size);
        game.play();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
