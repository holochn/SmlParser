#ifndef SML_PARSER_HPP
#define SML_PARSER_HPP

#include "SmlLexer.hpp"
#include "SmlLogger.hpp"
#include "SmlMessageBody.hpp"
#include "SmlTypes.hpp"
#include <cstring>

class SmlParser {
private:
  const unsigned char *buffer;
  int buffer_size;
  int position;
  SmlLexer lexer;
  SmlPublicOpenRes smlPubOpenRes;
  SmlPublicCloseRes smlPubCloseRes;
  SmlGetListRes smlGetListRes;

public:
  SmlParser(unsigned char *t_buffer, int t_buffer_size);
  ~SmlParser();

  /** @brief Main parsing function
   *  @param data Pointer to a vector of char
   *  @param position Pointer to the position of the octet string
   *  @return SML_OK on success
   *  @return SML_SML_ERROR_ZEROLENGTH if buffer is NULL or buffer_size is 0
   *  @return SML_ERROR_SYNTAX on syntax error
   *  @return SML_UNKNOWN_TYPE if message type if unknown
   */
  sml_error_t parseSml(void);

  /**  @brief parses the SML escape sequence
   *  @param buffer The aray of unsigned char to parse
   *  @param buffer_size The site of the buffer to check
   *  @param position The position in buffer to start working
   *  @return SML_OK on success
   *  @return SML_ERROR_SIZE if position is smaller than buffer_size minus 4
   *  @return SML_ERROR_HEADER on a syntax error
   */
  sml_error_t parseEscapeSequence(const unsigned char *buffer,
                                  const int buffer_size, int &position);

  /** @brief Parses a SML PublicOpen.Res message
   *  @param buffer A pointer to a array of unsigned char
   *  @param buffer_size Size of the buffer to lex
   *  @param position the positon in the buffer where to start parsing
   *  @return pointer to SmlPublicOpenRes
   */
  SmlPublicOpenRes parseSmlPublicOpenRes(const unsigned char *buffer,
                                         const int buffer_size, int &position);

  /** @brief Parses a SML PublicClose.Res message
   *  @param buffer A pointer to a array of unsigned char
   *  @param buffer_size Size of the buffer to lex
   *  @param position the positon in the buffer where to start parsing
   *  @return pointer to SmlPublicCloseRes
   */
  SmlPublicCloseRes parseSmlPublicCloseRes(const unsigned char *buffer,
                                           const int buffer_size,
                                           int &position);

  /** @brief Parses a SML GetList.Res message
   *  @param buffer A pointer to a array of unsigned char
   *  @param buffer_size Size of the buffer to lex
   *  @param position the positon in the buffer where to start parsing
   *  @return pointer to SmlGetListRes
   */
  SmlGetListRes parseSmlGetListRes(const unsigned char *buffer,
                                   const int buffer_size, int &position);

  /** @brief Parses a SML input stream as unsigned char for SMLListEntries
   *  @param buffer A pointer to a array of unsigned char
   *  @param buffer_size Size of the buffer to lex
   *  @param position the position pointer wehere to start parsing
   *  @return SmlListEntry
   */
  SmlListEntry parseSmlListEntry(const unsigned char *buffer, const int buffer_size,
                               int &position);

  /** @brief Searches a SmlListEntry and returns it
   *  @param obis The OBIS to search for
   *  @return SmlListEntry
   */
  SmlListEntry getElementByObis(std::string obis);

  /** @brief Returns a string representation os a SMLUnit
   *  @param SmlUnit hex-ID of the SmlUnit
   * @return String that represents the SmlUnit 
  */
 std::string getUnitAsString(uint8_t unit);

  void hexPrint(const unsigned char *buffer, int &position);
};

#endif // SML_PARSER_HPP