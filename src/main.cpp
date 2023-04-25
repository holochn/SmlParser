#include "SmlLogger.hpp"
#include "SmlParser.hpp"
#include <fstream>
#include <iostream>
#include <stdint.h>

SmlLogLevel SmlLogger::logLevel{SmlLogLevel::Verbose};

static const std::string INPUT_FILE = "../../assets/dump.hex";
static const uint16_t BUFFER_SIZE = 1400;
static uint8_t buffer[BUFFER_SIZE];


int main() {
  std::ifstream input(INPUT_FILE, std::ios::binary);

  if(input.is_open()) {
    input.read(reinterpret_cast<char*>(buffer), BUFFER_SIZE);

    SmlParser smlParser(reinterpret_cast<unsigned char*>(&buffer), (int) BUFFER_SIZE);
    smlParser.parseSml();

  } else {
    std::cout << "Error opening file " << INPUT_FILE << "\n";
  }
  
  // std::unique_ptr<SmlListEntry> manufacturer = smlParser.getElementByObis(OBIS_MANUFACTURER);
  
  return 0;
}