#include "game.h"

#include <memory>
#include <random>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace tkware::lightgame {
namespace {

TEST(Game, StartAndSolve) {
  // A simple 2x3 grid, no blocked tiles:
  // +--+--+--+
  // |St|  |  |
  // +--+--+--+
  // |  |  |  |
  // +--+--+--+

  Game game(2, 3);
  EXPECT_EQ(game.Height(), 2);
  EXPECT_EQ(game.Width(), 3);

  EXPECT_TRUE(game.IsSolvable(nullptr));
  EXPECT_TRUE(game.Start(1, 1));
  EXPECT_TRUE(game.HasStarted());

  EXPECT_EQ(game.X(), 1);
  EXPECT_EQ(game.Y(), 1);

  EXPECT_TRUE(game.Move(Game::kRight));
  EXPECT_EQ(game.X(), 3);
  EXPECT_EQ(game.Y(), 1);

  EXPECT_TRUE(game.Move(Game::kDown));
  EXPECT_EQ(game.X(), 3);
  EXPECT_EQ(game.Y(), 2);

  EXPECT_TRUE(game.Move(Game::kLeft));
  EXPECT_EQ(game.X(), 1);
  EXPECT_EQ(game.Y(), 2);

  EXPECT_EQ(game.ValidDirs(), Game::kNone);
  EXPECT_TRUE(game.HaveWon());
}

TEST(Game, UnsolvableLayout) {
  // An unsolvable 3x3 grid@
  // +--+--+--+
  // |##|  |##|
  // +--+--+--+
  // |  |  |  |
  // +--+--+--+
  // |##|  |##|
  // +--+--+--+
  Game game(3, 3);
  EXPECT_EQ(game.Height(), 3);
  EXPECT_EQ(game.Width(), 3);

  EXPECT_TRUE(game.SetBlocked(1, 1));
  EXPECT_TRUE(game.SetBlocked(1, 3));
  EXPECT_TRUE(game.SetBlocked(3, 1));
  EXPECT_TRUE(game.SetBlocked(3, 3));

  EXPECT_FALSE(game.IsSolvable(nullptr));
}

TEST(Game, InvalidOperations) {
  Game game(2, 3);
  EXPECT_FALSE(game.SetBlocked(3, 3));  // out of bound
  EXPECT_TRUE(game.SetBlocked(3, 2));
  EXPECT_FALSE(game.Start(3, 3));       // out of bounds
  EXPECT_FALSE(game.Start(3, 2));       // field blocked
  EXPECT_TRUE(game.Start(3, 1));
  EXPECT_FALSE(game.Start(3, 1));       // already started
  EXPECT_FALSE(game.SetBlocked(3, 1));  // already started
}

TEST(Game, ValidSolutions) {
  // Same layout as above. Every tile is a winning start.
  Game game(2, 3);
  SolutionTracker tracker;
  tracker.RecomputeFromGame(&game);
  EXPECT_EQ(tracker.TotalCount(), 6);
}

TEST(Game, LoadSave) {
  std::mt19937 rbg(1001);
  Game game(5, 7);

  // 5 * 7 = 35, so we need 9 hex digits (9 * 4 >= 35).
  EXPECT_EQ(SaveToHexString(game), "57000000000");
  for (int n : {3, 7, 5}) {
    game.AugmentRandomly(n, &rbg);
    std::string code = SaveToHexString(game);

    std::unique_ptr<Game> loaded_game = LoadFromHexString(code);
    ASSERT_TRUE(loaded_game != nullptr);
    ASSERT_EQ(game.Height(), loaded_game->Height());
    ASSERT_EQ(game.Width(), loaded_game->Width());
    for (int y = 1; y <= game.Height(); ++y) {
      for (int x = 1; x <= game.Width(); ++x) {
        EXPECT_EQ(game.At(x, y), loaded_game->At(x, y))
            << "at (" << x << ", " << y << ")";
      }
    }
  }
}

}  // namespace
}  // namespace tkware::lightgame
