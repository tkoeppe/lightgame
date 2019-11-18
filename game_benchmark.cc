#include "game.h"

#include <cassert>

#include "benchmark/benchmark.h"

namespace tkware::lightgame {
namespace {

void BM_SolveLargeGame(benchmark::State& state) {
  Game game(7, 9);
  game.SetBlocked(3, 3);
  for (auto _ : state) {
    bool b = game.IsSolvable(nullptr);
    benchmark::DoNotOptimize(b);
    assert(b);
  }
}

BENCHMARK(BM_SolveLargeGame);

void BM_GenerateLargeGames(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    Game game(5, 7);
    std::mt19937 rbg(1001);
    state.ResumeTiming();
    for (int i : {4, 5}) {
      bool b = game.AugmentRandomly(i, &rbg);
      benchmark::DoNotOptimize(b);
      assert(b);
    }
  }
}

BENCHMARK(BM_GenerateLargeGames);

}  // namespace
}  // namespace tkware::lightgame
