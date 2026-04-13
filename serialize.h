#ifndef INCLUDE__SERIALIZE_H_
#define INCLUDE__SERIALIZE_H_

#include "basic_types.h"

/**
 * \brief Deserializes a 16-bit word
 *
 * \param data_bytes vector containing the MSB and LSB bytes of the word
 *
 * \return the deserialized 16-bit word
 */
uint16_t deserialize_uint16(uint8_t data_bytes[]);

/**
 * \brief Deserializes a 32-bit word
 *
 * \param data_bytes vector containing the 4 bytes of the word
 *
 * \return the deserialized 32-bit word
 */
// TODO: declare function deserialize_uint32
uint32_t deserialize_uint32(uint8_t data_bytes[]);
#endif
