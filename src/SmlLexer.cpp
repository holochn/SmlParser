#include "SmlLexer.hpp"

bool SmlLexer::isOctetString(const unsigned char element) const {
  if ((element & 0xF0) != 0x00) {
    return false;
  }
  return true;
}

bool SmlLexer::isExtendedOctetString(const unsigned char element) const {
  if ((element & 0xF0) != 0x80) {
    return false;
  }
  return true;
}

bool SmlLexer::isUnsigned8(const unsigned char element) const {
  if (element != 0x62) {
    return false;
  }
  return true;
}

int SmlLexer::getOctetStringLength(const unsigned char element) const {
  if (static_cast<int>(element) == 0x00) {
    return -1;
  }

  if (static_cast<int>(element) & 0xF0) {
    return -2;
  }
  return static_cast<int>(element & 0x0F) - 1;
}

int SmlLexer::getExtendedOctetStringLength(const unsigned char *buffer,
                                           const int buffer_size,
                                           int &position) {
  int retval{0};
  int tl{0};

  if (buffer[position] == 0x00) {
    return -1;
  }

  if ((buffer[position] & 0xF0) != 0x80) {
    return -2;
  }

  if (position >= buffer_size) {
    return -3;
  }

  do {
    retval = retval << 4;
    retval |= (buffer[position] & 0x0F);
    ++position;
    ++tl;
  } while ((buffer[position] & 0xF0) == 0x80);

  retval = retval << 4;
  retval |= (buffer[position] & 0x0F);
  ++position;
  ++tl;

  return retval - tl;
}

uint8_t SmlLexer::getUnsigned8(const unsigned char *buffer, int buffer_size,
                               int &position) {
  int8_t retval = 0;
  if (position >= buffer_size - 1) {
    return 0xFF;
  }

  if (buffer[position] != 0x62) {
    return 0xFF;
  }

  ++position;
  retval = static_cast<int>(buffer[position]);
  ++position;
  return retval;
}

int8_t SmlLexer::getInteger8(const unsigned char *buffer, const int buffer_size,
                             int &position) {
  int8_t retval = 0;
  if (position >= buffer_size - 1) {
    return 0xFF;
  }

  if (buffer[position] != 0x52) {
    return 0xFF;
  }

  ++position;
  retval = static_cast<int>(buffer[position]);
  ++position;
  return retval;
}

uint16_t SmlLexer::getUnsigned16(const unsigned char *buffer,
                                 const int buffer_size, int &position) {
  int16_t retval = 0;
  if (position >= static_cast<const int>(buffer_size - 2)) {
    return 0xFFFF;
  }

  if (buffer[position] != 0x63) {
    return 0xFFFF;
  }

  ++position;
  retval = buffer[position] << 8;
  ++position;
  retval = retval | buffer[position];
  ++position;
  return retval;
}

int16_t SmlLexer::getInteger16(const unsigned char *buffer,
                               const int buffer_size, int &position) {
  int16_t retval = 0;
  if (position >= static_cast<const int>(buffer_size - 2)) {
    return 0xFFFF;
  }

  if (buffer[position] != 0x53) {
    return 0xFFFF;
  }

  ++position;
  retval = buffer[position] << 8;
  ++position;
  retval = retval | buffer[position];
  ++position;
  return retval;
}

uint32_t SmlLexer::getUnsigned32(const unsigned char *buffer,
                                 const int buffer_size, int &position) {
  int32_t retval = 0;
  if (position >= static_cast<const int>(buffer_size - 4)) {
    return 0xFFFFFFFF;
  }

  if (buffer[position] != 0x65) {
    return 0xFFFFFFFF;
  }

  ++position;
  retval = buffer[position] << 24;
  ++position;
  retval = retval | buffer[position] << 16;
  ++position;
  retval = retval | buffer[position] << 8;
  ++position;
  retval = retval | buffer[position];
  ++position;
  return retval;
}

int32_t SmlLexer::getInteger32(const unsigned char *buffer,
                               const int buffer_size, int &position) {
  int32_t retval = 0;
  if (position >= static_cast<const int>(buffer_size - 4)) {
    return 0xFFFFFFFF;
  }

  if (buffer[position] != 0x55) {
    return 0xFFFFFFFF;
  }

  ++position;
  retval = buffer[position] << 24;
  ++position;
  retval = retval | buffer[position] << 16;
  ++position;
  retval = retval | buffer[position] << 8;
  ++position;
  retval = retval | buffer[position];
  ++position;
  return retval;
}

uint64_t SmlLexer::getUnsigned64(const unsigned char *buffer,
                                 const int buffer_size, int &position) {
  int64_t retval = 0;
  if (position >= static_cast<const int>(buffer_size - 8)) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  if (buffer[position] != 0x69) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  ++position;
  retval = static_cast<int64_t>(buffer[position]) << 56;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 48;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 40;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 32;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 24;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 16;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 8;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]);
  ++position;

  return retval;
}

int64_t SmlLexer::getInteger64(const unsigned char *buffer,
                               const int buffer_size, int &position) {
  int64_t retval = 0;
  if (position >= static_cast<const int>(buffer_size - 8)) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  if (buffer[position] != 0x59) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  ++position;
  retval = static_cast<int64_t>(buffer[position]) << 56;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 48;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 40;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 32;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 24;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 16;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]) << 8;
  ++position;
  retval = retval | static_cast<int64_t>(buffer[position]);
  ++position;

  return retval;
}

uint8_t SmlLexer::getSmlListLength(const unsigned char *buffer,
                                   const int position) {
  if ((buffer[position] & 0xF0) != 0x70) {
    return 0xFF;
  }

  return buffer[position] & 0x0F;
}

SmlTime SmlLexer::getSmlTime(const unsigned char *buffer, const int buffer_size,
                             int &position) {
  SmlTime retval{SmlTimeType::secIndex, 0xFFFFFFFF};

  if (buffer[position] != 0x72) {
    SmlLogger::Warning("Syntax error in line %d. Expected 0x72 but found %02x",
                       __LINE__, buffer[position]);
    return retval;
  }
  ++position;

  int time_type = getUnsigned8(buffer, buffer_size, position);
  if (time_type == 0xFF) {
    SmlLogger::Warning("Syntax error in line %d. Found invalid time type",
                       __LINE__, buffer[position]);
    retval.timeValue = 0xFFFFFFFE;
    return retval;
  }

  switch (time_type) {
  case 1: // secIndex
    retval.timeType = SmlTimeType::secIndex;
    retval.timeValue = getUnsigned32(buffer, buffer_size, position);
    break;
  case 2: // timestamp
    retval.timeType = SmlTimeType::timeStamp;
    retval.timeValue = getUnsigned32(buffer, buffer_size, position);
    break;
  case 3: // timestamp local
    retval.timeType = SmlTimeType::localTimestamp;
    break;
  }

  return retval;
}

uint64_t SmlLexer::getSmlStatus(const unsigned char *buffer,
                                const int buffer_size, int &position) {
  uint64_t retval = 0xFFFFFFFFFFFFFFFF;

  if (position >= static_cast<int>(buffer_size) - 1) {
    return retval;
  }

  switch (buffer[position]) {
  case 0x62:
    retval = static_cast<uint64_t>(getUnsigned8(buffer, buffer_size, position));
    break;
  case 0x63:
    retval =
        static_cast<uint64_t>(getUnsigned16(buffer, buffer_size, position));
    break;
  case 0x65:
    retval =
        static_cast<uint64_t>(getUnsigned32(buffer, buffer_size, position));
    break;
  case 0x69:
    retval = getUnsigned64(buffer, buffer_size, position);
    break;
  }

  return retval;
}

uint64_t SmlLexer::getUnsigned(const unsigned char *buffer,
                               const int buffer_size, int &position) {
  uint64_t retval = 0xFFFFFFFFFFFFFFFF;
  if (buffer[position] == 0x62) {
    retval = static_cast<uint64_t>(getUnsigned8(buffer, buffer_size, position));
  }

  if (buffer[position] == 0x63) {
    retval = static_cast<uint64_t>(getUnsigned16(buffer, buffer_size, position));
  }

  if (buffer[position] == 0x65) {
    retval = static_cast<uint64_t>(getUnsigned32(buffer, buffer_size, position));
  }

  if (buffer[position] == 0x69) {
    retval = getUnsigned64(buffer, buffer_size, position);
  }

  return retval;
}

int64_t SmlLexer::getInteger(const unsigned char *buffer, const int buffer_size,
                             int &position) {
  uint64_t retval = 0xFFFFFFFFFFFFFFFF;
  if (buffer[position] == 0x52) {
    retval = static_cast<uint64_t>(getInteger8(buffer, buffer_size, position));
  }

  if (buffer[position] == 0x53) {
    retval = static_cast<uint64_t>(getInteger16(buffer, buffer_size, position));
  }

  if (buffer[position] == 0x55) {
    retval = static_cast<uint64_t>(getInteger32(buffer, buffer_size, position));
  }

  if (buffer[position] == 0x59) {
    retval = getInteger64(buffer, buffer_size, position);
  }

  return retval;
}

std::string SmlLexer::getOctetString(const unsigned char *buffer,
                                     const int buffer_size, int &position,
                                     const int length) {
  std::string ret = "";
  if (buffer == nullptr) {
    return ret;
  }

  if (length == 0) {
    return ret;
  }

  if (buffer_size < (position + length)) {
    return ret;
  }

  for (int i = 0; i < length; i++) {
    ret += buffer[position + i];
  }

  return ret;
}

std::string
SmlLexer::getExtendedOctetString(const unsigned char *buffer,
                                         const int buffer_size, int &position,
                                         int length) {
  std::string ret = "";
  if (buffer == nullptr) {
    return ret;
  }

  if (length <= 0) {
    return ret;
  }

  if (static_cast<int>(buffer_size) < (position + length)) {
    return ret;
  }

  while ((buffer[position] & 0xF0) == 0x80) {
    ++position;
  }

  for (int i = 0; i < length; i++) {
    ret += buffer[position + i];
  }

  return ret;
}