#ifndef _SML_PARSER_HPP_
#define _SML_PARSER_HPP_

#include "SmlListEntry.hpp"
#include <fstream>
#include <iterator>
#include <stdlib.h> 
#include <vector>

enum sml_error_t {
    SML_OK,
    SML_ERROR_HEADER,
    SML_ERROR_SYNTAX,
    SML_ERROR_NULLPTR,
    SML_ERROR_ZEROLENGTH,
    SML_ERROR_SIZE,
};

const uint16_t SML_MSG_TYPE_PUBOPEN_RES = 0x0101;
const uint16_t SML_MSG_TYPE_GETLIST_RES = 0x0701;

/* @brief Checks if a vector element is a SML Octet string
 * @param element The element to check as char
 * @return true if element is a SML octet string
 * @return false if element is not a SML octet string
 */
bool isOctetString( const unsigned char element);

/* @brief Gets the length of an octet string
 * @param element The element to check as char
 * @return The length of an actet string as int
 */
int getOctetStringLength(const unsigned char element);

/* @brief Gets the octet string as char*
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @param octetstring Pointer to the octete string
 * @param length The length of the octet string 
 */
sml_error_t getOctetStringAsVector(const std::vector<unsigned char> *data, int position, std::vector<unsigned char> *octetstring, int length);

/* @brief Checks if a vector element is a Unsigned8
 * @param element The element to check as char
 * @return true if element is a Unsigned8
 * @return false if element is not an Unsigned8
 */
bool isUnsigned8(const unsigned char element);

/* @brief Gets a SML Unsigned8 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned8 
 * @return 0xFF on error
 */
uint8_t getUnsigned8(const std::vector<unsigned char> *data, const int position);

/* @brief Gets a SML Unsigned16 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned16 
 * @return 0xFFFF on error
 */
uint16_t getUnsigned16(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML Unsigned32 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned32 
 * @return 0xFFFFFFFF on error
 */
uint32_t getUnsigned32(const std::vector<unsigned char> *data, const int position);

/* @brief Get number of elements in a SML list
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the list
 * @return number of list elements as uint8_t
 * @return 0xFF in case of an error
 */
uint8_t getSmlListLength(const std::vector<unsigned char> *data, const int position);

/* @brief Get SMl time stamp value
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the list
 * @return SMl time stamp as uint32_t
 */
uint32_t getSmlTime(const std::vector<unsigned char> *data, int &position);

#endif // _SML_PARSER_HPP_