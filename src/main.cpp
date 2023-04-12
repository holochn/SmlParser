#include "SML_Parser.hpp"

static const std::string INPUT_FILE = "/Users/holger/workspace/SML_Parser/assets/dump.hex";

int main() {
    std::ifstream input(INPUT_FILE, std::ios::binary);
    std::istreambuf_iterator<char> it(input);
    std::istreambuf_iterator<char> end;
    std::vector<char> buffer(it, end);

    printf("Buffer size: %lu\n", buffer.size());

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

    std::vector<char> transactionId;
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
        
        if(getOctetStringAsVector(&buffer, position_pointer, &transactionId, transactionIdLength) != SML_OK) {
            printf("Error parsing transactionId\n");
        }
        
        // group ID
        position_pointer++;
        if(isUnsigned8(buffer.at(position_pointer) == false)) {
            printf("Syntax error. Expected Unsigned8.\n");
            return SML_ERROR_SYNTAX;
        }
        uint8_t groupId = getUnsigned8(&buffer, position_pointer);
    }
}