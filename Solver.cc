// Copyright 2021 kkozlov

#include <functional>
#include <unordered_set>
#include <map>
#include <stack>

#include "Solver.h"

Solver::Solver(const Board& board)
    : n_moves_{-1}, initial_board_{board} {}

Solver::~Solver(void) {}

int Solver::moves(void) const {
  return n_moves_;
}

bool Solver::is_solvable(void) const {
  int n_inversions = initial_board_.count_inversions();
  return n_inversions % 2 ? false : true;
}

static std::vector<Board> arrange_solution(const Board& solved,
                                           std::map<Board, Board>& steps) {
  using MapIter = std::map<Board, Board>::iterator;
  
  std::vector<Board> tmp;
  tmp.push_back(solved);
  Board *it = &steps[solved];
  while (*it != steps[*it]) {
    tmp.push_back(*it);
    it = &steps[*it];
  }
  tmp.push_back(*it);
  return std::vector<Board>(tmp.rbegin(), tmp.rend());
}

std::vector<Board> Solver::solution(void) const {
  using PtrBoardConstIterator = std::vector<Board*>::const_iterator;
  
  std::priority_queue<std::pair<const Board*, unsigned>,
                      std::vector<std::pair<const Board*, unsigned>>,
                      BoardPtrGreaterManhattan> pq;
  std::map<const Board*, const Board*> previous;
  std::unordered_set<const Board*, BoardPtrHash, BoardPtrEqual> checked;
  const Board *board_p = nullptr;
  
  pq.push(std::make_pair(&initial_board_, 0));
  checked.insert(new Board(initial_board_));
  previous.insert(std::make_pair(&initial_board_, &initial_board_));
  while (!pq.empty()) {
    std::pair<const Board*, unsigned> top = pq.top();
    pq.pop();
    board_p = top.first;
    //    std::cout << "popped\n" << *board_p << std::endl;
    if (top.first->is_goal()) break;
    std::vector<Board*> neighbours = top.first->neighbours_ptr();
    unsigned n_moves = top.second;
    //    std::cout << "CURRENT:\n" << *board_p << "n_moves: " << n_moves << std::endl;
    //    std::cout << "Neighbours:\n";
    for (PtrBoardConstIterator it = neighbours.cbegin(); it != neighbours.cend(); ++it) {
      //      std::cout << *(*it)  << "n_moves: " << n_moves + 1 << std::endl;
      if (checked.find(*it) == checked.end()) {
        pq.push(std::make_pair(*it, n_moves + 1));
        checked.insert(*it);
        previous.insert(std::make_pair(*it, board_p));
      } else {
        delete *it;
      }
    }
  }
  std::stack<const Board*> stack;
  while (board_p != &initial_board_) {
    //    std::cout << *board_p << std::endl;
    stack.push(board_p);
    board_p = previous[board_p];
  }
  stack.push(board_p);
  std::vector<Board> ret;
  ret.reserve(stack.size());
  while (!stack.empty()) {
    ret.push_back(*stack.top());
    stack.pop();
  }
  using SetIterator = std::unordered_set<const Board*>::iterator;
  for (SetIterator it = checked.begin(); it != checked.end(); ++it) {
    delete *it;
  }
  return ret;
}
