/**
 * @file traits.h
 * @author Andrew Walsh (awalsh128@gmail.com)
 * @brief Traits for asserting object features.
 * @version 0.1
 * @date 2021-09-03
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

#ifndef FCPP_TRAITS_H
#define FCPP_TRAITS_H

#include <iterator>
#include <type_traits>

namespace fcpp::traits {

template <typename T, typename = void> struct is_additive : std::false_type {};

template <typename T>
struct is_additive<T,
                   typename std::enable_if<
                       true, decltype(std::declval<T &>() + std::declval<T &>(),
                                      (void)0)>::type> : std::true_type {};

template <typename T, typename = void>
struct is_equality_comparable : std::false_type {};

template <typename T>
struct is_equality_comparable<
    T, typename std::enable_if<true, decltype(std::declval<T &>() ==
                                                  std::declval<T &>(),
                                              (void)0)>::type>
    : std::true_type {};

template <typename T, typename = void>
struct is_less_than_comparable : std::false_type {};

template <typename T>
struct is_less_than_comparable<
    T, typename std::enable_if<true, decltype(std::declval<T &>() <
                                                  std::declval<T &>(),
                                              (void)0)>::type>
    : std::true_type {};

} // namespace fcpp::traits

#endif // FCPP_TRAITS_H