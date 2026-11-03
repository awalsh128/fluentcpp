/**
 * @file transforms.h
 * @author Andrew Walsh (awalsh128@gmail.com)
 * @brief Transforms between standard container types.
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

#ifndef FCPP_TRANSFORMS_H
#define FCPP_TRANSFORMS_H

#include <set>
#include <utility>
#include <vector>

namespace fcpp::transforms {

/**
 * @brief Transform a set to a vector using copy semantics.
 *
 * @tparam T Type of items in the set to copy.
 * @param items Set of items to transform.
 * @return std::vector<T> Transformed vector of items.
 */
template <typename T>
std::vector<T> to_vector(const std::set<T>& items) {
  std::vector<T> result;
  result.reserve(items.size());
  for (const auto& v : items) {
    result.emplace_back(v);
  }
  return result;
}

/**
 * @brief Transform a set to a vector using move semantics.
 *
 * @tparam T Type of items in the set to move.
 * @param items Set of items to transform.
 * @return std::vector<T> Transformed vector of moved items.
 */
template <typename T>
std::vector<T> to_vector(std::set<T>&& items) {
  std::vector<T> result;
  result.reserve(items.size());
  while (!items.empty()) {
    auto nh = items.extract(items.begin());
    result.emplace_back(std::move(nh.value()));
  }
  return result;
}

} // namespace fcpp::transforms

#endif // FCPP_TRANSFORMS_H
