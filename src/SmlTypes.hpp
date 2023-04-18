#ifndef SML_TYPES_HPP
#define SML_TYPES_HPP

#include "SmlTypes.hpp"
#include <stdint.h>
#include <vector>

const uint16_t SML_MSG_TYPE_PUBOPEN_RES = 0x0101;
const uint16_t SML_MSG_TYPE_GETLIST_RES = 0x0701;
const uint16_t SML_MSG_TYPE_PUBCLOS_RES = 0x0201;

const std::vector<unsigned char> OBIS_MANUFACTURER{0x81, 0x81, 0xc7, 0x82, 0x03, 0xff};
const std::vector<unsigned char> OBIS_PUB_KEY{0x81, 0x81, 0xc7, 0x82, 0x05, 0xff};
const std::vector<unsigned char> OBIS_DEVICE_ID{0x01, 0x00, 0x00, 0x00, 0x09, 0xff};
const std::vector<unsigned char> OBIS_TOTAL_ENERGY{0x01, 0x00, 0x01, 0x08, 0x00, 0xff};
const std::vector<unsigned char> OBIS_ENERGY_T1{0x01, 0x00, 0x01, 0x08, 0x01, 0xff};
const std::vector<unsigned char> OBIS_ENERGY_T2{0x01, 0x00, 0x01, 0x08, 0x02, 0xff};
const std::vector<unsigned char> OBIS_CURR_POWER{0x01, 0x00, 0x10, 0x07, 0x00, 0xff};

enum sml_error_t {
    SML_OK,
    SML_NOK, 
    SML_ERROR_HEADER,
    SML_ERROR_SYNTAX,
    SML_ERROR_NULLPTR,
    SML_ERROR_ZEROLENGTH,
    SML_ERROR_SIZE,
};

enum SmlTimeType {
    secIndex, 
    timeStamp, 
    localTimestamp
};

struct SmlTime {
    SmlTimeType timeType;
    uint32_t timeValue;
};

struct SmlListEntry {
    std::vector<unsigned char> objName;
    uint64_t status;
    SmlTime valTime;
    uint8_t unit;
    int8_t scaler;
    bool isString;
    uint64_t iValue;
    std::vector<unsigned char> sValue;
    std::vector<unsigned char> signature;
};

#endif // SML_TYPES_HPP