#include "SmlParser.hpp"

SmlFile parseSml(const std::vector<unsigned char> *buffer) {
  bool finished = false;

  if (buffer->size() == 0) {
    SmlLogger::Error("Buffer is empty. Nothing to parse.");
    abort();
  }

  int position_pointer = 0;
  if (parseEscapeSequence(buffer, position_pointer) != SML_OK) {
    SmlLogger::Error("Syntax error in %d. Expected start sequence 0x1b.",
                       __LINE__);
    abort();
  }
  for (position_pointer = 4; position_pointer < 8; position_pointer++) {
    if (buffer->at(position_pointer) != 0x01) {
      SmlLogger::Error("Syntax error in %d. Expected start sequence 0x01.",
                       __LINE__);
      abort();
    }
  }
  SmlFile smlFile;
  std::vector<unsigned char> transactionId;
  while (position_pointer < static_cast<int>(buffer->size())) {
    SmlMessage smlMessage;
    if (buffer->at(position_pointer) != 0x76) {
      SmlLogger::Warning("Syntax error in %d. Expected a list of 6.", __LINE__);
      break;
    }

    SmlLogger::Info("<<<<< New SML Message >>>>>");
    // transactionId
    position_pointer++;
    if (isOctetString(buffer->at(position_pointer)) == false) {
      SmlLogger::Error("Syntax error in %d. Expected octet string.", __LINE__);
      abort();
    }

    int transactionIdLength =
        getOctetStringLength(buffer->at(position_pointer));
    transactionId.resize(transactionIdLength);

    if (getOctetStringAsVector(buffer, ++position_pointer, &transactionId,
                               transactionIdLength) != SML_OK) {
      SmlLogger::Warning("Error parsing transactionId");
    }
    smlMessage.transactionId = transactionId;
    position_pointer += transactionIdLength;
    SmlLogger::Debug("transactionId: %s\n", vectorToString(&smlMessage.transactionId).c_str());

    // group ID
    if (isUnsigned8(buffer->at(position_pointer) == false)) {
      SmlLogger::Error("Syntax error in %d. Expected Unsigned8.", __LINE__);
      abort();
    }
    smlMessage.groupNo = getUnsigned8(buffer, position_pointer);
    SmlLogger::Debug("group id: %d", smlMessage.groupNo);

    // abortOnError
    if (isUnsigned8(buffer->at(position_pointer) == false)) {
      SmlLogger::Error("Syntax error. Expected Unsigned8.");
      abort();
    }
    smlMessage.abortOnError = getUnsigned8(buffer, position_pointer);
    SmlLogger::Debug("abortOnError id: %d\n", smlMessage.abortOnError);

    // message body
    uint8_t msgBodyElements = getSmlListLength(buffer, position_pointer);
    if (msgBodyElements != 2) {
      SmlLogger::Warning("Syntax error. Expected SML message Type and Body");
    }
    position_pointer++;

    // message body type
    uint16_t messageType = getUnsigned16(buffer, position_pointer);
    SmlLogger::Debug("Type of SML message is %04x", messageType);

    switch (messageType) {
    case SML_MSG_TYPE_PUBOPEN_RES:
      smlMessage.messageType = SML_MSG_TYPE_PUBOPEN_RES;
      smlMessage.messageBody = parseSmlPublicOpenRes(buffer, position_pointer);
      break;
    case SML_MSG_TYPE_GETLIST_RES:
      smlMessage.messageType = SML_MSG_TYPE_GETLIST_RES;
      smlMessage.messageBody = parseSmlGetListRes(buffer, position_pointer);
      break;
    case SML_MSG_TYPE_PUBCLOS_RES:
      smlMessage.messageType = SML_MSG_TYPE_PUBCLOS_RES;
      smlMessage.messageBody = parseSmlPublicCloseRes(buffer, position_pointer);
      finished = true;
      break;
    default:
      SmlLogger::Error("Unknown SML message type");
      abort();
      break;
    }

    smlMessage.crc16 = getUnsigned16(buffer, position_pointer);
    SmlLogger::Info("crc16 %04xd", smlMessage.crc16);

    if (buffer->at(position_pointer) != 0x00) {
      SmlLogger::Error("Expected EndOfMessage, but found %02x",
                       buffer->at(position_pointer));
      abort();
    } else {
      SmlLogger::Info("---------- EoM ----------\n");
      smlFile.addMessage(smlMessage);
    }

    ++position_pointer;

    if (finished) {
        if (parseEscapeSequence(buffer, position_pointer) != SML_OK) {
            SmlLogger::Error("Syntax error in %d. Expected escape sequence 0x1b.",
                            __LINE__);
            abort();
        }
  
        if(buffer->at(position_pointer) != 0x1a) {
            SmlLogger::Error("Syntax error in %d. Expected end sequence 0x1a.",
                        __LINE__);
            ++position_pointer;
            abort();
        }
      break;
    }
  }
  return smlFile;
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
  uint32_t tl{0};

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
    ++tl;
  } while ((buffer->at(position) & 0xF0) == 0x80);

  retval = retval << 4;
  retval |= (buffer->at(position) & 0x0F);
  ++position;
  ++tl;

  return retval-tl;
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

  if (length <= 0) {
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

SmlTime getSmlTime(const std::vector<unsigned char> *data, int &position) {
  SmlTime retval{SmlTimeType::secIndex,0xFFFFFFFF};

  if (data->at(position) != 0x72) {
    SmlLogger::Warning("Syntax error in line %d. Expected 0x72 but found %02x", __LINE__, data->at(position));
    return retval;
  }
  ++position;

  int time_type = getUnsigned8(data, position);
  if (time_type == 0xFF) {
    SmlLogger::Warning("Syntax error in line %d. Found invalid time type", __LINE__, data->at(position));
    retval.timeValue = 0xFFFFFFFE;
    return retval;
  }

  switch (time_type) {
  case 1: // secIndex
    retval.timeType = SmlTimeType::secIndex;
    retval.timeValue = getUnsigned32(data, position);
    break;
  case 2: // timestamp
  retval.timeType = SmlTimeType::timeStamp;
    retval.timeValue = getUnsigned32(data, position);
    break;
  case 3: // timestamp local
    retval.timeType = SmlTimeType::localTimestamp;
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

SmlPublicOpenRes parseSmlPublicOpenRes(const std::vector<unsigned char> *buffer,
                                  int &position) {
  SmlPublicOpenRes ret;
  if (getSmlListLength(buffer, position) != 6) {
    SmlLogger::Warning("Sytax error in %d. Expected list length of 6.", __LINE__);
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

    ret.codePage = codePage;
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

    ret.clientId = clientId;
  }
  position += clientIdLength;

  // reqField
  int reqFieldLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (reqFieldLength == 0) {
    SmlLogger::Warning(
        "Syntax error in line %d. Required element reqField missing", __LINE__);
    
  }
  std::vector<unsigned char> reqField;
  reqField.resize(reqFieldLength);
  getOctetStringAsVector(buffer, position, &reqField, reqFieldLength);
  SmlLogger::Info("reqField: %s", vectorToString(&reqField).c_str());
  ret.reqField = reqField;
  position += reqFieldLength;

  // serverId
  int serverIdLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (serverIdLength == 0) {
    SmlLogger::Warning(
        "Sytax error in line %d. Required element serverId missing", __LINE__);
  }
  std::vector<unsigned char> serverId;
  serverId.resize(serverIdLength);
  getOctetStringAsVector(buffer, position, &serverId, serverIdLength);
  SmlLogger::Info("serverID: %s", vectorToString(&serverId).c_str());
  ret.serverId = serverId;
  ++position;
  position += serverIdLength;

  // refTime
  // if the optional parameter is not available, it looks like a octet string
  if (isOctetString(buffer->at(position)) == false) {
    ret.refTime = getSmlTime(buffer, position);
    SmlLogger::Info("reftime: %d", ret.refTime.timeValue);
  } else {
    SmlLogger::Info("No refTime");
  }

  // smlVersion
  if (isUnsigned8(buffer->at(position))) {
    ret.smlVersion = getUnsigned8(buffer, position);
    ++position;
  }
  SmlLogger::Info("SML Version: %d", ret.smlVersion);
  ++position;

  return ret;
}

SmlPublicCloseRes parseSmlPublicCloseRes(const std::vector<unsigned char> *buffer,
                                   int &position) {
  SmlPublicCloseRes ret;
  if (getSmlListLength(buffer, position) != 1) {
    SmlLogger::Warning("Sytax error in %d. Expected list length of 1.", __LINE__);
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
      ret.globalSignature = globalSignature;
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
      ret.globalSignature = globalSignature;
      
      SmlLogger::Info("globalSignature %s",
                      vectorToString(&globalSignature).c_str());
    } else {
      SmlLogger::Info("No globalSignature");
    }
    position += globalSignatureLength;
  }

  SmlLogger::Debug("_____ End of List Entry _____\n");

  return ret;
}

SmlGetListRes parseSmlGetListRes(const std::vector<unsigned char> *buffer,
                               int &position) {
  SmlGetListRes ret;
  if (getSmlListLength(buffer, position) != 7) {
    SmlLogger::Warning("Sytax error in %d. Expected a list of 7.", __LINE__);
  }
  position++;

  // clientId
  int clientIdLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (clientIdLength > 0) {
    std::vector<unsigned char> clientId;
    clientId.resize(clientIdLength);
    getOctetStringAsVector(buffer, position, &clientId, clientIdLength);
    ret.clientId = clientId;
    SmlLogger::Info("clientId: %s", vectorToString(&clientId).c_str());
  } else {
    SmlLogger::Info("No clientId");
  }
  position += clientIdLength;

  // serverId
  int serverIdLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (serverIdLength == 0) {
    SmlLogger::Warning(
        "Syntax error in line %d. Required element serverId missing", __LINE__);
  }
  std::vector<unsigned char> serverId;
  serverId.resize(serverIdLength);
  getOctetStringAsVector(buffer, position, &serverId, serverIdLength);
  ret.serverId = serverId;
  SmlLogger::Info("serverId:, %s", vectorToString(&serverId).c_str());
  position += serverIdLength;

  // listName
  int listNameLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (listNameLength > 0) {
    std::vector<unsigned char> listName;
    listName.resize(listNameLength);
    getOctetStringAsVector(buffer, position, &listName, listNameLength);
    ret.listName = listName;
    SmlLogger::Info("listName: %s", vectorToString(&listName).c_str());
  } else {
    SmlLogger::Info("No listName");
  }
  position += listNameLength;

  // actSensorTime
  ret.actSensorTime = getSmlTime(buffer, position);
  if (ret.actSensorTime.timeValue > 0) {
    SmlLogger::Info("actSensorTime: %05x", ret.actSensorTime.timeValue);
  }

  // valList
  uint8_t valListLength = getSmlListLength(buffer, position);
  ++position;
  SmlLogger::Info("Found %d valList entries", valListLength);

  for (int i = 0; i < valListLength; i++) {
    ret.valList.push_back(parseSmlListEntry(buffer, position));
  }

  // listSignature
  int listSignatureLength = getOctetStringLength(buffer->at(position));
  ++position;
  if (listSignatureLength > 0) {
    std::vector<unsigned char> listSignature;
    listSignature.resize(listSignatureLength);
    getOctetStringAsVector(buffer, position, &listSignature,
                           listSignatureLength);
    ret.listSignature = listSignature;
    SmlLogger::Info("listSignature: %s",
                    vectorToString(&listSignature).c_str());

  } else {
    SmlLogger::Info("No listSignature");
  }
  position += listSignatureLength;
  
  // actGatewayTime
  if (buffer->at(position) != 0x01) {
    ret.actGatewayTime = getSmlTime(buffer, position);
    SmlLogger::Info("actGatewaytime: %05x", ret.actGatewayTime);
  } else {
    SmlLogger::Info("No actGatewaytime");
    ++position;
  }

  return ret;
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
    ret.status = getSmlStatus(buffer, position);
    SmlLogger::Info("status: %ld", ret.status);
  } else {
    SmlLogger::Info("No status");
    ++position;
  }

  // valTime
  if (buffer->at(position) != 0x01) {
    ret.valTime = getSmlTime(buffer, position);
    SmlLogger::Info("valTime: %05x", ret.valTime.timeValue);
  } else {
    SmlLogger::Info("No valTime");
    ++position;
  }

  // unit
  if (buffer->at(position) != 0x01) {
    ret.unit = getUnsigned8(buffer, position);
    SmlLogger::Info("unit: %02x", ret.unit);
  } else {
    SmlLogger::Info("No unit");
    ++position;
  }

  // scaler
  if (buffer->at(position) != 0x01) {
    ret.scaler = getInteger8(buffer, position);
    SmlLogger::Info("scaler: %02x", ret.scaler);
  } else {
    SmlLogger::Info("No scaler");
    ++position;
  }
  // value
  if( isOctetString(buffer->at(position)) ) {
    int valueLength = getOctetStringLength(buffer->at(position));
    ++position;
    if (valueLength > 0) {
      std::vector<unsigned char> value;
      value.resize(valueLength);
      getOctetStringAsVector(buffer, position, &value, valueLength);
      SmlLogger::Info("value: %s", vectorToString(&value).c_str());
      ret.isString = true;
      ret.sValue = value;
    } else {
      SmlLogger::Info("No value");
    }
    position += valueLength;
  } else if(isExtendedOctetString(buffer->at(position))) {
    int valueLength = getExtendedOctetStringLength(buffer, position);
    if (valueLength > 0) {
      std::vector<unsigned char> value;
      value.resize(valueLength);
      getExtendedOctetStringAsVector(
          buffer, position, &value,
          valueLength);
      ret.isString = true;
      ret.sValue = value;
      SmlLogger::Info("value %s",
                      vectorToString(&value).c_str());
    } else {
      SmlLogger::Info("No value");
    }
    position += valueLength;
    
    // currently only unsigned and signed values are supported
  } else if ((buffer->at(position) & 0xF0) == 0x50) {
      ret.isString = false;
      ret.iValue = getInteger(buffer, position);
      SmlLogger::Info("value: %ld", ret.iValue);
  } else if ((buffer->at(position) & 0xF0) == 0x60) {
      ret.isString = false;
      ret.iValue = getUnsigned(buffer, position);
      SmlLogger::Info("value: %ld", ret.iValue);
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
      ret.signature = valueSignature;
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
      getExtendedOctetStringAsVector(
          buffer, position, &valueSignature,
          valueSignatureLength);
      ret.signature = valueSignature;
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