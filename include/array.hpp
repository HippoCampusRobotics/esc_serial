#pragma once

#include <wchar.h>

namespace esc_serial {
template <typename T, size_t N>
struct array {
  T _data[N];
  T& operator[](size_t _index) { return _data[index]; }
  T* begin() { return &_data[0]; }
  T* end() { return &_data[N]; }
  constexpr size_t size() const { return N; }
};
}  // namespace esc_serial
