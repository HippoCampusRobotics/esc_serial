#include "crc.hpp"

uint16_t crc16_mcrf4xx(uint16_t crc, uint8_t *_data, int _length) {
  if (!_data || _length < 0) {
    return crc;
  }

  for (int i = 0; i < _length; ++i) {
    crc ^= *(_data++);
    for (int j = 0; j < 8; ++j) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0x8408;
      } else {
        crc = (crc >> 1);
      }
    }
  }
  return crc;
}
