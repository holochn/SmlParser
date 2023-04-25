#ifndef SML_LIST_ENTRY_HPP_
#define SML_LIST_ENTRY_HPP_

#include <stdint.h> 
#include <vector>

class SmlListEntry {
private:
    std::vector<unsigned char> objName;
    uint64_t status;
    uint32_t valTime;
    uint8_t unit;
    int8_t scaler;
    bool isString;
    uint64_t iValue;
    std::vector<unsigned char> sValue;
    std::vector<unsigned char> signature;

public:
    SmlListEntry();
    ~SmlListEntry();
    void setName(const std::vector<unsigned char> name);
    std::vector<unsigned char> getName() const;
    void setStatus(const uint64_t t_status);
    uint64_t getStatus() const;
    void setTime(const uint32_t t_time);
    void setUnit(const uint8_t t_unit); 
    void setScaler(const uint8_t t_scaler);
    void setNominalValue(const uint64_t t_value); 
    void setStringValue(const std::vector<unsigned char> t_value);
    void setSignature(const std::vector<unsigned char> t_value);
};

#endif // SML_LIST_ENTRY_HPP_