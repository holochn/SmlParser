#ifndef SML_MESSAGE_HPP
#define SML_MESSAGE_HPP

#include "SmlMessageBody.hpp"
#include <stdint.h>
#include <vector>

struct SmlMessage {
    uint16_t messageType;
    std::vector<unsigned char> transactionId;
    uint8_t groupNo;
    uint8_t abortOnError;
    SmlMessageBody messageBody;
    uint16_t crc16;
};

#endif