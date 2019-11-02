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

#ifndef H_TKWARE_LIGHTGAME_GAME_
#define H_TKWARE_LIGHTGAME_GAME_

#include <iostream>
#include <memory>
#include <random>
#include <string>

namespace tkware::lightgame {

// A board of size Height × Width. Valid coordinates are x ∈ [1, Width] and
// y ∈ [1, Height], but one extra field of blocked padding is stored around
// the board, so internally, valid indices lie in [0, {H, W} + 1].
//
// Each field on the board is either "off" (the initial state), "on" (the goal
// state), or "blocked" (i.e. it does not "participate"). The goal of the game
// is to make all non-blocked fields "on".
//
// At the start of the game, the player selects one non-blocked field. At each
// turn, a valid action consists of a choosing a direction from the selected
// field and walking along it until an obstacle is hit, switching all traversed
// fields "on"; the final field that is thus reached is the next selected field.
// The valid directions which may be chosen, if any, are those that point to a
// field that is "off" (i.e. neither blocked nor already traversed).
//
// The game ends when there are is no choice of direction left; the game is a
// win if no fields remain "off", and a loss otherwise.
class Game {
 public:
  enum class State { kOff = 0, kOn, kBlocked };
  enum Dir { kNone = 0, kUp = 1, kDown = 2, kLeft = 4, kRight = 8 };

  friend void operator|=(Dir& lhs, Dir rhs) { lhs = Dir(int(lhs) | int(rhs)); }

  // Creates a game of the given size.
  explicit Game(int height, int width);

  const State& At(int x, int y) const { return board_[x + (width_ + 2) * y]; }
        State& At(int x, int y)       { return board_[x + (width_ + 2) * y]; }

  int Height() const { return height_; }
  int Width() const { return width_; }
  int X() const { return x_; }
  int Y() const { return y_; }

  // Starts the game at the given field. Returns true if the game hadn't already
  // been started and the given field is "off" (not "blocked"), false otherwise.
  bool Start(int x, int y);

  bool HasStarted() const {
    return x_ != 0 && y_ != 0;
  }

  // Requests a move in the given direction; returns true if this is possible,
  // and false if either the direction was invalid or no game is in progress.
  // The MoveFast version keeps going as long as there is a unique direction.
  bool Move(Dir dir);
  bool MoveFast(Dir dir);

  // Returns whether the game is in the win state (no "off" fields left).
  bool HaveWon() const;

  // Returns whether the game is winnable in principle (ignoring its current
  // state if the game is already in progress). If solutions is not null, all
  // possible solutions are appended to *solutions consecutively in the format
  // "x, y, a_1, a_2, ..., a_N, 0", where the a_i are Dir-valued fast actions.
  bool IsSolvable(std::vector<int>* solutions);

  // Resets the game; after this call, the game is no longer in progress and all
  // fields are either "off" or "blocked".
  void Reset();

  // Returns the valid directions at the currently active field. Should only be
  // called when a game is in progress, but will return kNone if no game is in
  // progress. If a game is in progress and this returns kNone, the game is over
  // (and use HaveWon to distinguish win from loss).
  Dir ValidDirs() const {
    Dir dir = kNone;
    if (HasStarted()) {
      if (At(x_, y_ - 1) == State::kOff) dir |= kUp;
      if (At(x_, y_ + 1) == State::kOff) dir |= kDown;
      if (At(x_ - 1, y_) == State::kOff) dir |= kLeft;
      if (At(x_ + 1, y_) == State::kOff) dir |= kRight;
    } else {
      std::cout << "Game has not started yet!\n";
    }
    return dir;
  }

  // Marks the field x, y as "blocked". Should only be called when no game is
  // in progress, but will return false if either a game is already in progress
  // or if the given position is not on the board, and true if the operation
  // succeeded.
  bool SetBlocked(int x, int y) {
    if (HasStarted()) {
      std::cout << "Game has already started!\n";
      return false;
    } else  if (1 <= x && x <= width_ && 1 <= y && y <= height_) {
      At(x, y) = State::kBlocked;
      return true;
    } else {
      std::cout << "Invalid board position " << x << ", " << y << "!\n";
      return false;
    }
  }

  // Writes the board layout as a bitmask, 1 = blocked, 0 = not blocked, to the
  // array starting at dst; one bit per field in row-major order. Each output
  // byte receives bits_per_byte bits (which must be positive and not exceed
  // CHAR_BIT); the end of the output is padded with zeroes. Each output byte's
  // value lies in the range [0, 2^bits_per_byte). The output range must contain
  // at least LayoutByteSize(bits_per_byte) elemets. This function is useful to
  // prepare an encoding of the board layout (e.g. base-N).
  void WriteLayoutAsBits(unsigned char* dst, int bits_per_byte) const;

  // Loads a layout from a bitmask from an array starting at src, with
  // bits_per_byte bits in each input byte; see above for semantics.
  // The input range of length must contain at least LayoutByteSize() elements.
  void LoadLayoutFromBits(const unsigned char* src, int bits_per_byte);

  // See above.
  int LayoutByteSize(int bits_per_byte) const {
    return (Height() * Width() + (bits_per_byte - 1)) / bits_per_byte;
  }

  // Randomly adds n blocked fields to the current layout. This should only be
  // called when no game is in progress. Returns false if a game is already in
  // progress or n is too large or too small.
  bool AugmentRandomly(int n, std::mt19937* rbg);

private:
  int RawSize() const { return (height_ + 2) * (width_ + 2); }

  // We store four copies of the board:
  // * Area 0: the live board for the active game.
  // * Area 1: a copy of just the layout (e.g. used for reset).
  // * Area 2: backup copy of the live game, for the solver.
  // * Area 2: backup copy of the layout, for layout augmentation.
  // Valid from, to parameters are 0, 1, 2, subject to from != to.
  void CopyBoard(int from, int to);

  // Moves in the given direction.
  void MoveOne(Dir dir);

  const int height_;
  const int width_;
  int x_, y_;
  const std::unique_ptr<State[]> board_;
};

// Serialization as 4-bit (hex) strings. Loading returns null on error.
std::string SaveToHexString(const Game& game);
std::unique_ptr<Game> LoadFromHexString(std::string code);

}  //  namespace tkware::lightgame

#endif  // H_TKWARE_LIGHTGAME_GAME_
