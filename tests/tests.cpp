#include "models.h"
// Included before query.h to verify both headers can be included together.
#include "fluentcpp/query.h"
#include "fluentcpp/transforms.h"

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
// Enables string makers for pair and tuple among others.
#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#include <catch2/catch_tostring.hpp>

namespace fcpp::tests {

namespace {

using ::fcpp::tests::models::NonCopyObject;
using ::fcpp::tests::models::Object;

template <typename T>
std::vector<T> Create(std::initializer_list<int> items) {
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
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3}))
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

  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4}))
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
  REQUIRE(
      fcpp::query(std::vector<TestType>())
          .difference(std::vector<TestType>())
          .to_vector() == std::vector<TestType>());
}

TEMPLATE_TEST_CASE("difference all", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2}))
          .difference(Create<TestType>({3}))
          .to_vector() == Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("difference none", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2}))
          .difference(Create<TestType>({1, 2}))
          .to_vector() == std::vector<TestType>());
}

TEMPLATE_TEST_CASE("difference some", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 1}))
          .difference(Create<TestType>({1}))
          .to_vector() == Create<TestType>({1}));
}

TEMPLATE_TEST_CASE("distinct multiple", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).distinct().to_vector() ==
      Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("distinct multiset", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 1})).distinct().to_vector() ==
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
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4}))
          .group_by([](const auto &x) { return x % 2 == 0; })
          .to_vector() == Create<TestType>({{1, 3}, {2, 4}}));
}

TEMPLATE_TEST_CASE("group_by single", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4}))
          .group_by([](const auto &x) { return true; })
          .to_vector() == Create<TestType>({{1, 2, 3, 4}}));
}

TEMPLATE_TEST_CASE("intersect", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2}))
          .intersect(Create<TestType>({2, 3}))
          .to_vector() == Create<TestType>({2}));
}

TEMPLATE_TEST_CASE("intersect all", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2}))
          .intersect(Create<TestType>({1, 2}))
          .to_vector() == Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("intersect none", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2}))
          .intersect(Create<TestType>({2, 3}))
          .to_vector() == Create<TestType>({2}));
}

TEMPLATE_TEST_CASE("join", "", Object, NonCopyObject) {
  std::vector<std::tuple<TestType, TestType>> expected;
  expected.push_back({1, 3});
  expected.push_back({2, 2});

  REQUIRE(
      fcpp::query(Create<TestType>({1, 2}))
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
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3}))
          .order_by([](auto &x) { return x % 2; })
          .to_vector() == Create<TestType>({2, 1, 3}));
}

TEMPLATE_TEST_CASE("reverse", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3})).reverse().to_vector() ==
      Create<TestType>({3, 2, 1}));
}

TEMPLATE_TEST_CASE("reverse empty", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(std::vector<TestType>()).reverse().to_vector() ==
      std::vector<TestType>());
}

TEMPLATE_TEST_CASE("select", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3}))
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
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).skip(1).to_vector() ==
      Create<TestType>({2}));
}

TEMPLATE_TEST_CASE("skip none", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).skip(0).to_vector() ==
      Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("slice", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4, 5, 6, 7}))
          .slice(2, 3, 2)
          .to_vector() == Create<TestType>({3, 5, 7}));
}

TEMPLATE_TEST_CASE("slice no-stride", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4, 5})).slice(2, 3).to_vector() ==
      Create<TestType>({3, 4, 5}));
}

TEST_CASE("slice zero-stride") {
  REQUIRE(
      fcpp::query(Create<Object>({1, 2, 3})).slice(2, 3, 0).to_vector() ==
      Create<Object>({3, 3, 3}));
}

TEST_CASE("slice zero-stride non-copyable throws") {
  REQUIRE_THROWS_AS(
      fcpp::query(Create<NonCopyObject>({1, 2, 3})).slice(2, 3, 0),
      std::invalid_argument);
}

TEMPLATE_TEST_CASE("take", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).take(1).to_vector() ==
      Create<TestType>({1}));
}

TEMPLATE_TEST_CASE("take all", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).take(2).to_vector() ==
      Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("take none", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).take(0).to_vector() ==
      std::vector<TestType>());
}

TEMPLATE_TEST_CASE("take_random", "", Object, NonCopyObject) {
  REQUIRE(fcpp::query(Create<TestType>({1, 2})).take_random(1).size() == 1);
}

TEMPLATE_TEST_CASE("take_random none", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).take_random(0).to_vector() ==
      std::vector<TestType>());
}

TEMPLATE_TEST_CASE("to_multi_value_map", "", Object, NonCopyObject) {
  std::map<int, std::vector<TestType>> expected;
  expected[1] = Create<TestType>({1, 3});
  expected[0] = Create<TestType>({2});

  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3}))
          .to_multi_value_map([](const auto &x) { return x.value % 2; }) ==
      expected);
}

TEMPLATE_TEST_CASE("to_single_value_map", "", Object, NonCopyObject) {
  std::map<int, TestType> expected;
  expected[1] = 1;
  expected[0] = 2;

  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3}))
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
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2})).to_vector() ==
      Create<TestType>({1, 2}));
}

TEMPLATE_TEST_CASE("unionize", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2}))
          .unionize(Create<TestType>({2, 3}))
          .to_vector() == Create<TestType>({1, 2, 3}));
}

TEMPLATE_TEST_CASE("where", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4}))
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

  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4}))
          .zip(Create<TestType>({5, 6, 7, 8, 9}), /*truncate=*/false)
          .to_vector() == expected);
}

TEMPLATE_TEST_CASE("zip truncate", "", Object, NonCopyObject) {
  std::vector<std::tuple<TestType, TestType>> expected;
  expected.push_back({1, 5});
  expected.push_back({2, 6});
  expected.push_back({3, 7});
  expected.push_back({4, 8});

  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3, 4}))
          .zip(Create<TestType>({5, 6, 7, 8, 9}), /*truncate=*/true)
          .to_vector() == expected);
}

// Regression tests using std::string, which unlike the int based models above
// is left empty when moved from. This catches items being moved before use.
using Strings = std::vector<std::string>;

TEST_CASE("where by-value predicate keeps items") {
  REQUIRE(
      fcpp::query(Strings{"apple", "bob", "carrot"})
          .where(EXPR(s, s.size() > 3))
          .to_vector() == Strings{"apple", "carrot"});
}

TEST_CASE("first_or_default by-value predicate keeps item") {
  REQUIRE(
      fcpp::query(Strings{"apple", "bob"})
          .first_or_default(EXPR(s, s == "bob")) == "bob");
}

TEST_CASE("operator== different sizes") {
  REQUIRE_FALSE(fcpp::query(Strings{"a", "b"}) == Strings{"a"});
  REQUIRE_FALSE(fcpp::query(Strings{"a"}) == Strings{"a", "b"});
  REQUIRE_FALSE(fcpp::query(Strings{"a"}) == fcpp::query(Strings{"a", "b"}));
}

TEST_CASE("intersect unsorted") {
  REQUIRE(
      fcpp::query(Strings{"c", "a", "b", "c"})
          .intersect(Strings{"c", "b"})
          .to_vector() == Strings{"c", "b"});
}

TEST_CASE("difference unsorted") {
  REQUIRE(
      fcpp::query(Strings{"c", "a", "b", "c"})
          .difference(Strings{"c", "b"})
          .to_vector() == Strings{"a", "c"});
}

TEST_CASE("join one-to-many and many-to-one") {
  std::vector<std::tuple<std::string, std::string>> expected = {
      {"a1", "a2"}, {"a1", "a3"}, {"a4", "a2"}, {"a4", "a3"}};
  REQUIRE(
      fcpp::query(Strings{"a1", "a4"})
          .join(Strings{"a2", "a3"}, CR_EXPR(s, s[0]), CR_EXPR(s, s[0]))
          .to_vector() == expected);
}

TEST_CASE("join different item types") {
  std::vector<std::tuple<Object, std::string>> expected = {{1, "1"}};
  REQUIRE(
      fcpp::query(Create<Object>({1, 2}))
          .join(Strings{"1"}, CR_EXPR(o, o.value), CR_EXPR(s, std::stoi(s)))
          .to_vector() == expected);
}

TEST_CASE("join one-to-many non-copyable throws") {
  REQUIRE_THROWS_AS(
      fcpp::query(Create<NonCopyObject>({1}))
          .join(
              Create<NonCopyObject>({1, 1}), CR_EXPR(x, x.value),
              CR_EXPR(x, x.value)),
      std::invalid_argument);
}

TEST_CASE("slice past end throws") {
  REQUIRE_THROWS_AS(
      fcpp::query(Strings{"x", "y", "z"}).slice(1, 3), std::invalid_argument);
}

TEST_CASE("slice zero-stride strings") {
  REQUIRE(
      fcpp::query(Strings{"x", "y"}).slice(0, 3, 0).to_vector() ==
      Strings{"x", "x", "x"});
}

TEMPLATE_TEST_CASE("trim", "", Object, NonCopyObject) {
  REQUIRE(
      fcpp::query(Create<TestType>({1, 2, 3})).trim(1).to_vector() ==
      Create<TestType>({1, 2}));
}

TEST_CASE("trim too many throws") {
  REQUIRE_THROWS_AS(fcpp::query(Strings{"x"}).trim(2), std::invalid_argument);
}

struct NoDefault {
  int value;
  explicit NoDefault(int value) : value(value) {}
  bool operator==(const NoDefault &rhs) const { return value == rhs.value; }
};

TEST_CASE("zip truncate without default constructor") {
  std::vector<NoDefault> lhs;
  lhs.emplace_back(1);
  lhs.emplace_back(2);
  std::vector<NoDefault> rhs;
  rhs.emplace_back(3);
  auto zipped = fcpp::query(std::move(lhs))
                    .zip(std::move(rhs), /*truncate=*/true)
                    .to_vector();
  REQUIRE(zipped.size() == 1);
  REQUIRE(std::get<0>(zipped[0]).value == 1);
  REQUIRE(std::get<1>(zipped[0]).value == 3);
}

TEST_CASE("implicit conversion moves items") {
  Strings result = fcpp::query(Strings{"a", "b"}).reverse();
  REQUIRE(result == Strings{"b", "a"});
}

TEST_CASE("order_by is stable and supports descending") {
  REQUIRE(
      fcpp::query(Strings{"bb", "a", "cc", "d"})
          .order_by(CR_EXPR(s, s.size()), /*descending=*/true)
          .to_vector() == Strings{"bb", "cc", "a", "d"});
}

TEST_CASE("distinct and unionize strings") {
  REQUIRE(
      fcpp::query(Strings{"b", "a", "b"}).distinct().to_vector() ==
      Strings{"a", "b"});
  REQUIRE(
      fcpp::query(Strings{"b", "a"}).unionize(Strings{"c", "a"}).to_vector() ==
      Strings{"a", "b", "c"});
}

TEST_CASE("take_random returns distinct items") {
  auto taken = fcpp::query(Strings{"a", "b", "c", "d"}).take_random(4).to_set();
  REQUIRE(taken == std::set<std::string>{"a", "b", "c", "d"});
}

TEST_CASE("group_by and to_single_value_map keep items") {
  REQUIRE(
      fcpp::query(Strings{"ab", "c", "de"})
          .group_by(EXPR(s, s.size()))
          .to_vector() == std::vector<Strings>{{"c"}, {"ab", "de"}});
  REQUIRE(
      fcpp::query(Strings{"ab", "c", "de"})
          .to_single_value_map(EXPR(s, s.size())) ==
      std::map<size_t, std::string>{{1, "c"}, {2, "ab"}});
}

TEST_CASE("branch keeps items") {
  std::vector<std::tuple<std::string, std::string>> expected = {{"apple", "b"}};
  REQUIRE(
      fcpp::query(Strings{"apple", "b"})
          .branch(EXPR(s, s.size() > 1))
          .when_true([](auto q) { return std::move(q); })
          .when_false([](auto q) { return std::move(q); })
          .merge()
          .to_vector() == expected);
}

} // namespace

} // namespace fcpp::tests
