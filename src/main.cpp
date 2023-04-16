#include "SmlLogger.hpp"

SmlLogLevel SmlLogger::logLevel{SmlLogLevel::Error};

#include "SmlParser.hpp"

static const std::string INPUT_FILE = "../../assets/dump.hex";

int main() {
  std::ifstream input(INPUT_FILE, std::ios::binary);
  std::istreambuf_iterator<char> it(input);
  std::istreambuf_iterator<char> end;
  std::vector<unsigned char> buffer(it, end);

  parseSml(&buffer);

  return 0;
}