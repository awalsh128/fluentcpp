#include "query.h"

#include <iostream>
#include <vector>

int main()
{
  // @todo Add descriptions and use cases.
  std::vector<int> xs;
  for (int x = 0; x < 1000; x++)
    xs.push_back(x);

  fcpp::query(xs)
      .where(EXPR(x, x > 500))
      .shuffle()
      .skip(10)
      .select(EXPR(x, x % 10))
      .distinct()
      .order_by(EXPR(x, x))
      .branch(EXPR(x, x > 5))
      .when_true([](auto q)
                 { return q.select(EXPR(x, x + 100)); })
      .when_false([](auto q)
                  { return q.select(EXPR(x, x - 100)); })
      .merge()
      .select(EXPR(x, std::get<0>(x)))
      .action([](auto x)
              { std::cout << x; });
}