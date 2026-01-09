import math
import random
from abc import ABC, abstractmethod


class Board:
    EMPTY = ' '

    def __init__(self, size: int):
        if size < 3 or size > 6:
            raise ValueError("Board size must be between 3 and 6")
        self.size = size
        self.cells = [self.EMPTY] * (size * size)
        self.win_lines = self._generate_win_lines()

    def _generate_win_lines(self) -> list:
        lines = []
        n = self.size
        # Rows
        for r in range(n):
            lines.append([r * n + c for c in range(n)])
        # Columns
        for c in range(n):
            lines.append([r * n + c for r in range(n)])
        # Diagonals
        lines.append([i * n + i for i in range(n)])
        lines.append([i * n + (n - 1 - i) for i in range(n)])
        return lines

    def get(self, index: int) -> str:
        return self.cells[index]

    def set(self, index: int, symbol: str):
        self.cells[index] = symbol

    def is_empty(self, index: int) -> bool:
        return self.cells[index] == self.EMPTY

    def is_full(self) -> bool:
        return self.EMPTY not in self.cells

    def get_empty_cells(self) -> list:
        return [i for i in range(len(self.cells)) if self.cells[i] == self.EMPTY]

    def check_winner(self) -> str | None:
        for line in self.win_lines:
            values = [self.cells[i] for i in line]
            if values[0] != self.EMPTY and all(v == values[0] for v in values):
                return values[0]
        if self.is_full():
            return 'DRAW'
        return None

    def copy(self) -> 'Board':
        new_board = Board(self.size)
        new_board.cells = self.cells.copy()
        return new_board

    def display(self):
        n = self.size
        separator = "-" * (n * 4 + 1)
        print(f"\n{separator}")
        for r in range(n):
            row = []
            for c in range(n):
                idx = r * n + c
                cell = self.cells[idx] if self.cells[idx] != self.EMPTY else f"{idx:02}"
                row.append(cell)
            print("| " + " | ".join(row) + " |")
            print(separator)


class Player(ABC):
    def __init__(self, symbol: str):
        self.symbol = symbol

    @abstractmethod
    def get_move(self, board: Board) -> int:
        pass


class HumanPlayer(Player):
    def get_move(self, board: Board) -> int:
        max_index = board.size * board.size - 1
        while True:
            try:
                move = int(input(f"Enter your move (0-{max_index}): "))
                if move < 0 or move > max_index:
                    print(f"Please enter a number between 0 and {max_index}.")
                    continue
                if not board.is_empty(move):
                    print("Invalid move! Spot taken.")
                    continue
                return move
            except ValueError:
                print(f"Please enter a valid number (0-{max_index}).")


class AIEngine:
    WIN_SCORE = 1000000
    LOSS_SCORE = -1000000

    def __init__(self, ai_symbol: str, human_symbol: str, max_depth: int):
        self.ai_symbol = ai_symbol
        self.human_symbol = human_symbol
        self.max_depth = max_depth

    def evaluate_board(self, board: Board) -> int:
        score = 0
        n = board.size

        for line in board.win_lines:
            line_vals = [board.get(i) for i in line]
            my_count = line_vals.count(self.ai_symbol)
            opp_count = line_vals.count(self.human_symbol)

            if my_count > 0 and opp_count > 0:
                continue

            if my_count > 0:
                if my_count == n:
                    score += self.WIN_SCORE
                elif my_count == n - 1:
                    score += 50000
                elif my_count == n - 2:
                    score += 1000
                elif my_count >= 2:
                    score += 10

            if opp_count > 0:
                if opp_count == n:
                    score -= self.WIN_SCORE
                elif opp_count == n - 1:
                    score -= 55000
                elif opp_count == n - 2:
                    score -= 2000
                elif opp_count >= 2:
                    score -= 20

        return score

    def minimax(self, board: Board, depth: int, alpha: float, beta: float, is_maximizing: bool) -> int:
        winner = board.check_winner()
        if winner == self.ai_symbol:
            return self.WIN_SCORE
        if winner == self.human_symbol:
            return self.LOSS_SCORE
        if winner == 'DRAW':
            return 0

        if depth == 0:
            return self.evaluate_board(board)

        empty_cells = board.get_empty_cells()

        if is_maximizing:
            max_eval = -math.inf
            for i in empty_cells:
                board.set(i, self.ai_symbol)
                eval_score = self.minimax(board, depth - 1, alpha, beta, False)
                board.set(i, Board.EMPTY)
                max_eval = max(max_eval, eval_score)
                alpha = max(alpha, eval_score)
                if beta <= alpha:
                    break
            return max_eval
        else:
            min_eval = math.inf
            for i in empty_cells:
                board.set(i, self.human_symbol)
                eval_score = self.minimax(board, depth - 1, alpha, beta, True)
                board.set(i, Board.EMPTY)
                min_eval = min(min_eval, eval_score)
                beta = min(beta, eval_score)
                if beta <= alpha:
                    break
            return min_eval

    def get_best_move(self, board: Board) -> int:
        best_score = -math.inf
        best_move = -1
        for i in board.get_empty_cells():
            board.set(i, self.ai_symbol)
            score = self.minimax(board, self.max_depth, -math.inf, math.inf, False)
            board.set(i, Board.EMPTY)
            if score > best_score:
                best_score = score
                best_move = i
        return best_move


class AIPlayer(Player):
    def __init__(self, symbol: str, human_symbol: str, board_size: int):
        super().__init__(symbol)
        # Adjust depth based on board size for performance
        # 3x3 has no depth limit (full search), larger boards use limited depth
        depth_map = {3: float('inf'), 4: 5, 5: 4, 6: 3}
        max_depth = depth_map.get(board_size, 4)
        self.engine = AIEngine(symbol, human_symbol, max_depth)

    def get_move(self, board: Board) -> int:
        print("AI is thinking...")
        return self.engine.get_best_move(board)


class Game:
    def __init__(self, board_size: int):
        self.board = Board(board_size)
        self.human = HumanPlayer('O')
        self.ai = AIPlayer('X', 'O', board_size)

    def play(self):
        print(f"Welcome to {self.board.size}x{self.board.size} Tic-Tac-Toe!")
        print(f"You are '{self.human.symbol}', AI is '{self.ai.symbol}'")
        self.board.display()

        # Randomly choose who starts first
        current_player = random.choice([self.human, self.ai])
        if current_player == self.human:
            print("\n🎲 You go first!")
        else:
            print("\n🎲 AI goes first!")

        while True:
            if self.board.is_full():
                result = self.board.check_winner()
                if result == self.ai.symbol:
                    print("AI Wins!")
                elif result == self.human.symbol:
                    print("You Win!")
                else:
                    print("It's a Draw! Game Over.")
                break

            move = current_player.get_move(self.board)
            self.board.set(move, current_player.symbol)
            self.board.display()

            winner = self.board.check_winner()
            if winner:
                if winner == self.human.symbol:
                    print("You Win!")
                elif winner == self.ai.symbol:
                    print("AI Wins!")
                else:
                    print("It's a Draw!")
                break

            # Switch player
            current_player = self.ai if current_player == self.human else self.human


def main():
    print("=" * 40)
    print("      TIC-TAC-TOE")
    print("=" * 40)

    while True:
        try:
            size = int(input("Choose board size (3-6): "))
            if 3 <= size <= 6:
                break
            print("Please enter a number between 3 and 6.")
        except ValueError:
            print("Please enter a valid number.")

    game = Game(size)
    game.play()


if __name__ == "__main__":
    main()