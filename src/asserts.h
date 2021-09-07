/**
 * @file asserts.h
 * @author Andrew Walsh (awalsh128@gmail.com)
 * @brief Runtime validation of object states.
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

#ifndef FCPP_VALIDATIONS_H
#define FCPP_VALIDATIONS_H

#include <sstream>
#include <string>

namespace fcpp::asserts {

/**
 * @brief Enforce assertions that a condition will never change during the
 * running lifetime of that code.
 *
 */
class invariant {
private:
  const bool condition_;
  std::stringstream message_;

  invariant(bool condition, std::string message)
      : condition_(condition), message_(message) {}

  // https://herbsutter.com/2014/05/03/reader-qa-how-can-i-prevent-a-type-from-being-instantiated-on-the-stack/
  invariant() = delete;
  invariant(const invariant &) = default;

public:
  ~invariant() noexcept(false) {
    if (!condition_) {
      // Living dangerously, guaranteed not to live on the stack though. See
      // above link.
      throw std::invalid_argument(
          message_.str()); // lgtm [cpp/throw-in-destructor]
    }
  }

  /**
   * @brief Sets the message as an incoming stream.
   *
   * @tparam Text Type of element that will be streamed into message string.
   * @param item Invariant to set the message on.
   * @param text Element that will be streamed into message string.
   * @return invariant&&
   */
  template <typename Text>
  friend invariant &&operator<<(invariant &&item, const Text &text) {
    item.message_ << text;
    return std::move(item);
  }

  /**
   * @brief Evaluate the invariant and throw std::invalid_argument if violated.
   *
   * @remark Object cannot be an lvalue and will be destroyed right after
   * creation.
   *
   * @param condition Any predicate that affirms the invariant condition.
   * @return invariant Item holding invariant evaluation.
   */
  static invariant eval(bool condition) { return invariant(condition, ""); }
};

} // namespace fcpp::asserts

#endif // FCPP_VALIDATIONS_H
