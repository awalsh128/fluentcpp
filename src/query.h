/**
 * @file query.h
 * @author Andrew Walsh (awalsh128@gmail.com)
 * @brief A library to express information in a functional way that allows for
 * easy auto-complete and shorter expressions.
 * @version 0.1
 * @date 2021-08-20
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
#include <functional>
#include <iterator>
#include <map>
#include <optional>
#include <random>
#include <set>
#include <tuple>
#include <type_traits>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>

#include "asserts.h"
#include "traits.h"

// DO NOT USE
//
// Internal overloads of EXPR.
#define __EXPR1__(var, expr) [](auto var) { return expr; }
#define __EXPR2__(var1, var2, expr) [](auto var1, auto var2) { return expr; }
#define __GET_MACRO__(_1, _2, _3, NAME, ...) NAME

/**
 * @brief Syntactic sugar for terse expressions.
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
  __GET_MACRO__(__VA_ARGS__, __EXPR2__, __EXPR1__)                             \
  (__VA_ARGS__)

namespace fcpp {
// Forward declaration. See actual definition below.
template <typename T> class Queryable;

/**
 * @brief True / if block query of Queryable<T>::branch method.
 *
 * @tparam T Type of items to query over.
 */
template <typename T> class WhenTrue;

/**
 * @brief False / else block query of Queryable<T>::branch method.
 *
 * @tparam WhenTrueQueried Type of items projected from true / if block query.
 * @tparam T Type of items to query over.
 */
template <typename WhenTrueQueried, typename T> class WhenFalse;

/**
 * @brief Merge result of both if and else block queries from
 * Queryable<T>::branch method.
 *
 * @tparam FalseT Type of items projected from false / else block query.
 * @tparam TrueT Type of items projected from true / if block query.
 */
template <typename TrueT, typename FalseT> class Merge;

/**
 * @brief Queries the sequence of items using a vector.
 *
 * @tparam T Type of items to query over.
 * @param items Items to query over.
 * @return Queryable<T>
 */
template <typename T> Queryable<T> query(std::vector<T> items);

/**
 * @brief Queries the sequence of items using an initializer list.
 *
 * @tparam T Type of items to query over.
 * @param items Items to query over.
 * @return Queryable<T>
 */
template <typename T> Queryable<T> query(std::initializer_list<T> items);

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
template <typename T> class Queryable final {
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
  virtual ~Queryable() = default;
  /**
   * @brief Move assignment operator.
   * @remark Allowed since state will be stable as it isn't shared amongst
   * multiple copies.
   */
  Queryable(Queryable &&) = default;
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
   * @brief Implicit cast operator to vector.
   *
   * @return std::vector<T>
   */
  operator std::vector<T>() const {
    return std::vector<T>();
  } // std::move(items_); }
  // operator const std::vector<T> &() const { return items_; }

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
    return std::equal(lhs.items_.begin(), lhs.items_.end(), rhs.begin());
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
    return std::equal(lhs.items_.begin(), lhs.items_.end(), rhs.items_.begin());
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
  template <typename ActionFn> Queryable<T> action(ActionFn action_func) const;

  /**
   * @brief Determines whether all items of a sequence satisfy a condition.
   *
   * @param predicate Function to test each item for a condition.
   * @return true if all items satisfy the predicate.
   * @return false if one of the items doesn't satisfy the predicate.
   */
  template <typename Predicate> bool all(Predicate predicate) const;

  /**
   * @brief Determines whether a sequence contains any items.
   *
   * @param predicate Function to test each item for a condition.
   * @return true if all of the items satisfy the predicate.
   * @return false if none of the items satisfy the predicate.
   */
  template <typename Predicate> bool any(Predicate predicate) const;

  /**
   * @brief Branches the sequence into two based on a condition.
   *
   * @param predicate Function to test each item for a condition.
   * @return WhenTrue<T> True / if block query for items that evaluated to true.
   */
  template <typename Predicate> WhenTrue<T> branch(Predicate predicate) const;

  /**
   * @brief Produces the set difference of two sequences.
   *
   * @param rhs_items Right hand side sequence.
   * @return Queryable<T> Sequence of items not found in both sequences.
   */
  Queryable<T> difference(std::vector<T> rhs_items) const;

  /**
   * @brief Gets distinct items from a sequence.
   *
   * @return Queryable<T>
   */
  Queryable<T> distinct() const;

  /**
   * @brief Indicates of the sequence is empty.
   *
   * @return true if the sequence is empty.
   * @return false if the sequence is populated.
   */
  bool empty() const;

  /**
   * @brief Gets the first item of a sequence, or a default value if no item is
   * found.
   *
   * @param predicate Function to test each item for a condition.
   * @return std::optional<T> Either the satisfying item or the not found item
   * of std::nullopt
   */
  template <typename Predicate>
  std::optional<T> first_or_default(Predicate predicate) const;

  /**
   * @brief Projects each item in the sequence-of-sequences and flattens the
   * resulting sequences into one (i.e. vector<vector<T>> -> vector<T>).
   *
   * @return Queryable<U = std::vector<T>>
   */
  auto flatten() const;

  /**
   * @brief Groups the items of a sequence.
   *
   * @tparam KeySelector Transform function type. std::function<K(T)>
   * @param key_selector Transform to key function to apply to each item.
   * @return Queryable<std::vector<T>>
   */
  template <typename KeySelector>
  Queryable<std::vector<T>> group_by(KeySelector key_selector) const;

  /**
   * @brief Produces the set intersection of two sequences.
   *
   * @param rhs_items Right hand side sequence.
   * @return Queryable<T> The intersected set that share the same items.
   */
  Queryable<T> intersect(const std::vector<T> &rhs_items) const;

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
   */
  template <typename U, typename LhsKeySelector, typename RhsKeySelector>
  Queryable<std::tuple<T, U>> join(std::vector<U> rhs_items,
                                   LhsKeySelector lhs_key_selector,
                                   RhsKeySelector rhs_key_selector) const;

  /**
   * @brief Groups the items of a sequence by key and produces as a key-group
   * pair sequence.
   *
   * @tparam KeySelector Transform to key function type. std::function<K(T)>
   * @param key_selector Transform to key function to apply to each item.
   * @return Queryable<std::pair<K, std::vector<T>>>
   */
  template <typename KeySelector>
  auto keyed_group_by(KeySelector key_selector) const;

  /**
   * @brief Gets the maximum item from the sequence.
   *
   * @return T
   */
  T max() const;

  /**
   * @brief Gets the maximum item from the sequence based on a selected value.
   *
   * @tparam ValueSelector Transform to value function type. std::function<V(T)>
   * @param value_selector Transform to value function to apply to each item.
   * @return T
   */
  template <typename ValueSelector> T max(ValueSelector value_selector) const;

  /**
   * @brief Gets the minimum item from the sequence.
   *
   * @return T
   */
  T min() const;

  /**
   * @brief Gets the minimum item from the sequence based on a selected value.
   *
   * @tparam ValueSelector Transform to value function type. std::function<V(T)>
   * @param value_selector Transform to value function to apply to each item.
   * @return T
   */
  template <typename ValueSelector> T min(ValueSelector value_selector) const;

  /**
   * @brief Orders the sequence by the selected value.
   *
   * @tparam ValueSelector Transform to value function type. std::function<V(T)>
   * @param value_selector Transform to value function to apply to each item.
   * @param descending True if to order by greater to smaller values, otherwise
   * smaller to greater.
   * @return Queryable<T>
   */
  template <typename ValueSelector>
  Queryable<T> order_by(ValueSelector value_selector,
                        bool descending = false) const;

  /**
   * @brief Inverts the order of the items in the sequence.
   *
   * @return Queryable<T>
   */
  Queryable<T> reverse() const;

  /**
   * @brief Projects each item of a sequence into a new form.
   *
   * @tparam Selector Transform function type that takes. std::function<V(T)>
   * @param selector Transform function to apply to each item.
   * @return Queryable<decltype(selector(T))>
   */
  template <typename Selector> auto select(Selector selector) const;

  /**
   * @brief Randomizes / shuffles all the item's order in the sequence.
   *
   * @return Queryable<T>
   */
  Queryable<T> shuffle() const;

  /**
   * @brief Gets the size of the sequence.
   *
   * @return size_t
   */
  size_t size() const;

  /**
   * @brief Bypasses a specified number of items in the sequence and then
   * returns the remaining items.
   *
   * @param value The number of items from the beginning to skip.
   * @return Queryable<T>
   */
  Queryable<T> skip(size_t value) const;

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
   * @param stride The length of the skip to the next size items.
   * @return Queryable<T>
   */
  Queryable<T> slice(size_t start_index, size_t size, size_t stride = 1) const;

  /**
   * @brief Sorts the items in the sequence.
   *
   * @return Queryable<T>
   */
  Queryable<T> sort() const;

  /**
   * @brief Takes a specified number of contiguous items from the start of a
   * sequence.
   *
   * @param value The number of items from the beginning to keep.
   * @return Queryable<T>
   */
  Queryable<T> take(size_t value) const;

  /**
   * @brief Takes the specified number of items from the sequence at random
   * places.
   *
   * @param value The number of items to take randomly.
   * @return Queryable<T>
   */
  Queryable<T> take_random(size_t value) const;

  /**
   * @brief Trim from the back of the sequence.
   *
   * @param size The number of items to trim / erase from the back.
   * @return Queryable<T>
   */
  Queryable<T> trim(size_t size) const;

  /**
   * @brief Groups items by a selected key value and puts them into a hash map.
   *
   * @tparam KeySelector Transform to key function type. std::function<K(T)>
   * @param key_selector Transform to key function to apply to each item.
   * @return std::map<K, std::vector<T>>
   */
  template <typename KeySelector>
  auto to_multi_value_map(KeySelector key_selector) const;

  /**
   * @brief Groups items by selected key value, takes the first item of every
   * group, and puts them into a hash map.
   *
   * @tparam KeySelector Transform to key function type. std::function<K(T)>
   * @param key_selector Transform to key function to apply to each item.
   * @return std::map<K, T>
   */
  template <typename KeySelector /* = std::function<K(T)>*/>
  auto to_single_value_map(KeySelector key_selector) const;

  /**
   * @brief Gets the sequence as a set.
   *
   * @return std::set<T>
   */
  std::set<T> to_set() const;

  /**
   * @brief Gets the sequence as a vector.
   *
   * @return std::vector<T>
   */
  std::vector<T> to_vector() const;

  /**
   * @brief Unions the left hand and right hand side sequences.
   *
   * @param rhs_items The right hand side sequence to union.
   * @return Queryable<T>
   */
  Queryable<T> unionize(std::vector<T> rhs_items) const;

  /**
   * @brief Selects items in the sequence that satisfy the predicate /
   * conditional.
   *
   * @param predicate Function to test each item for a condition.
   * @return Queryable<T>
   */
  template <typename Predicate> Queryable<T> where(Predicate predicate) const;

  /**
   * @brief Produces a sequence of tuples with items from the two specified
   * sequences.
   *
   * @tparam U Type of the right hand side items.
   * @param rhs_items Right hand side items of the produced tuple sequence.
   * @param truncate Truncate the combined sequence by the minimum size of the
   * two. If false both T and U must have a default constructor so default
   * values can be provided if one sequence is larger than the other.
   * @return Queryable<std::tuple<T, U>>
   */
  template <typename U>
  Queryable<std::tuple<T, U>> zip(std::vector<U> rhs_items,
                                  bool truncate = false) const;

  /**
   * @brief Produces a sequence of tuples with items from the two specified
   * sequences.
   *
   * @tparam U Type of the right hand side items.
   * @param rhs_items Right hand side items of the produced tuple sequence.
   * @param truncate Truncate the combined sequence by the minimum size of the
   * two. If false both T and U must have a default constructor so default
   * values can be provided if one sequence is larger than the other.
   * @return Queryable<std::tuple<T, U>>
   */
  template <typename U>
  Queryable<std::tuple<T, U>> zip(std::initializer_list<U> rhs_items,
                                  bool truncate) const;

private:
  /**
   * @brief Sequence of items to be queried over.
   *
   * @remark Sequence is const qualified to avoid unintentional mutations in
   * future development. Any mutations are made explicit in implementation by
   * moving into a const removed vector.
   */
  const std::vector<T> items_;
};

} // namespace fcpp

/******************************************************************************
 *
 * @brief Keep definitions separate to allow the declarations to stay clean and
 * compact.
 *
 *****************************************************************************/

namespace fcpp {
template <typename T> Queryable<T> query(std::vector<T> items) {
  return Queryable<T>(std::move(items));
}

template <typename T> Queryable<T> query(std::initializer_list<T> items) {
  return query(std::vector<T>(items));
}

template <typename T>
Queryable<T>::Queryable(std::vector<T> items) : items_(std::move(items)) {}

template <typename T>
bool operator==(const Queryable<T> &lhs, const std::vector<T> &rhs) {
  return std::equal(lhs.items_.begin(), lhs.items_.end(), rhs.begin());
}

template <typename T>
bool operator==(const Queryable<T> &lhs, const Queryable<T> &rhs) {
  return std::equal(lhs.items_.begin(), lhs.items_.end(), rhs.items_.begin());
}

template <typename T>
template <typename U, typename AccumulateFn>
U Queryable<T>::accumulate(U initial, AccumulateFn accumulate_func) const {
  static_assert(traits::is_additive<U>::value,
                "Initial value type must be additive.");
  // @todo Check that AccumulateFn takes two args and produces an additive
  // return type.
  return std::accumulate(std::make_move_iterator(items_.begin()),
                         std::make_move_iterator(items_.end()), initial,
                         accumulate_func);
}

template <typename T>
template <typename ActionFn>
Queryable<T> Queryable<T>::action(ActionFn action_func) const {
  // @todo Check that ActionFn takes a single arg.
  std::for_each(items_.begin(), items_.end(),
                [&action_func](auto item) { action_func(item); });
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
Queryable<T> Queryable<T>::difference(std::vector<T> rhs_items) const {
  static_assert(traits::is_equality_comparable<T>::value,
                "T must be equality comparable.");
  // @todo Bubble up concepts for comparisons.
  std::vector<T> difference;
  std::set_difference(std::make_move_iterator(items_.begin()),
                      std::make_move_iterator(items_.end()),
                      std::make_move_iterator(rhs_items.begin()),
                      std::make_move_iterator(rhs_items.end()),
                      std::back_inserter(difference));
  return Queryable<T>(std::move(difference));
}

template <typename T> Queryable<T> Queryable<T>::distinct() const {
  static_assert(traits::is_equality_comparable<T>::value,
                "T must be equality comparable.");
  // @todo Reduce the number of transforms.
  auto distinguished = std::set<T>(std::make_move_iterator(items_.begin()),
                                   std::make_move_iterator(items_.end()));
  return Queryable<T>(
      std::vector<T>(std::make_move_iterator(distinguished.begin()),
                     std::make_move_iterator(distinguished.end())));
}

template <typename T> bool Queryable<T>::empty() const {
  return items_.empty();
}

template <typename T>
template <typename Predicate>
std::optional<T> Queryable<T>::first_or_default(Predicate predicate) const {
  std::vector<T> mutable_items(std::move(items_));
  auto it = std::find_if(mutable_items.begin(), mutable_items.end(), predicate);
  return it == mutable_items.end() ? std::nullopt
                                   : std::optional<T>(std::move(*it));
}

template <typename T> auto Queryable<T>::flatten() const {
  // @todo asserts::invariant T is a vector.
  using U = T::value_type;
  std::vector<T> mutable_items(std::move(items_));
  std::vector<U> flattened;
  for (T &item : mutable_items) {
    for (U &sub_item : std::move(item)) {
      flattened.push_back(std::move(sub_item));
    }
  }
  return Queryable<U>(std::move(flattened));
}

template <typename T>
template <typename KeySelector>
Queryable<std::vector<T>>
Queryable<T>::group_by(KeySelector key_selector) const {
  using K = decltype(key_selector(*items_.begin()));
  static_assert(
      traits::is_less_than_comparable<K>::value,
      "Key selector must produce a value that is less-than comparable.");

  std::map<K, std::vector<T>> keyed_groups;
  std::for_each(std::make_move_iterator(items_.begin()),
                std::make_move_iterator(items_.end()), [&](auto item) {
                  keyed_groups[key_selector(item)].push_back(std::move(item));
                });

  std::vector<std::vector<T>> groups;
  groups.resize(keyed_groups.size());
  std::transform(
      std::make_move_iterator(keyed_groups.begin()),
      std::make_move_iterator(keyed_groups.end()), groups.begin(),
      [](std::pair<K, std::vector<T>> pair) { return std::move(pair.second); });

  return Queryable<std::vector<T>>(std::move(groups));
}

template <typename T>
Queryable<T> Queryable<T>::intersect(const std::vector<T> &rhs_items) const {
  // @todo Investigate potential copies of rhs_items when set_intersection
  // called.
  std::vector<T> intersection;
  std::set_intersection(std::make_move_iterator(items_.begin()),
                        std::make_move_iterator(items_.end()),
                        rhs_items.begin(), rhs_items.end(),
                        std::back_inserter(intersection));
  return Queryable<T>(std::move(intersection));
}

template <typename T>
template <typename U, typename LhsKeySelector, typename RhsKeySelector>
Queryable<std::tuple<T, U>>
Queryable<T>::join(std::vector<U> rhs_items, LhsKeySelector lhs_key_selector,
                   RhsKeySelector rhs_key_selector) const {
  using KU = decltype(rhs_key_selector(*rhs_items.begin()));
  using KT = decltype(rhs_key_selector(*items_.begin()));
  static_assert(
      std::is_same<KU, KT>::value,
      "Left and right hand key selectors must produce the same type.");

  using K = KT;
  static_assert(
      traits::is_less_than_comparable<K>::value,
      "Key selectors must produce a type that is less-than compareable.");

  std::map<K, std::vector<U>> rhs_mapped;
  std::for_each(std::make_move_iterator(rhs_items.begin()),
                std::make_move_iterator(rhs_items.end()),
                [&rhs_mapped, &rhs_key_selector](U rhs_item) {
                  rhs_mapped[rhs_key_selector(rhs_item)].push_back(
                      std::move(rhs_item));
                });

  std::vector<std::tuple<T, U>> joined;
  std::for_each(
      std::make_move_iterator(items_.begin()),
      std::make_move_iterator(items_.end()),
      [&rhs_mapped, &lhs_key_selector, &joined](auto lhs_item) {
        auto lhs_key = lhs_key_selector(lhs_item);
        auto rhs_it = rhs_mapped.find(lhs_key);
        if (rhs_it != rhs_mapped.end()) {
          for (U &rhs_item : rhs_it->second) {
            joined.push_back({std::move(lhs_item), std::move(rhs_item)});
          }
        }
      });

  return Queryable<std::tuple<T, U>>(std::move(joined));
}

template <typename T>
template <typename KeySelector>
auto Queryable<T>::keyed_group_by(KeySelector key_selector) const {
  // @todo Allow non-copyable objects to use this function.
  static_assert(std::is_copy_constructible_v<T>,
                "T must be copy constructible as it is used by the "
                "key_selector and group add.");
  using K = decltype(key_selector(*items_.begin()));

  std::map<K, std::vector<T>> mapped;
  for (int i = 0; i < items_.size(); i++) {
    // @todo Avoid a copy of items_[i] on key_selector call for large objects.
    mapped[key_selector(items_[i])].push_back(std::move(items_[i]));
  }
  return Queryable<std::pair<K, std::vector<T>>>(
      {std::make_move_iterator(mapped.begin()),
       std::make_move_iterator(mapped.end())});
}

template <typename T> T Queryable<T>::max() const {
  static_assert(traits::is_less_than_comparable<T>::value,
                "T must be less-than compareable.");

  // @todo Investigate performance for large objects.
  std::vector<T> mutable_items(std::move(items_));
  return *std::max_element(std::make_move_iterator(mutable_items.begin()),
                           std::make_move_iterator(mutable_items.end()),
                           [](auto lhs, auto rhs) { return lhs < rhs; });
}

template <typename T>
template <typename ValueSelector>
T Queryable<T>::max(ValueSelector value_selector) const {
  static_assert(traits::is_less_than_comparable<decltype(
                    value_selector(*items_.begin()))>::value,
                "ValueSelector return type must be less-than compareable.");

  std::vector<T> mutable_items(std::move(items_));
  return *std::max_element(std::make_move_iterator(mutable_items.begin()),
                           std::make_move_iterator(mutable_items.end()),
                           [&value_selector](auto lhs, auto rhs) {
                             return value_selector(lhs) < value_selector(rhs);
                           });
}

template <typename T> T Queryable<T>::min() const {
  // @todo Guard with concepts as seen in min_element.
  std::vector<T> mutable_items(std::move(items_));
  return *std::min_element(std::make_move_iterator(mutable_items.begin()),
                           std::make_move_iterator(mutable_items.end()),
                           [](auto lhs, auto rhs) { return lhs < rhs; });
}

template <typename T>
template <typename ValueSelector /* = std::function<K(T)>*/>
T Queryable<T>::min(ValueSelector value_selector) const {
  static_assert(traits::is_less_than_comparable<decltype(
                    value_selector(*items_.begin()))>::value,
                "ValueSelector return type must be less-than compareable.");

  std::vector<T> mutable_items(std::move(items_));
  return *std::min_element(std::make_move_iterator(mutable_items.begin()),
                           std::make_move_iterator(mutable_items.end()),
                           [&value_selector](auto lhs, auto rhs) {
                             return value_selector(lhs) < value_selector(rhs);
                           });
}

template <typename T>
template <typename ValueSelector>
Queryable<T> Queryable<T>::order_by(ValueSelector value_selector,
                                    bool descending) const {
  static_assert(traits::is_less_than_comparable<decltype(
                    value_selector(*items_.begin()))>::value,
                "ValueSelector return type must be less-than compareable.");

  std::vector<T> items(std::make_move_iterator(items_.begin()),
                       std::make_move_iterator(items_.end()));

  using K = decltype(value_selector(*items_.begin()));
  std::sort(items.begin(), items.end(),
            [&value_selector, &descending](auto lhs, auto rhs) -> bool {
              const K lhs_value = value_selector(lhs);
              const K rhs_value = value_selector(rhs);
              return descending ? lhs_value > rhs_value : lhs_value < rhs_value;
            });

  return Queryable<T>(std::move(items));
}

template <typename T> Queryable<T> Queryable<T>::reverse() const {
  std::vector<T> reversed(std::make_move_iterator(items_.begin()),
                          std::make_move_iterator(items_.end()));
  std::reverse(reversed.begin(), reversed.end());
  return Queryable<T>(std::move(reversed));
}

template <typename T>
template <typename Selector /* = std::function<U(T)>*/>
auto Queryable<T>::select(Selector selector) const {
  using U = decltype(selector(*items_.begin()));
  std::vector<T> mutable_items(std::move(items_));
  std::vector<U> selected;
  selected.reserve(items_.size());
  std::transform(std::make_move_iterator(mutable_items.begin()),
                 std::make_move_iterator(mutable_items.end()),
                 std::back_inserter(selected), selector);

  return Queryable<U>(std::move(selected));
}

template <typename T> Queryable<T> Queryable<T>::shuffle() const {
  std::vector<T> mutable_items(std::move(items_));
  std::shuffle(mutable_items.begin(), mutable_items.end(),
               std::mt19937(std::random_device()()));
  return Queryable<T>(std::move(mutable_items));
}

template <typename T> size_t Queryable<T>::size() const {
  return items_.size();
}

template <typename T> Queryable<T> Queryable<T>::skip(size_t value) const {
  asserts::invariant::eval(value <= size())
      << "Skip value " << value
      << " must be less than or equal to sequence size of " << size() << ".";

  return Queryable<T>({std::make_move_iterator(items_.begin() + value),
                       std::make_move_iterator(items_.end())});
}

template <typename T>
Queryable<T> Queryable<T>::slice(size_t start_index, size_t size,
                                 size_t stride) const {
  asserts::invariant::eval(start_index < this->size())
      << "Slice start index " << start_index
      << " must be less than sequence size of " << this->size() << ".";

  size_t slice_length = (start_index - 1) + (size * stride);
  asserts::invariant::eval(stride == 0 || slice_length <= this->size())
      << "Slice length " << slice_length
      << " must be less than or equal to the sequence size of " << this->size()
      << ".";

  std::valarray<T> view(items_.data(), items_.size());
  std::valarray<T> sliced = view[std::slice(start_index, size, stride)];

  return Queryable<T>({std::begin(sliced), std::end(sliced)});
}

template <typename T> Queryable<T> Queryable<T>::sort() const {
  std::vector<T> sorted(std::move(items_));
  std::sort(sorted.begin(), sorted.end());
  return Queryable<T>(std::move(sorted));
}

template <typename T> Queryable<T> Queryable<T>::take(size_t value) const {
  asserts::invariant::eval(value <= size())
      << "Take value " << value
      << " must be less than or equal to sequence size of " << size() << ".";

  return Queryable<T>({std::make_move_iterator(items_.begin()),
                       std::make_move_iterator(items_.begin() + value)});
}

template <typename T>
Queryable<T> Queryable<T>::take_random(size_t value) const {
  asserts::invariant::eval(value <= size())
      << "Take random value " << value
      << " must be less than or equal to sequence size of " << size() << ".";

  std::vector<int> indices;
  indices.reserve(items_.size());
  for (int i = 0; i < items_.size(); i++) {
    indices.push_back(i);
  }
  std::shuffle(indices.begin(), indices.end(),
               std::mt19937(std::random_device()()));
  indices.resize(value);

  std::vector<T> random_items;
  random_items.reserve(std::min(items_.size(), value));
  std::transform(indices.begin(), indices.end(),
                 std::back_inserter(random_items),
                 [this](int i) { return std::move(items_[i]); });

  return Queryable<T>(std::move(random_items));
}

template <typename T>
template <typename KeySelector /* = std::function<K(T)>*/>
auto Queryable<T>::to_multi_value_map(KeySelector key_selector) const {
  static_assert(std::is_copy_constructible_v<T>,
                "T must be copy constructible as it is used by the "
                "key_selector and value add.");

  using K = decltype(key_selector(*items_.begin()));
  std::map<K, std::vector<T>> mapped;
  std::for_each(std::make_move_iterator(items_.begin()),
                std::make_move_iterator(items_.end()),
                [&](auto item) { mapped[key_selector(item)].push_back(item); });
  return mapped;
}

template <typename T>
template <typename KeySelector /* = std::function<K(T)>*/>
auto Queryable<T>::to_single_value_map(KeySelector key_selector) const {
  using K = decltype(key_selector(*items_.begin()));
  static_assert(traits::is_less_than_comparable<K>::value,
                "KeySelector return type must be less-than compareable.");

  std::map<K, T> mapped;
  std::for_each(std::make_move_iterator(items_.begin()),
                std::make_move_iterator(items_.end()),
                [&key_selector, &mapped](auto item) {
                  K key = key_selector(item);
                  auto it = mapped.find(key);
                  if (it == mapped.end()) {
                    mapped.insert({std::move(key), std::move(item)});
                  }
                });
  return mapped;
}

template <typename T> std::set<T> Queryable<T>::to_set() const {
  static_assert(traits::is_less_than_comparable<T>::value,
                "T must be less-than compareable.");
  return std::set<T>(items_.begin(), items_.end());
}

template <typename T> std::vector<T> Queryable<T>::to_vector() const {
  return std::move(items_);
}

template <typename T> Queryable<T> Queryable<T>::trim(size_t size) const {
  asserts::invariant(items_.size() <= size)
      << "Size " << size << " must be less than or equal to sequence size of "
      << this->size() << ".";

  std::vector<T> mutable_items(std::move(items_));
  mutable_items.resize(items_.size() - size);
  return Queryable<T>(std::move(mutable_items));
}

template <typename T>
Queryable<T> Queryable<T>::unionize(std::vector<T> rhs_items) const {
  static_assert(traits::is_less_than_comparable<T>::value,
                "T must be less-than compareable.");

  std::set<T> unionized({std::make_move_iterator(items_.begin()),
                         std::make_move_iterator(items_.end())});
  std::move(rhs_items.begin(), rhs_items.end(),
            std::inserter(unionized, unionized.end()));
  return Queryable<T>(
      std::move(std::vector<T>{std::make_move_iterator(unionized.begin()),
                               std::make_move_iterator(unionized.end())}));
}

template <typename T>
template <typename Predicate>
Queryable<T> Queryable<T>::where(Predicate predicate) const {
  std::vector<T> filtered;
  std::copy_if(std::make_move_iterator(items_.begin()),
               std::make_move_iterator(items_.end()),
               std::back_inserter(filtered), predicate);
  return Queryable<T>(std::move(filtered));
}

template <typename T>
template <typename U>
Queryable<std::tuple<T, U>> Queryable<T>::zip(std::vector<U> rhs_items,
                                              bool truncate) const {
  if (!truncate) {
    static_assert(std::is_default_constructible_v<T>,
                  "T must have a default constructor so default values can "
                  "populate the tuple.");
    static_assert(std::is_default_constructible_v<U>,
                  "U must have a default constructor so default values can "
                  "populate the tuple.");
  }

  std::vector<std::tuple<T, U>> zipped;
  zipped.reserve(truncate ? std::min({items_.size(), rhs_items.size()})
                          : std::max({items_.size(), rhs_items.size()}));

  int i = 0;
  for (; i < std::min({items_.size(), rhs_items.size()}); i++) {
    zipped.push_back({std::move(items_[i]), std::move(rhs_items[i])});
  }

  if (truncate) {
    return Queryable<std::tuple<T, U>>(std::move(zipped));
  }

  if (items_.size() > rhs_items.size()) {
    for (; i < items_.size(); i++) {
      zipped.push_back({std::move(items_[i]), U()});
    }
  } else {
    for (; i < rhs_items.size(); i++) {
      zipped.push_back({T(), std::move(rhs_items[i])});
    }
  }
  return Queryable<std::tuple<T, U>>(std::move(zipped));
}

template <typename T>
template <typename U>
Queryable<std::tuple<T, U>>
Queryable<T>::zip(std::initializer_list<U> rhs_items, bool truncate) const {
  return zip(std::vector<U>(std::move(rhs_items)), truncate);
}

template <typename TrueT, typename FalseT> class Merge {
public:
  explicit Merge(Queryable<TrueT> true_queried, Queryable<FalseT> false_queried)
      : true_queried_(std::move(true_queried)),
        false_queried_(std::move(false_queried)) {}
  Merge() = delete;
  virtual ~Merge() = default;
  Merge(const Merge &) = delete;
  Merge &operator=(const Merge &) = delete;

  // Merges the true and false branches as a zip.
  Queryable<std::tuple<TrueT, FalseT>> merge(bool truncate = false) const {
    return true_queried_.zip(std::move(false_queried_.to_vector()), truncate);
  }

private:
  const Queryable<TrueT> true_queried_;
  const Queryable<FalseT> false_queried_;
};

template <typename WhenTrueQueriedT, typename T> class WhenFalse {
public:
  explicit WhenFalse(Queryable<WhenTrueQueriedT> when_true_queried,
                     std::vector<T> when_false_items)
      : when_true_queried_(std::move(when_true_queried)),
        when_false_items_(std::move(when_false_items)) {}
  WhenFalse() = delete;
  virtual ~WhenFalse() = default;
  WhenFalse(const WhenFalse &) = delete;
  WhenFalse &operator=(const WhenFalse &) = delete;

  // Queries the false branched items from the Queryable<T>::branch predicate.
  template <
      typename WhenFalseQuery /* =
                                 std::function<Queryable<WhenFalseQueriedT>(Queryable<T>&)>
                               */
      ,
      typename WhenFalseQueriedT =
          std::invoke_result_t<WhenFalseQuery, Queryable<T>>::item_type>
  Merge<WhenTrueQueriedT, WhenFalseQueriedT>
  when_false(WhenFalseQuery when_false_query) const {
    return Merge<WhenTrueQueriedT, WhenFalseQueriedT>(
        Queryable<WhenTrueQueriedT>(when_true_queried_.to_vector()),
        when_false_query(Queryable<T>(std::move(when_false_items_))));
  }

private:
  const Queryable<WhenTrueQueriedT> when_true_queried_;
  const std::vector<T> when_false_items_;
};

// True branch from the Queryable<T>::branch predicate.
template <typename T> class WhenTrue {
public:
  explicit WhenTrue(std::vector<T> when_true_items,
                    std::vector<T> when_false_items)
      : when_true_items_(std::move(when_true_items)),
        when_false_items_(std::move(when_false_items)) {}
  WhenTrue() = delete;
  virtual ~WhenTrue() = default;
  WhenTrue(const WhenTrue &) = delete;
  WhenTrue &operator=(const WhenTrue &) = delete;

  // Queries the true branched items from the Queryable<T>::branch predicate.
  template <
      typename WhenTrueQuery /* =
                                std::function<Queryable<WhenTrueQueriedT>(Queryable<T>)>
                              */
      ,
      typename WhenTrueQueriedT =
          std::invoke_result_t<WhenTrueQuery, Queryable<T>>::item_type>
  WhenFalse<WhenTrueQueriedT, T>
  when_true(WhenTrueQuery when_true_query) const {
    return WhenFalse<WhenTrueQueriedT, T>(
        std::move(when_true_query(Queryable<T>(std::move(when_true_items_)))),
        std::move(when_false_items_));
  }

private:
  const std::vector<T> when_true_items_;
  const std::vector<T> when_false_items_;
};

template <typename T>
template <typename Predicate>
WhenTrue<T> Queryable<T>::branch(Predicate predicate) const {
  std::vector<T> when_true_items;
  std::vector<T> when_false_items;
  std::for_each(std::make_move_iterator(items_.begin()),
                std::make_move_iterator(items_.end()), [&](auto item) {
                  if (predicate(item)) {
                    when_true_items.push_back(std::move(item));
                  } else {
                    when_false_items.push_back(std::move(item));
                  }
                });

  return WhenTrue<T>(std::move(when_true_items), std::move(when_false_items));
}

} // namespace fcpp

#endif // FCPP_QUERY_H