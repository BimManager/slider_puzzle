// Copyright 2021 kkozlov

#ifndef SOLVER_H_
#define SOLVER_H_

#include <cstddef>

#include <vector>
#include <queue>
#include <unordered_set>
#include <functional>
#include <utility>

#include "Board.h"

struct BoardPtrGreaterManhattan {
  bool operator()(const std::pair<const Board*, unsigned>& lhs,
                  const std::pair<const Board*, unsigned>& rhs) const {
    int lhs_priority = lhs.first->manhattan() + lhs.second;
    int rhs_priority = rhs.first->manhattan() + rhs.second;
    if (lhs_priority > rhs_priority) return true;
    else if (lhs_priority < rhs_priority) return false;
    else {
      if (lhs.first->manhattan() > rhs.first->manhattan()) return true;
      else if (lhs.first->manhattan() < rhs.first->manhattan()) return false;
      else return lhs.first->hamming() > rhs.first->hamming();
    }
  }
};

struct BoardPtrEqual {
  bool operator()(const Board* const& lhs, const Board* const& rhs) const {
    return *lhs == *rhs;
  }
};

struct BoardPtrHash {
  std::size_t operator()(const Board* const& pb) const {
    return pb->hash();
  }
};

class Solver {
public:
  Solver(const Board& board);
  ~Solver(void);
  Solver(const Solver&) = delete;
  Solver& operator=(Solver) = delete;

  bool is_solvable(void) const;
  int moves(void) const;
  std::vector<Board> old_solution(void) const;
  std::vector<Board> solution(void) const;
  
private:
  mutable int n_moves_;
  const Board initial_board_;
};

#endif
