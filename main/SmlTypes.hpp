#ifndef SML_TYPES_HPP
#define SML_TYPES_HPP

#include <stdint.h>
#include <string>
#include <cmath>

const uint16_t SML_MSG_TYPE_PUBOPEN_RES = 0x0101;
const uint16_t SML_MSG_TYPE_GETLIST_RES = 0x0701;
const uint16_t SML_MSG_TYPE_PUBCLOS_RES = 0x0201;

const std::string OBIS_MANUFACTURER{0x81, 0x81, 0xc7, 0x82, 0x03, 0xff};
const std::string OBIS_PUB_KEY{0x81, 0x81, 0xc7, 0x82, 0x05, 0xff};
const std::string OBIS_DEVICE_ID{0x01, 0x00, 0x00, 0x00, 0x09, 0xff};
const std::string OBIS_TOTAL_ENERGY{0x01, 0x00, 0x01, 0x08, 0x00, 0xff};
const std::string OBIS_ENERGY_T1{0x01, 0x00, 0x01, 0x08, 0x01, 0xff};
const std::string OBIS_ENERGY_T2{0x01, 0x00, 0x01, 0x08, 0x02, 0xff};
const std::string OBIS_SUM_ACT_INST_PWR{0x01, 0x00, 0x10, 0x07, 0x00, 0xff};
const std::string OBIS_SUM_ACT_INST_PWR_L1{0x01, 0x00, 0x24, 0x07, 0x00, 0xff};
const std::string OBIS_SUM_ACT_INST_PWR_L2{0x01, 0x00, 0x38, 0x07, 0x00, 0xff};
const std::string OBIS_SUM_ACT_INST_PWR_L3{0x01, 0x00, 0x4c, 0x07, 0x00, 0xff};

enum sml_error_t {
    SML_OK,
    SML_NOK, 
    SML_ERROR_HEADER,
    SML_ERROR_SYNTAX,
    SML_ERROR_NULLPTR,
    SML_ERROR_ZEROLENGTH,
    SML_ERROR_SIZE,
    SML_UNKNOWN_TYPE,
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
    std::string objName;
    uint64_t status;
    SmlTime valTime;
    uint8_t unit;
    int8_t scaler{0};
    bool isString;
    uint64_t iValue;
    std::string sValue;
    std::string signature;

    double value() {
        double val=0.0f;

        if(!isString) {
            val = static_cast<double>(iValue);
            val *= std::pow(10, scaler);
        }

        return val;
    }
};

#endif // SML_TYPES_HPP