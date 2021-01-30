// Copyright 2021 kkozlov

#include <cstddef>
#include <cmath>

#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <set>
#include <unordered_set>
#include <vector>
#include <iterator> // std::inserter
#include <algorithm> // std::copy

#include "Board.h"

/* Board::Row */
Board::Row::Row(int *ptr, uint16_t sz)
    : data_p_{ptr}, size_{sz} {}

Board::Row::~Row(void) {}

int& Board::Row::operator[](std::size_t idx) {
  return data_p_[idx];
}

const int& Board::Row::operator[](std::size_t idx) const {
  return data_p_[idx];
}

/* Constructors, destructor and copy constructor */
Board::Board(void)
    : cells_{}, dimension_{0}, hamming_{-1}, manhattan_{-1}, hash_{-1} {}
      
Board::Board(int dim)
    : dimension_{dim}, blank_idx_{dim - 1},
      hamming_{0},manhattan_{0}, hash_{-1} {
  cells_.reserve(dim * dim);
  dim *= dim;
  for (int i = 1; i < dim; ++i) {
    cells_.push_back(i);
  }
}

Board::~Board(void) {}

Board::Board(const Board& rhs)
    : dimension_{rhs.dimension_}, cells_{rhs.cells_},
      blank_idx_{rhs.blank_idx_}, hamming_{rhs.hamming_},
      manhattan_{rhs.manhattan_}, hash_{rhs.hash_} {
        //        std::cout << rhs << " has been copy constructed\n";
                                  }

Board::Board(Board&& rv) {
  swap(*this, rv);
}

Board& Board::operator=(const Board& rhs) {
  Board tmp(rhs);
  swap(*this, tmp);
  return *this;
}

Board& Board::operator=(Board&& rv) {
  swap(*this, rv);
  return *this;
}

/* Operator overloading */
Board::Row Board::operator[](std::size_t idx) {
  return Row(&cells_[idx * dimension_], dimension_);
}

const Board::Row Board::operator[](std::size_t idx) const {
  return Board::Row(const_cast<int*>(&cells_[idx * dimension_]), dimension_);
}

bool Board::operator==(const Board& rhs) const {
  if (hash_ != rhs.hash_) return false;
  for (int i = 0; i < cells_.size(); ++i) {
    if (cells_[i] != rhs.cells_[i]) return false;
  }
  return true;
}

bool Board::operator!=(const Board& rhs) const {
  return !(*this == rhs);
}

bool Board::operator<(const Board& rhs) const {
  for (int i = 0; i < cells_.size(); ++i) {
    if (cells_[i] < rhs.cells_[i]) return true;
    else if (cells_[i] > rhs.cells_[i]) return false;
  }
  return false;
}

bool Board::operator>(const Board& rhs) const {
  return !(*this < rhs) && (*this != rhs);
}

/* Getters */
int Board::dimension(void) const {
  return dimension_;
}

int Board::hamming(void) const {
  if (-1 != hamming_) return hamming_;
  hamming_ = 0;
  for (int i = 0; i < cells_.size(); ++i) {
    hamming_ += calculate_hamming(i);
  }
  return hamming_;
}

int Board::manhattan(void) const {
  if (-1 != manhattan_) return manhattan_;
  manhattan_ = 0;
  for (int i = 0; i < cells_.size(); ++i) {
    manhattan_ += calculate_manhattan(i);
  }
  return manhattan_;
}

int Board::hash(void) const {
  if (-1 == hash_) {
    hash_ = 0;
    for (int i = 0; i < cells_.size(); ++i) {
      hash_ ^= i * cells_[i];
    }
  }
  return hash_;
}

bool Board::is_goal(void) const {
  for (int i = 0; i < cells_.size() - 1; ++i) {
    if (cells_[i] != i + 1) return false;
  }
  return true;
}

/* Setters */
int Board::parse_file(const char *filename) {
  std::fstream input(filename);
  std::vector<int> nums;
  
  if (input.is_open()) {
    std::copy(std::istream_iterator<int>(input),
              std::istream_iterator<int>(),
              std::back_inserter<std::vector<int>>(nums));
    input.close();
  } else {
    std::cerr << "Error:\n" << filename << " could not be opened\n";
    return 1;
  }
  if (!verify_parsed_board(nums)) return 2;
  dimension_ = nums[0];
  cells_.resize(nums.size() - 1);
  std::copy(nums.begin() + 1, nums.end(), cells_.begin());
  hash();
  manhattan();
  hamming();
  return 0;
}

std::vector<Board> Board::neighbours(void) const {
  std::vector<Board> boards;
  if (can_move_up()) {
    boards.push_back(*this);
    boards.back().move_up();
  }
  if (can_move_down()) {
    boards.push_back(*this);
    boards.back().move_down();
  }
  if (can_move_left()) {
    boards.push_back(*this);
    boards.back().move_left();
  }
  if (can_move_right()) {
    boards.push_back(*this);
    boards.back().move_right();
  }
  return boards;
}


std::vector<Board*> Board::neighbours_ptr(void) const {
  std::vector<Board*> boards;
  if (can_move_up()) {
    boards.push_back(new Board(*this));
    boards.back()->move_up();
  }
  if (can_move_down()) {
    boards.push_back(new Board(*this));
    boards.back()->move_down();
  }
  if (can_move_left()) {
    boards.push_back(new Board(*this));
    boards.back()->move_left();
  }
  if (can_move_right()) {
    boards.push_back(new Board(*this));
    boards.back()->move_right();
  }
  return boards;
}

void swap(Board& lhs, Board& rhs) {
  using std::swap;
  swap(lhs.dimension_, rhs.dimension_);
  swap(lhs.cells_, rhs.cells_);
  swap(lhs.blank_idx_, rhs.blank_idx_);
  swap(lhs.hamming_, rhs.hamming_);
  swap(lhs.manhattan_, rhs.manhattan_);
  swap(lhs.hash_, rhs.hash_);
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
  int dim = board.dimension();
  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      std::cout << std::setw(3) << board[i][j];
    }
    std::cout << std::endl;
  }
#ifdef DEBUG
  std::cout << "hamming: " << board.hamming() << std::endl;
  std::cout << "manhattan: " << board.manhattan() << std::endl;
  std::cout << "hash: " << board.hash() << std::endl;
  std::cout << "blank idx: " << board.blank() << std::endl;
#endif
  return os;
}

int Board::verify_parsed_board(const std::vector<int>& parsed_board) {
  dimension_ = parsed_board[0];
  int sz = dimension_ * dimension_;
  if (parsed_board.size() - 1 != sz) return 0;
  std::unordered_set<int> values;
  for (int i = 1; i <= sz; ++i) {
    if (0 == parsed_board[i]) blank_idx_ = i - 1;
    if (values.find(parsed_board[i]) != values.end()) return 0;
    if (parsed_board[i] >= sz) return 0;
    values.insert(parsed_board[i]);
  }
  return 1;
}

bool Board::can_move(int idx) const {
  return idx >= 0 && idx < cells_.size();
}

bool Board::can_move_up(void) const {
  return can_move(blank_idx_ - dimension_);
}

bool Board::can_move_down(void) const {
  return can_move(blank_idx_ + dimension_);
}

bool Board::can_move_left(void) const {
  return can_move(blank_idx_ - 1)
      && 0 != (blank_idx_ % dimension_);
}

bool Board::can_move_right(void) const {
  return can_move(blank_idx_ + 1)
      && (blank_idx_ + 1) % dimension_;
}

void Board::move_blank(int idx) {
  //  if (-1 == manhattan_) manhattan();  
  manhattan_ -=  calculate_manhattan(idx);
  //  if (-1 == hamming_) hamming();
  hash_ ^= cells_[idx] * idx;
  hamming_ -= calculate_hamming(idx);
  std::swap(cells_[idx], cells_[blank_idx_]);
  std::swap(blank_idx_, idx);
  manhattan_ += calculate_manhattan(idx);
  hamming_ += calculate_hamming(idx);
  hash_ ^= cells_[idx] * idx;
  //  hash_ = -1;
}

void Board::move_up(void) {
  move_blank(blank_idx_ - dimension_);
}

void Board::move_down(void) {
  move_blank(blank_idx_ + dimension_);
}

void Board::move_left(void) {
  move_blank(blank_idx_ - 1);
}

void Board::move_right(void) {
  move_blank(blank_idx_ + 1);
}

int Board::blank(void) const {
  return blank_idx_;
}

int Board::calculate_manhattan(int idx) const {
  if (!cells_[idx]) return 0;
  int drow = std::abs(idx - cells_[idx] + 1) / dimension_;
  int dcol = std::abs(idx - cells_[idx] + 1) % dimension_;
  return drow + dcol;
}

int Board::calculate_hamming(int idx) const {
  if (!cells_[idx]) return 0;
  return (cells_[idx] - 1 - idx) ? 1 : 0;
}


namespace {
int _Merge(std::vector<int>& nums, int low, int mid, int high, std::vector<int>& aux) {
  std::copy(nums.begin() + low, nums.begin() + high + 1, aux.begin() + low);
  int n_inversions = 0;
  //  std::cout << "low: " << low << "; mid: " << mid << "; high: " << high << std::endl;
  for (int i = low, j = mid, k = low; i < mid || j <= high; k++) {
    if (i == mid) nums[k] = aux[j++];
    else if (j > high) nums[k] = aux[i++];
    else if (aux[i] > aux[j]) {
      n_inversions += mid - i;
      nums[k] = aux[j++];
    } else {
      nums[k] = aux[i++];
    }
  }
  //  for_each(nums.begin() + low, nums.begin() + high + 1, [] (int x) { std::cout << x << ", "; });
  //  std::cout << std::endl;
  //  std::cout << n_inversions << std::endl;
  return n_inversions;
}

int _Count_inversions(std::vector<int>& nums, int low, int high, std::vector<int>& aux) {
  if (low >= high) return 0;
  int n_inversions = 0;
  int mid = low + (high - low) / 2;
  n_inversions += _Count_inversions(nums, low, mid, aux);
  n_inversions += _Count_inversions(nums, mid + 1, high, aux);
  n_inversions += _Merge(nums, low, mid + 1, high, aux);
  return n_inversions;
}

int _Count_inversions(std::vector<int>& nums) {
  std::vector<int> aux;
  aux.resize(nums.size());
  return _Count_inversions(nums, 0, nums.size() - 1, aux);
}
} // namespace

int Board::count_inversions(void) const {
  std::vector<int> aux;
  aux.reserve(cells_.size());
  for (int i = 0; i < cells_.size(); ++i) {
    if (cells_[i]) aux.push_back(cells_[i]);
  }
  //  std::vector<int> test = { 2, 4, 1, 3, 5 };
  return _Count_inversions(aux);
}
