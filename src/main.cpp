#include "SmlLogger.hpp"
#include "SmlParser.hpp"
#include <fstream>
#include <iostream>
#include <stdint.h>

SmlLogLevel SmlLogger::logLevel{SmlLogLevel::Warning};

static const std::string INPUT_FILE = "../../assets/dump.hex";
static const uint16_t BUFFER_SIZE = 1400;
static uint8_t buffer[BUFFER_SIZE];


int main() {
  std::ifstream input(INPUT_FILE, std::ios::binary);

  if(input.is_open()) {
    input.read(reinterpret_cast<char*>(buffer), BUFFER_SIZE);

    SmlParser smlParser(reinterpret_cast<unsigned char*>(&buffer), (int) BUFFER_SIZE);
    smlParser.parseSml();

    SmlListEntry manufacturer = smlParser.getElementByObis(OBIS_MANUFACTURER);

    if( manufacturer.objName.empty() ) {
      std::cout << "No manufacturer found\n";
    } else {
      if(manufacturer.isString) {
        std::cout << std::hex << manufacturer.sValue << "\n";
      } else {
        std::cout << "Manufacturer value is not a string\n";
      }
    }

  } else {
    std::cout << "Error opening file " << INPUT_FILE << "\n";
  }
  
  return 0;
}