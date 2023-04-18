#ifndef SML_PARSER_HPP_
#define SML_PARSER_HPP_

#include "SmlFile.hpp"
#include "SmlMessage.hpp"
#include "SmlMessageBody.hpp"
#include "SmlTypes.hpp"
#include <fstream>
#include <iterator>
#include <stdlib.h> 
#include <vector>
#include "SmlLogger.hpp"

/* @brief Main parsing function
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return SML_OK on success
 * @return SML_NOK on failure
 */
SmlFile parseSml(const std::vector<unsigned char> *buffer);

/* @brief Checks if a vector element is a SML Octet string
 * @param element The element to check as char
 * @return true if element is a SML octet string
 * @return false if element is not a SML octet string
 */
bool isOctetString( const unsigned char element);

/* @brief Gets the length of an octet string
 * @param element The element to check as char
 * @return The length of an octet string as int
 */
int getOctetStringLength(const unsigned char element);

/* @brief Gets the length of an extended octet string
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return The length of an extended octet string as int
 */
bool isExtendedOctetString(const unsigned char element);

/* @brief Gets the length of a octet string with extended Type-Length field
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return The length of the octet string 
 */
uint32_t getExtendedOctetStringLength(const std::vector<unsigned char> *buffer, int &position);

/* @brief Gets the octet string as char*
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @param octetstring Pointer to the octete string
 * @param length The length of the octet string 
 */
sml_error_t getOctetStringAsVector(const std::vector<unsigned char> *data, int position, std::vector<unsigned char> *octetstring, int length);

/* @brief Gets the extended octet string as char*
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @param octetstring Pointer to the octete string
 * @param length The length of the octet string 
 */
sml_error_t getExtendedOctetStringAsVector(const std::vector<unsigned char> *data, int &position, std::vector<unsigned char> *octetstring, int length);

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
uint8_t getUnsigned8(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML Signed8 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Signed8 
 * @return 0xFF on error
 */
int8_t getInteger8(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML Unsigned16 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned16 
 * @return 0xFFFF on error
 */
uint16_t getUnsigned16(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML Integer16 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Integer16 
 * @return 0xFFFF on error
 */
int16_t getInteger16(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML Unsigned32 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned32 
 * @return 0xFFFFFFFF on error
 */
uint32_t getUnsigned32(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML Integer32 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Integer32 
 * @return 0xFFFFFFFF on error
 */
uint32_t getUnsigned32(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML Unsigned64 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned64 
 * @return 0xFFFFFFFFFFFFFFFF on error
 */
uint64_t getUnsigned64(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML Integer64 from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Integer64 
 * @return 0xFFFFFFFFFFFFFFFF on error
 */
uint64_t getUnsigned64(const std::vector<unsigned char> *data, int &position);

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
SmlTime getSmlTime(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a SML status as uint64_t from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned64 
 * @return 0xFFFFFFFFFFFFFFFF on error
 */
uint64_t getSmlStatus(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a Unsigned (8/16/32/64) as uint64_t from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Unsigned64 
 * @return 0xFFFFFFFFFFFFFFFF on error
 */
uint64_t getUnsigned(const std::vector<unsigned char> *data, int &position);

/* @brief Gets a Integer(8/16/32/64) as int64_t from vector of chars
 * @param data Pointer to a vector of char
 * @param position Pointer to the position of the octet string
 * @return Integer64 
 * @return 0xFFFFFFFFFFFFFFFF on error
 */
int64_t getInteger(const std::vector<unsigned char> *data, int &position);

/* @brief Parses a SML PublicOpen.Res message
 * @param buffer A pointer to a std::vector<char> buffer
 * @param position the positon in the buffer where to start parsing
 * @return SmlPublicOpenRes
 */
SmlPublicOpenRes parseSmlPublicOpenRes(const std::vector<unsigned char> *buffer,
                                  int &position);

/* @brief Parses a SML PublicClose.Res message
 * @param buffer A pointer to a std::vector<char> buffer
 * @param position the positon in the buffer where to start parsing
 * @return SmlPublicCloseRes message
 */
SmlPublicCloseRes parseSmlPublicCloseRes(const std::vector<unsigned char> *buffer,
                                  int &position);                            

/* @brief Parses a SML GetList.Res message
 * @param buffer A pointer to a std::vector<unsigned char> buffer
 * @param position the positon in the buffer where to start parsing
 * @return SmlGetListRes
 */
SmlGetListRes parseSmlGetListRes(const std::vector<unsigned char> *buffer,
                               int &position);

/* @brief Parses a SML input stream as std::vector<unsigned char> for
 * SMLListEntries
 * @param buffer The input stream as std::vector<unsigned char>
 * @param position the position pointer wehere to start parsing
 * @return SmlListEntry
 */
SmlListEntry parseSmlListEntry(const std::vector<unsigned char> *buffer,
                               int &position);


sml_error_t parseEscapeSequence(const std::vector<unsigned char> *buffer,
                               int &position);

void printCharVector(std::vector<unsigned char> v);

/* @brief Merges the elements of an vectior<unsigned char> to a string
 * @param the_vector The vector to be converted to a string 
 * @return The elements of the inut vector as string
 */
std::string vectorToString(std::vector<unsigned char> *the_vector);

void hexPrint(const std::vector<unsigned char> *buffer, int &position);



#endif // SML_PARSER_HPP_