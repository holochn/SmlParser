#include "SML_Parser.hpp"

bool isOctetString(const char element) {
    if( (element & 0xF0)  != 0x00) {
        return false;
    }
    return true;
}

int getOctetStringLength(const char element) {
    if(static_cast<int>(element) & 0xF0) {
        return -1;
    }
    if(static_cast<int>(element) == 0x00) {
        return 0;
    }
    return static_cast<int> (element & 0x0F) - 1;
}

sml_error_t getOctetStringAsVector(const std::vector<char> *data, int position, std::vector<char> *octetstring, int length) {
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

bool isUnsigned8(const char element) {
    if( element != 0x52 ) {
        return false;
    }
    return true;
}

uint8_t getUnsigned8(const std::vector<char> *data, const int position) {
    return data->at(position + 1);
}