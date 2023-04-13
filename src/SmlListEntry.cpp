#include "SmlListEntry.hpp"

SmlListEntry::SmlListEntry(){};

SmlListEntry::~SmlListEntry(){};

void SmlListEntry::setName(const std::vector<unsigned char> name) {
  objName = name;
}

std::vector<unsigned char> SmlListEntry::getName() const { return objName; }

void SmlListEntry::setStatus(const uint64_t t_status) { status = t_status; }

uint64_t SmlListEntry::getStatus() const { return status; }

void SmlListEntry::setTime(const uint32_t t_time) { valTime = t_time; }

void SmlListEntry::setUnit(const uint8_t t_unit) { unit = t_unit; }

void SmlListEntry::setScaler(const uint8_t t_scaler) { scaler = t_scaler; }

void SmlListEntry::setNominalValue(const uint64_t t_value) {
  isString = false;
  iValue = t_value;
}

void SmlListEntry::setStringValue(const std::vector<unsigned char> t_value) {
  isString = true;
  sValue = t_value;
}

void SmlListEntry::setSignature(const std::vector<unsigned char> t_value) {
  signature = t_value;
}