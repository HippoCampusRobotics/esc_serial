#pragma once

#include <stdint.h>
#include <string.h>
namespace esc_serial {
namespace impl {
template <typename T>
void Put(uint8_t *_buffer, size_t _offset, T _data) {
  *(T *)&_buffer[_offset] = _data;
}

template <typename T>
T Get(const uint8_t *_buffer, size_t _offset) {
  return *(const T *)&_buffer[_offset];
}
}  // namespace impl

class Serializer {
 public:
  explicit Serializer(uint8_t *_buf)
      : buf_(_buf), const_buf_(_buf), position_(0){};
  explicit Serializer(const uint8_t *_const_buf)
      : buf_(nullptr), const_buf_(_const_buf), position_(0){};
  inline void reset() { position_ = 0; }

  template <typename T>
  void Serialize(const T _data) {
    if (!buf_) {
      return;
    }
    impl::Put<T>(buf_, position_, _data);
    position_ += sizeof(T);
  }

  template <typename T, size_t S>
  void Serialize(const T *_data) {
    for (size_t i = 0; i < S; ++i) {
      Serialize(_data[i]);
    }
  }

  template <typename T>
  void Deserialize(T &_data) {
    if (!const_buf_) {
      return;
    }
    _data = impl::Get<T>(const_buf_, position_);
    position_ += sizeof(T);
  }

  template <typename T, size_t S>
  void Deserialize(T *_data) {
    for (size_t i = 0; i < S; ++i) {
      Deserialize(_data[i]);
    }
  }

  inline size_t ByteCount() const { return position_; }

 private:
  uint8_t *buf_;
  const uint8_t *const_buf_;
  size_t position_;
};
}
