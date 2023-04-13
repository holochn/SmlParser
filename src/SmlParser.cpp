#include "SmlParser.hpp"

std::vector<unsigned char> OBIS_MANUFACTURER{0x81, 0x81, 0xc7, 0x82, 0x03, 0xff};

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

uint8_t getUnsigned8(const std::vector<unsigned char> *data, const int position) {
    if( data->at(position) != 0x62 ) {
        return 0xFF;
    }
    
    if(position >= static_cast<const int>(data->size()-1)) {
        return 0xFF;
    }
    return data->at(position + 1);
}

uint16_t getUnsigned16(const std::vector<unsigned char> *data, const int position) {
    uint16_t retval = 0;
    int pos = position;
    if( data->at(position) != 0x63 ) {
        return 0xFFFF;
    }

    if(position >= static_cast<const int>(data->size()-2)) {
        return 0xFFFF;
    }

    pos++;
    retval = data->at(pos) << 8;
    pos++;
    retval = retval | data->at(pos);
    return retval;
}

uint32_t getUnsigned32(const std::vector<unsigned char> *data, const int position) {
    uint32_t retval = 0;
    int pos = position;
    if( data->at(pos) != 0x65 ) {
        return 0xFFFFFFFF;
    }

    if(pos >= static_cast<const int>(data->size()-4)) {
        return 0xFFFFFFFF;
    }

    pos++;
    retval = data->at(pos) << 24;
    pos++;
    retval = retval | data->at(pos) << 16;
    pos++;
    retval = retval | data->at(pos) << 8;
    pos++;
    retval = retval | data->at(pos);
    return retval;
}

uint8_t getSmlListLength(const std::vector<unsigned char> *data, const int position) {
    if((data->at(position) & 0xF0) != 0x70) {
        return 0xFF;
    }

    return data->at(position) & 0x0F;
}

uint32_t getSmlTime(const std::vector<unsigned char> *data, const int position) {
    [[maybe_unused]]
    uint32_t retval=0;
    int pos = position;

    if(data->at(pos) != 0x72) {
        return 0;
    }
    ++pos;

    int time_type = getUnsigned8(data, pos);
    pos += 2;

    switch(time_type) {
        case 1: // secIndex
            retval = getUnsigned32(data, pos);
            break;
        case 2: // timestamp
            retval = getUnsigned32(data, pos);
            break;
        case 3: // timestamp local
            break;
    }

    return retval;
}