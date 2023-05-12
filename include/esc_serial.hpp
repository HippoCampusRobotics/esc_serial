#pragma once
#include <endian.h>
#include <stdint.h>

#include <cstring>

#include "crc.hpp"
#include "serializer.hpp"

namespace impl {}  // namespace impl
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))

typedef uint8_t msg_size_t;
typedef uint8_t msg_id_t;

class MessageHeader {
 public:
  struct Header {
    msg_size_t msg_size{0};
    msg_id_t msg_id{0};
  };
  static constexpr msg_size_t HEADER_SIZE = sizeof(Header);
  void Serialize(uint8_t* _buffer) {
    Serializer serializer(_buffer);
    serializer.Serialize(header_.msg_size);
    serializer.Serialize(header_.msg_id);
  }
  void Deserialize(const uint8_t* _buffer) {
    Serializer serializer(_buffer);
    serializer.Deserialize(header_.msg_size);
    serializer.Deserialize(header_.msg_id);
  }
  inline void SetMsgSize(msg_size_t _msg_size) { header_.msg_size = _msg_size; }
  inline void SetMsgId(msg_id_t _msg_id) { header_.msg_id = _msg_id; }

  inline msg_size_t MsgSize() const { return header_.msg_size; }

  inline msg_id_t MsgId() const { return header_.msg_id; }

 protected:
  Header header_;
};

class Message {
 public:
  struct Payload {};
  Payload payload_;
  static constexpr msg_id_t MSG_ID = 0;
  static constexpr msg_size_t MSG_SIZE = 0;
  Message() {}
  Message(msg_id_t _msg_id, msg_size_t _msg_size) {
    header_.SetMsgId(_msg_id);
    header_.SetMsgSize(_msg_size);
  }
  MessageHeader& Header() { return header_; }
  virtual inline void SerializePayload(uint8_t* _buffer,
                                       size_t _buffer_length) const {};
  virtual inline void DeserializePayload(const uint8_t* _buffer,
                                         size_t _buffer_length){};

 protected:
  MessageHeader header_;
};

class InvalidMessage {
 public:
  static constexpr msg_id_t MSG_ID = 0;
};

class ActuatorControlsMessage : public Message {
 public:
  struct Payload {
    uint16_t pwm[8];
  };
  Payload payload_;
  static constexpr msg_id_t MSG_ID = 1;
  static constexpr msg_size_t MSG_SIZE = sizeof(Payload);
  ActuatorControlsMessage() : Message(MSG_ID, MSG_SIZE) {}

  inline void SerializePayload(uint8_t* _buffer,
                               size_t _buffer_length) const override {
    if (_buffer_length < sizeof(Payload)) {
      return;
    }
    Serializer serializer(_buffer);
    serializer.Serialize<uint16_t, ARRAY_LENGTH(payload_.pwm)>(payload_.pwm);
  }

  inline void DeserializePayload(const uint8_t* _buffer,
                                 size_t _buffer_length) override {
    if (_buffer_length != sizeof(Payload)) {
      return;
    }
    Serializer serializer(_buffer);
    serializer.Deserialize<uint16_t, ARRAY_LENGTH(payload_.pwm)>(payload_.pwm);
  }
};

class BatteryVoltageMessage : public Message {
 public:
  struct Payload {
    uint16_t voltage_mv;
  };
  Payload payload_;
  static constexpr msg_id_t MSG_ID = 2;
  static constexpr msg_size_t MSG_SIZE = sizeof(Payload);
  BatteryVoltageMessage() : Message(MSG_ID, MSG_SIZE) {}

  inline void SerializePayload(uint8_t* _buffer,
                               size_t _buffer_length) const override {
    if (_buffer_length < MSG_SIZE) {
      return;
    }
    Serializer serializer(_buffer);
    serializer.Serialize<uint16_t>(payload_.voltage_mv);
  }
  inline void DeserializePayload(const uint8_t* _buffer,
                                 size_t _buffer_length) override {
    if (_buffer_length != MSG_SIZE) {
      return;
    }
    Serializer serializer(_buffer);
    serializer.Deserialize<uint16_t>(payload_.voltage_mv);
  }

 protected:
};

class Packet {
 public:
  static constexpr uint8_t kDelimiter{0};
  msg_id_t ParseMessage();
  bool CompletelyReceived() { return complete_; }
  bool AddByte(const uint8_t byte);
  void Packetize();
  // bool SetPayload(const uint8_t* _payload, int _length);
  // uint8_t* MutablePayload() { return buffer_ + kPayloadOffset; }
  // const uint8_t* Data() const { return buffer_; }
  int Size() const { return size_; }
  void SetPayloadSize(uint8_t _size) { size_ = _size + kTotalOverhead; }
  void Reset();
  const uint8_t* PayloadStart() const { return buffer_ + kPayloadOffset; }
  uint8_t* MutablePayloadStart() { return buffer_ + kPayloadOffset; }
  const uint8_t* Data() const { return buffer_; }

 private:
  static constexpr int kCobsOverhead = 2;
  static constexpr int kPayloadOffset = 1;
  static constexpr int kCrcOverhead = sizeof(uint32_t);
  static constexpr int kTotalOverhead = kCobsOverhead + kCrcOverhead;
  static constexpr int kBufferSize{256};
  static constexpr int kPayloadMinSize = MessageHeader::HEADER_SIZE;
  void WriteCrc();
  inline uint32_t ReadCrc() {
    Serializer serializer(buffer_ + kPayloadOffset + PayloadSize());
    uint32_t crc;
    serializer.Deserialize(crc);
    return crc;
  }
  bool CrcOk();
  bool Decode();
  int PayloadSize() const { return Size() - kTotalOverhead; }
  const uint8_t* CrcStart() const { return PayloadStart() + PayloadSize(); }
  uint8_t* MutableCrcStart() { return MutablePayloadStart() + PayloadSize(); }
  uint8_t buffer_[kBufferSize];
  uint8_t* write_pointer_ = buffer_;
  const uint8_t* buffer_end_ = buffer_ + kBufferSize;
  int size_{0};
  bool complete_{false};
};
