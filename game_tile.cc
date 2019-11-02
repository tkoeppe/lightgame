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

#include "game_tile.h"

#include <string>

#include "game.h"

namespace tkware::lightgame {

MouseLabel::MouseLabel(Game* game, int x, int y)
    : QLabel(), game_(game), x_(x), y_(y) {
  setAlignment(Qt::AlignCenter);
}

QSize MouseLabel::minimumSizeHint() const {
  return QSize(75, 75);
}

void MouseLabel::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::RightButton) {
    emit gameChanged(1, x_, y_);
  } else if (event->button() == Qt::LeftButton) {
    emit gameChanged(2, x_, y_);
  }
  event->accept();
}

void MouseLabel::updateState() {
  std::string s;
  if (x_ == game_->X() && y_ == game_->Y()) {
    s = "*";
    setStyleSheet("text-align: center; background-color: orange;");
  } else {
    switch (game_->At(x_, y_)) {
      case Game::State::kOff:;
        s = "O";
        setStyleSheet("background-color: #A00;");
        break;
      case Game::State::kOn:
        s = "X";
        setStyleSheet("background-color: #0A0;");
        break;
      case Game::State::kBlocked:
        s = "#";
        setStyleSheet("background-color: #666;");
        break;
    }
  }

  setText((s + "_" + std::to_string(x_) + "_" + std::to_string(y_)).c_str());
}

}  //  namespace tkware::lightgame
