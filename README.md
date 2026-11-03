<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="assets/artwork/fluentcpp-sm.png">
    <img src="assets/artwork/fluentcpp-sm-light.png" alt="Fluent C++ logo" width="240" height="69">
  </picture>
</p>

<h1 align="center">Fluent C++</h1>

<p align="center">
  Statically typed, fluent query functions for C++20 containers that are easy to discover through
  auto-complete.
</p>

<p align="center">
  <a href="https://github.com/awalsh128/fluentcpp/actions/workflows/integrate.yml"><img src="https://github.com/awalsh128/fluentcpp/actions/workflows/integrate.yml/badge.svg" alt="Build status"></a>
  <a href="https://github.com/awalsh128/fluentcpp/blob/master/LICENSE"><img src="https://img.shields.io/badge/license-Apache%202.0-blue.svg" alt="Apache 2.0 license"></a>
  <a href="https://en.cppreference.com/w/cpp/20"><img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg" alt="C++20"></a>
  <a href="https://cmake.org/"><img src="https://img.shields.io/badge/CMake-3.16.3+-064F8C.svg?logo=cmake" alt="CMake 3.16.3+"></a>
  <a href="https://github.com/awalsh128/fluentcpp"><img src="https://img.shields.io/badge/platform-linux-lightgrey.svg" alt="Linux"></a>
</p>

<!-- prettier-ignore -->
> [!TIP]
> If you find this project useful, please consider supporting it
> [as a sponsor and show some ❤️ for open source maintainers](https://github.com/sponsors/awalsh128).

Wrap a container in `fcpp::query` and chain operations on it, instead of writing loops and
temporary containers by hand.

```cpp
fcpp::query(xs)
    .where(EXPR(x, x > 500))
    .select(EXPR(x, x % 10))
    .distinct()
    .to_vector();
```

- :link: Chain filtering, projection, ordering, set and grouping operations in a single expression.
- :mag: Find every operation through auto-complete on the returned `fcpp::Queryable<T>`.
- :zap: Write terse lambdas with the `EXPR` and `CR_EXPR` macros.
- :package: Works with move-only types; items are moved through the chain rather than copied.

---

## Installation

Install the latest release with a single command:

```bash
curl -fsSL https://raw.githubusercontent.com/awalsh128/fluentcpp/master/scripts/install.sh | bash
```

This downloads the source, builds the library and installs it to `/usr/local/lib/fluentcpp`, with
the headers in `/usr/local/include/fluentcpp`. It runs as your user and only asks for `sudo` to copy
the files if the install location isn't writable. Requires `curl`, CMake 3.16.3 or newer, and a
C++20 compiler; `clang++` and Ninja are used when available.

Pass options after `bash -s --` to pick a version or install location:

```bash
curl -fsSL https://raw.githubusercontent.com/awalsh128/fluentcpp/master/scripts/install.sh |
  bash -s -- --version v0.2 --prefix "$HOME/.local"
```

| Option      | Default        | Description                                                    |
| ----------- | -------------- | -------------------------------------------------------------- |
| `--version` | Latest release | Release tag or branch to install, such as `v0.2` or `master`.  |
| `--prefix`  | `/usr/local`   | Installation prefix for the `lib` and `include` directories.   |

To build with a specific compiler, set `CXX` on the `bash` side of the pipe, for example
`... | CXX=g++ bash`. To read the script before running it, download it with `curl -fsSLO` and
run `bash install.sh`.

Alternatively, download `fluentcpp-<tag>.tar.gz` from a
[release](https://github.com/awalsh128/fluentcpp/releases), extract it and run `./install.sh` from
the extracted directory.

## Usage

Include the query header and wrap any `std::vector`:

```cpp
#include <fluentcpp/query.h>

#include <iostream>
#include <vector>

int main() {
  std::vector<int> xs;
  for (int x = 0; x < 1000; x++) xs.push_back(x);

  std::vector<int> result =
      fcpp::query(xs)
          .where(EXPR(x, x > 500))
          .shuffle()
          .skip(10)
          .select(EXPR(x, x % 10))
          .distinct()
          .order_by(EXPR(x, x))
          .branch(EXPR(x, x > 5))
          .when_true([](auto q) { return q.select(EXPR(x, x + 100)); })
          .when_false([](auto q) { return q.select(EXPR(x, x - 100)); })
          .merge()
          .select(EXPR(x, std::get<0>(x)))
          .to_vector();

  for (int x : result) std::cout << x << " ";
}
```

Link against the installed static library:

```bash
clang++ -std=c++20 main.cpp -L/usr/local/lib/fluentcpp -lfluentcpp
```

Find more in the [examples folder](examples). The full API reference is generated with Doxygen from
the comments in [include/fluentcpp](include/fluentcpp).

### Operations

| Category     | Operations                                                                       |
| ------------ | -------------------------------------------------------------------------------- |
| Filtering    | `where`, `distinct`, `skip`, `take`, `take_random`, `slice`, `trim`              |
| Projection   | `select`, `flatten`, `zip`, `join`                                               |
| Ordering     | `order_by`, `sort`, `reverse`, `shuffle`                                         |
| Sets         | `difference`, `intersect`, `unionize`                                            |
| Grouping     | `group_by`, `keyed_group_by`, `branch`, `when_true`, `when_false`, `merge`       |
| Aggregation  | `accumulate`, `all`, `any`, `min`, `max`, `first_or_default`                     |
| Side effects | `action`                                                                         |
| Conversion   | `to_vector`, `to_set`, `to_single_value_map`, `to_multi_value_map`               |

## Building

Building from source requires Clang (`clang++`), CMake and Ninja:

```bash
sudo apt-get install -y clang cmake ninja-build
```

### Debug Build with Tests

The `debug` preset in [CMakePresets.json](CMakePresets.json) configures clang++, Ninja, tests and
examples into `build/debug`:

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Or, without presets:

```bash
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

### Release Build and Install

```bash
cmake --preset release
cmake --build --preset release --target fluentcpp
sudo cmake --install build/release
```

### Build Options

| Option             | Default | Description                                                         |
| ------------------ | ------- | ------------------------------------------------------------------- |
| `BUILD_TESTS`      | `OFF`   | Build the Catch2 unit tests in [tests](tests).                      |
| `BUILD_BENCHMARKS` | `OFF`   | Build the Google Benchmark tests in [benchmarks](benchmarks).       |
| `BUILD_EXAMPLES`   | `OFF`   | Build the example programs in [examples](examples).                 |
| `BUILD_WEBSITE`    | `OFF`   | Build the Doxygen API website from [docs](docs); requires Doxygen. |

### Project Layout

```text
include/fluentcpp/  Public headers, included as <fluentcpp/query.h>.
src/                Library implementation files.
tests/              Catch2 unit tests.
benchmarks/         Google Benchmark tests and reporting.
examples/           Example programs.
docs/               Doxygen configuration and website.
cmake/              CMake helper modules.
scripts/            Install, release and developer scripts.
assets/             Artwork and other non-code assets.
```

## Dependencies

- [CMake](https://cmake.org/) - Build system.
- [Clang](https://clang.llvm.org/) - C++20 compiler used for builds, formatting and static
  analysis.
- [Catch2](https://github.com/catchorg/Catch2) - Unit test framework, fetched by CMake when
  `BUILD_TESTS=ON`.
- [Google Benchmark](https://github.com/google/benchmark) - Benchmark framework, fetched by CMake
  when `BUILD_BENCHMARKS=ON`.
- [Doxygen](https://www.doxygen.nl/) - API documentation generator, used when `BUILD_WEBSITE=ON`.

Report bugs or request features through
[GitHub Issues](https://github.com/awalsh128/fluentcpp/issues). For development guidelines, see
[CONTRIBUTING.md](CONTRIBUTING.md).

## License

[Apache 2.0](LICENSE).
