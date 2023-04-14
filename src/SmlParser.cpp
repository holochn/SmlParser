#include "SmlParser.hpp"

bool isOctetString(const unsigned char element) {
    if( (element & 0xF0)  != 0x00) {
        return false;
    }
    return true;
}

int getOctetStringLength(const unsigned char element) {
    if(static_cast<int>(element) & 0xF0) {
        return -1;
    }
    if(static_cast<int>(element) == 0x00) {
        return 0;
    }
    return static_cast<int> (element & 0x0F) - 1;
}

sml_error_t getOctetStringAsVector(const std::vector<unsigned char> *data, int position, std::vector<unsigned char> *octetstring, int length) {
    if(data == nullptr) {
        return SML_ERROR_NULLPTR;
    }

    if(octetstring == nullptr) {
        return SML_ERROR_NULLPTR;
    }
    
    if(length == 0) {
        return SML_ERROR_ZEROLENGTH;
    }

    if( static_cast<int>(data->size()) < (position + length) ) {
        return SML_ERROR_SIZE;
    }

    for (int i = 0; i < length; i++)
    {
        octetstring->at(i) = data->at(position + i);
    }
    
    return SML_OK;
}

bool isUnsigned8(const unsigned char element) {
    if( element != 0x62 ) {
        return false;
    }
    return true;
}

uint8_t getUnsigned8(const std::vector<unsigned char> *data, int &position) {
    uint8_t retval = 0;
    if( data->at(position) != 0x62 ) {
        printf("Expected Unsigned8 (0x62), but found %02x\n", data->at(position) );
        return 0xFF;
    }
    [[maybe_unused]]
    size_t tmp = data->size();
    printf("size: %d\n", int(tmp));
    if(position >= static_cast<int>(data->size()-1)) {
        return 0xFF;
    }
    ++position;
    retval = data->at(position);
    ++position;
    return retval;
}

int8_t getSigned8(const std::vector<unsigned char> *data, int &position) {
    int8_t retval = 0;
    if( data->at(position) != 0x52 ) {
        printf("Expected Unsigned8 (0x52), but found %02x\n", data->at(position) );
        return 0xFF;
    }

    if(position >= static_cast<int>(data->size()-1)) {
        return 0xFF;
    }
    ++position;
    retval = static_cast<int8_t>(data->at(position));
    ++position;
    return retval;
}

uint16_t getUnsigned16(const std::vector<unsigned char> *data, int &position) {
    uint16_t retval = 0;
    if( data->at(position) != 0x63 ) {
        printf("Expected Unsigned16 (0x63), but found %02x\n", data->at(position) );
        return 0xFFFF;
    }

    if(position >= static_cast<const int>(data->size()-2)) {
        return 0xFFFF;
    }

    ++position;
    retval = data->at(position) << 8;
    ++position;
    retval = retval | data->at(position);
    return retval;
}

uint32_t getUnsigned32(const std::vector<unsigned char> *data, int &position) {
    uint32_t retval = 0;
    if( data->at(position) != 0x65 ) {
        printf("Expected Unsigned32 (0x65), but found %02x\n", data->at(position) );
        return 0xFFFFFFFF;
    }

    if(position >= static_cast<const int>(data->size()-4)) {
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
    return retval;
}

uint64_t getUnsigned64(const std::vector<unsigned char> *data, int &position) {
    uint64_t retval = 0;
    if( data->at(position) != 0x69 ) {
        printf("Expected Unsigned64 (0x69), but found %02x\n", data->at(position) );
        return 0xFFFFFFFFFFFFFFFF;
    }

    if(position >= static_cast<const int>(data->size()-8)) {
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
    return retval;

    return retval;
}

uint8_t getSmlListLength(const std::vector<unsigned char> *data, const int position) {
    if((data->at(position) & 0xF0) != 0x70) {
        return 0xFF;
    }

    return data->at(position) & 0x0F;
}

uint32_t getSmlTime(const std::vector<unsigned char> *data, int &position) {
    uint32_t retval=0;

    if(data->at(position) != 0x72) {
        return 0;
    }
    ++position;

    int time_type = getUnsigned8(data, position);
    if(time_type == 0xFF) {
        return 0;
    }

    switch(time_type) {
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
    if( data->at(position) == 0x62 ) { 
        retval = static_cast<uint64_t>( getUnsigned8(data, position) );
    }

    if( data->at(position) == 0x63 ) { 
        retval = static_cast<uint64_t>( getUnsigned16(data, position) );
    }

    if( data->at(position) == 0x65 ) { 
        retval = static_cast<uint64_t>( getUnsigned32(data, position) );
    }

    if( data->at(position) == 0x69 ) { 
        retval = getUnsigned64(data, position);
    }

    return retval;
}