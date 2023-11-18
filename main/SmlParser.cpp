#include "SmlParser.hpp"
#include "SmlCrc.hpp"
#include <iostream>
#include <map>

static const std::map<uint8_t, std::string> SmlUnit = {
  {0x1b, "W"},
  {0x1e, "Wh"}
};

SmlParser::SmlParser(unsigned char *t_buffer, int t_buffer_size)
    : buffer{t_buffer}, buffer_size{t_buffer_size}, position{0}
{
  lexer = SmlLexer();
}

SmlParser::~SmlParser() { buffer = nullptr; }

sml_error_t SmlParser::parseSml()
{
  bool finished = false;

  if (buffer_size == 0 || buffer == nullptr)
  {
    SmlLogger::Error("Buffer is empty. Nothing to parse.");
    return SML_ERROR_ZEROLENGTH;
  }

  char pattern[] = {0x1b, 0x1b, 0x1b, 0x1b, 0x01, 0x01, 0x01, 0x01};
  position = 0;
  while (memcmp(&buffer[position], &pattern, 8) != 0)
  {
    position++;
    if (position >= buffer_size)
    {
      SmlLogger::Error("Unable to find start sequence in %d.",__LINE__);
      return SML_ERROR_SIZE;
    }
  }
  SmlLogger::Verbose("Starting to parse on position %d", position);

  auto retval = parseEscapeSequence(buffer, buffer_size, position);
  if (retval != SML_OK)
  {
    SmlLogger::Error("Syntax error in %d. Expected start sequence 0x1b.",
                     __LINE__);
    return SML_ERROR_SYNTAX;
  }

  auto stop = position + 4;
  for(int i = position; i < stop; ++i) 
  {
    if (buffer[position] != 0x01)
    {
      SmlLogger::Error("Syntax error in %d. Expected start sequence 0x01.",
                       __LINE__);
      return SML_ERROR_SYNTAX;
    }
    ++position;
  }

  while (position < buffer_size)
  {
    if (buffer[position] != 0x76)
    {
      SmlLogger::Error("Syntax error in %d. Expected a list of 6.", __LINE__);
      return SML_ERROR_SYNTAX;
    }

    int start_crc = position;
  
    SmlLogger::Info("<<<<< New SML Message >>>>>");
    // transactionId
    position++;
    if (lexer.isOctetString(buffer[position]) == false)
    {
      SmlLogger::Error("Syntax error in %d. Expected octet string.", __LINE__);
      return SML_ERROR_SYNTAX;
    }

    int transactionIdLength = lexer.getOctetStringLength(buffer[position]);
    std::string transactionId = lexer.getOctetString(
        buffer, buffer_size, ++position, transactionIdLength);
    if (transactionId.empty())
    {
      SmlLogger::Warning("Error parsing transactionId");
    }

    position += transactionIdLength;
    SmlLogger::Debug("transactionId: %02x\n", transactionId.c_str());

    // group ID
    if (lexer.isUnsigned8(buffer[position] == false))
    {
      SmlLogger::Error("Syntax error in %d. Expected Unsigned8.", __LINE__);
      return SML_ERROR_SYNTAX;
    }

    uint8_t groupNo = lexer.getUnsigned8(buffer, buffer_size, position);
    SmlLogger::Debug("group id: %d", groupNo);

    // abortOnError
    if (lexer.isUnsigned8(buffer[position] == false))
    {
      SmlLogger::Error("Syntax error. Expected Unsigned8.");
      return SML_ERROR_SYNTAX;
    }

    [[maybe_unused]] uint8_t abortOnError =
        lexer.getUnsigned8(buffer, buffer_size, position);
    SmlLogger::Debug("abortOnError id: %d\n", abortOnError);

    // message body
    uint8_t msgBodyElements = lexer.getSmlListLength(buffer, position);
    if (msgBodyElements != 2)
    {
      SmlLogger::Warning("Syntax error. Expected SML message Type and Body");
      if (abortOnError == 0xFF)
      {
        return SML_ERROR_SYNTAX;
      }
    }
    position++;

    // message body type
    uint16_t messageType = lexer.getUnsigned16(buffer, buffer_size, position);
    SmlLogger::Debug("Type of SML message is %04x", messageType);

    switch (messageType)
    {
    case SML_MSG_TYPE_PUBOPEN_RES:
      smlPubOpenRes = parseSmlPublicOpenRes(buffer, buffer_size, position);
      break;
    case SML_MSG_TYPE_GETLIST_RES:
      smlGetListRes = parseSmlGetListRes(buffer, buffer_size, position);
      break;
    case SML_MSG_TYPE_PUBCLOS_RES:
      smlPubCloseRes = parseSmlPublicCloseRes(buffer, buffer_size, position);
      finished = true;
      break;
    default:
      SmlLogger::Error("Unknown SML message type");
      return SML_UNKNOWN_TYPE;
      break;
    }
    int end_crc = position;
    
    uint16_t crc16 = lexer.getUnsigned16(buffer, buffer_size, position);
    if (crc16 !=
        sml_crc16(const_cast<unsigned char *>(&buffer[start_crc]), end_crc - start_crc))
    {
      SmlLogger::Error(
          "CRC error: Should be %04x, but is %04x", crc16,
          sml_crc16(const_cast<unsigned char *>(buffer), end_crc - start_crc));
    }

    if (buffer[position] != 0x00)
    {
      SmlLogger::Error("Expected EndOfMessage, but found %02x",
                       buffer[position]);
      if (abortOnError == 0xFF)
      {
        return SML_ERROR_SYNTAX;
      }
    }
    else
    {
      SmlLogger::Info("---------- EoM ----------\n");
    }

    ++position;

    if (finished)
    {
      if (parseEscapeSequence(buffer, buffer_size, position) != SML_OK)
      {
        SmlLogger::Error("Syntax error in %d. Expected escape sequence 0x1b.",
                         __LINE__);
        if (abortOnError == 0xFF)
        {
          return SML_ERROR_SYNTAX;
        }
      }

      if (buffer[position] != 0x1a)
      {
        SmlLogger::Error("Syntax error in %d. Expected end sequence 0x1a.",
                         __LINE__);
        ++position;
        if (abortOnError == 0xFF)
        {
          return SML_ERROR_SYNTAX;
        }
      }
      // delete smlMessage;
      break;
    }
  }

  return SML_OK;
}

sml_error_t SmlParser::parseEscapeSequence(const unsigned char *buffer,
                                           const int buffer_size,
                                           int &position)
{
  if (position > buffer_size - 4)
  {
    return SML_ERROR_SIZE;
  }
  
  int stop = position+4;
  for (int i = position; i < stop; ++i)
  {
    if (buffer[i] != 0x1b)
    {
      return SML_ERROR_HEADER;
    }
    ++position;
  }

  return SML_OK;
}

SmlPublicOpenRes SmlParser::parseSmlPublicOpenRes(const unsigned char *buffer,
                                                  const int buffer_size,
                                                  int &position)
{

  if (lexer.getSmlListLength(buffer, position) != 6)
  {
    SmlLogger::Warning("Sytax error in %d. Expected list length of 6.",
                       __LINE__);
  }
  position++;

  SmlPublicOpenRes ret;
  // code page
  int codePageLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (codePageLength > 0)
  {
    std::string codePage =
        lexer.getOctetString(buffer, buffer_size, position, codePageLength);
    SmlLogger::Info("codePage: %s", codePage.c_str());

    ret.codePage = codePage;
  }
  position += codePageLength;

  // clientId
  int clientIdLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (clientIdLength > 0)
  {
    std::string clientId =
        lexer.getOctetString(buffer, buffer_size, position, clientIdLength);
    SmlLogger::Info("clientId: %s", clientId.c_str());

    ret.clientId = clientId;
  }
  position += clientIdLength;

  // reqField
  int reqFieldLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (reqFieldLength == 0)
  {
    SmlLogger::Warning(
        "Syntax error in line %d. Required element reqField missing", __LINE__);
  }
  std::string reqField =
      lexer.getOctetString(buffer, buffer_size, position, reqFieldLength);
  SmlLogger::Info("reqField: %s", reqField.c_str());
  ret.reqField = reqField;
  position += reqFieldLength;

  // serverId
  int serverIdLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (serverIdLength == 0)
  {
    SmlLogger::Warning(
        "Sytax error in line %d. Required element serverId missing", __LINE__);
  }
  std::string serverId =
      lexer.getOctetString(buffer, buffer_size, position, serverIdLength);
  SmlLogger::Info("serverID: %s", serverId.c_str());
  ret.serverId = serverId;
  ++position;
  position += serverIdLength;

  // refTime
  // if the optional parameter is not available, it looks like a octet string
  if (lexer.isOctetString(buffer[position]) == false)
  {
    ret.refTime = lexer.getSmlTime(buffer, buffer_size, position);
    SmlLogger::Info("reftime: %d", ret.refTime.timeValue);
  }
  else
  {
    SmlLogger::Info("No refTime");
  }

  // smlVersion
  if (lexer.isUnsigned8(buffer[position]))
  {
    ret.smlVersion = lexer.getUnsigned8(buffer, buffer_size, position);
    ++position;
  }
  SmlLogger::Info("SML Version: %d", ret.smlVersion);
  ++position;

  return ret;
}

SmlPublicCloseRes SmlParser::parseSmlPublicCloseRes(const unsigned char *buffer,
                                                    const int buffer_size,
                                                    int &position)
{

  if (lexer.getSmlListLength(buffer, position) != 1)
  {
    SmlLogger::Warning("Sytax error in %d. Expected list length of 1.",
                       __LINE__);
  }
  position++;

  SmlPublicCloseRes ret;
  if (lexer.isOctetString(buffer[position]))
  {
    int globalSignatureLength = lexer.getOctetStringLength(buffer[position]);
    ++position;
    if (globalSignatureLength > 0)
    {
      std::string globalSignature = lexer.getOctetString(
          buffer, buffer_size, position, globalSignatureLength);
      ret.globalSignature = globalSignature;
      SmlLogger::Info("globalSignature %s", globalSignature.c_str());
    }
    else
    {
      SmlLogger::Info("No globalSignature");
    }
    position += globalSignatureLength;
  }
  else if (lexer.isExtendedOctetString(buffer[position]))
  {
    int globalSignatureLength =
        lexer.getExtendedOctetStringLength(buffer, buffer_size, position);
    if (globalSignatureLength > 0)
    {
      std::string globalSignature = lexer.getExtendedOctetString(
          buffer, buffer_size, position, globalSignatureLength);
      ret.globalSignature = globalSignature;

      SmlLogger::Info("globalSignature %s", globalSignature.c_str());
    }
    else
    {
      SmlLogger::Info("No globalSignature");
    }
    position += globalSignatureLength;
  }

  SmlLogger::Debug("_____ End of List Entry _____\n");

  return ret;
}

SmlGetListRes SmlParser::parseSmlGetListRes(const unsigned char *buffer,
                                            const int buffer_size,
                                            int &position)
{

  if (lexer.getSmlListLength(buffer, position) != 7)
  {
    SmlLogger::Warning("Sytax error in %d. Expected a list of 7.", __LINE__);
  }
  position++;

  SmlGetListRes ret;
  // clientId
  int clientIdLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (clientIdLength > 0)
  {
    std::string clientId =
        lexer.getOctetString(buffer, buffer_size, position, clientIdLength);
    ret.clientId = clientId;
    SmlLogger::Info("clientId: %s", clientId.c_str());
  }
  else
  {
    SmlLogger::Info("No clientId");
  }
  position += clientIdLength;

  // serverId
  int serverIdLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (serverIdLength == 0)
  {
    SmlLogger::Warning(
        "Syntax error in line %d. Required element serverId missing", __LINE__);
  }
  std::string serverId =
      lexer.getOctetString(buffer, buffer_size, position, serverIdLength);
  ret.serverId = serverId;
  SmlLogger::Info("serverId:, %s", serverId.c_str());
  position += serverIdLength;

  // listName
  int listNameLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (listNameLength > 0)
  {
    std::string listName =
        lexer.getOctetString(buffer, buffer_size, position, listNameLength);
    ret.listName = listName;
    SmlLogger::Info("listName: %s", listName.c_str());
  }
  else
  {
    SmlLogger::Info("No listName");
  }
  position += listNameLength;

  // actSensorTime
  ret.actSensorTime = lexer.getSmlTime(buffer, buffer_size, position);
  if (ret.actSensorTime.timeValue > 0)
  {
    SmlLogger::Info("actSensorTime: %05x", ret.actSensorTime.timeValue);
  }

  // valList
  uint8_t valListLength = lexer.getSmlListLength(buffer, position);
  ++position;
  SmlLogger::Info("Found %d valList entries", valListLength);

  for (int i = 0; i < valListLength; i++)
  {
    ret.valList.emplace_back(parseSmlListEntry(buffer, buffer_size, position));
    SmlLogger::Verbose("List size: %d", (int)ret.valList.size());
  }

  // listSignature
  int listSignatureLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (listSignatureLength > 0)
  {
    std::string listSignature = lexer.getOctetString(
        buffer, buffer_size, position, listSignatureLength);
    ret.listSignature = listSignature;
    SmlLogger::Info("listSignature: %s", listSignature.c_str());
  }
  else
  {
    SmlLogger::Info("No listSignature");
  }
  position += listSignatureLength;

  // actGatewayTime
  if (buffer[position] != 0x01)
  {
    ret.actGatewayTime = lexer.getSmlTime(buffer, buffer_size, position);
    SmlLogger::Info("actGatewaytime: %05x", ret.actGatewayTime);
  }
  else
  {
    SmlLogger::Info("No actGatewaytime");
    ++position;
  }

  return ret;
}

SmlListEntry SmlParser::parseSmlListEntry(const unsigned char *buffer,
                                          const int buffer_size,
                                          int &position)
{
  if (buffer[position] != 0x77)
  {
    SmlLogger::Warning("Syntax error in line %d: Expected a list of 7 entries, "
                       "but found %02x",
                       __LINE__, buffer[position]);
  }
  ++position;

  SmlListEntry ret;
  // objName
  int nameLength = lexer.getOctetStringLength(buffer[position]);
  ++position;
  if (nameLength > 0)
  {
    std::string name =
        lexer.getOctetString(buffer, buffer_size, position, nameLength);

    if (name == OBIS_MANUFACTURER)
    {
      SmlLogger::Info("Manufacturer: ", name);
    }
    else if (name == OBIS_DEVICE_ID)
    {
      SmlLogger::Info("Device ID: ", name);
    }
    else if (name == OBIS_TOTAL_ENERGY)
    {
      SmlLogger::Info("Zählerstand Total: %02x", name.c_str());
    }
    else if (name == OBIS_ENERGY_T1)
    {
      SmlLogger::Info("Zählerstand Tarif 1: %s", name.c_str());
    }
    else if (name == OBIS_ENERGY_T2)
    {
      SmlLogger::Info("Zählerstand Tarif 2: %s", name.c_str());
    }
    else if (name == OBIS_PUB_KEY)
    {
      SmlLogger::Info("Public Key: %s", name.c_str());
    }
    else if (name == OBIS_SUM_ACT_INST_PWR)
    {
      SmlLogger::Info("Sum current active power: %s", name.c_str());
    }
    else if (name == OBIS_SUM_ACT_INST_PWR_L1)
    {
      SmlLogger::Info("Current active power L1: %s", name.c_str());
    }
    else if (name == OBIS_SUM_ACT_INST_PWR_L2)
    {
      SmlLogger::Info("Current active power L2: %s", name.c_str());
    }
    else if (name == OBIS_SUM_ACT_INST_PWR_L3)
    {
      SmlLogger::Info("Current active power L3: %s", name.c_str());
    }

    ret.objName = name;
  }
  else
  {
    SmlLogger::Info("No name");
  }
  position += nameLength;

  // status
  if (buffer[position] != 0x01)
  {
    ret.status = lexer.getSmlStatus(buffer, buffer_size, position);
    SmlLogger::Info("status: %ld", ret.status);
  }
  else
  {
    SmlLogger::Info("No status");
    ++position;
  }

  // valTime
  if (buffer[position] != 0x01)
  {
    SmlLogger::Info("valTime: %05x", ret.valTime.timeValue);
  }
  else
  {
    SmlLogger::Info("No valTime");
    ++position;
  }

  // unit
  if (buffer[position] != 0x01)
  {
    ret.unit = lexer.getUnsigned8(buffer, buffer_size, position);
    SmlLogger::Info("unit: %02x", ret.unit);
  }
  else
  {
    SmlLogger::Info("No unit");
    ++position;
  }

  // scaler
  if (buffer[position] != 0x01)
  {
    ret.scaler = lexer.getInteger8(buffer, buffer_size, position);
    SmlLogger::Info("scaler: %02d", ret.scaler);
  }
  else
  {
    SmlLogger::Info("No scaler");
    ++position;
  }

  // value
  if (lexer.isOctetString(buffer[position]))
  {
    int valueLength = lexer.getOctetStringLength(buffer[position]);
    SmlLogger::Verbose("Octet string length: %d", valueLength);

    ++position;

    if (valueLength > 0)
    {
      std::string value =
          lexer.getOctetString(buffer, buffer_size, position, valueLength);
      SmlLogger::Info("value: ", value);
      ret.isString = true;
      ret.sValue = value;
    }
    else
    {
      SmlLogger::Info("No value");
    }
    position += valueLength;
  }
  else if (lexer.isExtendedOctetString(buffer[position]))
  {
    int valueLength =
        lexer.getExtendedOctetStringLength(buffer, buffer_size, position);
    if (valueLength > 0)
    {
      std::string value = lexer.getExtendedOctetString(buffer, buffer_size,
                                                       position, valueLength);
      ret.isString = true;
      ret.sValue = value;
      SmlLogger::Info("value ", value);
    }
    else
    {
      SmlLogger::Info("No value");
    }
    position += valueLength;

    // currently only unsigned and signed values are supported
  }
  else if ((buffer[position] & 0xF0) == 0x50)
  {
    ret.isString = false;
    ret.iValue = uint64_t(lexer.getInteger(buffer, buffer_size, position));
    SmlLogger::Info("value: %ld", ret.iValue);
  }
  else if ((buffer[position] & 0xF0) == 0x60)
  {
    ret.isString = false;
    ret.iValue = lexer.getUnsigned(buffer, buffer_size, position);
    SmlLogger::Info("value: %ld", ret.iValue);
  }

  // valueSignature
  if (lexer.isOctetString(buffer[position]))
  {
    int valueSignatureLength = lexer.getOctetStringLength(buffer[position]);
    ++position;
    if (valueSignatureLength > 0)
    {
      std::string valueSignature = lexer.getOctetString(
          buffer, buffer_size, position, valueSignatureLength);
      ret.signature = valueSignature;
      SmlLogger::Info("valueSignature %s", valueSignature.c_str());
    }
    else
    {
      SmlLogger::Info("No valueSignature");
    }
    position += valueSignatureLength;
  }
  else if (lexer.isExtendedOctetString(buffer[position]))
  {
    int valueSignatureLength =
        lexer.getExtendedOctetStringLength(buffer, buffer_size, position);
    if (valueSignatureLength > 0)
    {
      std::string valueSignature = lexer.getExtendedOctetString(
          buffer, buffer_size, position, valueSignatureLength);
      ret.signature = valueSignature;
      SmlLogger::Info("valueSignature %s", valueSignature.c_str());
    }
    else
    {
      SmlLogger::Info("No valueSignature");
    }
    position += valueSignatureLength;
  }

  SmlLogger::Debug("_____ End of List Entry _____\n");

  return ret;
}

void SmlParser::hexPrint(const unsigned char *buffer, int &position)
{
  for (int i = position - 10; i < position + 10; ++i)
  {
    if (i == position)
    {
      printf("\033[0;33m");
    }
    printf("%02x ", buffer[i]);
    if (i == position)
    {
      printf("\033[0;37m");
    }
  }
  printf("\n");
  for (int i = position - 10; i < position + 10; ++i)
  {
    if (i == position)
    {
      printf("%03d", position);
    }
    else
    {
      printf("   ");
    }
  }
  printf("\n");
}

SmlListEntry SmlParser::getElementByObis(std::string obis)
{
  for (auto le : smlGetListRes.valList)
  {
    if (le.objName == obis)
    {
      return le;
    }
  }

  return SmlListEntry();
}

std::string SmlParser::getUnitAsString(uint8_t unit) {
  if(SmlUnit.find(unit) == SmlUnit.end()) {
    return "";
  } else {
    return SmlUnit.at(unit);
  }
}