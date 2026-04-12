#include <stdio.h>
#include "basic_types.h"
#include "ccsds_pus_stdio.h"
#include "ccsds_pus_format.h"

void ccsds_pus_tmtc_print_packet_header_fields(uint16_t tc_packet_id) {

	printf("APID: 0x%X\n", ccsds_pus_tc_get_APID(tc_packet_id));

}

// TODO: Define the remaining functions

void ccsds_pus_tmtc_print_packet_seq_ctrl_fields(uint16_t tc_packet_seq_ctrl){

    printf("Secuence flags: 0x%X\n", ccsds_pus_tc_get_seq_flags(tc_packet_seq_ctrl));
    printf("Secuence count: %d\n", ccsds_pus_tc_get_seq_count(tc_packet_seq_ctrl));
}

void ccsds_pus_tc_print_df_header_fields(uint32_t data_field_header){
   
    printf("ACK: 0x%X\n", ccsds_pus_tc_get_ack(data_field_header));
    printf("Service type: %d\n", ccsds_pus_tc_get_type(data_field_header));
    printf("Service subtype: %d\n", ccsds_pus_tc_get_subtype(data_field_header));
    printf("Source ID: 0x%X\n", ccsds_pus_tc_get_sourceID(data_field_header));
}
