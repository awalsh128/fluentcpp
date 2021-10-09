#include <set>
#include <vector>

namespace fcpp::transforms {

template <typename T>
std::vector<T> to_vector(std::set<T> items) {
  std::vector<T> result;
  result.reserve(items.size());
  for (auto it = items.begin(); it != items.end();) {
    result.push_back(std::move(items.extract(it++).value()));
  }
  return result;
}

} // namespace fcpp::transforms