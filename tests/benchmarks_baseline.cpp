#include "benchmarks_common.h"
#include "query.h"

#include <benchmark/benchmark.h>
#include <random>
#include <string>
#include <vector>

namespace fcpp::benchmarks::baseline {

namespace {

static void BM_Select_int(benchmark::State &state) {
  for (auto _ : state) {
    CREATE_ITEMS(int, state)
    std::vector<int> result;
    result.reserve(items.size());
    std::transform(
        items.begin(), items.end(), std::back_inserter(result),
        [](auto x) { return x + 1; });
  }
}
BENCHMARK(BM_Select_int)->Range(INT_RANGE_LOW, INT_RANGE_HIGH);

static void BM_Select_string(benchmark::State &state) {
  for (auto _ : state) {
    CREATE_ITEMS(std::string, state)
    std::vector<std::string> result;
    result.reserve(items.size());
    std::transform(
        items.begin(), items.end(), std::back_inserter(result),
        [](auto x) { return x + x; });
  }
}
BENCHMARK(BM_Select_string)->Range(STRING_RANGE_LOW, STRING_RANGE_HIGH);

} // namespace

} // namespace fcpp::benchmarks::baseline

BENCHMARK_MAIN();
