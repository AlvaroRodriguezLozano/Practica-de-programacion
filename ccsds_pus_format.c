#include "basic_types.h"
#include "ccsds_pus_format.h"
    
uint16_t ccsds_pus_tc_get_APID(uint16_t tc_packet_id) {

    return (tc_packet_id & 0x07FF);
    
}

uint8_t ccsds_pus_tc_get_seq_flags(uint16_t tc_packet_seq_ctrl) {

    uint8_t sec_flags = tc_packet_seq_ctrl >> 14;
    sec_flags = sec_flags & 0x3;
    return (sec_flags);
}

uint16_t ccsds_pus_tc_get_seq_count(uint16_t tc_packet_seq_ctrl) {

    uint16_t sec_count = tc_packet_seq_ctrl & 0x3FFF;
    return (sec_count);
}

uint16_t ccsds_pus_tc_get_ack(uint32_t data_field_header) {

    uint32_t ack =  data_field_header >> 24;
    ack = ack & 0xF;
    return(ack);
}

uint16_t ccsds_pus_tc_get_type(uint32_t data_field_header) {

    uint32_t service_type = data_field_header >> 16;
    service_type = service_type & 0xFF;
    return(service_type);
}

uint8_t ccsds_pus_tc_get_subtype(uint32_t data_field_header){

    uint8_t service_subtype = data_field_header >>8;
    return(service_subtype);

}

uint8_t ccsds_pus_tc_get_sourceID(uint32_t data_field_header){

    uint8_t source_ID = data_field_header;
    return(source_ID);

}