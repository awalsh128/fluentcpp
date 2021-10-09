#include "query.h"

#include <iostream>
#include <string_view>
#include <vector>

void print(std::string_view title, const std::vector<int> items) {
  std::cout << title << "\n";
  for (int x : items) {
    std::cout << x << " ";
  }
  std::cout << "\n\n";
}

void complex_query() {
  std::vector<int> xs;
  for (int x = 0; x < 1000; x++) xs.push_back(x);

  print(
      "complex_query",
      fcpp::query(xs)
          .where(EXPR(x, x > 500))
          .shuffle()
          .skip(10)
          .select(EXPR(x, x % 10))
          .distinct()
          .order_by(EXPR(x, x))
          .branch(EXPR(x, x > 5))
          .when_true([](auto q) { return q.select(EXPR(x, x + 100)); })
          .when_false([](auto q) { return q.select(EXPR(x, x - 100)); })
          .merge()
          .select(EXPR(x, std::get<0>(x)))
          .to_vector());
}

void simple_filter() {
  // Return value is an rvalue reference that gets moved into result.
  std::vector<int> result =
      std::move(fcpp::query<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
                    .where(EXPR(x, x % 2 == 0))
                    .select(EXPR(x, x + 2)));

  print("simple_filter", result);
}

int main() {
  // @todo Add descriptions and use cases.
  complex_query();
  simple_filter();
}