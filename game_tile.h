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

#ifndef H_TKWARE_LIGHTGAME_GAME_TILE_
#define H_TKWARE_LIGHTGAME_GAME_TILE_

#include <QtGui/QMouseEvent>
#include <QtWidgets/QLabel>

#include "game.h"

namespace tkware::lightgame {

class MouseLabel : public QLabel {
  Q_OBJECT

 signals:
  void gameChanged(int type, int a, int b);

 public:
  explicit MouseLabel(Game* game, int x, int y);
  void updateState();

 private:
  void mousePressEvent(QMouseEvent *event) override;
  QSize minimumSizeHint() const override;
  
  Game* const game_;
  const int x_, y_;
};

}  //  namespace tkware::lightgame

#endif  // H_TKWARE_LIGHTGAME_GAME_TILE_
