#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

#define INT_RANGE_LOW 1 << 12
#define INT_RANGE_HIGH 1 << 30
#define STRING_RANGE_LOW 1 << 8
#define STRING_RANGE_HIGH 1 << 18

#define CREATE_ITEMS(type, state)                                              \
  state.PauseTiming();                                                         \
  auto items = CreateSequence<type>(state.range(0));                           \
  state.ResumeTiming();

namespace fcpp::benchmarks {

template <typename T>
T CreateItem();

template <>
int CreateItem() {
  return std::rand();
}

template <>
std::string CreateItem() {
  int length = std::rand() % 20;
  std::string item;
  item.reserve(length);
  for (int i = 0; i < length; i++) {
    item[i] = (std::rand() % (126 - 32)) + 32;
  }
  return item;
}

template <typename T>
std::vector<T> CreateSequence(size_t size) {
  std::vector<T> sequence;
  sequence.reserve(size);
  for (int i = 0; i < size; i++) {
    sequence.push_back(CreateItem<T>());
  }
  return sequence;
}

} // namespace fcpp::benchmarks