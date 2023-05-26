#pragma once

#include <stdint.h>

namespace esc_serial {
/**
 * @brief
 *
 * @param _buffer
 * @param _length Total length including COBS byte at the start and the
 * delimiter byte at the end.
 */
void cobs_encode(uint8_t *_buffer, int _length);
/**
 * @brief
 *
 * @param _buffer
 * @param _length Total length including COBS byte at the start and the
 * delimiter byte at the end.
 * @return uint8_t* Start of the decoded data, i.e. without the COBS byte at the
 * start and the delimiter bye at the end.
 */
uint8_t *cobs_decode(uint8_t *_buffer, int _length);
}
