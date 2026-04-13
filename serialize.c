#include "serialize.h"

uint16_t deserialize_uint16(uint8_t data_bytes[]) {

    uint16_t var;
	
    var = data_bytes[0];
    var = var << 8;
    var = var | data_bytes[1];
	
    return var;

}
    
// TODO: Define function deserialize_uint32

uint32_t deserialize_uint32(uint8_t data_bytes[]) {

    uint32_t var;
	
    var = data_bytes[0];
    var = var << 8;
    var = var | data_bytes[1];
    var = var << 8;
    var = var | data_bytes[2];
    var = var << 8;
    var = var | data_bytes[3];

    return var;

}