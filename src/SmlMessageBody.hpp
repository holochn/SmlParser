#ifndef SML_MESSAGE_BODY_HPP
#define SML_MESSAGE_BODY_HPP

#include "SmlTypes.hpp"
#include <vector>

struct SmlMessageBody {

};

struct SmlPublicOpenRes : SmlMessageBody {
    std::vector<unsigned char> codePage;
    std::vector<unsigned char> clientId;
    std::vector<unsigned char> reqField;
    std::vector<unsigned char> serverId;
    SmlTime refTime;
    uint8_t smlVersion{1};
};

struct SmlPublicCloseRes : SmlMessageBody {
    std::vector<unsigned char> globalSignature;
};

struct SmlGetListRes : SmlMessageBody {
    std::vector<unsigned char> clientId;
    std::vector<unsigned char> serverId;
    std::vector<unsigned char> listName;
    SmlTime actSensorTime;
    std::vector<SmlListEntry> valList;
    std::vector<unsigned char> listSignature;
    SmlTime actGatewayTime;
};

#endif // SML_MESSAGE_BODY_HPP