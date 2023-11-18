#include <stdint.h>

/** @brief Calculates the crc checksum of a string and returns it
 *  @param cp The string to compute the crc of as unsigned char*
 *  @param len The length of the string which should be used for calculating the crc as int
 *  @return the computed crc as uint16_t
*/
uint16_t sml_crc16(unsigned char *cp, int len);
