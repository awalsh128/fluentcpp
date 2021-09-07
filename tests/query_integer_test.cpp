#include <query.h>

#include <catch2/catch_test_macros.hpp>

struct NonCopyableObject {
  int x;
  std::string y;

  NonCopyableObject() : x(0), y("") {}
  NonCopyableObject(int x, std::string y) : x(x), y(y) {}

  // @todo Restore this once constructor detection and resolution is built in.
  NonCopyableObject(NonCopyableObject &&) = default;     // move assignment
  NonCopyableObject(const NonCopyableObject &) = delete; // copy constructor
  NonCopyableObject &
  operator=(const NonCopyableObject &) = delete; // copy assignment

  inline bool operator==(const NonCopyableObject &a) const {
    return a.x == x && a.y == y;
  }
  inline bool operator>(const NonCopyableObject &a) const { return a.x > x; }
  inline bool operator<(const NonCopyableObject &a) const { return a.x < x; }
  inline bool operator+(const NonCopyableObject &a) const { return a.x + x; }
};

TEST_CASE("accumulate") {
  REQUIRE(fcpp::query({1, 2, 3}).accumulate(
              1, [](auto x, auto y) { return x + y; }) == 7);
}

TEST_CASE("accumulate empty") {
  REQUIRE(fcpp::query<int>({}).accumulate(
              1, [](auto x, auto y) { return x + y; }) == 1);
}

TEST_CASE("all empty") {
  REQUIRE(fcpp::query<int>({}).all([](auto x) { return x; }));
}

TEST_CASE("all false") {
  REQUIRE_FALSE(fcpp::query({1, 2}).all([](auto x) { return x == 1; }));
}

TEST_CASE("all true") {
  REQUIRE(fcpp::query({1, 2}).all([](auto x) { return x < 3; }));
}

TEST_CASE("branch") {
  REQUIRE(fcpp::query({1, 2, 3, 4})
              .branch([](auto x) { return x % 2 == 0; })
              .when_true(
                  [](auto q) { return q.select([](auto x) { return --x; }); })
              .when_false(
                  [](auto q) { return q.select([](auto x) { return ++x; }); })
              .merge()
              .to_vector() ==
          std::vector<std::tuple<int, int>>{{1, 2}, {3, 4}});
}

TEST_CASE("difference empty") {
  REQUIRE(fcpp::query<int>({}).difference({}).empty());
}

TEST_CASE("difference all") {
  REQUIRE(fcpp::query({1, 2}).difference({3}).to_vector() ==
          std::vector<int>{1, 2});
}

TEST_CASE("difference none") {
  REQUIRE(fcpp::query({1, 1}).difference({1}).to_vector() ==
          std::vector<int>{1});
}

TEST_CASE("distinct multiple") {
  REQUIRE(fcpp::query({1, 2}).distinct().to_vector() == std::vector<int>{1, 2});
}

TEST_CASE("distinct multiset") {
  REQUIRE(fcpp::query({1, 1}).distinct().to_vector() == std::vector<int>{1});
}

TEST_CASE("first_or_default default") {
  REQUIRE(fcpp::query({1, 1}).first_or_default([](auto x) { return x != 1; }) ==
          std::nullopt);
}

TEST_CASE("first_or_default value") {
  REQUIRE(fcpp::query({1, 1}).first_or_default([](auto x) { return x == 1; }) ==
          1);
}

TEST_CASE("flatten") {
  REQUIRE(fcpp::query(std::vector<std::vector<int>>{{1, 2}, {3, 4}})
              .flatten()
              .to_vector() == std::vector<int>{1, 2, 3, 4});
}

TEST_CASE("flatten empty") {
  REQUIRE(fcpp::query(std::vector<std::vector<int>>{}).flatten().to_vector() ==
          std::vector<int>());
}

TEST_CASE("group_by multiple") {
  REQUIRE(fcpp::query({1, 2, 3, 4})
              .group_by([](auto x) { return x % 2 == 0; })
              .to_vector() == std::vector<std::vector<int>>{{1, 3}, {2, 4}});
}

TEST_CASE("group_by single") {
  REQUIRE(fcpp::query({1, 2, 3, 4})
              .group_by([](auto x) { return true; })
              .to_vector() == std::vector<std::vector<int>>{{1, 2, 3, 4}});
}

TEST_CASE("intersect") {
  REQUIRE(fcpp::query({1, 2}).intersect(std::vector<int>{2, 3}).to_vector() ==
          std::vector<int>{2});
}

TEST_CASE("intersect all") {
  REQUIRE(fcpp::query({1, 2}).intersect(std::vector<int>{1, 2}).to_vector() ==
          std::vector<int>{1, 2});
}

TEST_CASE("intersect none") {
  REQUIRE(fcpp::query({1, 2}).intersect(std::vector<int>{2, 3}).to_vector() ==
          std::vector<int>{2});
}

TEST_CASE("join") {
  REQUIRE(fcpp::query({1, 2})
              .join(
                  std::vector<int>{2, 3}, [](auto x) { return x % 2 == 0; },
                  [](auto x) { return x % 2 == 0; })
              .to_vector() ==
          std::vector<std::tuple<int, int>>{{1, 3}, {2, 2}});
}

TEST_CASE("keyed_group_by") {
  REQUIRE(fcpp::query({1, 2}).keyed_group_by([](auto x) {
    return x % 2 == 0;
  }) == std::vector<std::pair<bool, std::vector<int>>>{{false, {1}},
                                                       {true, {2}}});
}

TEST_CASE("max") { REQUIRE(fcpp::query({1, 3, 2}).max() == 3); }

TEST_CASE("max lambda") {
  REQUIRE(fcpp::query({1, 3, 2}).max([](auto x) { return x == 3 ? 0 : x; }) ==
          2);
}

TEST_CASE("min") { REQUIRE(fcpp::query({2, 1, 3}).min() == 1); }

TEST_CASE("min lambda") {
  REQUIRE(fcpp::query({2, 1, 3}).min([](auto x) { return x == 1 ? 4 : x; }) ==
          2);
}

TEST_CASE("order_by") {
  REQUIRE(fcpp::query({1, 2, 3})
              .order_by([](auto x) { return x % 2; })
              .to_vector() == std::vector<int>{2, 1, 3}); // stable ordering
}

TEST_CASE("reverse") {
  REQUIRE(fcpp::query({1, 2, 3}).reverse().to_vector() ==
          std::vector<int>{3, 2, 1});
}

TEST_CASE("reverse empty") {
  REQUIRE(fcpp::query<int>({}).reverse().to_vector() == std::vector<int>());
}

TEST_CASE("select") {
  REQUIRE(fcpp::query({1, 2, 3})
              .select([](auto x) { return x + 100; })
              .to_vector() == std::vector<int>{101, 102, 103});
}

TEST_CASE("shuffle") {
  REQUIRE_FALSE(fcpp::query({1, 2, 3, 4, 5, 6, 7, 8, 9}).shuffle() ==
                std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9});
}

TEST_CASE("size") { REQUIRE(fcpp::query({1, 2}).size() == 2); }

TEST_CASE("size empty") { REQUIRE(fcpp::query<int>({}).size() == 0); }

TEST_CASE("skip") {
  REQUIRE(fcpp::query({1, 2}).skip(1).to_vector() == std::vector<int>{2});
}

TEST_CASE("skip none") {
  REQUIRE(fcpp::query({1, 2}).skip(0).to_vector() == std::vector<int>{1, 2});
}

TEST_CASE("slice") {
  REQUIRE(fcpp::query({1, 2, 3, 4, 5, 6, 7}).slice(2, 3, 2).to_vector() ==
          std::vector<int>{3, 5, 7});
}

TEST_CASE("slice no-stride") {
  REQUIRE(fcpp::query({1, 2, 3, 4, 5}).slice(2, 3).to_vector() ==
          std::vector<int>{3, 4, 5});
}

TEST_CASE("slice zero-stride") {
  REQUIRE(fcpp::query({1, 2, 3}).slice(2, 3, 0).to_vector() ==
          std::vector<int>{3, 3, 3});
}

TEST_CASE("take") {
  REQUIRE(fcpp::query({1, 2}).take(1).to_vector() == std::vector<int>{1});
}

TEST_CASE("take all") {
  REQUIRE(fcpp::query({1, 2}).take(2).to_vector() == std::vector<int>{1, 2});
}

TEST_CASE("take none") {
  REQUIRE(fcpp::query({1, 2}).take(0).to_vector() == std::vector<int>{});
}

TEST_CASE("take_random") {
  REQUIRE(fcpp::query({1, 2}).take_random(1).size() == 1);
}

TEST_CASE("take_random none") {
  REQUIRE(fcpp::query({1, 2}).take_random(0).to_vector() == std::vector<int>{});
}

TEST_CASE("to_multi_value_map") {
  REQUIRE(fcpp::query({1, 2, 3}).to_multi_value_map([](auto x) {
    return x % 2;
  }) == std::map<int, std::vector<int>>{{1, {1, 3}}, {0, {2}}});
}

TEST_CASE("to_single_value_map") {
  REQUIRE(fcpp::query({1, 2, 3}).to_single_value_map([](auto x) {
    return x % 2;
  }) == std::map<int, int>{{1, 1}, {0, 2}});
}

TEST_CASE("to_set") {
  REQUIRE(fcpp::query({1, 2, 2, 1}).to_set() == std::set<int>{1, 2});
}

TEST_CASE("to_vector") {
  REQUIRE(fcpp::query({1, 2}).to_vector() == std::vector<int>{1, 2});
}

TEST_CASE("unionize") {
  REQUIRE(fcpp::query({1, 2}).unionize({2, 3}).to_vector() ==
          std::vector<int>{1, 2, 3});
}

TEST_CASE("where") {
  REQUIRE(fcpp::query({1, 2, 3, 4})
              .where([](auto x) { return x % 2 == 0; })
              .to_vector() == std::vector<int>{2, 4});
}

TEST_CASE("zip") {
  REQUIRE(fcpp::query({1, 2, 3, 4}).zip({5, 6, 7, 8, 9}, false).to_vector() ==
          std::vector<std::tuple<int, int>>{
              {1, 5}, {2, 6}, {3, 7}, {4, 8}, {0, 9}});
}

TEST_CASE("zip truncate") {
  REQUIRE(fcpp::query({1, 2, 3, 4}).zip({5, 6, 7, 8, 9}, true).to_vector() ==
          std::vector<std::tuple<int, int>>{{1, 5}, {2, 6}, {3, 7}, {4, 8}});
}
