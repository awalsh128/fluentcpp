#include <memory>
#include <ostream>

namespace fcpp::tests::models {

struct Object {
  int value;

  Object() : value(0) {}
  Object(int value) : value(value) {}

  inline bool operator==(const Object &rhs) const { return value == rhs.value; }
  inline bool operator==(int rhs) const { return value == rhs; }

  inline bool operator>(const Object &rhs) const { return value > rhs.value; }
  inline bool operator>(int rhs) const { return value > rhs; }

  inline bool operator<(const Object &rhs) const { return value < rhs.value; }
  inline bool operator<(int rhs) const { return value < rhs; }

  inline Object operator%(int rhs) {
    value %= rhs;
    return std::move(*this);
  }
  inline Object operator+(const Object &rhs) {
    value += rhs.value;
    return std::move(*this);
  }
  inline Object operator+(int rhs) {
    value += rhs;
    return std::move(*this);
  }
  inline Object operator++() {
    value++;
    return std::move(*this);
  }
  inline Object operator-(int rhs) {
    value -= rhs;
    return std::move(*this);
  }
  inline Object operator--() {
    value--;
    return std::move(*this);
  }

  operator int() const { return value; }
};

std::ostream &operator<<(std::ostream &os, const Object &item) {
  os << item.value;
  return os;
}

struct NonCopyObject : Object {
  NonCopyObject() : Object() {}
  NonCopyObject(int value) : Object(value) {}

  NonCopyObject(NonCopyObject &&) = default;                // move constructor
  NonCopyObject &operator=(NonCopyObject &&) = default;     // move assignment
  NonCopyObject(const NonCopyObject &) = delete;            // copy constructor
  NonCopyObject &operator=(const NonCopyObject &) = delete; // copy assignment

  inline bool operator==(const Object &rhs) const { return value == rhs.value; }
  inline bool operator==(int rhs) const { return value == rhs; }

  inline bool operator>(const Object &rhs) const { return value > rhs.value; }
  inline bool operator>(int rhs) const { return value > rhs; }

  inline bool operator<(const Object &rhs) const { return value < rhs.value; }
  inline bool operator<(int rhs) const { return value < rhs; }

  inline NonCopyObject operator%(int rhs) {
    value %= rhs;
    return std::move(*this);
  }
  inline NonCopyObject operator+(const NonCopyObject &rhs) {
    value += rhs.value;
    return std::move(*this);
  }
  inline NonCopyObject operator+(int rhs) {
    value += rhs;
    return std::move(*this);
  }
  inline NonCopyObject operator++() {
    value++;
    return std::move(*this);
  }
  inline NonCopyObject operator-(int rhs) {
    value -= rhs;
    return std::move(*this);
  }
  inline NonCopyObject operator--() {
    value--;
    return std::move(*this);
  }

  operator int() const { return value; }
};

std::ostream &operator<<(std::ostream &os, const NonCopyObject &item) {
  os << item.value;
  return os;
}

} // namespace fcpp::tests::models