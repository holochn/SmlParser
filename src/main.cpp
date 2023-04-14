#include "SmlParser.hpp"

static const std::string INPUT_FILE = "../../assets/dump.hex";

/* @brief Parses a SML PublicOpen.Res message
 * @param buffer A pointer to a std::vector<char> buffer
 * @param position the positon in the buffer where to start parsing
 * @return SML_OK on success
 * @return SML_ERR_SIZE on error
 */
sml_error_t parseSmlPublicOpenRes(const std::vector<unsigned char> *buffer, int &position);

/* @brief Parses a SML GetList.Res message
 * @param buffer A pointer to a std::vector<unsigned char> buffer
 * @param position the positon in the buffer where to start parsing
 * @return SML_OK on success
 * @return SML_ERR_SIZE on error
 */
sml_error_t parseSmlGetListRes(const std::vector<unsigned char> *buffer, int &position);

/* @brief Parses a SML input stream as std::vector<unsigned char> for SMLListEntries
 * @param buffer The input stream as std::vector<unsigned char>
 * @param position the position pointer wehere to start parsing
 * @return SmlListEntry
 */
SmlListEntry parseSmlListEntry(const std::vector<unsigned char> *buffer, int &position);

void printCharVector(std::vector<unsigned char> v);

int main() {
    std::ifstream input(INPUT_FILE, std::ios::binary);
    std::istreambuf_iterator<char> it(input);
    std::istreambuf_iterator<char> end;
    std::vector<unsigned char> buffer(it, end);

    printf("Buffer size: %d\n", static_cast<int>(buffer.size()));
    if(buffer.size() == 0) {
        abort();
    }

    int position_pointer = 0;
    for (int position_pointer = 0; position_pointer < 4; position_pointer++)
    {
        if( buffer[position_pointer] != 0x1b) {
            printf("Error in Header\n");
            return SML_ERROR_HEADER;
        }
    }
    for (position_pointer = 4; position_pointer < 8; position_pointer++)
    {
        if( buffer[position_pointer] != 0x01) {
            printf("Error in Header\n");
            return SML_ERROR_HEADER;
        }
    }

    std::vector<unsigned char> transactionId;
    while(position_pointer < static_cast<int>(buffer.size()))
    {
        if( buffer[position_pointer] != 0x76) {
            break;
        }

        printf("Found a new SML file\n");

        // transactionId
        position_pointer++;
        if( isOctetString(buffer.at(position_pointer)) == false ) {
            printf("Syntax error. Expected octet string.\n");
            return SML_ERROR_SYNTAX;
        }
        
        int transactionIdLength = getOctetStringLength(buffer.at(position_pointer));
        transactionId.resize(transactionIdLength);
        
        if(getOctetStringAsVector(&buffer, ++position_pointer, &transactionId, transactionIdLength) != SML_OK) {
            printf("Error parsing transactionId\n");
        }
        position_pointer += transactionIdLength;
        
        // group ID
        if(isUnsigned8(buffer.at(position_pointer) == false)) {
            printf("Syntax error. Expected Unsigned8.\n");
            return SML_ERROR_SYNTAX;
        }
        uint8_t groupId = 0x00;
        groupId = getUnsigned8(&buffer, position_pointer);
        printf("group id: %d\n", groupId);

        // abortOnError
        if(isUnsigned8(buffer.at(position_pointer) == false)) {
            printf("Syntax error. Expected Unsigned8.\n");
            return SML_ERROR_SYNTAX;
        }
        uint8_t abortOnError = 0x00;
        groupId = getUnsigned8(&buffer, position_pointer);
        printf("abortOnError id: %d\n", abortOnError);

        // message body
        uint8_t msgBodyElements = getSmlListLength(&buffer, position_pointer);
        if(msgBodyElements != 2) {
            printf("Syntax error. Expected SML message Type and Body\n");
        }
        position_pointer++;

        // message body type
        uint16_t messageType = getUnsigned16(&buffer, position_pointer);
        printf("Type of SML message is %04x\n", messageType);
        ++position_pointer;

        switch(messageType) {
            case SML_MSG_TYPE_PUBOPEN_RES: parseSmlPublicOpenRes(&buffer, position_pointer);
                    break;
            case SML_MSG_TYPE_GETLIST_RES: parseSmlGetListRes(&buffer, position_pointer);
                    break;
            default: printf("Unknown SML message type\n");
                    abort();
                    break;
        }
        
        [[maybe_unused]] 
        uint16_t crc16 = getUnsigned16(&buffer, position_pointer);
        ++position_pointer;
        
        if(buffer.at(position_pointer) != 0x00 ) {
            printf("Expected EndOfMessage, but found %02x\n", buffer.at(position_pointer));
            return SML_ERROR_SYNTAX;
        } else {
            printf("---------- EoM ----------\n");
        }

        ++position_pointer;
    }
}

sml_error_t parseSmlPublicOpenRes(const std::vector<unsigned char> *buffer, int &position) {
    if(getSmlListLength(buffer, position) != 6) {
        return SML_ERROR_SIZE;
    }
    position++;

    // code page
    int codePageLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(codePageLength > 0) {
        std::vector<unsigned char> codePage;
        codePage.resize(codePageLength);
        getOctetStringAsVector(buffer, position, &codePage, codePageLength);
    }
    position += codePageLength;

    // clientId
    int clientIdLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(clientIdLength > 0) {
        std::vector<unsigned char> clientId;
        clientId.resize(clientIdLength);
        getOctetStringAsVector(buffer, position, &clientId, clientIdLength);
    }
    position += clientIdLength;

    // reqField
    int reqFieldLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(reqFieldLength == 0) {
        printf("Required element reqField missing\n");
        return SML_ERROR_SYNTAX;
    }
    std::vector<unsigned char> reqField;
    reqField.resize(reqFieldLength);
    getOctetStringAsVector(buffer, position, &reqField, reqFieldLength);
    printf("reqField: ");
    printCharVector(reqField);
    printf("\n");
    position += reqFieldLength;

    // serverId
    int serverIdLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(serverIdLength == 0) {
        printf("Required element serverId missing\n");
        return SML_ERROR_SYNTAX;
    }
    std::vector<unsigned char> serverId;
    serverId.resize(serverIdLength);
    getOctetStringAsVector(buffer, position, &serverId, serverIdLength);
    printf("serverId: ");
    printCharVector(serverId);
    printf("\n");
    ++position;
    position += serverIdLength;

    // refTime
    // if the optional parameter is not available, it looks like a octet string
    if(isOctetString(buffer->at(position)) == false) {
        [[maybe_unused]]
        uint32_t smlTime = getSmlTime(buffer, position);   
    } else {
        printf("No refTime\n");
    }

    // smlVersion
    uint8_t smlVersion = 1;
    if(isUnsigned8(buffer->at(position))) {
        smlVersion = getUnsigned8(buffer, position);
        ++position;
    }
    printf("SML Version: %d\n", smlVersion);
    ++position;

    return SML_OK;
}

sml_error_t parseSmlGetListRes(const std::vector<unsigned char> *buffer, int &position) {
    if(getSmlListLength(buffer, position) != 7) {
        return SML_ERROR_SIZE;
    }
    position++;

    // clientId
    int clientIdLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(clientIdLength > 0) {
        std::vector<unsigned char> clientId;
        clientId.resize(clientIdLength);
        getOctetStringAsVector(buffer, position, &clientId, clientIdLength);
    } else {
        printf("No clientId\n");
    }
    position += clientIdLength;

    // serverId
    int serverIdLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(serverIdLength == 0) {
        printf("Required element serverId missing\n");
        return SML_ERROR_SYNTAX;
    }
    std::vector<unsigned char> serverId;
    serverId.resize(serverIdLength);
    getOctetStringAsVector(buffer, position, &serverId, serverIdLength);
    printf("serverID: ");
    printCharVector(serverId);
    printf("\n");
    position += serverIdLength;

    // listName
    int listNameLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(listNameLength > 0) {
        std::vector<unsigned char> listName;
        listName.resize(listNameLength);
        getOctetStringAsVector(buffer, position, &listName, listNameLength);
        printf("listName: ");
        printCharVector(listName);
        printf("\n");
    } else {
        printf("No listName\n");
    }
    position += listNameLength;

    // actSensorTime
    uint32_t actSensorTime = getSmlTime(buffer, position);
    if(actSensorTime > 0) {
        printf("actSensorTime: %05x\n", actSensorTime);
    }
    ++position;

    printf("%02x\n", buffer->at(position));

    // valList
    uint8_t valListLength = getSmlListLength(buffer, position);
    ++position;
    printf("Found %d valList entries\n", valListLength);

    for (int i = 0; i < valListLength; i++)
    {   
        SmlListEntry entry = parseSmlListEntry(buffer, position);
    }
    

    // listSignature
    int listSignatureLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(listSignatureLength > 0) {
        std::vector<unsigned char> listSignature;
        listSignature.resize(listSignatureLength);
        getOctetStringAsVector(buffer, position, &listSignature, listSignatureLength);
        printCharVector(listSignature);
        printf("\n");
    } else {
        printf("No listSignature\n");
    }
    position += listSignatureLength;

    // actGatewaytime
    if(buffer->at(position) != 0x01) {
        uint32_t actGatewaytime = getSmlTime(buffer, position);
        printf("actGatewaytime: %05x\n", actGatewaytime);
    } else {
        printf("No actGatewaytime\n");
    }
    ++position;

    return SML_OK;
}

SmlListEntry parseSmlListEntry(const std::vector<unsigned char> *buffer, int &position) {
    SmlListEntry ret;
    if(buffer->at(position) != 0x77) {
        printf("Error: Expected a list of 7 entries, but found %02x\n", buffer->at(position));
        abort();
    }
    ++position;

    // objName
    int nameLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(nameLength > 0) {
        std::vector<unsigned char> name;
        name.resize(nameLength);
        getOctetStringAsVector(buffer, position, &name, nameLength);
        if(name == OBIS_MANUFACTURER) {
            printf("Manufacturer: ");
            printCharVector(name);
            printf("\n");
        } else if(name == OBIS_DEVICE_ID) {
            printf("Device ID: ");
            printCharVector(name);
            printf("\n");
        } else if(name == OBIS_ACTIVE_ENERGY) {
            printf("Aktive Wirkarbeit: ");
            printCharVector(name);
            printf("\n");
        }
    } else {
        printf("No name\n");
    }
    position += nameLength;

    // status
    if(buffer->at(position) != 0x01) {
        uint64_t status = getSmlStatus(buffer, position);
        ret.setStatus(status);
    } else {
        printf("No status\n");
    }
    ++position;

    // valTime
    if(buffer->at(position) != 0x01) {
        uint32_t valTime = getSmlTime(buffer, position);
        ret.setTime(valTime);
        printf("valTime: %05x\n", valTime);
    } else {
        printf("No valTime\n");
    }
    ++position;

    // unit
    if(buffer->at(position) != 0x01) {
        uint8_t unit = getUnsigned8(buffer, position);
        ret.setUnit(unit);
        printf("unit: %02x\n", unit);
    } else {
        printf("No unit\n");
    }
    ++position;

    // scaler
    if(buffer->at(position) != 0x01) {
        int8_t scaler = getSigned8(buffer, position);
        ret.setScaler(scaler);
        printf("scaler: %02x\n", scaler);
    } else {
        printf("No scaler\n");
    }
    ++position;

    // value
    if( (buffer->at(position) & 0xF0) == 0x00 ) {
        int valueLength = getOctetStringLength(buffer->at(position));
        ++position;
        if(valueLength > 0) {
            std::vector<unsigned char> value;
            value.resize(valueLength);
            getOctetStringAsVector(buffer, position, &value, valueLength);
            printCharVector(value);
            printf("\n");
            ret.setStringValue(value);
        } else {
            printf("No value\n");
        }
        position += valueLength;
    } else {
        // todo: parse numeric value
    }

    // valueSignature
    int valueSignatureLength = getOctetStringLength(buffer->at(position));
    ++position;
    if(valueSignatureLength > 0) {
        std::vector<unsigned char> valueSignature;
        valueSignature.resize(valueSignatureLength);
        getOctetStringAsVector(buffer, position, &valueSignature, valueSignatureLength);
        ret.setSignature(valueSignature);
        printCharVector(valueSignature);
        printf("\n");
    } else {
        printf("No valueSignature\n");
    }
    position += valueSignatureLength;

    printf("_____ End of List Entry _____\n");

    return ret;
}

void printCharVector(std::vector<unsigned char> v) {
    for(unsigned int i=0; i < v.size(); i++) {
        printf("%02x ", static_cast<int>(v.at(i)));
    }
}