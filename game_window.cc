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

#include "game_window.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <utility>

#include <QtCore/QTimer>
#include <QtGui/QClipboard>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "game_tile.h"

namespace tkware::lightgame {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), rbg_(std::random_device{}()) {
  QWidget* window = new QWidget;
  QHBoxLayout* main_layout = new QHBoxLayout;
  QVBoxLayout* buttons_layout = new QVBoxLayout;
  QGridLayout* size_layout = new QGridLayout;
  QGridLayout* board_layout = new QGridLayout;
  QHBoxLayout* newbutton_layout = new QHBoxLayout;
  QPushButton* button1a = new QPushButton("&New (blank) layout");
  QPushButton* button1b = new QPushButton("&Random layout");
  QPushButton* button1c = new QPushButton("Random aug&ment");
  QHBoxLayout* resetbutton_layout = new QHBoxLayout;
  QPushButton* button2 = new QPushButton("(Re)&start current layout");
  QPushButton* button3 = new QPushButton("From s&ame start");
  QSpinBox* height_box = new QSpinBox;
  QSpinBox* width_box = new QSpinBox;
  QSpinBox* rand_min_box = new QSpinBox;
  QSpinBox* rand_max_box = new QSpinBox;
  QSpinBox* aug_box = new QSpinBox;
  QLabel* height_label = new QLabel("&Height:");
  QLabel* width_label = new QLabel("&Width:");
  QLabel* rand_min_label = new QLabel("min rand block:");
  QLabel* rand_max_label = new QLabel("max rand block:");
  QLabel* aug_label = new QLabel("augment count:");
  QLabel* win_label = new QLabel("Victory!");
  QLabel* lose_label = new QLabel("Game over");
  QCheckBox* fast_actions = new QCheckBox("auto-ta&ke actions");
  QHBoxLayout* code_layout = new QHBoxLayout;
  QLabel* code_label = new QLabel("&Code:");
  QLineEdit* code_edit = new QLineEdit;
  QPushButton* code_load = new QPushButton("&load");
  QPushButton* code_clip = new QPushButton(QIcon::fromTheme("edit-paste"), QString());
  QHBoxLayout* meta_layout = new QHBoxLayout;
  QLabel* mode_label = new QLabel("[<i>layout mode</i>]");
  QLabel* star_label = new QLabel("stars");
  QPushButton* hint_button = new QPushButton("\u2618");
  QPushButton* help_button = new QPushButton("Hel&p");
  QPushButton* quit_button = new QPushButton("&Quit");

  height_box->setMinimum(1);
  height_box->setMaximum(12);
  height_box->setValue(5);
  height_label->setBuddy(height_box);
  width_box->setMinimum(1);
  width_box->setMaximum(12);
  width_box->setValue(5);
  width_label->setBuddy(width_box);
  rand_min_box->setMinimum(0);
  rand_min_box->setMaximum(10);
  rand_min_box->setValue(2);
  rand_min_label->setBuddy(rand_min_box);
  rand_max_box->setMinimum(0);
  rand_max_box->setMaximum(10);
  rand_max_box->setValue(6);
  rand_max_label->setBuddy(rand_max_box);
  aug_box->setMinimum(1);
  aug_box->setMaximum(1000);
  aug_label->setBuddy(aug_box);

  board_layout->setSpacing(1);

  main_layout->addLayout(buttons_layout);
  main_layout->addSpacing(25);
  main_layout->addLayout(board_layout);

  button1c->setDisabled(true);
  button2->setDisabled(true);
  button3->setDisabled(true);
  win_label->hide();
  win_label->setAlignment(Qt::AlignCenter);
  win_label->setStyleSheet("font-size: 24pt; font-weight: bold; color: #0A0;");
  lose_label->hide();
  lose_label->setAlignment(Qt::AlignCenter);
  lose_label->setStyleSheet("font-size: 24pt; font-weight: bold; color: #A00;");

  code_layout->addWidget(code_label);
  code_layout->addWidget(code_edit);
  code_layout->addWidget(code_load);
  code_layout->addWidget(code_clip);
  code_clip->setToolTip("Load from clipboard (if possible)");
  code_label->setBuddy(code_edit);
  code_edit->setPlaceholderText("level code");
  hint_button->setStyleSheet("color: #0A0; font-weight: bold;");

  newbutton_layout->addWidget(button1a);
  newbutton_layout->addWidget(button1b);
  newbutton_layout->addWidget(button1c);
  resetbutton_layout->addWidget(button2);
  resetbutton_layout->addWidget(button3);
  resetbutton_layout->setStretch(0, 3);
  resetbutton_layout->setStretch(1, 2);
  mode_label->hide();
  mode_label->setTextFormat(Qt::RichText);
  star_label->hide();
  star_label->setTextFormat(Qt::RichText);
  meta_layout->addWidget(star_label);
  meta_layout->addWidget(mode_label);
  meta_layout->addStretch();
  meta_layout->addWidget(hint_button);
  meta_layout->addWidget(help_button);
  meta_layout->addWidget(quit_button);
  buttons_layout->addLayout(newbutton_layout);
  buttons_layout->addLayout(size_layout);
  buttons_layout->addLayout(resetbutton_layout);
  buttons_layout->addWidget(fast_actions);
  buttons_layout->addLayout(code_layout);
  buttons_layout->addStretch();
  buttons_layout->addWidget(win_label);
  buttons_layout->addWidget(lose_label);
  buttons_layout->addStretch();
  buttons_layout->addLayout(meta_layout);

  size_layout->addWidget(height_label, 0, 0);
  size_layout->addWidget(height_box, 0, 1);
  size_layout->addItem(new QSpacerItem(10, 0), 0, 2);
  size_layout->addWidget(rand_min_label, 0, 3);
  size_layout->addWidget(rand_min_box, 0, 4);
  size_layout->addWidget(width_label, 1, 0);
  size_layout->addWidget(width_box, 1, 1);
  size_layout->addItem(new QSpacerItem(10, 0), 1, 2);
  size_layout->addWidget(rand_max_label, 1, 3);
  size_layout->addWidget(rand_max_box, 1, 4);
  size_layout->addItem(new QSpacerItem(10, 0), 2, 2);
  size_layout->addWidget(aug_label, 2, 3);
  size_layout->addWidget(aug_box, 2, 4);

  auto set_key_grabbing = [=](bool on) {
    auto mfp = on ? &QObject::installEventFilter : &QObject::removeEventFilter;
    (button1a->*mfp)(&key_grabber_);
    (button1b->*mfp)(&key_grabber_);
    (button1c->*mfp)(&key_grabber_);
    (button2->*mfp)(&key_grabber_);
    (button3->*mfp)(&key_grabber_);
    (height_box->*mfp)(&key_grabber_);
    (width_box->*mfp)(&key_grabber_);
    (rand_min_box->*mfp)(&key_grabber_);
    (rand_max_box->*mfp)(&key_grabber_);
    (aug_box->*mfp)(&key_grabber_);
    (fast_actions->*mfp)(&key_grabber_);
    (code_edit->*mfp)(&key_grabber_);
    (code_load->*mfp)(&key_grabber_);
    (code_clip->*mfp)(&key_grabber_);
    (hint_button->*mfp)(&key_grabber_);
    (help_button->*mfp)(&key_grabber_);
    (quit_button->*mfp)(&key_grabber_);
  };

  auto handle = [=](int type, int a, int b) {
    bool (Game::*mover)(Game::Dir, Game::Path*) =
        fast_actions->isChecked() ? &Game::MoveFast : &Game::Move;

    auto as_label = [board_layout](int x, int y) -> MouseLabel* {
      auto* const lbl = board_layout->itemAtPosition(y, x)->widget();
      assert(lbl != nullptr);
      auto* const p = dynamic_cast<MouseLabel*>(lbl);
      assert(p != nullptr);
      return p;
    };

    switch (type) {
      case 1:
        if (!game_->HasStarted()) {
          [](Game::State& s) { s = Game::State(2 - static_cast<int>(s)); }(game_->At(a, b));
          RecomputeSolvability();
          RedrawStars(star_label);
          for (int y = 0; y != game_->Height(); ++y) {
            for (int x = 0; x != game_->Width(); ++x) {
              as_label(x, y)->markAsWinning(false);
            }
          }
        }
        break;
      case 2:
        if (game_->Start(a, b)) {
          start_pos_ = {a, b};
          mode_label->hide();
          button1c->setDisabled(true);
          button3->setDisabled(false);
        } else if (a + 1 == game_->X() && b == game_->Y()) {
          (*game_.*mover)(Game::kLeft, nullptr);
        } else if (a == game_->X() + 1 && b == game_->Y()) {
          (*game_.*mover)(Game::kRight, nullptr);
        } else if (a == game_->X() && b + 1 == game_->Y()) {
          (*game_.*mover)(Game::kUp, nullptr);
        } else if (a == game_->X() && b == game_->Y() + 1) {
          (*game_.*mover)(Game::kDown, nullptr);
        }
        break;
      case 3:
        auto dir_for_key = [](int key) {
          switch (key) {
            default:
              return Game::kNone;
            case Qt::Key_Up:
              return Game::kUp;
            case Qt::Key_Down:
              return Game::kDown;
            case Qt::Key_Left:
              return Game::kLeft;
            case Qt::Key_Right:
              return Game::kRight;
          }
        };

        (*game_.*mover)(dir_for_key(a), nullptr);

        break;
    }

    for (int y = 0; y != game_->Height(); ++y) {
      for (int x = 0; x != game_->Width(); ++x) {
        as_label(x, y)->updateState();
      }
    }

    if (game_->HasStarted()) {
      if (Game::Dir dirs = game_->ValidDirs(); dirs == Game::kNone) {
        if (game_->HaveWon()) {
          win_label->show();
          if (sol_tracker_.ReportSolution(start_pos_)) {
            RedrawStars(star_label);
            as_label(start_pos_.x - 1, start_pos_.y - 1)->markAsWinning(true);
          }
        } else {
          lose_label->show();
        }
      } else {
        set_key_grabbing(true);
      }
    } else {
      win_label->hide();
      lose_label->hide();
      set_key_grabbing(false);
      code_edit->setText(QString::fromStdString(SaveToHexString(*game_)));
    }
  };

  auto init_grid = [=]() {
    while (auto* p = board_layout->takeAt(0)) {
      delete p->widget();
      delete p;
    }

    for (int y = 0; y != game_->Height(); ++y) {
      for (int x = 0; x != game_->Width(); ++x) {
        auto* lbl = new MouseLabel(game_.get(), x + 1, y + 1);
        QObject::connect(lbl, &MouseLabel::gameChanged, handle);
        board_layout->addWidget(lbl, y, x);
      }
    }

    handle(0, 0, 0);
    button1c->setDisabled(false);
    button2->setDisabled(false);
    button3->setDisabled(true);
    start_pos_ = {0, 0};

    RecomputeSolvability();
    RedrawStars(star_label);
    star_label->show();
    mode_label->show();

    QTimer::singleShot(0, this, [=](){ resize(sizeHint()); });
  };

  QObject::connect(this, &MainWindow::gameChanged, handle);

  QObject::connect(rand_min_box, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) {
    if (i > rand_max_box->value()) rand_max_box->setValue(i);
  });

  QObject::connect(rand_max_box, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) {
    if (i < rand_min_box->value()) rand_min_box->setValue(i);
  });

  QObject::connect(button1a, &QPushButton::clicked, [=]() {
    game_ = std::make_unique<Game>(height_box->value(), width_box->value());
    init_grid();
  });

  QObject::connect(button1b, &QPushButton::clicked, [=]() {
    int h = height_box->value(), w = width_box->value();
    int rmin = rand_min_box->value(), rmax = rand_max_box->value();
    if (rmax < rmin) rmax = rmin;

    if (rmax >= h * w) {
      QMessageBox::warning(
          this, "Invalid randomness parameters",
          QString("Too many blocked fields requested; no free fields would remain! You "
                  "may block at most %1 fields.").arg(h * w - 1));
      return;
    }
    game_ = std::make_unique<Game>(h, w);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    game_->AugmentRandomly(std::uniform_int_distribution(rmin, rmax)(rbg_), &rbg_);
    QApplication::restoreOverrideCursor();
    init_grid();
  });

  QObject::connect(button1c, &QPushButton::clicked, [=]() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    game_->AugmentRandomly(aug_box->value(), &rbg_);
    QApplication::restoreOverrideCursor();
    init_grid();
  });

  QObject::connect(button2, &QPushButton::clicked, [=]() {
    if (game_ != nullptr) {
      game_->Reset();
      handle(0, 0, 0);
      start_pos_ = {0, 0};
      mode_label->show();
      button1c->setDisabled(false);
      button3->setDisabled(true);
    }
  });

  QObject::connect(button3, &QPushButton::clicked, [=]() {
    if (game_ != nullptr) {
      game_->Reset();
      handle(0, 0, 0);
      handle(2, start_pos_.x, start_pos_.y);
      mode_label->show();
    }
  });

  QObject::connect(code_load, &QPushButton::clicked, [=]() {
    const std::string code = code_edit->text().toStdString();
    if (std::unique_ptr<Game> new_game = LoadFromHexString(code)) {
      game_ = std::move(new_game);
      init_grid();
    } else {
      printf("Error during loading of '%s'\n", code.c_str());
    }
  });

  QObject::connect(code_clip, &QPushButton::clicked, [=]() {
    const std::string code =
        QGuiApplication::clipboard()->text().trimmed().toStdString();
    if (std::unique_ptr<Game> new_game = LoadFromHexString(code)) {
      game_ = std::move(new_game);
      init_grid();
    }
  });

  QObject::connect(hint_button, &QPushButton::clicked, [=]() {
    if (game_ == nullptr) return;
    std::vector<int> s;
    if (!game_->IsSolvable(&s)) {
      QMessageBox::information(this, "Hint", QString("This layout is not solvable."));
    } else {
      printf("Solutions:\n");
      for (auto it = s.begin(); it != s.end(); ++it) {
        printf("- from (%d, ", *it++);
        printf("%d) move [ ", *it++);
        while (*it != 0) printf("%d ", *it++);
        printf("]\n");
      }
      // QMessageBox::question(this, "Get hint?", QString("Would you like to spend 75 tkoins for a hint?\n\n(You have 82.25 tkoins at the moment.)"));
      // QMessageBox::information(this, "Hint", QString("This layout can be solved from (%1, %2).").arg(s[0]).arg(s[1]));
    }
  });

  QObject::connect(help_button, &QPushButton::clicked, [=]() {
    QMessageBox::information(
        this, "How to play",
        "Switch all the unlit (red 'O') lights on (green 'X') by moving in "
        "straight lines.\n\n"
        "The game starts in \"layout mode\". When in layout mode, a left-click "
        "on an unlit light starts the game. Use the arrow keys to switch on "
        "lights in available directions.\n\n"
        "In layout-mode, a right-click marks a tile as \"blocked\". The \"new "
        "layout\", \"random layout\", \"random augment\", \"restart\" and "
        "\"load\" buttons return the game to layout mode.\n\n"
        "Random generation may take a very long time on large grids. Random "
        "augmentation may never complete if the starting layout is not "
        "solvable.");
  });

  QObject::connect(quit_button, &QPushButton::clicked, this, &QMainWindow::close);

  QObject::connect(&key_grabber_, &KeyGrabber::gameChanged, handle);

  QObject::connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this),
                   &QShortcut::activated, this, &QMainWindow::close);

  window->setLayout(main_layout);
  setCentralWidget(window);
}

void MainWindow::RedrawStars(QLabel* lbl) {
  const std::size_t found_count = sol_tracker_.FoundCount();
  const std::size_t total_count = sol_tracker_.TotalCount();

  if (total_count == 0) {
    lbl->setText(QString::fromUtf16(u"<font color='#A00'>\u274C</font>"));
    lbl->setToolTip("This layout is unsolvable.");
  } else {
    QString star = QString::fromUtf16(u"\u2605");
    QString t = "Found starts:";
    for (const Game::Coord& pos : sol_tracker_.FoundSolutions()) {
      t.append(QString(" [%1, %2]").arg(pos.x).arg(pos.y));
    }
    if (found_count == total_count) {
      lbl->setText(
          QString("<font color='#DAA520' style='text-decoration: overline underline;'>%1</font>")
              .arg(star.repeated(found_count)));
    } else {
      lbl->setText(
          QString("<font color='#DAA520'>%1</font><font color='#AAA'>%2</font>")
              .arg(star.repeated(found_count))
              .arg(star.repeated(total_count - found_count)));
    }
    lbl->setToolTip(t);
  }
}

void MainWindow::RecomputeSolvability() {
  sol_tracker_.RecomputeFromGame(game_.get());
}

}  //  namespace tkware::lightgame
