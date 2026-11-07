#include "benchmarks_common.h"
#include "fluentcpp/query.h"

#include <benchmark/benchmark.h>
#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace fcpp::benchmarks {

namespace {

static void BM_Select_int(benchmark::State &state) {
  for (auto _ : state) {
    CREATE_ITEMS(int, state)
    fcpp::query(std::move(items)).select([](auto x) { return x + 1; });
  }
}
BENCHMARK(BM_Select_int)->Range(INT_RANGE_LOW, INT_RANGE_HIGH);

static void BM_Select_string(benchmark::State &state) {
  for (auto _ : state) {
    CREATE_ITEMS(std::string, state)
    fcpp::query(std::move(items)).select([](auto x) { return x + x; });
  }
}
BENCHMARK(BM_Select_string)->Range(STRING_RANGE_LOW, STRING_RANGE_HIGH);

} // namespace

} // namespace fcpp::benchmarks

BENCHMARK_MAIN();
