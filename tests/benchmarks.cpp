#include "query.h"

#include <benchmark/benchmark.h>
#include <chrono>
#include <random>
#include <thread>
#include <vector>

#define RANGE_LOW 1 << 10
#define RANGE_HIGH 1 << 26

namespace {

std::vector<int> CreateSequence(size_t size) {
  std::vector<int> sequence;
  sequence.reserve(size);
  for (int i = 0; i < size; i++) {
    sequence.push_back(std::rand());
  }
  return sequence;
}

static void BM_Select_int_Std(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto items = CreateSequence(state.range(0));
    state.ResumeTiming();
    std::vector<int> result;
    result.reserve(items.size());
    std::transform(
        items.begin(), items.end(), std::back_inserter(result),
        [](auto x) { return x + 1; });
  }
}
BENCHMARK(BM_Select_int_Std)->Range(RANGE_LOW, RANGE_HIGH);

static void BM_Select_int_FCpp(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto items = CreateSequence(state.range(0));
    state.ResumeTiming();
    fcpp::query(std::move(items)).select([](auto x) { return x + 1; });
  }
}
BENCHMARK(BM_Select_int_FCpp)->Range(RANGE_LOW, RANGE_HIGH);

} // namespace

BENCHMARK_MAIN();
