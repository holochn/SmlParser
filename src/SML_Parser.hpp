#ifndef _SML_PARSER_HPP_
#define _SML_PARSER_HPP_

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

/* @brief Checks if a vector element is a SML Octet string
 * @param element The element to check as char
 * @return true if element is a SML octet string
 * @return false if element is not a SML octet string
 */
bool isOctetString( const char element);

/* @brief Gets the length of an octet string
 * @param element The element to check as char
 * @return The length of an actet string as int
 */
int getOctetStringLength(const char element);

/* @brief Gets the octet string as char*
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @param octetstring Pointer to the octete string
 * @param length The length of the octet string 
 */
sml_error_t getOctetStringAsVector(const std::vector<char> *data, int position, std::vector<char> *octetstring, int length);

/* @brief Checks if a vector element is a Unsigned8
 * @param element The element to check as char
 * @return true if element is a Unsigned8
 * @return false if element is not an Unsigned8
 */
bool isUnsigned8(const char element);

/* @brief Gets a SML Unsigned8 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned8
 */
uint8_t getUnsigned8(const std::vector<char> *data, const int position);

#endif // _SML_PARSER_HPP_