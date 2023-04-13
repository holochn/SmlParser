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
        position_pointer += 2;
        printf("group id: %d\n", groupId);

        // abortOnError
        if(isUnsigned8(buffer.at(position_pointer) == false)) {
            printf("Syntax error. Expected Unsigned8.\n");
            return SML_ERROR_SYNTAX;
        }
        uint8_t abortOnError = 0x00;
        groupId = getUnsigned8(&buffer, position_pointer);
        position_pointer += 2;
        printf("abortOnError id: %d\n", abortOnError);

        // message body
        uint8_t msgBodyElements = getSmlListLength(&buffer, position_pointer);
        if(msgBodyElements != 2) {
            printf("Syntax error. Expected SML message Type and Body\n");
        }
        position_pointer++;

        // message body type
        uint16_t messageType = getUnsigned16(&buffer, position_pointer);
        position_pointer += 3;

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
        
        position_pointer += 3;

        if(buffer.at(position_pointer) != 0x00 ) {
            printf("Expected EndOfMessage.\n");
            return SML_ERROR_SYNTAX;
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
    printCharVector(serverId);
    printf("\n");
    position += serverIdLength;

    // refTime
    // if the optional parameter is not available, it looks like a octet string
    if(isOctetString(buffer->at(position)) == false) {
        [[maybe_unused]]
        uint32_t smlTime = getSmlTime(buffer, position);   
    }

    // smlVersion
    uint8_t smlVersion = 1;
    if(isUnsigned8(buffer->at(position))) {
        smlVersion = getUnsigned8(buffer, position);
        ++position;
    }
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
    }
    position += listNameLength;

    // actSensorTime

    // valList

    // listSignature

    // actGatewaytime

    return SML_OK;
}

void printCharVector(std::vector<unsigned char> v) {
    for(unsigned int i=0; i < v.size(); i++) {
        printf("%02x ", static_cast<int>(v.at(i)));
    }
}