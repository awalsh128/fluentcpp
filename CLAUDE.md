# CLAUDE: C++ / YAML / Clang — Best Practices

This file collects concise, actionable best practices for working with C++ (modern idioms), YAML workflow files (GitHub Actions), and Clang/tooling in this repository. Use it as a single reference for code quality, CI, and local developer ergonomics.

## Goals

- Keep the code modern, readable, and portable (prefer C++20 idioms).
- Make CI workflows reliable, reproducible, and secure.
- Prefer tooling that gives fast, deterministic feedback: clang-format, clang-tidy, compiler warnings, sanitizers.

---

## C++ Best Practices (applies to this project)

1. Use modern C++ features (C++17/C++20) sparingly and clearly:
   - Prefer `auto` for local type inference when it improves readability.
   - Use `constexpr` and `consteval` for compile-time computation where appropriate.
   - Prefer `std::span`, `std::optional`, `std::variant`, and `std::string_view` over raw pointers/hand-rolled alternatives.

2. Be explicit about ownership and lifetime:
   - Prefer value semantics and `std::unique_ptr`/`std::shared_ptr` only when needed.
   - Avoid raw `new`/`delete`.

3. API design:
   - Keep interfaces small and expressive. Favor free functions when appropriate.
   - Use `noexcept` on functions that do not throw.
   - Prefer returning objects by value (RVO/NRVO) for small types.

4. Performance-aware idioms:
   - Prefer algorithms from `<algorithm>` instead of hand-rolled loops when they are clear.
   - Use `reserve()` on containers when size is known.
   - Be mindful of copies; prefer `std::move` for transfer of ownership.

5. Testing and benchmarks:
   - Add unit tests for public APIs (Catch2 used in this project).
   - Keep tests fast and deterministic; avoid flaky timing-based assertions.
   - Benchmarks should be isolated and repeatable (use Google Benchmark where appropriate).

6. Documentation and examples:
   - Keep example code small and focused.
   - Add doc comments for public API functions and types; aim for Doxygen-friendly comments.

### Comment / documentation style (use the `query.h` style)

- This project uses a Doxygen / Javadoc-like comment style as seen in `include/fluentcpp/query.h`.
- Use `/** ... */` block comments for public APIs and types. Start with a short
  `@brief` summary line, then provide parameter and return documentation using
  `@param`, `@tparam`, and `@return` tags. Use `@remark`, `@todo`, `@code`/
  `@endcode` sections when useful.
- Keep the `@brief` concise (one sentence). Follow with an empty line, then a
  slightly longer explanatory paragraph if needed.
- Tag guidance:
  - `@brief` — a single sentence summary of what the function/type does.
  - `@param name` — short description of the parameter; mention ownership or
    whether it is moved/borrowed when relevant.
  - `@tparam name` — describe template parameters and any required concepts.
  - `@return` — describe what is returned and important post-conditions.
  - `@remark` — optional implementation notes or important caveats.
  - `@todo` — used for future work; keep sparse and actionable.
- Style rules:
  - Use sentence case and end sentences with a period.
  - Wrap text at ~80 characters to match repository formatting.
  - Prefer describing behaviour and side-effects clearly (ownership, moves,
    invalidation of sources) — e.g. "Object state will be invalid after this
    call since items have been moved out." like in `query.h`.
  - Include short examples with `@code` / `@endcode` for non-trivial APIs.
- Example (adapted from `query.h`):

```cpp
/**
 * @brief Gets the first item of a sequence, or a default value if no item is
 * found.
 *
 * @param predicate Function to test each item for a condition.
 * @return std::optional<T> Either the satisfying item or the not found item
 * of std::nullopt.
 */
template <typename Predicate>
std::optional<T> first_or_default(Predicate predicate);
```

- Automate basic checks: add a CI linting step that flags missing `@brief`,
  `@param/@tparam/@return` for public APIs (or surface a documentation
  checklist as a PR template).

7. Include hygiene:
   - Use header guards or `#pragma once` consistently.
   - Minimize header includes in public headers; prefer forward declarations where possible.

8. Safe concurrency:
   - Use high-level concurrency primitives (`std::mutex`, `std::atomic`, `std::lock_guard`) and document invariants.

---

## YAML (GitHub Actions) Best Practices

1. Keep workflows small and focused:
   - One workflow per concern: `ci.yml` (build/test/lint), `codeql.yml` (security analysis), `release.yml` (releases), `deploy_docs.yml` (docs).

2. Pin actions when stability matters:
   - Prefer tags like `actions/checkout@v3` for normal use. For maximum reproducibility, pin to a commit SHA (e.g., `actions/checkout@<sha>`).
   - Example: `uses: actions/checkout@v3` (or a SHA when you want exact reproducibility).

3. Don't run costly checks on every push by default:
   - Run expensive analysis (e.g., CodeQL, heavy static analysis) on schedule or on `push` to main only.

4. Use caching for toolchain artifacts:
   - Cache build tools and ccache to reduce runtime.

5. Make builds reproducible and explicit:
   - Pass explicit compiler variables to CMake to force the desired compiler (e.g., `-DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++`).
   - Export `compile_commands.json` for clang tooling: `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`.

6. Write robust shell steps:
   - Fail fast on real errors, but make informational checks (clang-format/clazy) optionally non-blocking in analysis jobs.
   - Use `|| true` only for intentional non-fatal steps.

7. YAML syntax:
   - Use 2-space indentation (consistent with this repo).
   - Avoid tabs (YAML treats tabs as invalid indentation).
   - Quote strings that contain special characters (e.g., cron expressions) if your linter complains.

---

## Clang & Tooling Best Practices

1. Formatting: `clang-format`
   - Include a repository `.clang-format` that defines the style (we provide one).
   - Add CI checks to verify formatting on PRs; either fail the job or surface the diff to the author.
   - Editor integration: recommend installing the `clangd` and `clang-format` extensions and enabling "format on save" for supported files.

2. Static analysis: `clang-tidy`
   - Export `compile_commands.json` from CMake and run `clang-tidy -p build <files>`.
   - Use a baseline configuration (a `.clang-tidy` file or CLI args) and treat warnings as actionable.
   - In CI: run clang-tidy in informational mode for pull requests and as a gate on main if your team is ready.

3. Compiler warnings and sanitizers
   - Treat warnings as errors in CI for release builds: `-Werror` (use cautiously) or selectively escalate important warnings.
   - Recommended warning groups (adjust to taste):
     - -Wall -Wextra -Wpedantic
     - Useful: -Wconversion -Wcast-align -Wsign-conversion (add carefully)
   - Use sanitizers in CI jobs for catch-all dynamic analysis on relevant platforms:
     - AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan), LeakSanitizer (LSan)
   - Example CMake flags for sanitizer builds:
     -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"

4. Toolchain selection
   - Prefer clang for static analysis and tool compatibility, but test with GCC as well.
   - Force clang in CI when the project depends on clang-specific behavior, otherwise keep matrix builds.

5. Language server: `clangd`
   - Use clangd for accurate completions and diagnostics (it consumes compile_commands.json).
   - Configure the editor to point clangd at the project's `compile_commands.json` (CMake Tools can generate it automatically).

---

## Example: CMake + Clang CI snippet

```bash
mkdir -p build
cd build
# Force Clang and generate compile commands for tools
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
cmake --build . -- -j$(nproc)
ctest --output-on-failure
```

## Quick commands for devs

- Format all sources:

```bash
git ls-files '*.cpp' '*.h' | xargs -r clang-format -i
```

- Run clang-tidy (requires compile_commands.json):

```bash
clang-tidy -p build src/*.cpp
```

- Build with sanitizers (local):

```bash
mkdir -p build-asan && cd build-asan
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build .
```

---

## Policy notes and team preferences

- Decide on a single authoritative style for the project (the `.clang-format` file). Make PRs comply with it.
- Choose whether clang-tidy warnings are blocking or advisory and document that in repo policies.
- Use branch protection rules to require CI passing for merges to `master`/`main`.
