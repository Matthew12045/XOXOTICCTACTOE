# 🎮 XOXO Tic-Tac-Toe

A feature-rich Tic-Tac-Toe game with an intelligent AI opponent, available in both **Python** and **C++** implementations.

## ✨ Features

- **Variable Board Sizes**: Play on 3x3, 4x4, 5x5, or 6x6 grids
- **Smart AI Opponent**: Uses the Minimax algorithm with Alpha-Beta pruning
- **Random First Player**: The game randomly decides who goes first

## 🎯 How to Play

1. Choose a board size (3-6)
2. The game randomly selects who starts first
3. Enter the cell number (0 to n²-1) to place your mark
4. Try to get a complete row, column, or diagonal to win!

You play as **O** and the AI plays as **X**.

## 🚀 Running the Game

### Python Version

**Requirements**: Python 3.10+

```bash
python script.py
```

### C++ Version

**Compile and run**:

```bash
# Using g++
g++ -o tictactoe script.cpp -std=c++17
./tictactoe

# On Windows
g++ -o tictactoe.exe script.cpp -std=c++17
tictactoe.exe
```

## 🧠 AI Algorithm

The AI uses the **Minimax algorithm** with **Alpha-Beta pruning** for optimal decision-making:

| Board Size | Search Depth | Description |
|------------|--------------|-------------|
| 3x3 | Unlimited | Perfect play (unbeatable) |
| 4x4 | 6 levels | Very strong |
| 5x5 | 4-5 levels | Strong |
| 6x6 | 3-4 levels | Good |

The search depth of the python program is usally lower because it takes more time to run with the same depth compared to the cpp version.

### Evaluation Heuristics

- **Winning lines**: +1,000,000 points
- **Near-win (n-1 marks)**: +50,000 points
- **Blocking opponent's near-win**: Prioritized with -55,000 penalty
- **Building potential**: Points for 2+ marks in a line

## 📁 Project Structure

```
XOXOTICCTACTOE/
├── script.py      # Python implementation
├── script.cpp     # C++ implementation
└── README.md      # This file
```

## 🎲 Game Example

```
-----------------
| 00 | 01 | 02 |
-----------------
| 03 | 04 | 05 |
-----------------
| 06 | 07 | 08 |
-----------------

Enter your move (0-8): 4

-----------------
| 00 | 01 | 02 |
-----------------
| 03 | O  | 05 |
-----------------
| 06 | 07 | 08 |
-----------------
```

## 📝 License

This project is open source and available for educational purposes.

---

*Have fun playing! Can you beat the AI?* 🏆
