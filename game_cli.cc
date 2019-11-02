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
//
////////////////////////////////////////////////////////////////////////////////
//
// A command-line interface for the game.
//
// Commands:
//
//   n <h> <w>:  new, blank layout of dimensions h times w
//   g <h> <w>:  randomly generated, solvable layout
//   b <x> <y>:  marks tile x, y as blocked
//   s <x> <y>:  starts a game at tile x, y (if possible)
//   r        :  resets a game in progress, returns to layout mode
//   a <N>    :  takes an action: N = 1 (up), 2 (down), 3 (left), 4 (right)

#include <cctype>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>

#include "game.h"

namespace tkware::lightgame {
namespace {

bool ParseCommand2Arg(const std::string& line, int* h, int* w, char X) {
  std::istringstream iss(line);
  char c;
  bool b(iss >> c >> *h >> *w);
  iss >> std::ws;
  return b && iss.eof() && (c == std::tolower(X) || c == std::toupper(X));
}

bool ParseCommand0Arg(const std::string& line, char X) {
  std::istringstream iss(line);
  char c;
  bool b(iss >> c);
  iss >> std::ws;
  return b && iss.eof() && (c == std::tolower(X) || c == std::toupper(X));
}

bool ParseNewGame(const std::string& line, int* h, int* w) {
  return ParseCommand2Arg(line, h, w, 'n');
}

bool ParseSetBlocked(const std::string& line, int* h, int* w) {
  return ParseCommand2Arg(line, h, w, 'b');
}

bool ParseStart(const std::string& line, int* h, int* w) {
  return ParseCommand2Arg(line, h, w, 's');
}

bool ParseReset(const std::string& line) {
  return ParseCommand0Arg(line, 'r');
}

bool ParseCreateRandom(const std::string& line, int* h, int* w) {
  return ParseCommand2Arg(line, h, w, 'g');
}

bool ParseAction(const std::string& line, int* d) {
  std::istringstream iss(line);
  char c;
  bool b(iss >> c >> *d);
  iss >> std::ws;
  return b && iss.eof() && (c == 'a' || c == 'A') && 1 <= *d && *d <= 4;
}

std::string PrintDirs(const Game::Dir dir) {
  if (dir == Game::kNone) return "[None!]";
  std::string result = "[";
  if ((dir & Game::kUp) == Game::kUp) result += "Up";
  if ((dir & Game::kDown) == Game::kDown) result += "Down";
  if ((dir & Game::kLeft) == Game::kLeft) result += "Left";
  if ((dir & Game::kRight) == Game::kRight) result += "Right";
  result += "]";
  return result;
}

void PrintBoard(std::ostream& os, const Game& game) {
  auto to_char = [](Game::State s) {
    switch (s) {
      case Game::State::kOff:
        return 'O';
      case Game::State::kOn:
        return 'X';
      case Game::State::kBlocked:
        return '#';
      default:
        return ' ';
    }
  };

  auto render_at = [&game, &to_char](int x, int y) {
    return x == game.X() && y == game.Y() ? '*' : to_char(game.At(x, y));
  };

  for (int y = 1; y <= game.Height(); ++y) {
    os << '+';
    for (int x = 1; x <= game.Width(); ++x) {
      os << "-+";
    }
    os << "\n|";
    for (int x = 1; x <= game.Width(); ++x) {
      os << render_at(x, y) << '|';
    }
    os << "\n";
  }
  os << '+';
  for (int x = 1; x <= game.Width(); ++x) {
    os << "-+";
  }
  os << "\n";
}

void Run() {
  std::unique_ptr<Game> game;
  for (std::string line; std::cout << "> " && std::getline(std::cin, line);) {
    int a, b, d;
    if (ParseNewGame(line, &a, &b)) {
      std::cout << "New game: " << a << " x " << b << ".\n";
      game = std::make_unique<Game>(a, b);
      PrintBoard(std::cout, *game);
    } else if (ParseSetBlocked(line, &a, &b)) {
      if (game == nullptr) {
        std::cout << "No game in progress!\n";
      } else if (!game->SetBlocked(a, b)) {
        std::cout << "Error setting blocked field.\n";
      } else {
        PrintBoard(std::cout, *game);
      }
    } else if (ParseStart(line, &a, &b)) {
      if (game == nullptr) {
        std::cout << "No game in progress!\n";
      } else if (!game->Start(a, b)) {
        std::cout << "Invalid start position (" << a << ", " << b << ")!\n";
      } else {
        PrintBoard(std::cout, *game);
        std::cout << "Valid directions: " << PrintDirs(game->ValidDirs()) << "\n";
      }
    } else if (ParseReset(line)) {
      if (game == nullptr) {
        std::cout << "No game in progress!\n";
      } else {
        game->Reset();
        PrintBoard(std::cout, *game);
      }
    } else if (ParseCreateRandom(line, &a, &b)) {
      std::mt19937 rbg(std::random_device{}());
      std::uniform_int_distribution dh(1, a);
      std::uniform_int_distribution dw(1, b);

      for (;;) {
        game = std::make_unique<Game>(a, b);
        int n = std::uniform_int_distribution(3, 6)(rbg);
        for (int i = 0; i != n; ++i) {
          game->SetBlocked(dw(rbg), dh(rbg));
        }
        if (game->IsSolvable(nullptr)) {
          PrintBoard(std::cout, *game);
          break;
        }
      }
    } else if (ParseAction(line, &d)) {
      if (game == nullptr) {
        std::cout << "No game in progress!\n";
      } else if ((d == 1 && game->Move(Game::kUp))   ||
                 (d == 2 && game->Move(Game::kDown)) ||
                 (d == 3 && game->Move(Game::kLeft)) ||
                 (d == 4 && game->Move(Game::kRight))) {
        PrintBoard(std::cout, *game);
        if (Game::Dir dirs = game->ValidDirs(); dirs == Game::kNone) {
          if (game->HaveWon()) {
            std::cout << "You won!!\n";
          } else {
            std::cout << "Game over, you lose.\n";
          }
        } else {
          std::cout << "Valid directions: " << PrintDirs(dirs) << "\n";
        }
      }
    } else {
      std::cout << "Unrecognized command '" << line << "'.\n";
    }
  }

  std::cout << "Goodbye!\n";
}

}  // namespace
}  // namespace tkware::lightgame

int main() {
  tkware::lightgame::Run();
}
