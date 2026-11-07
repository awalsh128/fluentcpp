/**
 * @file query.h
 * @author Andrew Walsh (awalsh128@gmail.com)
 * @brief A library to express information in a functional way that allows for
 * easy auto-complete and shorter expressions.
 * @version 0.2
 * @date 2026-10-21
 *
 * @copyright Copyright 2021 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef FCPP_QUERY_H
#define FCPP_QUERY_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>

#include "asserts.h"
#include "traits.h"
#include "transforms.h"

// DO NOT USE
//
// Internal overloads of [CR_]EXPR.
//! @cond Doxygen_Suppress
#define __EXPR1__(type, var, expr) [](type var) { return expr; }
#define __EXPR2__(type, var1, var2, expr)                                      \
  [](type var1, type var2) { return expr; }
#define __GET_MACRO__(_1, _2, _3, NAME, ...) NAME
//! @endcond

/**
 * @brief Syntactic sugar for terse expressions using pass by value.
 *
 * @arg List of variable names passed in.
 * @returns Produced value of the variables and operations evaluated.
 *
 * Examples:
 *  EXPR(x, y, x + y)
 *  EXPR(x, x + 10)
 *
 */
#define EXPR(...)                                                              \
  __GET_MACRO__(__VA_ARGS__, __EXPR2__, __EXPR1__)(auto, __VA_ARGS__)

/**
 * @brief Syntactic sugar for terse expressions using pass by constant
 * reference.
 *
 * @arg List of variable names passed in.
 * @returns Produced value of the variables and operations evaluated.
 *
 * Examples:
 *  CR_EXPR(x, y, x + y)
 *  CR_EXPR(x, x + 10)
 *
 */
#define CR_EXPR(...)                                                           \
  __GET_MACRO__(__VA_ARGS__, __EXPR2__, __EXPR1__)(const auto &, __VA_ARGS__)

namespace fcpp {

//! @cond Doxygen_Suppress
namespace detail {

// Seeding from std::random_device is expensive, so do it once per thread.
inline std::mt19937 &random_engine() {
  thread_local std::mt19937 engine(std::random_device{}());
  return engine;
}

// Copies the item if possible, otherwise fails since it can't be duplicated.
template <typename T>
T copy_or_throw(const T &item, const char *operation) {
  if constexpr (std::is_copy_constructible_v<T>) {
    return item;
  } else {
    throw std::invalid_argument(
        std::string(operation) + " requires copying a non-copyable item.");
  }
}

} // namespace detail
//! @endcond

// Forward declaration. See actual definition below.
template <typename T>
class Queryable;

/**
 * @brief True / if block query of Queryable<T>::branch method.
 *
 * @tparam T Type of items to query over.
 */
template <typename T>
class WhenTrue;

/**
 * @brief False / else block query of Queryable<T>::branch method.
 *
 * @tparam WhenTrueQueried Type of items projected from true / if block query.
 * @tparam T Type of items to query over.
 */
template <typename WhenTrueQueried, typename T>
class WhenFalse;

/**
 * @brief Merge result of both if and else block queries from
 * Queryable<T>::branch method.
 *
 * @tparam FalseT Type of items projected from false / else block query.
 * @tparam TrueT Type of items projected from true / if block query.
 */
template <typename TrueT, typename FalseT>
class Merge;

/**
 * @brief Queries the sequence of items using a vector.
 *
 * @tparam T Type of items to query over.
 * @param items Items to query over.
 * @return Queryable<T>
 */
template <typename T>
Queryable<T> query(std::vector<T> items);

/**
 * @brief Core object used to query items and hold the sequence state.
 *
 * Any operation performed from @ref Queryable<T> will be moved into the new
 * instance of @ref Queryable<T>. This means that any reference to the previous
 * instance of @ref Queryable<T> may have unexpected behavior dependent on the
 * operation performed and memory semantics of the object under query.
 *
 * The nomenclature used in the documentation is as follows:
 * - Sequence refers to an ordered collection that can contain duplicates.
 * - Item refers to an object in a sequence. It doesn't have any inherit
 * properties.
 * - Key refers to an object that has equality.
 * - Value refers to an object that has comparison. It the context of hash
 * tables, it refers to the value part of the key-value pair.
 *
 * @remark Note that not all methods are type dependent and will not always be
 * valid. For example the
 * @ref max method will not work on items that don't have comparability. This
 * will show as an error at compile time though.
 *
 * In terms of memory semantics, if a signature takes by value and not
 * reference, it is because it makes modifications to the value and doesn't want
 * to cause side effects. To prevent a copy of a single use variable, it must be
 * moved.
 *
 * Lambdas can be passed with arguments by reference or value depending on the
 * caller's preference. Pass by reference may be the case if the object is
 * large.
 *   [](auto x) { return x.y; }   // Fundamental type
 *   [](auto& x) { return x.y; }  // Larger composite type
 *
 * @tparam T Type of items to query over.
 */
template <typename T>
class Queryable final {
public:
  /**
   * @brief Construct a new Queryable object from a sequence of items.
   *
   * @param items Sequence of items to query over.
   */
  explicit Queryable(std::vector<T> items);
  /**
   * @brief Empty constructor.
   * @remark Removed since no-op state is unusable.
   */
  Queryable() = delete;
  ~Queryable() = default;
  /**
   * @brief Move assignment operator.
   * @remark Allowed since state will be stable as it isn't shared amongst
   * multiple copies.
   */
  Queryable(Queryable &&) noexcept = default;
  /**
   * @brief Copy constructor operator.
   * @remark Not allowed since state will not be stable as multiple Queryables
   * are moving items.
   */
  Queryable(const Queryable &) = delete;
  /**
   * @brief Copy assignment operator.
   * @remark Not allowed since state will not be stable as multiple Queryables
   * are moving items.
   * @return Queryable&
   */
  Queryable &operator=(const Queryable &) = delete;
  /**
   * @brief Implicit cast operator to vector from an rvalue.
   * @remark Object state will be invalid after this call since items have been
   * moved out.
   * @return std::vector<T>
   */
  operator std::vector<T>() && { return std::move(items_); }
  /**
   * @brief Implicit cast operator to vector from an lvalue.
   * @remark Copies the items; requires T to be copy constructible.
   * @return std::vector<T>
   */
  operator std::vector<T>() const & { return items_; }

  /**
   * @brief Equals overload operator for Queryable<T> against a
   * std::vector<T>.
   *
   * @remark Based off of internal items_ stored inside
   * Queryable<T>.
   *
   * @param lhs Left operand Queryable<T> to compare.
   * @param rhs  Right operand std::vector<T> to compare.
   * @return true if both sides are equal.
   * @return false if both side are not equal.
   */
  friend bool operator==(const Queryable<T> &lhs, const std::vector<T> &rhs) {
    return lhs.items_ == rhs;
  }
  /**
   * @brief Equals overload operator.
   *
   * @param lhs Left operand Queryable<T> to compare.
   * @param rhs  Right operand Queryable<T> to compare.
   * @return true if both sides are equal.
   * @return false if both side are not equal.
   */
  friend bool operator==(const Queryable<T> &lhs, const Queryable<T> &rhs) {
    return lhs.items_ == rhs.items_;
  }

  /**
   * @brief Type of items to query over. Used for type deduction.
   */
  typedef T item_type;

  /**
   * @brief Sums all the projected values of the sequence into a single value.
   *
   * @tparam U Accumulating value type.
   * @tparam AccumulateFn std::function<U(U, T)> Transform function type.
   * @param initial Value to start with before iterating sequence.
   * @param accumulate_func Transform function to apply to each item.
   * @return U The final accumulated value.
   */
  template <typename U, typename AccumulateFn>
  U accumulate(U initial, AccumulateFn accumulate_func) const;

  /**
   * @brief Executes a void action to create side effects.
   *
   * @param action_func Void function to apply to each item.
   * @return Queryable<T>
   */
  template <typename ActionFn>
  Queryable<T> action(ActionFn action_func);

  /**
   * @brief Determines whether all items of a sequence satisfy a condition.
   *
   * @param predicate Function to test each item for a condition.
   * @return true if all items satisfy the predicate.
   * @return false if one of the items doesn't satisfy the predicate.
   */
  template <typename Predicate>
  bool all(Predicate predicate) const;

  /**
   * @brief Determines whether a sequence contains any items.
   *
   * @param predicate Function to test each item for a condition.
   * @return true if all of the items satisfy the predicate.
   * @return false if none of the items satisfy the predicate.
   */
  template <typename Predicate>
  bool any(Predicate predicate) const;

  /**
   * @brief Branches the sequence into two based on a condition.
   *
   * @param predicate Function to test each item for a condition.
   * @return WhenTrue<T> True / if block query for items that evaluated to true.
   */
  template <typename Predicate>
  WhenTrue<T> branch(Predicate predicate);

  /**
   * @brief Produces the multiset difference of two sequences.
   *
   * Each right hand side item cancels out at most one equivalent left hand
   * side item. Neither sequence needs to be sorted.
   *
   * @param rhs_items Right hand side sequence.
   * @return Queryable<T> Left hand side items not cancelled out, in their
   * original order.
   */
  Queryable<T> difference(std::vector<T> rhs_items);

  /**
   * @brief Gets distinct items from a sequence.
   *
   * @return Queryable<T> Distinct items in sorted order.
   */
  Queryable<T> distinct();

  /**
   * @brief Indicates of the sequence is empty.
   *
   * @return true if the sequence is empty.
   * @return false if the sequence is populated.
   */
  [[nodiscard]] bool empty() const;

  /**
   * @brief Gets the first item of a sequence, or a default value if no item is
   * found.
   *
   * @param predicate Function to test each item for a condition.
   * @return std::optional<T> Either the satisfying item or the not found item
   * of std::nullopt
   */
  template <typename Predicate>
  std::optional<T> first_or_default(Predicate predicate);

  /**
   * @brief Projects each item in the sequence-of-sequences and flattens the
   * resulting sequences into one (i.e. vector<vector<T>> -> vector<T>).
   *
   * @return Queryable<U = std::vector<T>>
   */
  auto flatten();

  /**
   * @brief Groups the items of a sequence.
   *
   * @tparam KeySelector Transform function type. std::function<K(T)>
   * @param key_selector Transform to key function to apply to each item.
   * @return Queryable<std::vector<T>>
   */
  template <typename KeySelector>
  Queryable<std::vector<T>> group_by(KeySelector key_selector);

  /**
   * @brief Produces the multiset intersection of two sequences.
   *
   * Each right hand side item matches at most one equivalent left hand side
   * item. Neither sequence needs to be sorted.
   *
   * @param rhs_items Right hand side sequence.
   * @return Queryable<T> Left hand side items that were matched, in their
   * original order.
   */
  Queryable<T> intersect(std::vector<T> rhs_items);

  /**
   * @brief Correlates the items of two sequences based on matching keys.
   *
   * @tparam U Type of the right hand side sequence.
   * @tparam LhsKeySelector Transform to key function type. std::function<K(T)>
   * @tparam RhsKeySelector Transform to key function type. std::function<K(U)>
   * @param rhs_items The right hand side sequence to join against.
   * @param lhs_key_selector Transform to key function to apply to each item in
   * this / left hand side sequence.
   * @param rhs_key_selector Transform to key function to apply to each item in
   * the right hand side sequence.
   * @return Queryable<std::tuple<T, U>>
   *
   * @remark An item that matches more than one item on the other side is
   * copied, so this throws std::invalid_argument for non-copyable items.
   */
  template <typename U, typename LhsKeySelector, typename RhsKeySelector>
  Queryable<std::tuple<T, U>> join(
      std::vector<U> rhs_items, LhsKeySelector lhs_key_selector,
      RhsKeySelector rhs_key_selector);

  /**
   * @brief Groups the items of a sequence by key and produces as a key-group
   * pair sequence.
   *
   * @tparam KeySelector Transform to key function type. std::function<K(T)>
   * @param key_selector Transform to key function to apply to each item.
   * @return Queryable<std::pair<K, std::vector<T>>>
   */
  template <typename KeySelector>
  auto keyed_group_by(KeySelector key_selector);

  /**
   * @brief Gets the maximum item from the sequence.
   *
   * @return T
   */
  T max();

  /**
   * @brief Gets the maximum item from the sequence based on a selected value.
   *
   * @tparam ValueSelector Transform to value function type. std::function<V(T)>
   * @param value_selector Transform to value function to apply to each item.
   * @return T
   */
  template <typename ValueSelector>
  T max(ValueSelector value_selector);

  /**
   * @brief Gets the minimum item from the sequence.
   *
   * @return T
   */
  T min();

  /**
   * @brief Gets the minimum item from the sequence based on a selected value.
   *
   * @tparam ValueSelector Transform to value function type. std::function<V(T)>
   * @param value_selector Transform to value function to apply to each item.
   * @return T
   */
  template <typename ValueSelector>
  T min(ValueSelector value_selector);

  /**
   * @brief Orders the sequence by the selected value.
   *
   * The sort is stable and the value selector is called once per item.
   *
   * @tparam ValueSelector Transform to value function type. std::function<V(T)>
   * @param value_selector Transform to value function to apply to each item.
   * @param descending True if to order by greater to smaller values, otherwise
   * smaller to greater.
   * @return Queryable<T>
   */
  template <typename ValueSelector>
  Queryable<T> order_by(ValueSelector value_selector, bool descending = false);

  /**
   * @brief Inverts the order of the items in the sequence.
   *
   * @return Queryable<T>
   */
  Queryable<T> reverse();

  /**
   * @brief Projects each item of a sequence into a new form.
   *
   * @tparam Selector Transform function type that takes. std::function<V(T)>
   * @param selector Transform function to apply to each item.
   * @return Queryable<decltype(selector(T))>
   */
  template <typename Selector>
  auto select(Selector selector);

  /**
   * @brief Randomizes / shuffles all the item's order in the sequence.
   *
   * @return Queryable<T>
   */
  Queryable<T> shuffle();

  /**
   * @brief Gets the size of the sequence.
   *
   * @return size_t
   */
  [[nodiscard]] size_t size() const;

  /**
   * @brief Bypasses a specified number of items in the sequence and then
   * returns the remaining items.
   *
   * @param value The number of items from the beginning to skip.
   * @return Queryable<T>
   */
  Queryable<T> skip(size_t value);

  /**
   * @brief Gets a slice of the sequence.
   *
   * @remark See std::slice for exact behavior.
   *
   * @code
   * query({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}).slice(2, 3, 2)
   *  .to_vector() == {3, 5, 7};
   * @endcode
   *
   * @param start_index The initial / starting index into the sequence
   * inclusively.
   * @param size The number of the items to retrieve starting at start_index.
   * @param stride The length of the skip to the next size items. A stride
   * of zero repeats the item at start_index, which requires T to be copyable.
   * @return Queryable<T>
   */
  Queryable<T> slice(size_t start_index, size_t size, size_t stride = 1);

  /**
   * @brief Sorts the items in the sequence.
   *
   * @return Queryable<T>
   */
  Queryable<T> sort();

  /**
   * @brief Takes a specified number of contiguous items from the start of a
   * sequence.
   *
   * @param value The number of items from the beginning to keep.
   * @return Queryable<T>
   */
  Queryable<T> take(size_t value);

  /**
   * @brief Takes the specified number of items from the sequence at random
   * places.
   *
   * @param value The number of items to take randomly.
   * @return Queryable<T>
   */
  Queryable<T> take_random(size_t value);

  /**
   * @brief Trim from the back of the sequence.
   *
   * @param size The number of items to trim / erase from the back.
   * @return Queryable<T>
   */
  Queryable<T> trim(size_t size);

  /**
   * @brief Groups items by a selected key value and puts them into a hash map.
   *
   * @tparam KeySelector Transform to key function type. std::function<K(T)>
   * @param key_selector Transform to key function to apply to each item.
   * @return std::map<K, std::vector<T>>
   */
  template <typename KeySelector>
  auto to_multi_value_map(KeySelector key_selector);

  /**
   * @brief Groups items by selected key value, takes the first item of every
   * group, and puts them into a hash map.
   *
   * @tparam KeySelector Transform to key function type. std::function<K(T)>
   * @param key_selector Transform to key function to apply to each item.
   * @return std::map<K, T>
   */
  template <typename KeySelector /* = std::function<K(T)>*/>
  auto to_single_value_map(KeySelector key_selector);

  /**
   * @brief Gets the sequence as a set.
   *
   * @return std::set<T>
   */
  std::set<T> to_set();

  /**
   * @brief Gets the sequence as a vector.
   *
   * @return std::vector<T>
   */
  std::vector<T> to_vector();

  /**
   * @brief Unions the left hand and right hand side sequences.
   *
   * @param rhs_items The right hand side sequence to union.
   * @return Queryable<T> Distinct items from both sequences in sorted order.
   */
  Queryable<T> unionize(std::vector<T> rhs_items);

  /**
   * @brief Selects items in the sequence that satisfy the predicate /
   * conditional.
   *
   * @param predicate Function to test each item for a condition.
   * @return Queryable<T>
   */
  template <typename Predicate>
  Queryable<T> where(Predicate predicate);

  /**
   * @brief Produces a sequence of tuples with items from the two specified
   * sequences.
   *
   * @tparam U Type of the right hand side items.
   * @param rhs_items Right hand side items of the produced tuple sequence.
   * @param truncate Truncate the combined sequence by the minimum size of the
   * two. If false and the sizes differ, the shorter side is padded with default
   * values, which throws std::invalid_argument if that type has no default
   * constructor.
   * @return Queryable<std::tuple<T, U>>
   */
  template <typename U>
  Queryable<std::tuple<T, U>>
  zip(std::vector<U> rhs_items, bool truncate = false);

  /**
   * @brief Produces a sequence of tuples with items from the two specified
   * sequences.
   *
   * @tparam U Type of the right hand side items.
   * @param rhs_items Right hand side items of the produced tuple sequence.
   * @param truncate Truncate the combined sequence by the minimum size of the
   * two. If false and the sizes differ, the shorter side is padded with default
   * values, which throws std::invalid_argument if that type has no default
   * constructor.
   * @return Queryable<std::tuple<T, U>>
   */
  template <typename U>
  Queryable<std::tuple<T, U>>
  zip(std::initializer_list<U> rhs_items, bool truncate = false);

private:
  /**
   * @brief Sequence of items to be queried over.
   */
  std::vector<T> items_;
};

} // namespace fcpp

/******************************************************************************
 *
 * @brief Keep definitions separate to allow the declarations to stay clean and
 * compact.
 *
 *****************************************************************************/

namespace fcpp {

template <typename T>
Queryable<T> query(std::vector<T> items) {
  return Queryable<T>(std::move(items));
}

template <typename T>
Queryable<T>::Queryable(std::vector<T> items) : items_(std::move(items)) {}

template <typename T>
template <typename U, typename AccumulateFn>
U Queryable<T>::accumulate(U initial, AccumulateFn accumulate_func) const {
  // @todo Check that AccumulateFn is a binary operation.
  // This method is const: do not attempt to move-from `items_` here. Use
  // regular iterators so callers don't unexpectedly invalidate state.
  return std::accumulate(
      items_.begin(), items_.end(), initial, accumulate_func);
}

template <typename T>
template <typename ActionFn>
Queryable<T> Queryable<T>::action(ActionFn action_func) {
  // @todo Check that ActionFn is a unary operation.
  // Avoid copying each item into the lambda; forward as const-ref to reduce
  // unnecessary copies for large types.
  std::for_each(items_.begin(), items_.end(), [&action_func](const auto &item) {
    action_func(item);
  });
  return Queryable<T>(std::move(items_));
}

template <typename T>
template <typename Predicate>
bool Queryable<T>::all(Predicate predicate) const {
  return std::all_of(items_.begin(), items_.end(), predicate);
}

template <typename T>
template <typename Predicate>
bool Queryable<T>::any(Predicate predicate) const {
  return std::any_of(items_.begin(), items_.end(), predicate);
}

template <typename T>
Queryable<T> Queryable<T>::difference(std::vector<T> rhs_items) {
  static_assert(
      traits::is_less_than_comparable<T>::value,
      "T must be less-than comparable.");
  std::sort(rhs_items.begin(), rhs_items.end());
  // Number of matches consumed so far for each run of equivalent rhs items,
  // indexed by the start of the run.
  std::vector<size_t> consumed(rhs_items.size(), 0);
  std::erase_if(items_, [&](const T &item) {
    auto [lo, hi] = std::equal_range(rhs_items.begin(), rhs_items.end(), item);
    size_t &used = consumed[lo - rhs_items.begin()];
    if (lo + used < hi) {
      used++;
      return true;
    }
    return false;
  });
  return Queryable<T>(std::move(items_));
}

template <typename T>
Queryable<T> Queryable<T>::distinct() {
  static_assert(
      traits::is_less_than_comparable<T>::value,
      "T must be less-than comparable.");
  std::sort(items_.begin(), items_.end());
  // Adjacent sorted items are equivalent when the first isn't less.
  items_.erase(
      std::unique(
          items_.begin(), items_.end(),
          [](const T &lhs, const T &rhs) { return !(lhs < rhs); }),
      items_.end());
  return Queryable<T>(std::move(items_));
}

template <typename T>
bool Queryable<T>::empty() const {
  return items_.empty();
}

template <typename T>
template <typename Predicate>
std::optional<T> Queryable<T>::first_or_default(Predicate predicate) {
  // @todo Require predicate to be a unary operation.
  auto it =
      std::find_if(items_.begin(), items_.end(), [&predicate](const T &item) {
        return predicate(item);
      });
  if (it == items_.end()) {
    return std::nullopt;
  }
  return std::optional<T>(std::move(*it));
}

template <typename T>
auto Queryable<T>::flatten() {
  // @todo asserts::invariant T is a vector.
  using U = typename T::value_type;
  // Reserve capacity when possible: sum sizes of inner sequences to avoid
  // repeated reallocations.
  size_t total = 0;
  for (T &item : items_) {
    total += item.size();
  }
  std::vector<U> flattened;
  flattened.reserve(total);
  for (T &item : items_) {
    for (U &sub_item : item) {
      flattened.push_back(std::move(sub_item));
    }
  }
  return Queryable<U>(std::move(flattened));
}

template <typename T>
template <typename KeySelector>
Queryable<std::vector<T>> Queryable<T>::group_by(KeySelector key_selector) {
  using K = decltype(key_selector(*items_.begin()));
  static_assert(
      traits::is_less_than_comparable<K>::value,
      "Key selector must produce a value that is less-than comparable.");

  std::map<K, std::vector<T>> keyed_groups;
  for (T &item : items_) {
    keyed_groups[key_selector(std::as_const(item))].push_back(std::move(item));
  }

  std::vector<std::vector<T>> groups;
  groups.reserve(keyed_groups.size());
  for (auto &[key, group] : keyed_groups) {
    groups.push_back(std::move(group));
  }
  return Queryable<std::vector<T>>(std::move(groups));
}

template <typename T>
Queryable<T> Queryable<T>::intersect(std::vector<T> rhs_items) {
  static_assert(
      traits::is_less_than_comparable<T>::value,
      "T must be less-than comparable.");
  std::sort(rhs_items.begin(), rhs_items.end());
  // Number of matches consumed so far for each run of equivalent rhs items,
  // indexed by the start of the run.
  std::vector<size_t> consumed(rhs_items.size(), 0);
  std::erase_if(items_, [&](const T &item) {
    auto [lo, hi] = std::equal_range(rhs_items.begin(), rhs_items.end(), item);
    size_t &used = consumed[lo - rhs_items.begin()];
    if (lo + used < hi) {
      used++;
      return false;
    }
    return true;
  });
  return Queryable<T>(std::move(items_));
}

template <typename T>
template <typename U, typename LhsKeySelector, typename RhsKeySelector>
Queryable<std::tuple<T, U>> Queryable<T>::join(
    std::vector<U> rhs_items, LhsKeySelector lhs_key_selector,
    RhsKeySelector rhs_key_selector) {
  using KU = decltype(rhs_key_selector(*rhs_items.begin()));
  using KT = decltype(lhs_key_selector(*items_.begin()));
  static_assert(
      std::is_same<KU, KT>::value,
      "Left and right hand key selectors must produce the same type.");

  using K = KT;
  static_assert(
      traits::is_less_than_comparable<K>::value,
      "Key selectors must produce a type that is less-than compareable.");

  std::map<K, std::vector<U>> rhs_mapped;
  for (U &rhs_item : rhs_items) {
    rhs_mapped[rhs_key_selector(std::as_const(rhs_item))].push_back(
        std::move(rhs_item));
  }

  // Count how many lhs items use each rhs group so the last use can move.
  std::vector<K> lhs_keys;
  lhs_keys.reserve(items_.size());
  std::map<K, size_t> uses_left;
  for (T &lhs_item : items_) {
    lhs_keys.push_back(lhs_key_selector(std::as_const(lhs_item)));
    if (rhs_mapped.count(lhs_keys.back())) {
      uses_left[lhs_keys.back()]++;
    }
  }

  std::vector<std::tuple<T, U>> joined;
  for (size_t i = 0; i < items_.size(); i++) {
    auto rhs_it = rhs_mapped.find(lhs_keys[i]);
    if (rhs_it == rhs_mapped.end()) {
      continue;
    }
    bool last_lhs_use = --uses_left[lhs_keys[i]] == 0;
    std::vector<U> &rhs_group = rhs_it->second;
    for (size_t j = 0; j < rhs_group.size(); j++) {
      bool last_rhs_use = j + 1 == rhs_group.size();
      joined.emplace_back(
          last_rhs_use ? std::move(items_[i])
                       : detail::copy_or_throw(items_[i], "join"),
          last_lhs_use ? std::move(rhs_group[j])
                       : detail::copy_or_throw(rhs_group[j], "join"));
    }
  }

  return Queryable<std::tuple<T, U>>(std::move(joined));
}

template <typename T>
template <typename KeySelector>
auto Queryable<T>::keyed_group_by(KeySelector key_selector) {
  using K = decltype(key_selector(*items_.begin()));

  std::map<K, std::vector<T>> mapped;
  for (T &item : items_) {
    mapped[key_selector(std::as_const(item))].push_back(std::move(item));
  }
  return Queryable<std::pair<K, std::vector<T>>>(
      {std::make_move_iterator(mapped.begin()),
       std::make_move_iterator(mapped.end())});
}

template <typename T>
T Queryable<T>::max() {
  static_assert(
      traits::is_less_than_comparable<T>::value,
      "T must be less-than compareable.");
  asserts::invariant::eval(!items_.empty()) << "Sequence cannot be empty.";

  // @todo Investigate performance for large objects.
  return std::move(*std::max_element(items_.begin(), items_.end()));
}

template <typename T>
template <typename ValueSelector>
T Queryable<T>::max(ValueSelector value_selector) {
  static_assert(
      traits::is_less_than_comparable<decltype(value_selector(
          *items_.begin()))>::value,
      "ValueSelector return type must be less-than compareable.");
  asserts::invariant::eval(!items_.empty()) << "Sequence cannot be empty.";
  // @todo Require unary operation.

  return std::move(*std::max_element(
      items_.begin(), items_.end(),
      [&value_selector](const auto &lhs, const auto &rhs) {
        return value_selector(lhs) < value_selector(rhs);
      }));
}

template <typename T>
T Queryable<T>::min() {
  static_assert(
      traits::is_less_than_comparable<T>::value,
      "T must be less-than compareable.");
  asserts::invariant::eval(!items_.empty()) << "Sequence cannot be empty.";

  // @todo Guard with concepts as seen in min_element.
  return std::move(*std::min_element(items_.begin(), items_.end()));
}

template <typename T>
template <typename ValueSelector /* = std::function<K(T)>*/>
T Queryable<T>::min(ValueSelector value_selector) {
  static_assert(
      traits::is_less_than_comparable<decltype(value_selector(
          *items_.begin()))>::value,
      "ValueSelector return type must be less-than compareable.");
  asserts::invariant::eval(!items_.empty()) << "Sequence cannot be empty.";
  // @todo Require unary operation.

  return std::move(*std::min_element(
      items_.begin(), items_.end(),
      [&value_selector](const auto &lhs, const auto &rhs) {
        return value_selector(lhs) < value_selector(rhs);
      }));
}

template <typename T>
template <typename ValueSelector>
Queryable<T>
Queryable<T>::order_by(ValueSelector value_selector, bool descending) {
  static_assert(
      std::is_copy_assignable_v<T> ||
          (std::is_move_assignable_v<T> && std::is_move_constructible_v<T>),
      "T must either be copy assignable, or move assignable and "
      "constructible.");
  static_assert(
      traits::is_less_than_comparable<decltype(value_selector(
          *items_.begin()))>::value,
      "ValueSelector return type must be less-than compareable.");

  using K = std::decay_t<decltype(value_selector(*items_.begin()))>;
  // Select each value once, then stable sort the indices by value.
  std::vector<K> values;
  values.reserve(items_.size());
  for (T &item : items_) {
    values.push_back(value_selector(std::as_const(item)));
  }
  std::vector<size_t> order(items_.size());
  std::iota(order.begin(), order.end(), 0);
  std::stable_sort(
      order.begin(), order.end(),
      [&values, descending](size_t lhs, size_t rhs) {
        return descending ? values[rhs] < values[lhs]
                          : values[lhs] < values[rhs];
      });

  std::vector<T> ordered;
  ordered.reserve(items_.size());
  for (size_t i : order) {
    ordered.push_back(std::move(items_[i]));
  }
  return Queryable<T>(std::move(ordered));
}

template <typename T>
Queryable<T> Queryable<T>::reverse() {
  std::reverse(items_.begin(), items_.end());
  return Queryable<T>(std::move(items_));
}

template <typename T>
template <typename Selector /* = std::function<U(T)>*/>
auto Queryable<T>::select(Selector selector) {
  using U = decltype(selector(*items_.begin()));
  std::vector<U> selected;
  selected.reserve(items_.size());
  std::transform(
      std::make_move_iterator(items_.begin()),
      std::make_move_iterator(items_.end()), std::back_inserter(selected),
      selector);

  return Queryable<U>(std::move(selected));
}

template <typename T>
Queryable<T> Queryable<T>::shuffle() {
  std::shuffle(items_.begin(), items_.end(), detail::random_engine());
  return Queryable<T>(std::move(items_));
}

template <typename T>
size_t Queryable<T>::size() const {
  return items_.size();
}

template <typename T>
Queryable<T> Queryable<T>::skip(size_t value) {
  asserts::invariant::eval(value <= size())
      << "Skip value " << value
      << " must be less than or equal to sequence size of " << size() << ".";

  items_.erase(items_.begin(), items_.begin() + value);
  return Queryable<T>(std::move(items_));
}

template <typename T>
Queryable<T>
Queryable<T>::slice(size_t start_index, size_t size, size_t stride) {
  if (size == 0) {
    return Queryable<T>(std::vector<T>());
  }
  asserts::invariant::eval(start_index < this->size())
      << "Slice start index " << start_index
      << " must be less than sequence size of " << this->size() << ".";

  size_t last_index = start_index + (size - 1) * stride;
  asserts::invariant::eval(last_index < this->size())
      << "Slice last index " << last_index
      << " must be less than the sequence size of " << this->size() << ".";

  std::vector<T> sliced;
  sliced.reserve(size);
  if (stride == 0) {
    // Every item is the same one, so all but the last must be copies.
    for (size_t i = 0; i + 1 < size; i++) {
      sliced.push_back(detail::copy_or_throw(items_[start_index], "slice"));
    }
    sliced.push_back(std::move(items_[start_index]));
  } else {
    for (size_t i = start_index; sliced.size() < size; i += stride) {
      sliced.push_back(std::move(items_[i]));
    }
  }

  return Queryable<T>(std::move(sliced));
}

template <typename T>
Queryable<T> Queryable<T>::sort() {
  std::sort(items_.begin(), items_.end());
  return Queryable<T>(std::move(items_));
}

template <typename T>
Queryable<T> Queryable<T>::take(size_t value) {
  asserts::invariant::eval(value <= size())
      << "Take value " << value
      << " must be less than or equal to sequence size of " << size() << ".";

  items_.erase(items_.begin() + value, items_.end());
  return Queryable<T>(std::move(items_));
}

template <typename T>
Queryable<T> Queryable<T>::take_random(size_t value) {
  asserts::invariant::eval(value <= size())
      << "Take random value " << value
      << " must be less than or equal to sequence size of " << size() << ".";

  // Partial Fisher-Yates shuffle: only the first value positions are drawn.
  auto &engine = detail::random_engine();
  for (size_t i = 0; i < value; i++) {
    std::uniform_int_distribution<size_t> dist(i, items_.size() - 1);
    std::swap(items_[i], items_[dist(engine)]);
  }
  items_.erase(items_.begin() + value, items_.end());
  return Queryable<T>(std::move(items_));
}

template <typename T>
template <typename KeySelector /* = std::function<K(T)>*/>
auto Queryable<T>::to_multi_value_map(KeySelector key_selector) {
  using K = decltype(key_selector(*items_.begin()));
  std::map<K, std::vector<T>> mapped;
  for (T &item : items_) {
    mapped[key_selector(std::as_const(item))].push_back(std::move(item));
  }
  return mapped;
}

template <typename T>
template <typename KeySelector /* = std::function<K(T)>*/>
auto Queryable<T>::to_single_value_map(KeySelector key_selector) {
  using K = decltype(key_selector(*items_.begin()));
  static_assert(
      traits::is_less_than_comparable<K>::value,
      "KeySelector return type must be less-than compareable.");

  std::map<K, T> mapped;
  for (T &item : items_) {
    // Only moves the item if the key isn't already present.
    mapped.try_emplace(key_selector(std::as_const(item)), std::move(item));
  }
  return mapped;
}

template <typename T>
std::set<T> Queryable<T>::to_set() {
  static_assert(
      traits::is_less_than_comparable<T>::value,
      "T must be less-than compareable.");
  return std::set<T>(
      std::make_move_iterator(items_.begin()),
      std::make_move_iterator(items_.end()));
}

template <typename T>
std::vector<T> Queryable<T>::to_vector() {
  return std::move(items_);
}

template <typename T>
Queryable<T> Queryable<T>::trim(size_t size) {
  asserts::invariant::eval(size <= items_.size())
      << "Size " << size << " must be less than or equal to sequence size of "
      << this->size() << ".";

  items_.erase(items_.end() - size, items_.end());
  return Queryable<T>(std::move(items_));
}

template <typename T>
Queryable<T> Queryable<T>::unionize(std::vector<T> rhs_items) {
  static_assert(
      traits::is_less_than_comparable<T>::value,
      "T must be less-than compareable.");

  items_.insert(
      items_.end(), std::make_move_iterator(rhs_items.begin()),
      std::make_move_iterator(rhs_items.end()));
  return distinct();
}

template <typename T>
template <typename Predicate>
Queryable<T> Queryable<T>::where(Predicate predicate) {
  // Filter in place; the predicate sees each item before any are moved.
  std::erase_if(
      items_, [&predicate](const T &item) { return !predicate(item); });
  return Queryable<T>(std::move(items_));
}

template <typename T>
template <typename U>
Queryable<std::tuple<T, U>>
Queryable<T>::zip(std::vector<U> rhs_items, bool truncate) {
  size_t common = std::min(items_.size(), rhs_items.size());
  size_t total = truncate ? common : std::max(items_.size(), rhs_items.size());

  std::vector<std::tuple<T, U>> zipped;
  zipped.reserve(total);
  for (size_t i = 0; i < common; i++) {
    zipped.emplace_back(std::move(items_[i]), std::move(rhs_items[i]));
  }
  if (total == common) {
    return Queryable<std::tuple<T, U>>(std::move(zipped));
  }

  // Pad the shorter side with default values.
  if (items_.size() > rhs_items.size()) {
    if constexpr (std::is_default_constructible_v<U>) {
      for (size_t i = common; i < total; i++) {
        zipped.emplace_back(std::move(items_[i]), U());
      }
    } else {
      throw std::invalid_argument(
          "zip padding requires a default constructible right hand type.");
    }
  } else {
    if constexpr (std::is_default_constructible_v<T>) {
      for (size_t i = common; i < total; i++) {
        zipped.emplace_back(T(), std::move(rhs_items[i]));
      }
    } else {
      throw std::invalid_argument(
          "zip padding requires a default constructible left hand type.");
    }
  }
  return Queryable<std::tuple<T, U>>(std::move(zipped));
}

template <typename T>
template <typename U>
Queryable<std::tuple<T, U>>
Queryable<T>::zip(std::initializer_list<U> rhs_items, bool truncate) {
  return zip(std::vector<U>(std::move(rhs_items)), truncate);
}

/**
 * @brief Merge result of both if and else block queries from
 * Queryable<T>::branch method.
 *
 * @tparam FalseT Type of items projected from false / else block query.
 * @tparam TrueT Type of items projected from true / if block query.
 */
template <typename TrueT, typename FalseT>
class Merge {
public:
  /**
   * @brief Construct a new Merge object from the transformed true and false
   * branches of the predicate.
   *
   * @param true_queried Queryable that is the transformed sequence of true
   * branched items of the predicate.
   * @param false_queried Queryable that is the transformed sequence of false
   * branched items of the predicate.
   */
  explicit Merge(Queryable<TrueT> true_queried, Queryable<FalseT> false_queried)
      : true_queried_(std::move(true_queried)),
        false_queried_(std::move(false_queried)) {}
  Merge() = delete;
  ~Merge() = default;
  Merge(const Merge &) = delete;
  Merge &operator=(const Merge &) = delete;

  /**
   * @brief Merges the true and false branch sequences as a zip.
   *
   * @param truncate Truncate the combined sequence by the minimum size of the
   * two. If false both T and U must have a default constructor so default
   * values can be provided if one sequence is larger than the other.
   * @return Queryable<std::tuple<TrueT, FalseT>>
   */
  Queryable<std::tuple<TrueT, FalseT>> merge(bool truncate = false) {
    return true_queried_.zip(false_queried_.to_vector(), truncate);
  }

private:
  Queryable<TrueT> true_queried_;
  Queryable<FalseT> false_queried_;
};

/**
 * @brief False / else block query of Queryable<T>::branch method.
 *
 * @tparam WhenTrueQueried Type of items projected from true / if block query.
 * @tparam T Type of items to query over.
 */
template <typename WhenTrueQueriedT, typename T>
class WhenFalse {
public:
  /**
   * @brief Construct a new WhereFalse object from a sequence of transformed
   * items from the true branch of the predicate and the not yet transformed
   * false branch sequence.
   *
   * @param when_true_queried Queryable that is the transformed sequence of true
   * branched items of the predicate.
   * @param when_false_items Sequence of false branched items of the predicate
   * to query over.
   */
  explicit WhenFalse(
      Queryable<WhenTrueQueriedT> when_true_queried,
      std::vector<T> when_false_items)
      : when_true_queried_(std::move(when_true_queried)),
        when_false_items_(std::move(when_false_items)) {}
  WhenFalse() = delete;
  ~WhenFalse() = default;
  WhenFalse(const WhenFalse &) = delete;
  WhenFalse &operator=(const WhenFalse &) = delete;

  /**
   * @brief Projects each item of the false branched sequence into a new form
   * from the Queryable<T>::branch of the predicate.
   *
   * @tparam WhenFalseQuery Type of transform function.
   * @tparam WhenFalseQueriedT Type of items that will be transformed.
   * @param when_false_query Transform function that projects each item into a
   * new form.
   * @return Merge<WhenTrueQueriedT, WhenFalseQueriedT>
   */
  template <
      typename WhenFalseQuery /* =
                                 std::function<Queryable<WhenFalseQueriedT>(Queryable<T>&)>
                               */
      ,
      typename WhenFalseQueriedT = typename std::invoke_result_t<
          WhenFalseQuery, Queryable<T>>::item_type>
  Merge<WhenTrueQueriedT, WhenFalseQueriedT>
  when_false(WhenFalseQuery when_false_query) {
    return Merge<WhenTrueQueriedT, WhenFalseQueriedT>(
        std::move(when_true_queried_),
        when_false_query(Queryable<T>(std::move(when_false_items_))));
  }

private:
  Queryable<WhenTrueQueriedT> when_true_queried_;
  std::vector<T> when_false_items_;
};

/**
 * @brief True / if block query of Queryable<T>::branch method.
 *
 * @tparam T Type of items to query over.
 */
template <typename T>
class WhenTrue {
public:
  /**
   * @brief Construct a new WhereTrue object from a sequence of true and false
   * branched items of the predicate that are to be transformed.
   *
   * @param when_true_items Sequence of items from the true branch of the
   * predicate.
   * @param when_false_items Sequence of items from the false branch of the
   * predicate. over.
   */
  explicit WhenTrue(
      std::vector<T> when_true_items, std::vector<T> when_false_items)
      : when_true_items_(std::move(when_true_items)),
        when_false_items_(std::move(when_false_items)) {}
  WhenTrue() = delete;
  ~WhenTrue() = default;
  WhenTrue(const WhenTrue &) = delete;
  WhenTrue &operator=(const WhenTrue &) = delete;

  /**
   * @brief Projects each item of the true branched sequence into a new form
   * from the Queryable<T>::branch of the predicate.
   *
   * @tparam WhenTrueQuery Type of transform function.
   * @tparam WhenTrueQueriedT Type of items that will be transformed.
   * @param when_true_query Transform function that projects each item into a
   * new form.
   * @return WhenFalse<WhenTrueQueriedT, T>
   */
  template <
      typename WhenTrueQuery /* =
                                std::function<Queryable<WhenTrueQueriedT>(Queryable<T>)>
                              */
      ,
      typename WhenTrueQueriedT =
          typename std::invoke_result_t<WhenTrueQuery, Queryable<T>>::item_type>
  WhenFalse<WhenTrueQueriedT, T> when_true(WhenTrueQuery when_true_query) {
    return WhenFalse<WhenTrueQueriedT, T>(
        when_true_query(Queryable<T>(std::move(when_true_items_))),
        std::move(when_false_items_));
  }

private:
  std::vector<T> when_true_items_;
  std::vector<T> when_false_items_;
};

template <typename T>
template <typename Predicate>
WhenTrue<T> Queryable<T>::branch(Predicate predicate) {
  std::vector<T> when_true_items;
  std::vector<T> when_false_items;
  for (T &item : items_) {
    if (predicate(std::as_const(item))) {
      when_true_items.push_back(std::move(item));
    } else {
      when_false_items.push_back(std::move(item));
    }
  }

  return WhenTrue<T>(std::move(when_true_items), std::move(when_false_items));
}

} // namespace fcpp

#endif // FCPP_QUERY_H