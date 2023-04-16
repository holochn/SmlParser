#include "SmlParser.hpp"

sml_error_t parseSml(const std::vector<unsigned char> *buffer) {
  bool finished = false;

  if (buffer->size() == 0) {
    SmlLogger::Error("Buffer is empty. Nothing to parse.");
    return SML_NOK;
  }

  int position_pointer = 0;
  if (parseEscapeSequence(buffer, position_pointer) != SML_OK) {
    SmlLogger::Error("Syntax error in %d. Expected start sequence 0x1b.",
                       __LINE__);
    return SML_NOK;
  }
  for (position_pointer = 4; position_pointer < 8; position_pointer++) {
    if (buffer->at(position_pointer) != 0x01) {
      SmlLogger::Error("Syntax error in %d. Expected start sequence 0x01.",
                       __LINE__);
      return SML_ERROR_HEADER;
    }
  }

  std::vector<unsigned char> transactionId;
  while (position_pointer < static_cast<int>(buffer->size())) {
    if (buffer->at(position_pointer) != 0x76) {
      SmlLogger::Error("Syntax error in %d. Expected a list of 6.", __LINE__);
      break;
    }

    SmlLogger::Info("Found a new SML file");

    // transactionId
    position_pointer++;
    if (isOctetString(buffer->at(position_pointer)) == false) {
      SmlLogger::Error("Syntax error in %d. Expected octet string.", __LINE__);
      return SML_ERROR_SYNTAX;
    }

    int transactionIdLength =
        getOctetStringLength(buffer->at(position_pointer));
    transactionId.resize(transactionIdLength);

    if (getOctetStringAsVector(buffer, ++position_pointer, &transactionId,
                               transactionIdLength) != SML_OK) {
      SmlLogger::Error("Error parsing transactionId");
    }
    position_pointer += transactionIdLength;

    // group ID
    if (isUnsigned8(buffer->at(position_pointer) == false)) {
      SmlLogger::Error("Syntax error in %d. Expected Unsigned8.", __LINE__);
      return SML_ERROR_SYNTAX;
    }
    uint8_t groupId = 0x00;
    groupId = getUnsigned8(buffer, position_pointer);
    SmlLogger::Debug("group id: %d", groupId);

    // abortOnError
    if (isUnsigned8(buffer->at(position_pointer) == false)) {
      SmlLogger::Error("Syntax error. Expected Unsigned8.");
      return SML_ERROR_SYNTAX;
    }
    uint8_t abortOnError = 0x00;
    groupId = getUnsigned8(buffer, position_pointer);
    SmlLogger::Debug("abortOnError id: %d\n", abortOnError);

    // message body
    uint8_t msgBodyElements = getSmlListLength(buffer, position_pointer);
    if (msgBodyElements != 2) {
      SmlLogger::Error("Syntax error. Expected SML message Type and Body");
    }
    position_pointer++;

    // message body type
    uint16_t messageType = getUnsigned16(buffer, position_pointer);
    SmlLogger::Debug("Type of SML message is %04x", messageType);

    switch (messageType) {
    case SML_MSG_TYPE_PUBOPEN_RES:
      parseSmlPublicOpenRes(buffer, position_pointer);
      break;
    case SML_MSG_TYPE_GETLIST_RES:
      parseSmlGetListRes(buffer, position_pointer);
      break;
    case SML_MSG_TYPE_PUBCLOS_RES:
      parseSmlPublicCloseRes(buffer, position_pointer);
      finished = true;
      break;
    default:
      SmlLogger::Error("Unknown SML message type");
      abort();
      break;
    }

    [[maybe_unused]] uint16_t crc16 = getUnsigned16(buffer, position_pointer);

    if (buffer->at(position_pointer) != 0x00) {
      SmlLogger::Error("Expected EndOfMessage, but found %02x",
                       buffer->at(position_pointer));
      return SML_ERROR_SYNTAX;
    } else {
      SmlLogger::Info("---------- EoM ----------\n");
    }

    ++position_pointer;

    if (finished) {
        if (parseEscapeSequence(buffer, position_pointer) != SML_OK) {
            SmlLogger::Error("Syntax error in %d. Expected escape sequence 0x1b.",
                            __LINE__);
            return SML_NOK;
        }
        hexPrint(buffer, position_pointer);
        if(buffer->at(position_pointer) != 0x1a) {
            SmlLogger::Error("Syntax error in %d. Expected end sequence 0x1a.",
                        __LINE__);
            ++position_pointer;
            return SML_NOK;
        }
      break;
    }
  }
  return SML_OK;
}

bool isOctetString(const unsigned char element) {
  if ((element & 0xF0) != 0x00) {
    return false;
  }
  return true;
}

bool isExtendedOctetString(const unsigned char element) {
  if ((element & 0xF0) != 0x80) {
    return false;
  }
  return true;
}

int getOctetStringLength(const unsigned char element) {
  if (static_cast<int>(element) == 0x00) {
    return 0;
  }

  if (static_cast<int>(element) & 0xF0) {
    return -1;
  }
  return static_cast<int>(element & 0x0F) - 1;
}

uint32_t getExtendedOctetStringLength(const std::vector<unsigned char> *buffer,
                                      int &position) {
  uint32_t retval{0};

  if (buffer->at(position) == 0x00) {
    return 0;
  }

  if ((buffer->at(position) & 0xF0) != 0x80) {
    return -1;
  }

  do {
    retval = retval << 4;
    retval |= (buffer->at(position) & 0x0F);
    ++position;
  } while ((buffer->at(position) & 0xF0) == 0x80);

  retval = retval << 4;
  retval |= (buffer->at(position) & 0x0F);
  ++position;

  return retval;
}

sml_error_t getOctetStringAsVector(const std::vector<unsigned char> *data,
                                   int position,
                                   std::vector<unsigned char> *octetstring,
                                   int length) {
  if (data == nullptr) {
    return SML_ERROR_NULLPTR;
  }

  if (octetstring == nullptr) {
    return SML_ERROR_NULLPTR;
  }

  if (length == 0) {
    return SML_ERROR_ZEROLENGTH;
  }

  if (static_cast<int>(data->size()) < (position + length)) {
    return SML_ERROR_SIZE;
  }

  for (int i = 0; i < length; i++) {
    octetstring->at(i) = data->at(position + i);
  }

  return SML_OK;
}

sml_error_t getExtendedOctetStringAsVector(
    const std::vector<unsigned char> *data, int &position,
    std::vector<unsigned char> *octetstring, int length) {
  if (data == nullptr) {
    return SML_ERROR_NULLPTR;
  }

  if (octetstring == nullptr) {
    return SML_ERROR_NULLPTR;
  }

  if (length == 0) {
    return SML_ERROR_ZEROLENGTH;
  }

  if (static_cast<int>(data->size()) < (position + length)) {
    return SML_ERROR_SIZE;
  }

  while ((data->at(position) & 0xF0) == 0x80) {
    ++position;
  }

  for (int i = 0; i < length; i++) {
    octetstring->at(i) = data->at(position + i);
  }

  return SML_OK;
}

bool isUnsigned8(const unsigned char element) {
  if (element != 0x62) {
    return false;
  }
  return true;
}

uint8_t getUnsigned8(const std::vector<unsigned char> *data, int &position) {
  uint8_t retval = 0;
  if (position >= static_cast<int>(data->size()) - 1) {
    return 0xFF;
  }

  if (data->at(position) != 0x62) {
    printf("Expected Unsigned8 (0x62), but found %02x\n", data->at(position));
    return 0xFF;
  }
  ++position;
  retval = data->at(position);
  if (position < static_cast<int>(data->size())) {
    ++position;
  }
  return retval;
}

int8_t getInteger8(const std::vector<unsigned char> *data, int &position) {
  int8_t retval = 0;
  if (position >= static_cast<int>(data->size() - 1)) {
    return 0xFF;
  }

  if (data->at(position) != 0x52) {
    printf("Expected Unsigned8 (0x52), but found %02x\n", data->at(position));
    return 0xFF;
  }

  ++position;
  retval = static_cast<int8_t>(data->at(position));
  ++position;
  return retval;
}

uint16_t getUnsigned16(const std::vector<unsigned char> *data, int &position) {
  uint16_t retval = 0;
  if (position >= static_cast<const int>(data->size() - 2)) {
    return 0xFFFF;
  }

  if (data->at(position) != 0x63) {
    printf("Expected Unsigned16 (0x63), but found %02x\n", data->at(position));
    return 0xFFFF;
  }

  ++position;
  retval = data->at(position) << 8;
  ++position;
  retval = retval | data->at(position);
  ++position;
  return retval;
}

int16_t getInteger16(const std::vector<unsigned char> *data, int &position) {
  int16_t retval = 0;
  if (position >= static_cast<const int>(data->size() - 2)) {
    return 0xFFFF;
  }

  if (data->at(position) != 0x53) {
    printf("Expected Integer16 (0x53), but found %02x\n", data->at(position));
    return 0xFFFF;
  }

  ++position;
  retval = data->at(position) << 8;
  ++position;
  retval = retval | data->at(position);
  ++position;
  return retval;
}

uint32_t getUnsigned32(const std::vector<unsigned char> *data, int &position) {
  uint32_t retval = 0;
  if (position >= static_cast<const int>(data->size() - 4)) {
    return 0xFFFFFFFF;
  }

  if (data->at(position) != 0x65) {
    printf("Expected Unsigned32 (0x65), but found %02x\n", data->at(position));
    return 0xFFFFFFFF;
  }

  ++position;
  retval = data->at(position) << 24;
  ++position;
  retval = retval | data->at(position) << 16;
  ++position;
  retval = retval | data->at(position) << 8;
  ++position;
  retval = retval | data->at(position);
  ++position;
  return retval;
}

int32_t getInteger32(const std::vector<unsigned char> *data, int &position) {
  int32_t retval = 0;
  if (position >= static_cast<const int>(data->size() - 4)) {
    return 0xFFFFFFFF;
  }

  if (data->at(position) != 0x55) {
    printf("Expected Integer32 (0x55), but found %02x\n", data->at(position));
    return 0xFFFFFFFF;
  }

  ++position;
  retval = data->at(position) << 24;
  ++position;
  retval = retval | data->at(position) << 16;
  ++position;
  retval = retval | data->at(position) << 8;
  ++position;
  retval = retval | data->at(position);
  ++position;
  return retval;
}

uint64_t getUnsigned64(const std::vector<unsigned char> *data, int &position) {
  uint64_t retval = 0;
  if (position >= static_cast<const int>(data->size() - 8)) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  if (data->at(position) != 0x69) {
    printf("Expected Unsigned64 (0x69), but found %02x\n", data->at(position));
    return 0xFFFFFFFFFFFFFFFF;
  }

  ++position;
  retval = static_cast<uint64_t>(data->at(position)) << 56;
  ++position;
  retval = retval | static_cast<uint64_t>(data->at(position)) << 48;
  ++position;
  retval = retval | static_cast<uint64_t>(data->at(position)) << 40;
  ++position;
  retval = retval | static_cast<uint64_t>(data->at(position)) << 32;
  ++position;
  retval = retval | static_cast<uint64_t>(data->at(position)) << 24;
  ++position;
  retval = retval | static_cast<uint64_t>(data->at(position)) << 16;
  ++position;
  retval = retval | static_cast<uint64_t>(data->at(position)) << 8;
  ++position;
  retval = retval | static_cast<uint64_t>(data->at(position));
  ++position;

  return retval;
}

int64_t getInteger64(const std::vector<unsigned char> *data, int &position) {
  int64_t retval = 0;
  if (position >= static_cast<const int>(data->size() - 8)) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  if (data->at(position) != 0x59) {
    printf("Expected Integer64 (0x59), but found %02x\n", data->at(position));
    return 0xFFFFFFFFFFFFFFFF;
  }

  ++position;
  retval = static_cast<int64_t>(data->at(position)) << 56;
  ++position;
  retval = retval | static_cast<int64_t>(data->at(position)) << 48;
  ++position;
  retval = retval | static_cast<int64_t>(data->at(position)) << 40;
  ++position;
  retval = retval | static_cast<int64_t>(data->at(position)) << 32;
  ++position;
  retval = retval | static_cast<int64_t>(data->at(position)) << 24;
  ++position;
  retval = retval | static_cast<int64_t>(data->at(position)) << 16;
  ++position;
  retval = retval | static_cast<int64_t>(data->at(position)) << 8;
  ++position;
  retval = retval | static_cast<int64_t>(data->at(position));
  ++position;

  return retval;
}

uint8_t getSmlListLength(const std::vector<unsigned char> *data,
                         const int position) {
  if ((data->at(position) & 0xF0) != 0x70) {
    return 0xFF;
  }

  return data->at(position) & 0x0F;
}

uint32_t getSmlTime(const std::vector<unsigned char> *data, int &position) {
  uint32_t retval = 0;

  if (data->at(position) != 0x72) {
    return 0;
  }
  ++position;

  int time_type = getUnsigned8(data, position);
  if (time_type == 0xFF) {
    return 0;
  }

  switch (time_type) {
  case 1: // secIndex
    retval = getUnsigned32(data, position);
    break;
  case 2: // timestamp
    retval = getUnsigned32(data, position);
    break;
  case 3: // timestamp local
    break;
  }

  return retval;
}

uint64_t getSmlStatus(const std::vector<unsigned char> *data, int &position) {
  uint64_t retval = 0xFFFFFFFFFFFFFFFF;

  if (position >= static_cast<int>(data->size()) - 1) {
    return retval;
  }

  switch (data->at(position)) {
  case 0x62:
    retval = static_cast<uint64_t>(getUnsigned8(data, position));
    break;
  case 0x63:
    retval = static_cast<uint64_t>(getUnsigned16(data, position));
    break;
  case 0x65:
    retval = static_cast<uint64_t>(getUnsigned32(data, position));
    break;
  case 0x69:
    retval = getUnsigned64(data, position);
    break;
  }

  return retval;
}

uint64_t getUnsigned(const std::vector<unsigned char> *data, int &position) {
  uint64_t retval = 0xFFFFFFFFFFFFFFFF;
  if (data->at(position) == 0x62) {
    retval = static_cast<uint64_t>(getUnsigned8(data, position));
  }

  if (data->at(position) == 0x63) {
    retval = static_cast<uint64_t>(getUnsigned16(data, position));
  }

  if (data->at(position) == 0x65) {
    retval = static_cast<uint64_t>(getUnsigned32(data, position));
  }

  if (data->at(position) == 0x69) {
    retval = getUnsigned64(data, position);
  }

  return retval;
}

int64_t getInteger(const std::vector<unsigned char> *data, int &position) {
  uint64_t retval = 0xFFFFFFFFFFFFFFFF;
  if (data->at(position) == 0x62) {
    retval = static_cast<uint64_t>(getInteger8(data, position));
  }

  if (data->at(position) == 0x63) {
    retval = static_cast<uint64_t>(getInteger16(data, position));
  }

  if (data->at(position) == 0x65) {
    retval = static_cast<uint64_t>(getInteger32(data, position));
  }

  if (data->at(position) == 0x69) {
    retval = getInteger64(data, position);
  }

  return retval;
}

sml_error_t parseSmlPublicOpenRes(const std::vector<unsigned char> *buffer,
                                  int &position) {
  if (getSmlListLength(buffer, position) != 6) {
    SmlLogger::Error("Sytax error in %d. Expected list length of 6.", __LINE__);
    return SML_ERROR_SIZE;
  }
  position++;

  // code page
  int codePageLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (codePageLength > 0) {
    std::vector<unsigned char> codePage;
    codePage.resize(codePageLength);
    getOctetStringAsVector(buffer, position, &codePage, codePageLength);
    SmlLogger::Info("cocePage: %s", vectorToString(&codePage).c_str());
  }
  position += codePageLength;

  // clientId
  int clientIdLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (clientIdLength > 0) {
    std::vector<unsigned char> clientId;
    clientId.resize(clientIdLength);
    getOctetStringAsVector(buffer, position, &clientId, clientIdLength);
    SmlLogger::Info("clientId: %s", vectorToString(&clientId).c_str());
  }
  position += clientIdLength;

  // reqField
  int reqFieldLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (reqFieldLength == 0) {
    SmlLogger::Error(
        "Syntax error in line %d. Required element reqField missing", __LINE__);
    return SML_ERROR_SYNTAX;
  }
  std::vector<unsigned char> reqField;
  reqField.resize(reqFieldLength);
  getOctetStringAsVector(buffer, position, &reqField, reqFieldLength);
  SmlLogger::Info("reqField: %s", vectorToString(&reqField).c_str());
  position += reqFieldLength;

  // serverId
  int serverIdLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (serverIdLength == 0) {
    SmlLogger::Error(
        "Sytax error in line %d. Required element serverId missing", __LINE__);
    return SML_ERROR_SYNTAX;
  }
  std::vector<unsigned char> serverId;
  serverId.resize(serverIdLength);
  getOctetStringAsVector(buffer, position, &serverId, serverIdLength);
  SmlLogger::Info("serverID: %s", vectorToString(&serverId).c_str());
  ++position;
  position += serverIdLength;

  // refTime
  // if the optional parameter is not available, it looks like a octet string
  if (isOctetString(buffer->at(position)) == false) {
    uint32_t smlTime = getSmlTime(buffer, position);
    SmlLogger::Info("reftime: %d", smlTime);
  } else {
    SmlLogger::Info("No refTime");
  }

  // smlVersion
  uint8_t smlVersion = 1;
  if (isUnsigned8(buffer->at(position))) {
    smlVersion = getUnsigned8(buffer, position);
    ++position;
  }
  SmlLogger::Info("SML Version: %d", smlVersion);
  ++position;

  return SML_OK;
}

sml_error_t parseSmlPublicCloseRes(const std::vector<unsigned char> *buffer,
                                   int &position) {
  if (getSmlListLength(buffer, position) != 1) {
    SmlLogger::Error("Sytax error in %d. Expected list length of 1.", __LINE__);
    return SML_ERROR_SIZE;
  }
  position++;

  if (isOctetString(buffer->at(position))) {
    int globalSignatureLength = getOctetStringLength(buffer->at(position));
    ++position;
    if (globalSignatureLength > 0) {
      std::vector<unsigned char> globalSignature;
      globalSignature.resize(globalSignatureLength);
      getOctetStringAsVector(buffer, position, &globalSignature,
                             globalSignatureLength);

      SmlLogger::Info("globalSignature %s",
                      vectorToString(&globalSignature).c_str());
    } else {
      SmlLogger::Info("No globalSignature");
    }
    position += globalSignatureLength;
  } else if (isExtendedOctetString(buffer->at(position))) {
    int globalSignatureLength = getExtendedOctetStringLength(buffer, position);
    if (globalSignatureLength > 0) {
      std::vector<unsigned char> globalSignature;
      globalSignature.resize(globalSignatureLength);
      getExtendedOctetStringAsVector(buffer, position, &globalSignature,
                                     globalSignatureLength);

      SmlLogger::Info("globalSignature %s",
                      vectorToString(&globalSignature).c_str());
    } else {
      SmlLogger::Info("No globalSignature");
    }
    position += globalSignatureLength;
  }

  SmlLogger::Debug("_____ End of List Entry _____\n");

  return SML_OK;
}

sml_error_t parseSmlGetListRes(const std::vector<unsigned char> *buffer,
                               int &position) {
  if (getSmlListLength(buffer, position) != 7) {
    return SML_ERROR_SIZE;
  }
  position++;

  // clientId
  int clientIdLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (clientIdLength > 0) {
    std::vector<unsigned char> clientId;
    clientId.resize(clientIdLength);
    getOctetStringAsVector(buffer, position, &clientId, clientIdLength);
    SmlLogger::Info("clientId: %s", vectorToString(&clientId).c_str());
  } else {
    SmlLogger::Info("No clientId");
  }
  position += clientIdLength;

  // serverId
  int serverIdLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (serverIdLength == 0) {
    SmlLogger::Error(
        "Syntax error in line %d. Required element serverId missing", __LINE__);
    return SML_ERROR_SYNTAX;
  }
  std::vector<unsigned char> serverId;
  serverId.resize(serverIdLength);
  getOctetStringAsVector(buffer, position, &serverId, serverIdLength);
  SmlLogger::Info("serverId:, %s", vectorToString(&serverId).c_str());
  position += serverIdLength;

  // listName
  int listNameLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (listNameLength > 0) {
    std::vector<unsigned char> listName;
    listName.resize(listNameLength);
    getOctetStringAsVector(buffer, position, &listName, listNameLength);
    SmlLogger::Info("listName: %s", vectorToString(&listName).c_str());
  } else {
    SmlLogger::Info("No listName");
  }
  position += listNameLength;

  // actSensorTime
  uint32_t actSensorTime = getSmlTime(buffer, position);
  if (actSensorTime > 0) {
    SmlLogger::Info("actSensorTime: %05x", actSensorTime);
  }

  // valList
  uint8_t valListLength = getSmlListLength(buffer, position);
  ++position;
  SmlLogger::Info("Found %d valList entries", valListLength);

  for (int i = 0; i < valListLength; i++) {
    SmlListEntry entry = parseSmlListEntry(buffer, position);
  }

  // listSignature
  int listSignatureLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (listSignatureLength > 0) {
    std::vector<unsigned char> listSignature;
    listSignature.resize(listSignatureLength);
    getOctetStringAsVector(buffer, position, &listSignature,
                           listSignatureLength);
    position += listSignatureLength;
    SmlLogger::Info("listSignature: %s",
                    vectorToString(&listSignature).c_str());

  } else {
    SmlLogger::Info("No listSignature");
  }

  // actGatewaytime
  if (buffer->at(position) != 0x01) {
    uint32_t actGatewaytime = getSmlTime(buffer, position);
    SmlLogger::Info("actGatewaytime: %05x", actGatewaytime);
  } else {
    SmlLogger::Info("No actGatewaytime");
    ++position;
  }

  return SML_OK;
}

sml_error_t parseEscapeSequence(const std::vector<unsigned char> *buffer,
                               int &position) {
    for (int i = 0; i < 4; i++) {
        if (buffer->at(i) != 0x1b) {
            return SML_ERROR_HEADER;
        }
        ++position;
    }

    return SML_OK;
}

SmlListEntry parseSmlListEntry(const std::vector<unsigned char> *buffer,
                               int &position) {
  SmlListEntry ret;
  if (buffer->at(position) != 0x77) {
    SmlLogger::Error("Syntax error in line %d: Expected a list of 7 entries, "
                     "but found %02x",
                     __LINE__, buffer->at(position));
    abort();
  }
  ++position;

  // objName
  int nameLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (nameLength > 0) {
    std::vector<unsigned char> name;
    name.resize(nameLength);
    getOctetStringAsVector(buffer, position, &name, nameLength);

    if (name == OBIS_MANUFACTURER) {
      SmlLogger::Info("Manufacturer: %s", vectorToString(&name).c_str());
    } else if (name == OBIS_DEVICE_ID) {
      SmlLogger::Info("Device ID: %s", vectorToString(&name).c_str());
    } else if (name == OBIS_TOTAL_ENERGY) {
      SmlLogger::Info("Zählerstand Total: %s", vectorToString(&name).c_str());
    } else if (name == OBIS_ENERGY_T1) {
      SmlLogger::Info("Zählerstand Tarif 1: %s", vectorToString(&name).c_str());
    } else if (name == OBIS_ENERGY_T2) {
      SmlLogger::Info("Zählerstand Tarif 2: %s", vectorToString(&name).c_str());
    } else if (name == OBIS_PUB_KEY) {
      SmlLogger::Info("Public Key: %s", vectorToString(&name).c_str());
    } else if (name == OBIS_CURR_POWER) {
      SmlLogger::Info("Current Power: %s", vectorToString(&name).c_str());
    }
  } else {
    SmlLogger::Info("No name");
  }
  position += nameLength;

  // status
  if (buffer->at(position) != 0x01) {
    uint64_t status = getSmlStatus(buffer, position);
    ret.setStatus(status);
    SmlLogger::Info("status: %ld", status);
  } else {
    SmlLogger::Info("No status");
    ++position;
  }

  // valTime
  if (buffer->at(position) != 0x01) {
    uint32_t valTime = getSmlTime(buffer, position);
    ret.setTime(valTime);
    SmlLogger::Info("valTime: %05x", valTime);
  } else {
    SmlLogger::Info("No valTime");
    ++position;
  }

  // unit
  if (buffer->at(position) != 0x01) {
    uint8_t unit = getUnsigned8(buffer, position);
    ret.setUnit(unit);
    SmlLogger::Info("unit: %02x", unit);
  } else {
    SmlLogger::Info("No unit");
    ++position;
  }

  // scaler
  if (buffer->at(position) != 0x01) {
    int8_t scaler = getInteger8(buffer, position);
    ret.setScaler(scaler);
    SmlLogger::Info("scaler: %02x", scaler);
  } else {
    SmlLogger::Info("No scaler");
    ++position;
  }

  // value
  if (((buffer->at(position) & 0xF0) == 0x00) ||
      ((buffer->at(position) & 0xF0) == 0x80)) {
    int valueLength = getOctetStringLength(buffer->at(position));
    ++position;
    if (valueLength > 0) {
      std::vector<unsigned char> value;
      value.resize(valueLength);
      getOctetStringAsVector(buffer, position, &value, valueLength);
      SmlLogger::Info("value: %s", vectorToString(&value).c_str());
      ret.setStringValue(value);
    } else {
      SmlLogger::Info("No value");
    }
    position += valueLength;
  } else {
    // currently only unsigned and signed values are supported
    if ((buffer->at(position) & 0xF0) == 0x50) {
      int64_t value = getInteger(buffer, position);
      SmlLogger::Info("value: %ld", value);
    } else if ((buffer->at(position) & 0xF0) == 0x60) {
      uint64_t value = getUnsigned(buffer, position);
      SmlLogger::Info("value: %ld", value);
    }
  }

  // valueSignature
  if (isOctetString(buffer->at(position))) {
    int valueSignatureLength = getOctetStringLength(buffer->at(position));
    ++position;
    if (valueSignatureLength > 0) {
      std::vector<unsigned char> valueSignature;
      valueSignature.resize(valueSignatureLength);
      getOctetStringAsVector(buffer, position, &valueSignature,
                             valueSignatureLength);
      ret.setSignature(valueSignature);
      SmlLogger::Info("valueSignature %s",
                      vectorToString(&valueSignature).c_str());
    } else {
      SmlLogger::Info("No valueSignature");
    }
    position += valueSignatureLength;
  } else if (isExtendedOctetString(buffer->at(position))) {
    int valueSignatureLength = getExtendedOctetStringLength(buffer, position);
    if (valueSignatureLength > 0) {
      std::vector<unsigned char> valueSignature;
      valueSignature.resize(valueSignatureLength);
      getExtendedOctetStringAsVector(buffer, position, &valueSignature,
                                     valueSignatureLength);
      ret.setSignature(valueSignature);
      SmlLogger::Info("valueSignature %s",
                      vectorToString(&valueSignature).c_str());
    } else {
      SmlLogger::Info("No valueSignature");
    }
    position += valueSignatureLength;
  }

  SmlLogger::Debug("_____ End of List Entry _____\n");

  return ret;
}

void printCharVector(std::vector<unsigned char> v) {
  for (unsigned int i = 0; i < v.size(); i++) {
    printf("%02x ", static_cast<int>(v.at(i)));
  }
}

std::string vectorToString(std::vector<unsigned char> *the_vector) {
  std::string ret;
  for (int i = 0; i < static_cast<int>(the_vector->size()); ++i) {
    ret.append(std::to_string(the_vector->at(i)));
  }
  std::to_string(the_vector->at(0));

  return ret;
}

void hexPrint(const std::vector<unsigned char> *buffer, int &position) {
  for (int i = position - 10; i < position + 10; ++i) {
    if (i == position) {
      printf("\033[0;33m");
    }
    printf("%02x ", buffer->at(i));
    if (i == position) {
      printf("\033[0;37m");
    }
  }
  printf("\n");
  for (int i = position - 10; i < position + 10; ++i) {
    if (i == position) {
      printf("%03d", position);
    } else {
      printf("   ");
    }
  }
  printf("\n");
}