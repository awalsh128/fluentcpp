#!/usr/bin/env bash
#
# Builds and installs the Fluent C++ library.
#
# Single command install of the latest release:
#
#   curl -fsSL https://raw.githubusercontent.com/awalsh128/fluentcpp/master/scripts/install.sh | bash
#
# Pass options after "bash -s --":
#
#   curl -fsSL .../install.sh | bash -s -- --version v0.2 --prefix "$HOME/.local"
#
# When run from an extracted release package, that package is built instead of
# downloading one.

set -euo pipefail

REPO="awalsh128/fluentcpp"

usage() {
  cat <<EOF
Usage: install.sh [--version <tag|branch>] [--prefix <dir>]

Options:
  --version  Release tag or branch to install (default: latest release).
  --prefix   Installation prefix (default: /usr/local).
  -h, --help Show this help.

Environment:
  CXX        C++20 compiler to build with (default: clang++ if found).
EOF
}

# Global so the EXIT trap can still see it after main returns.
WORK_DIR=""

if [ -t 2 ]; then
  BLUE=$'\033[1;34m' RED=$'\033[1;31m' RESET=$'\033[0m'
else
  BLUE="" RED="" RESET=""
fi

log() { printf '%s==>%s %s\n' "$BLUE" "$RESET" "$*" >&2; }
fail() {
  printf '%serror:%s %s\n' "$RED" "$RESET" "$*" >&2
  exit 1
}

# Runs a command quietly, showing its output only if it fails.
run_logged() {
  if ! "$@" >>"$WORK_DIR/build.log" 2>&1; then
    cat "$WORK_DIR/build.log" >&2
    fail "Command failed: $*"
  fi
}

require() {
  command -v "$1" >/dev/null 2>&1 || fail "$1 is required but was not found."
}

latest_version() {
  # Follow the latest release redirect rather than the rate limited API.
  local url
  url=$(curl -fsSLI -o /dev/null -w '%{url_effective}' \
    "https://github.com/${REPO}/releases/latest") ||
    fail "Could not determine the latest release; pass --version."
  echo "${url##*/}"
}

# Wrapped in a function so a partially downloaded script never runs.
main() {
  local version="" prefix="/usr/local"
  while [ $# -gt 0 ]; do
    case "$1" in
    --version)
      version="${2:?--version requires a value}"
      shift 2
      ;;
    --prefix)
      prefix="${2:?--prefix requires a value}"
      shift 2
      ;;
    -h | --help)
      usage
      exit 0
      ;;
    *)
      usage >&2
      fail "Unknown option: $1"
      ;;
    esac
  done

  require cmake

  local source_dir
  WORK_DIR=$(mktemp -d)
  trap 'rm -rf "$WORK_DIR"' EXIT

  # BASH_SOURCE is unset when the script is piped into bash.
  local script_dir=""
  if [ -n "${BASH_SOURCE[0]:-}" ] && [ -f "${BASH_SOURCE[0]}" ]; then
    script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
  fi

  if [ -z "$version" ] && [ -f "$script_dir/CMakeLists.txt" ]; then
    log "Building release package in $script_dir."
    source_dir="$script_dir"
  else
    require curl
    require tar
    if [ -z "$version" ]; then
      version=$(latest_version)
    fi
    log "Downloading Fluent C++ ${version}."
    curl -fsSL -o "$WORK_DIR/source.tar.gz" \
      "https://github.com/${REPO}/archive/${version}.tar.gz" ||
      fail "Could not download version ${version}."
    source_dir="$WORK_DIR/source"
    mkdir -p "$source_dir"
    tar -xf "$WORK_DIR/source.tar.gz" -C "$source_dir" --strip-components=1
  fi

  local cmake_args=(-DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=${prefix}")
  if [ -n "${CXX:-}" ]; then
    cmake_args+=("-DCMAKE_CXX_COMPILER=${CXX}")
  elif command -v clang++ >/dev/null 2>&1; then
    cmake_args+=(-DCMAKE_CXX_COMPILER=clang++)
  fi
  if command -v ninja >/dev/null 2>&1; then
    cmake_args+=(-G Ninja)
  fi

  log "Building."
  run_logged cmake -S "$source_dir" -B "$WORK_DIR/build" "${cmake_args[@]}"
  run_logged cmake --build "$WORK_DIR/build" --config Release --target fluentcpp

  log "Installing to ${prefix}."
  local sudo=""
  if [ "$(id -u)" -ne 0 ]; then
    mkdir -p "$prefix" 2>/dev/null || true
    if [ ! -w "$prefix" ]; then
      require sudo
      sudo="sudo"
    fi
  fi
  $sudo cmake --install "$WORK_DIR/build" |
    sed -e '/^-- Install configuration/d' -e 's/^-- Installing: /    /' >&2

  local libdir
  libdir=$(sed -n 's/^CMAKE_INSTALL_LIBDIR:PATH=//p' "$WORK_DIR/build/CMakeCache.txt")
  log "Installed Fluent C++. Include with #include <fluentcpp/query.h> and"
  log "link with -L${prefix}/${libdir:-lib}/fluentcpp -lfluentcpp."
}

main "$@"
