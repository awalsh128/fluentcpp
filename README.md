# Fluent C++ :: Readme

[![Project Status](https://www.repostatus.org/badges/latest/active.svg)](http://www.repostatus.org/#active)
[![Build](https://github.com/awalsh128/fluentcpp/actions/workflows/integrate.yml/badge.svg)](https://github.com/awalsh128/fluentcpp/actions/workflows/integrate.yml)
[![License: Apache2](https://shields.io/badge/license-apache2-blue.svg)](https://github.com/awalsh128/fluentcpp/blob/master/LICENSE)
[![](https://tokei.rs/b1/github/awalsh128/fluentcpp)](https://github.com/awalsh128/fluentcpp)
[![Code Grade](https://img.shields.io/lgtm/grade/cpp/g/awalsh128/fluentcpp.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/awalsh128/fluentcpp/context:cpp)
[![Code Alerts](https://img.shields.io/lgtm/alerts/g/awalsh128/fluentcpp.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/awalsh128/fluentcpp/alerts/)

**Fluent C++** is a library that can be used to express information in a functional way that allows for easy auto-complete and shorter expressions.

```cpp
  std::vector<int> xs;
  for (int x = 0; x < 1000; x++) xs.push_back(x);

  fcpp::query(xs)
      .where(EXPR(x, x > 500))
      .shuffle()
```

## Installing

- Go to [the release](https://github.com/awalsh128/fluentcpp/releases) you want to install.
- Download the `fluentcpp-<tag>.tar.gz` file.
- Decompress the file in the directory `tar -xvzf fluentcpp-<tag>.tar.gz`.
- Run the install command `sudo ./install.sh`.
- The library files are now installed on your system and can be used as in the examples.

## Building

If you just want to build the source code and not install it, you can run these commands instead.

Two types of builds:

one for local debugging and library generation,

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug # Builds and tests
cmake --build . --config Debug --target fluentcpp
```

and the other for all release assets.

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug # Builds, tests and creates documentation
cmake --build . --config Debug --target fluentcpp
```

Lastly, to install the library on the system for use.

```bash

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target fluentcpp
make install
```

This will install artifacts in the ``/usr/local/lib/fluentcpp`` and ``/usr/local/include/fluentcpp`` directories so that they can be referenced as the usage example below (i.e. ``#include <fluentcpp/query.h``.

## Usage Examples

Find more in the [examples folder](https://github.com/awalsh128/fluentcpp/tree/master/examples).

```cpp
#include <fluentcpp/query.h>
#include <iostream>
#include <vector>

int main()
{
  // @todo Add descriptions and use cases.
  std::vector<int> xs;
  for (int x = 0; x < 1000; x++) xs.push_back(x);

  fcpp::query(xs)
      .where(EXPR(x, x > 500))
      .shuffle()
      .skip(10)
      .select(EXPR(x, x % 10))
      .distinct()
      .order_by(EXPR(x, x))
      .branch(EXPR(x, x > 5))
      .when_true([](auto q)
                 { return q.select(EXPR(x, x + 100)); })
      .when_false([](auto q)
                  { return q.select(EXPR(x, x - 100)); })
      .merge()
      .select(EXPR(x, std::get<0>(x)))
      .action([](auto x)
              { std::cout << x; });
}
```

## Dependencies

- [CMake](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) - C++ build system for generating library objects.
- [Catch](https://github.com/catchorg/Catch2/blob/devel/README.md) - Catch v2 test framework. This is pulled in by CMake at build time.
- [Doxygen](https://www.doxygen.nl/index.html) - API documentation product.
