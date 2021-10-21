#include "models.h"
#include "query.h"

#include <memory>
#include <ostream>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
// Enables string makers for pair and tuple among others.
#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#include <catch2/catch_tostring.hpp>

namespace fcpp::tests {

namespace {

using ::fcpp::tests::models::NonCopyObject;
using ::fcpp::tests::models::Object;

template <typename T> std::vector<T> Create(std::initializer_list<int> items) {
  std::vector<T> created;
  created.reserve(items.size());
  for (int item : items) {
    created.push_back(T(item));
  }
  return std::move(created);
}

template <typename T>
std::vector<std::vector<T>>
Create(std::initializer_list<std::initializer_list<int>> items) {
  std::vector<std::vector<T>> created;
  for (std::initializer_list<int> item : items) {
    created.push_back(Create<T>(item));
  }
  return std::move(created);
}

TEMPLATE_TEST_CASE("accumulate", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3}))
              .accumulate(1, [](auto x, auto &y) { return x + y.value; }) == 7);
}

TEMPLATE_TEST_CASE("accumulate empty", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(std::vector<TestType>()).accumulate(1, [](auto x, auto &y) {
        return x + y;
      }) == 1);
}

TEMPLATE_TEST_CASE("all empty", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(std::vector<TestType>()).all([](const auto &x) {
    return static_cast<bool>(x);
  }));
}

TEMPLATE_TEST_CASE("all false", "", Object, NonCopyObject) {
  REQUIRE_FALSE(fcpp::query(Create<TestType>({1, 2})).all([](const auto &x) {
    return x == 1;
  }));
}

TEMPLATE_TEST_CASE("all true", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).all([](const auto &x) {
    return x < 3;
  }));
}

TEMPLATE_TEST_CASE("any empty", "", Object, NonCopyObject) {
  REQUIRE_FALSE(fcpp::query(std::vector<TestType>()).any([](const auto &x) {
    return true;
  }));
}

TEMPLATE_TEST_CASE("any false", "", Object, NonCopyObject) {
  REQUIRE_FALSE(fcpp::query(Create<TestType>({1, 2})).any([](const auto &x) {
    return x > 2;
  }));
}

TEMPLATE_TEST_CASE("any true", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).any([](const auto &x) {
    return x == 1;
  }));
}

TEMPLATE_TEST_CASE("branch", "", Object, NonCopyObject) {
  std::vector<std::tuple<TestType, TestType>> expected;
  expected.push_back({3, 0});
  expected.push_back({5, 2});

  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3, 4}))
              .branch([](const auto &x) { return x % 2 == 0; })
              .when_true([](auto q) {
                return q.select([](auto &&x) { return TestType(x.value + 1); });
              })
              .when_false([](auto q) {
                return q.select([](auto &&x) { return TestType(x.value - 1); });
              })
              .merge()
              .to_vector() == expected);
}

TEMPLATE_TEST_CASE("difference empty", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(std::vector<TestType>())
              .difference(std::vector<TestType>())
              .to_vector() == std::vector<TestType>());
}

TEMPLATE_TEST_CASE("difference all", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2}))
              .difference(Create<TestType>({3}))
              .to_vector() == Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("difference none", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2}))
              .difference(Create<TestType>({1, 2}))
              .to_vector() == std::vector<TestType>());
}

TEMPLATE_TEST_CASE("difference some", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 1}))
              .difference(Create<TestType>({1}))
              .to_vector() == Create<TestType>({1}));
}

TEMPLATE_TEST_CASE("distinct multiple", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).distinct().to_vector() ==
          Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("distinct multiset", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 1})).distinct().to_vector() ==
          Create<TestType>({1}));
}

TEMPLATE_TEST_CASE("first_or_default default", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 1})).first_or_default([](const auto &x) {
        return x != 1;
      }) == std::nullopt);
}

TEMPLATE_TEST_CASE("first_or_default value", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 1})).first_or_default([](const auto &x) {
        return x == 1;
      }) == 1);
}

TEMPLATE_TEST_CASE("flatten", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({{1, 2}, {3, 4}})).flatten().to_vector() ==
      Create<TestType>({1, 2, 3, 4}));
}

TEMPLATE_TEST_CASE("flatten empty", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(std::vector<std::vector<TestType>>()).flatten().to_vector() ==
      std::vector<TestType>());
}

TEMPLATE_TEST_CASE("group_by multiple", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3, 4}))
              .group_by([](const auto &x) { return x % 2 == 0; })
              .to_vector() == Create<TestType>({{1, 3}, {2, 4}}));
}

TEMPLATE_TEST_CASE("group_by single", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3, 4}))
              .group_by([](const auto &x) { return true; })
              .to_vector() == Create<TestType>({{1, 2, 3, 4}}));
}

TEMPLATE_TEST_CASE("intersect", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2}))
              .intersect(Create<TestType>({2, 3}))
              .to_vector() == Create<TestType>({2}));
}

TEMPLATE_TEST_CASE("intersect all", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2}))
              .intersect(Create<TestType>({1, 2}))
              .to_vector() == Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("intersect none", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2}))
              .intersect(Create<TestType>({2, 3}))
              .to_vector() == Create<TestType>({2}));
}

TEMPLATE_TEST_CASE("join", "", Object, NonCopyObject) {
  std::vector<std::tuple<TestType, TestType>> expected;
  expected.push_back({1, 3});
  expected.push_back({2, 2});

  REQUIRE(fcpp::query(Create<TestType>({1, 2}))
              .join(
                  Create<TestType>({2, 3}),
                  [](const auto &x) { return x % 2 == 0; },
                  [](const auto &x) { return x % 2 == 0; })
              .to_vector() == expected);
}

TEMPLATE_TEST_CASE("keyed_group_by", "", Object, NonCopyObject) {
  std::vector<std::pair<bool, std::vector<TestType>>> expected;
  expected.push_back(std::make_pair(false, Create<TestType>({1})));
  expected.push_back(std::make_pair(true, Create<TestType>({2})));

  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).keyed_group_by([](const auto &x) {
        return x % 2 == 0;
      }) == expected);
}

TEMPLATE_TEST_CASE("max", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 3, 2})).max() == 3);
}

TEMPLATE_TEST_CASE("max lambda", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 3, 2})).max([](const auto &x) {
    return x == 3 ? 0 : 3;
  }) == 1);
}

TEMPLATE_TEST_CASE("min", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({2, 1, 3})).min() == 1);
}

TEMPLATE_TEST_CASE("min lambda", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({2, 1, 3})).min([](auto &x) {
    return x == 1 ? 4 : 1;
  }) == 2);
}

TEMPLATE_TEST_CASE("order_by", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3}))
              .order_by([](auto &x) { return x % 2; })
              .to_vector() == Create<TestType>({2, 1, 3}));
}

TEMPLATE_TEST_CASE("reverse", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3})).reverse().to_vector() ==
          Create<TestType>({3, 2, 1}));
}

TEMPLATE_TEST_CASE("reverse empty", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(std::vector<TestType>()).reverse().to_vector() ==
          std::vector<TestType>());
}

TEMPLATE_TEST_CASE("select", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3}))
              .select([](auto &&x) { return x + 100; })
              .to_vector() == Create<TestType>({101, 102, 103}));
}

TEMPLATE_TEST_CASE("shuffle", "", Object, NonCopyObject) {
  REQUIRE_FALSE(
      fcpp::query(Create<TestType>({1, 2, 3, 4, 5, 6, 7, 8, 9})).shuffle() ==
      Create<TestType>({1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEMPLATE_TEST_CASE("size", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).size() == 2);
}

TEMPLATE_TEST_CASE("size empty", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(std::vector<TestType>()).size() == 0);
}

TEMPLATE_TEST_CASE("skip", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).skip(1).to_vector() ==
          Create<TestType>({2}));
}

TEMPLATE_TEST_CASE("skip none", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).skip(0).to_vector() ==
          Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("slice", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3, 4, 5, 6, 7}))
              .slice(2, 3, 2)
              .to_vector() == Create<TestType>({3, 5, 7}));
}

TEMPLATE_TEST_CASE("slice no-stride", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4, 5})).slice(2, 3).to_vector() ==
      Create<TestType>({3, 4, 5}));
}

TEMPLATE_TEST_CASE("slice zero-stride", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3})).slice(2, 3, 0).to_vector() ==
          Create<TestType>({3, 3, 3}));
}

TEMPLATE_TEST_CASE("take", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).take(1).to_vector() ==
          Create<TestType>({1}));
}

TEMPLATE_TEST_CASE("take all", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).take(2).to_vector() ==
          Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("take none", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).take(0).to_vector() ==
          std::vector<TestType>());
}

TEMPLATE_TEST_CASE("take_random", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).take_random(1).size() == 1);
}

TEMPLATE_TEST_CASE("take_random none", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).take_random(0).to_vector() ==
          std::vector<TestType>());
}

TEMPLATE_TEST_CASE("to_multi_value_map", "", Object, NonCopyObject) {
  std::map<int, std::vector<TestType>> expected;
  expected[1] = Create<TestType>({1, 3});
  expected[0] = Create<TestType>({2});

  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3}))
              .to_multi_value_map([](const auto &x) { return x.value % 2; }) ==
          expected);
}

TEMPLATE_TEST_CASE("to_single_value_map", "", Object, NonCopyObject) {
  std::map<int, TestType> expected;
  expected[1] = 1;
  expected[0] = 2;

  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3}))
              .to_single_value_map([](const auto &x) { return x.value % 2; }) ==
          expected);
}

TEMPLATE_TEST_CASE("to_set", "", Object, NonCopyObject) {
  std::set<TestType> expected;
  expected.insert(1);
  expected.insert(2);
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 2, 1})).to_set() == expected);
}

TEMPLATE_TEST_CASE("to_vector", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).to_vector() ==
          Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("unionize", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2}))
              .unionize(Create<TestType>({2, 3}))
              .to_vector() == Create<TestType>({1, 2, 3}));
}

TEMPLATE_TEST_CASE("where", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3, 4}))
              .where([](const auto &x) { return x.value % 2 == 0; })
              .to_vector() == Create<TestType>({2, 4}));
}

TEMPLATE_TEST_CASE("zip", "", Object, NonCopyObject) {
  std::vector<std::tuple<TestType, TestType>> expected;
  expected.push_back({1, 5});
  expected.push_back({2, 6});
  expected.push_back({3, 7});
  expected.push_back({4, 8});
  expected.push_back({0, 9});

  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3, 4}))
              .zip(Create<TestType>({5, 6, 7, 8, 9}), /*truncate=*/false)
              .to_vector() == expected);
}

TEMPLATE_TEST_CASE("zip truncate", "", Object, NonCopyObject) {
  std::vector<std::tuple<TestType, TestType>> expected;
  expected.push_back({1, 5});
  expected.push_back({2, 6});
  expected.push_back({3, 7});
  expected.push_back({4, 8});

  REQUIRE(fcpp::query(Create<TestType>({1, 2, 3, 4}))
              .zip(Create<TestType>({5, 6, 7, 8, 9}), /*truncate=*/true)
              .to_vector() == expected);
}

} // namespace

} // namespace fcpp::tests