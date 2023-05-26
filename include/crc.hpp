#pragma once
#include <stdint.h>
#include <wchar.h>

namespace esc_serial {

uint32_t crc32(const uint8_t *_data, size_t _length);

}
