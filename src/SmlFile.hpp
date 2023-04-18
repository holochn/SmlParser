#ifndef SML_FILE_HPP
#define SML_FILE_HPP

#include "SmlMessage.hpp"
#include "SmlMessageBody.hpp"
#include "SmlTypes.hpp"
#include <string>
#include <vector>

class SmlFile {
private:
    std::vector<SmlMessage> messages;
public:
    void addMessage(SmlMessage msg) {
        messages.push_back(msg);
    }

    std::string getAsString_Manufacturer() const {
        std::string manufacturer;
        for(SmlMessage sm : messages) {
            if(sm.messageType == SML_MSG_TYPE_GETLIST_RES) {
                SmlGetListRes body = sm.messageBody;
                std::vector<SmlListEntry> valList = body.valList; 
                for(SmlListEntry le : valList) {

                }
            }
        }

        return manufacturer;
    }
};

#endif // SML_FILE_HPP