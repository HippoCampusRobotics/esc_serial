#include "esc_serial.hpp"

#include "cobs.hpp"
namespace esc_serial {
msg_id_t Packet::ParseMessage() {
  // make sure we have a complete packet
  if (!CompletelyReceived()) {
    return InvalidMessage::MSG_ID;
  }
  if (!HasMinimumLength()) {
    return InvalidMessage::MSG_ID;
  }
  // cobs decode the packet
  if (!Decode()) {
    return InvalidMessage::MSG_ID;
  }
  // verify crc
  if (!CrcOk()) {
    return InvalidMessage::MSG_ID;
  }
  // reject data not sufficient in size to represent a message
  if (PayloadSize() < kPayloadMinSize) {
    return InvalidMessage::MSG_ID;
  }
  // get the start of the message inside the whole buffer
  const uint8_t *_buffer = PayloadStart();
  Message msg;
  msg.Header().Deserialize(_buffer);

  // unlikely to happen. a transmission error would have caused the crc to fail.
  // this means, we have an implementation error in writing or reading
  // the message.
  if ((size_t)(msg.Header().MsgSize() + msg.Header().HEADER_SIZE) !=
      PayloadSize()) {
    return InvalidMessage::MSG_ID;
  }
  return msg.Header().MsgId();
}

bool Packet::AddByte(const uint8_t _byte) {
  if (!complete_ && (write_pointer_ < buffer_end_)) {
    *write_pointer_++ = _byte;
    ++size_;
    if (_byte == kDelimiter) {
      complete_ = true;
    }
    return true;
  }
  return false;
}

// bool Packet::SetPayload(const uint8_t *_payload, int _length) {
//   write_pointer_ = buffer_ + kPayloadOffset;
//   if (_length > kBufferSize - kTotalOverhead) {
//     return false;
//   }
//   for (int i = 0; i < _length; ++i) {
//     *write_pointer_++ = *_payload++;
//   }
//   size_ = _length + kTotalOverhead;
//   return true;
// }

void Packet::Packetize() {
  WriteCrc();
  cobs_encode(buffer_, Size());
}

bool Packet::Decode() {
  uint8_t *data = cobs_decode(buffer_, Size());
  if (!data) {
    return false;
  }
  return true;
}

bool Packet::CrcOk() {
  uint32_t crc_msg = ReadCrc();
  uint32_t crc_computed = crc32(PayloadStart(), PayloadSize());
  return crc_msg == crc_computed;
}

void Packet::WriteCrc() {
  uint32_t crc = crc32(PayloadStart(), PayloadSize());
  Serializer serializer(MutableCrcStart());
  serializer.Serialize(crc);
}

void Packet::Reset() {
  complete_ = false;
  write_pointer_ = buffer_;
  size_ = 0;
}
}  // namespace esc_serial
