#pragma once
#include <stdint.h>

namespace esc_serial {
namespace crc {

uint16_t crc16_mcrf4xx(uint16_t crc, uint8_t *_data, int _length);

}  // namespace crc
}  // namespace esc_serial
