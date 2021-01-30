// Copyright 2021 kkozlov

#include <iostream>

#include "Board.h"
#include "Solver.h"

static int _Usage(const char *progname) {
  std::cout << "Usage: " << progname << " filename\n";
  return 1;
}

static void _TestBracketsOperator(const Board& board) {
  std::size_t sz = board.dimension();
  for (int i = 0; i < sz; ++i) {
    for (int j = 0; j < sz; ++j) {
      std::cout << board[i][j] << ",";
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}

static void _TestCopyConstructor(const Board& board) {
  std::cout << board.hamming() << std::endl;
  Board copy_board = board;
  std::cout << copy_board.hamming() << std::endl;
  std::cout << copy_board << std::endl;
}

static void _TestMovements(const Board& board) {
  typedef std::vector<Board>::const_iterator ConstIterator;
  std::vector<Board> neighbours = board.neighbours();
  for (ConstIterator it = neighbours.cbegin(); it != neighbours.end(); ++it) {
    std::cout << *it << std::endl;
  }
}

static void _SolutionPrint(const std::vector<Board>& solution) {
  for (int i = 0; i < solution.size(); ++i) {
    std::cout << "moves: " << i << std::endl;
    std::cout << solution[i] << std::endl;
  }
}

#include <unistd.h> // getpid()

int main(int argc, char **argv) {
  if (2 != argc) {
    return _Usage(argv[0]);
  }
  Board board;
  if (0 == board.parse_file(argv[1])) {
    Solver solver(board);
    if (solver.is_solvable()) {
      std::vector<Board> solution = solver.solution();
      _SolutionPrint(solution);
      /*      
      char buf[128];
      bzero(buf, sizeof(buf));
      pid_t pid = getpid();
      snprintf(buf, sizeof(buf), "leaks %d", pid);
      printf("buf: %s\n", buf);
      system(buf);*/
      
    } else {
      std::cout << "Error:\nthe puzzle cannot be solved" << std::endl;
    }
  } else {
    std::cerr << "Error:\ncould not parse file " << argv[1] << std::endl;
  }  
  return 0;
}
