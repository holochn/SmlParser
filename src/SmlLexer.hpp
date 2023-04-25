#ifndef SML_LEXER_HPP
#define SML_LEXER_HPP

#include "SmlLogger.hpp"
#include "SmlTypes.hpp"
#include <stdint.h>

class SmlLexer {

public:
  /* @brief Checks if a vector element is a SML Octet string
   * @param element The element to check as char
   * @return true if element is a SML octet string
   * @return false if element is not a SML octet string
   */
  bool isOctetString(const unsigned char element) const;

  /* @brief Gets the length of an extended octet string
   * @param data Pointer to a vector of char
   * @param position Pointer to the position of the octet string
   * @return The length of an extended octet string as int
   */
  bool isExtendedOctetString(const unsigned char element) const;

  /* @brief Checks if a vector element is a Unsigned8
   * @param element The element to check as char
   * @return true if element is a Unsigned8
   * @return false if element is not an Unsigned8
   */
  bool isUnsigned8(const unsigned char element) const;

  /* @brief Gets the length of an octet string
   * @param element The element to check as char
   * @return The length of an octet string as int
   * @return -1 if the element to check is zero
   * @return -2 if the element to check is not of type octet string
   */
  int getOctetStringLength(const unsigned char element) const;

  /* @brief Gets the length of a octet string with extended Type-Length field
   * @param data Pointer to a vector of char
   * @param position Pointer to the position of the octet string
   * @return The length of the octet string
   * @return -1 if the element to check is zero
   * @return -2 if the element to check is not of type octet string
   */
  int getExtendedOctetStringLength(const unsigned char *buffer,
                                   const int buffer_size, int &position);

  /* @brief Gets a SML Unsigned8 from vector of chars
   * @param data Pointer to a vector of char
   * @param position Pointer to the position of the octet string
   * @return Unsigned8
   * @return 0xFF on error
   */
  uint8_t getUnsigned8(const unsigned char *buffer, int buffer_size,
                       int &position);

  /* @brief Gets a SML Signed8 from vector of chars
   * @param buffer Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Signed8
   * @return 0xFF on error
   */
  int8_t getInteger8(const unsigned char *buffer, const int buffer_size,
                     int &position);

  /* @brief Gets a SML Unsigned16 from array of unsignd chars
   * @param data Pointer to a vector of char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Unsigned16
   * @return 0xFFFF on error
   */
  uint16_t getUnsigned16(const unsigned char *buffer, const int buffer_size,
                         int &position);

  /* @brief Gets a SML Integer16 from vector of chars
   * @param data Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Integer16
   * @return 0xFFFF on error
   */
  int16_t getInteger16(const unsigned char *buffer, const int buffer_size,
                       int &position);

  /* @brief Gets a SML Unsigned32 from vector of chars
   * @param data Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Unsigned32
   * @return 0xFFFFFFFF on error
   */
  uint32_t getUnsigned32(const unsigned char *buffer, const int buffer_size,
                         int &position);

  /* @brief Gets a SML Integer32 from vector of chars
   * @param data Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Integer32
   * @return 0xFFFFFFFF on error
   */
  int32_t getInteger32(const unsigned char *buffer, const int bufer_size,
                       int &position);

  /* @brief Gets a SML Unsigned64 from vector of chars
   * @param data Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Unsigned64
   * @return 0xFFFFFFFFFFFFFFFF on error
   */
  uint64_t getUnsigned64(const unsigned char *buffer, const int buffer_size,
                         int &position);

  /* @brief Gets a SML Integer64 from vector of chars
   * @param data Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Integer64
   * @return 0xFFFFFFFFFFFFFFFF on error
   */
  int64_t getInteger64(const unsigned char *buffer, const int buffer_size,
                       int &position);

  /* @brief Get number of elements in a SML list
   * @param buffer Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the list
   * @return number of list elements as uint8_t
   * @return 0xFF in case of an error
   */
  uint8_t getSmlListLength(const unsigned char *buffer, const int position);

  /* @brief Get SMl time stamp value
   * @param buffer Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the list
   * @return SMl time stamp as uint32_t
   */
  SmlTime getSmlTime(const unsigned char *buffer, const int buffer_size,
                     int &position);

  /* @brief Gets a SML status as uint64_t from vector of chars
   * @param buffer Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Unsigned64
   * @return 0xFFFFFFFFFFFFFFFF on error
   */
  uint64_t getSmlStatus(const unsigned char *buffer, const int buffer_size,
                        int &position);

  /* @brief Gets a Unsigned (8/16/32/64) as uint64_t from vector of chars
   * @param buffer Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Unsigned64
   * @return 0xFFFFFFFFFFFFFFFF on error
   */
  uint64_t getUnsigned(const unsigned char *buffer, const int buffer_size,
                       int &position);

  /* @brief Gets a Integer(8/16/32/64) as int64_t from vector of chars
   * @param buffer Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @return Integer64
   * @return 0xFFFFFFFFFFFFFFFF on error
   */
  int64_t getInteger(const unsigned char *buffer, const int buffer_size,
                     int &position);

  /* @brief returns a SML octet string as std::string
   * @param buffer Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @param The length of the octet string to return
   * @return std::string
   */
  std::string getOctetString(const unsigned char *buffer, const int buffer_size,
                             int &position, const int length);

  /* @brief returns a SML octet string as std::string
   * @param buffer Pointer to a array of unsigned char
   * @param buffer_size Size of the buffer to lex
   * @param position Pointer to the position of the octet string
   * @param The length of the octet string to return
   * @return std::string
   */
  std::string getExtendedOctetString(const unsigned char *buffer,
                                           const int buffer_size, int &position,
                                           int length);
};

#endif // SML_LEXER_HPP