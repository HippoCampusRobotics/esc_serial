#pragma once

#include <endian.h>
#include <stdint.h>
#include <string.h>

namespace impl {
template <size_t _N>
struct UintBufferHelper;

template <>
struct UintBufferHelper<1> {
  typedef uint8_t Type;
};

template <>
struct UintBufferHelper<2> {
  typedef uint16_t Type;
};

template <>
struct UintBufferHelper<4> {
  typedef uint32_t Type;
};

template <>
struct UintBufferHelper<8> {
  typedef uint64_t Type;
};

template <typename T>
struct UintBuffer {
  typedef typename UintBufferHelper<sizeof(T)>::Type Type;
};

template <typename T>
T HostToLittleEndian(T);

template <>
inline uint8_t HostToLittleEndian<uint8_t>(uint8_t _data) {
  return _data;
}

template <>
inline uint16_t HostToLittleEndian<uint16_t>(uint16_t _data) {
  return htole16(_data);
}

template <>
inline uint32_t HostToLittleEndian<uint32_t>(uint32_t _data) {
  return htole32(_data);
}

template <>
inline uint64_t HostToLittleEndian<uint64_t>(uint64_t _data) {
  return htole64(_data);
}

typename UintBuffer<float>::Type HostToLittleEndian(float _data) {
  typedef typename UintBuffer<float>::Type ReturnType;
  ReturnType buf;
  memcpy(&buf, &_data, sizeof(ReturnType));
  return HostToLittleEndian<ReturnType>(buf);
}

typename UintBuffer<double>::Type HostToLittleEndian(double _data) {
  typedef typename UintBuffer<double>::Type ReturnType;
  ReturnType buf;
  memcpy(&buf, &_data, sizeof(ReturnType));
  return HostToLittleEndian<ReturnType>(buf);
}

template <typename Tin, typename Tout>
Tout LittleEndianToHost(Tin);

template <>
inline uint8_t LittleEndianToHost<uint8_t, uint8_t>(uint8_t _data) {
  return _data;
}

template <>
inline uint16_t LittleEndianToHost<uint16_t, uint16_t>(uint16_t _data) {
  return le16toh(_data);
}

template <>
inline uint32_t LittleEndianToHost<uint32_t, uint32_t>(uint32_t _data) {
  return le32toh(_data);
}

template <>
inline uint64_t LittleEndianToHost<uint64_t, uint64_t>(uint64_t _data) {
  return le64toh(_data);
}

template <>
inline float LittleEndianToHost<typename UintBuffer<float>::Type, float>(
    typename UintBuffer<float>::Type _data) {
  typedef float ReturnType;
  typedef typename UintBuffer<ReturnType>::Type InputType;
  _data = LittleEndianToHost<InputType, InputType>(_data);
  ReturnType buf;
  memcpy(&buf, &_data, sizeof(ReturnType));
  return buf;
}

template <>
inline double LittleEndianToHost<typename UintBuffer<double>::Type, double>(
    typename UintBuffer<double>::Type _data) {
  typedef double ReturnType;
  typedef typename UintBuffer<ReturnType>::Type InputType;
  _data = LittleEndianToHost<InputType, InputType>(_data);
  ReturnType buf;
  memcpy(&buf, &_data, sizeof(ReturnType));
  return buf;
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
    auto data_le = impl::HostToLittleEndian<T>(_data);
    memcpy(&buf_[position_], &data_le, sizeof(data_le));
    position_ += sizeof(data_le);
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
    typedef typename impl::UintBuffer<T>::Type BufferType;
    BufferType buf;
    memcpy(&buf, &const_buf_[position_], sizeof(T));
    _data = impl::LittleEndianToHost<BufferType, T>(buf);
    position_ += sizeof(T);
  }

  template <typename T, size_t S>
  void Deserialize(T *_data) {
    for (size_t i = 0; i < S; ++i) {
      Deserialize(_data[i]);
    }
  }

 private:
  uint8_t *buf_;
  const uint8_t *const_buf_;
  size_t position_;
};
