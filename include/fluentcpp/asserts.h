/**
 * @file asserts.h
 * @author Andrew Walsh (awalsh128@gmail.com)
 * @brief Runtime validation of object states.
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

#ifndef FCPP_VALIDATIONS_H
#define FCPP_VALIDATIONS_H

#include <exception>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace fcpp::asserts {

/**
 * @brief Enforce assertions that a condition will never change during the
 * running lifetime of that code.
 *
 * The message is only formatted when the condition is violated, so passing
 * checks stay cheap.
 *
 * @code
 * asserts::invariant::eval(value <= size()) << "Value " << value << ".";
 * @endcode
 */
class invariant {
private:
  // Only engaged when the condition is violated.
  std::optional<std::ostringstream> message_;

  explicit invariant(bool condition) {
    if (!condition) {
      message_.emplace();
    }
  }

  // https://herbsutter.com/2014/05/03/reader-qa-how-can-i-prevent-a-type-from-being-instantiated-on-the-stack/
  invariant() = delete;
  invariant(const invariant &) = delete;
  invariant &operator=(const invariant &) = delete;

public:
  ~invariant() noexcept(false) { // NOLINT(bugprone-exception-escape)
    // Throwing while another exception unwinds the stack would terminate.
    if (message_ && std::uncaught_exceptions() == 0) {
      // Living dangerously, guaranteed not to live on the stack though. See
      // above link.
      throw std::invalid_argument(message_->str());
    }
  }

  /**
   * @brief Sets the message as an incoming stream.
   *
   * @remark No-op when the condition holds.
   *
   * @tparam Text Type of element that will be streamed into message string.
   * @param item Invariant to set the message on.
   * @param text Element that will be streamed into message string.
   * @return invariant&&
   */
  template <typename Text>
  friend invariant &&operator<<(invariant &&item, const Text &text) {
    if (item.message_) {
      *item.message_ << text;
    }
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
  static invariant eval(bool condition) { return invariant(condition); }
};

} // namespace fcpp::asserts

#endif // FCPP_VALIDATIONS_H
