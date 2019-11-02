// Copyright 2019 by Thomas Köppe
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "game.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

namespace tkware::lightgame {

Game::Game(int height, int width)
    : height_(height),
      width_(width),
      x_(0),
      y_(0),
      board_(std::make_unique<State[]>(4 * RawSize())) {
  for (int x = 0; x != width_ + 2; ++x) {
    At(x, 0) = At(x, height_ + 1) = State::kBlocked;
  }
  for (int y = 1; y != height_ + 1; ++y) {
    At(0, y) = At(width_ + 1, y) = State::kBlocked;
  }
}

void Game::CopyBoard(int from, int to) {
  std::copy_n(board_.get() + from * RawSize(), RawSize(),
              board_.get() + to * RawSize());
}

bool Game::Start(int x, int y) {
  if (HasStarted()) {
    std::cout << "Game has already started!\n";
    return false;
  } else if (1 <= x && x <= width_ && 1 <= y && y <= height_ &&
             At(x, y) == State::kOff) {
    x_ = x;
    y_ = y;
    CopyBoard(0, 1);
    At(x_, y_) = State::kOn;
    return true;
  } else {
    return false;
  }
}

bool Game::HaveWon() const {
  return std::find(board_.get(), board_.get() + RawSize(), State::kOff) ==
         board_.get() + RawSize();
}

void Game::Reset() {
  if (HasStarted()) {
    x_ = y_ = 0;
    CopyBoard(1, 0);
  }
}

bool Game::Move(Dir dir) {
  if (!HasStarted()) {
    std::cout << "Game has not started yet!\n";
    return false;
  } else if ((dir & ValidDirs()) != dir) {
    std::cout << "Invalid move!\n";
    return false;
  } else {
    MoveOne(dir);
    return true;
  }
}

bool Game::MoveFast(Dir dir) {
  if (!HasStarted()) {
    std::cout << "Game has not started yet!\n";
    return false;
  } else if ((dir & ValidDirs()) != dir) {
    std::cout << "Invalid move!\n";
    return false;
  } else {
    for (;;) {
      MoveOne(dir);
      switch (Dir d = ValidDirs()) {
        case kUp:
        case kDown:
        case kLeft:
        case kRight:
          dir = d;
          break;
        default:
          return true;
      }
    }
  }
}

void Game::MoveOne(Dir dir) {
  struct IncType {
    int a, b;
  };
  auto increment_for_move = [](Dir d) -> IncType {
    switch (d) {
      case kUp:
        return {0, -1};
      case kDown:
        return {0, +1};
      case kLeft:
        return {-1, 0};
      case kRight:
        return {+1, 0};
      default:
        __builtin_unreachable();
    }
  };
  auto [dx, dy] = increment_for_move(dir);
  while (At(x_ + dx, y_ + dy) == State::kOff) {
    x_ += dx;
    y_ += dy;
    At(x_, y_) = State::kOn;
  }
}

void Game::WriteLayoutAsBits(unsigned char* dst, int bits_per_byte) const {
  // (This won't work if sizeof(unsigned int) == 1.)
  unsigned int kEnd = 1U << bits_per_byte;

  unsigned int b = 1U;
  for (int y = 1; y <= Height(); ++y) {
    for (int x = 1; x <= Width(); ++x) {
      if (b == kEnd) { b = 1; ++dst; }
      *dst |= At(x, y) == State::kBlocked ? b : 0;
      b <<= 1;
    }
  }
}

void Game::LoadLayoutFromBits(const unsigned char* src, int bits_per_byte) {
  // (This won't work if sizeof(unsigned int) == 1.)
  unsigned int kEnd = 1U << bits_per_byte;

  unsigned int b = 1U;
  for (int i = 0; i != Height() * Width(); ++i) {
    if (b == kEnd) { b = 1; ++src; }
    if ((*src & b) == b) SetBlocked(1 + i % Width(), 1 + i / Width());
    b <<= 1;
  }
}

bool Game::IsSolvable(std::vector<int>* solutions) {
  class StateSaver {
   public:
    StateSaver(Game* game)
        : game_(game),
          x_(game_->x_),
          y_(game_->y_) {
      game_->CopyBoard(0, 2);
    }

    ~StateSaver() {
      game_->CopyBoard(2, 0);
      game_->x_ = x_;
      game_->y_ = y_;
    }

   private:
    Game* game_;
    int x_, y_;;
  } state_saver(this);

  struct Node {
    Game::Dir value;  // Game:kNone == root node
    Game::Dir children;
    int next;
  };

  std::vector<Node> nodes;
  nodes.reserve(100);

  auto solve_one = [this, &nodes, solutions](int x, int y) -> bool {
    Reset();
    if (!Start(x, y)) return false;

    nodes.clear();
    nodes.push_back(Node{Game::kNone, ValidDirs(), 0});

    while (!nodes.empty()) {
      Node& node = nodes.back();
      if (node.children == Game::kNone) {
        if (HaveWon()) {
          if (solutions != nullptr) {
            solutions->push_back(x);
            solutions->push_back(y);
            for (auto it = std::next(nodes.cbegin()); it != nodes.cend(); ++it) {
              solutions->push_back(it->value);
            }
            solutions->push_back(0);
          }
          return true;
        } else {
          nodes.pop_back();
        }
      } else if (node.next == 4) {
        nodes.pop_back();
      } else {
        auto dir = Game::Dir(1 << node.next);
        ++node.next;
        if ((node.children & dir) != dir) continue;

        Reset();
        // Replay the action stack.
        if (!(Start(x, y))) {
          std::cerr << "Bad start!\n";
          abort();
        }
        for (auto it = std::next(nodes.cbegin()); it != nodes.cend(); ++it) {
          if (!MoveFast(it->value)) {
            std::cerr << "Bad replay.\n";
            abort();
          }
        }
        // Perform the next candidate move.
        if (!MoveFast(dir)) {
          std::cerr << "Bad replay.\n";
          abort();
        }
        // Record the result.
        nodes.push_back(Node{dir, ValidDirs(), 0});
      }
    }
    return false;
  };

  int num_solutions = 0;

  for (int y = 1; y <= Height(); ++y) {
    for (int x = 1; x <= Width(); ++x) {
      if (solve_one(x, y)) {
        if (solutions == nullptr) {
          return true;
        } else {
          ++num_solutions;
        }
      }
    }
  }

  return num_solutions > 0;
}

bool Game::AugmentRandomly(int n, std::mt19937* rbg) {
  if (HasStarted()) {
    std::cout << "Game has already started!\n";
    return false;
  }

  auto num_free =
      std::count(board_.get(), board_.get() + RawSize(), State::kOff);

  if (n < 0) {
    std::cout << "Bad value for n (" << n << "), must be positive.\n";
    return false;
  } else if (num_free <= n) {
    std::cout << "Not enough free fields left to fill (have " << num_free
              << " free field, want to fill " << n << ")!\n";
    return false;
  }

  // Backup copy of the original layout.
  CopyBoard(0, 3);
  std::unique_ptr<State[]> p = std::make_unique<State[]>(num_free);
  std::fill_n(p.get(), n, State::kBlocked);
  std::vector<int> solutions;

  for (;;) {
    std::shuffle(p.get(), p.get() + num_free, *rbg);
    CopyBoard(3, 0);
    for (int i = 0, k = 0; i != num_free; ++i) {
      while (board_[k] != State::kOff) ++k;
      board_[k] = p[i];
      ++k;
    }
    if (IsSolvable(&solutions)) {
      std::cout << "It worked! [[" << SaveToHexString(*this) << "]]:\n";
      for (auto it = solutions.begin(); it != solutions.end(); ++it) {
        ++it; ++it; int n = 0; while (*it != 0) ++it, ++n;
        std::cout << "- from [REDACTED] move [ " << n << " times ]\n";
      }

      CopyBoard(0, 1);
      return true;
    }
  }
}

std::string SaveToHexString(const Game& game) {
  if (game.Height() >= 16 || game.Width() >= 16) {
    return "[layout too large]";
  }

  constexpr char alphabet[] = "0123456789ABCDEF";

  int n = game.LayoutByteSize(4);
  std::string s(2 + n, '\0');
  s[0] = alphabet[game.Height()];
  s[1] = alphabet[game.Width()];
  game.WriteLayoutAsBits(reinterpret_cast<unsigned char*>(&s[2]), 4);
  for (std::size_t i = 2; i != s.size(); ++i) {
    s[i] = alphabet[s[i]];
  }
  return s;
}

std::unique_ptr<Game> LoadFromHexString(std::string code) {
  auto parse_hex = [](char c) -> int {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return -1;
  };
  if (code.size() < 2) return nullptr;
  const int h = parse_hex(code[0]), w = parse_hex(code[1]);
  if (h == -1 || w == -1) return nullptr;
  auto game = std::make_unique<Game>(h, w);
  if (code.size() != 2 + game->LayoutByteSize(4)) return nullptr;
  for (std::size_t i = 2; i != code.size(); ++i) {
    if (int d = parse_hex(code[i]); d == -1) {
      return nullptr;
    } else {
      code[i] = d;
    }
  }
  game->LoadLayoutFromBits(reinterpret_cast<const unsigned char*>(&code[2]), 4);
  return game;
}

}  //  namespace tkware::lightgame
