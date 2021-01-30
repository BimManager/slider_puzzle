// Copyright 2021 kkozlov

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include <cstddef>
#include <cstdint>

#include <iostream>
#include <vector>

class Board {
 public:
  class Row {
   public:
    Row(int *ptr, uint16_t sz);
    ~Row(void);
    int& operator[](std::size_t idx);
    const int& operator[](std::size_t idx) const;

   private:
    int *data_p_;
    uint16_t size_;
  };
  
  Board(void);
  Board(int dim);
  ~Board(void);
  Board(const Board& rhs);
  Board(Board&& rv);

  Board& operator=(const Board &rhs);
  Board& operator=(Board&& rv);
  Board::Row operator[](std::size_t idx);
  const Board::Row operator[](std::size_t idx) const;
  bool operator==(const Board& rhs) const;
  bool operator!=(const Board& rhs) const;
  bool operator<(const Board& rhs) const;
  bool operator>(const Board& rhs) const;

  int dimension(void) const;
  int hamming(void) const;
  int manhattan(void) const;
  int hash(void) const;
  
  int blank(void) const;

  bool is_goal(void) const;
  int parse_file(const char *filename);
  int count_inversions(void) const;
  std::vector<Board> neighbours(void) const;
  std::vector<Board*> neighbours_ptr(void) const;

  friend void swap(Board& lhs, Board& rhs);
  
 private:
  int verify_parsed_board(const std::vector<int>& parsed_board);

  bool can_move(int idx) const;
  bool can_move_up(void) const;
  bool can_move_down(void) const;
  bool can_move_left(void) const;
  bool can_move_right(void) const;

  void move_blank(int idx);
  void move_up(void);
  void move_down(void);
  void move_left(void);
  void move_right(void);

  int calculate_manhattan(int idx) const;
  int calculate_hamming(int idx) const;
  
  std::vector<int> cells_;
  int dimension_;
  int blank_idx_;
  mutable int hash_;
  mutable int hamming_;
  mutable int manhattan_;
};

std::ostream& operator<<(std::ostream& os, const Board& in);

#endif
