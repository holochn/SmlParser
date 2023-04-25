#ifndef SML_MESSAGE_BODY_HPP
#define SML_MESSAGE_BODY_HPP

#include "SmlTypes.hpp"
#include <memory>
#include <vector>

struct SmlMessageBody {

};

struct SmlPublicOpenRes : SmlMessageBody {
    std::string codePage;
    std::string clientId;
    std::string reqField;
    std::string serverId;
    SmlTime refTime;
    uint8_t smlVersion{1};
};

struct SmlPublicCloseRes : SmlMessageBody {
    std::string globalSignature;
};

struct SmlGetListRes : SmlMessageBody {
    std::string clientId;
    std::string serverId;
    std::string listName;
    SmlTime actSensorTime;
    std::vector<SmlListEntry> valList;
    std::string listSignature;
    SmlTime actGatewayTime;
};

#endif // SML_MESSAGE_BODY_HPP