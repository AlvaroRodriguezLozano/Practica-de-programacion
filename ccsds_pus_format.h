#ifndef INCLUDE_CCSDS_PUS_FORMAT_H_
#define INCLUDE_CCSDS_PUS_FORMAT_H_

#include "basic_types.h"

/**
 * \brief Get APID from a telecommand's Packet ID.
 *
 * \param tc_packet_id the telecommand's Packet ID
 *
 * \return the telecommand's APID 
 */
uint16_t ccsds_pus_tc_get_APID(uint16_t tc_packet_id);

/**
 * \brief Get Sequence Count from a telecommand's Packet Sequence Control.
 *
 * \param tc_packet_seq_ctrl the telecommand's Packet Sequence Control
 *
 * \return the telecommand's Sequence Flags
 */
// TODO: Declare function ccsds_pus_tc_get_seq_flags
uint8_t ccsds_pus_tc_get_seq_flags(uint16_t tc_packet_seq_ctrl);
/**
 * \brief Get Sequence Count from a telecommand's Packet Sequence Control.
 *
 * \param tc_packet_seq_ctrl the telecommand's Packet Sequence Control
 *
 * \return the telecommand's Sequence Count
 */
// TODO: Declare function ccsds_pus_tc_get_seq_count
uint16_t ccsds_pus_tc_get_seq_count(uint16_t tc_packet_seq_ctrl);
/**
 * \brief Get Ack from a telecommand's Data Field Header.
 *
 * \param tc_df_header the telecommand's Data Field Header
 *
 * \return the telecommand's Ack
 */
// TODO: Declare function ccsds_pus_tc_get_ack
uint16_t ccsds_pus_tc_get_ack(uint32_t data_field_header);
/**
 * \brief Get Service Type from a telecommand's Data Field Header.
 *
 * \param tc_df_header the telecommand's Data Field Header
 *
 * \return the telecommand's Service Type
 */
// TODO: Declare function ccsds_pus_tc_get_type
uint16_t ccsds_pus_tc_get_type(uint32_t data_field_header);
/**
 * \brief Get Service Subtype from a telecommand's Data Field Header.
 *
 * \param tc_df_header the telecommand's Data Field Header
 *
 * \return the telecommand's Service Subtype
 */
// TODO: Declare function ccsds_pus_tc_get_subtype
uint8_t ccsds_pus_tc_get_subtype(uint32_t data_field_header);
/**
 * \brief Get Source ID from a telecommand's Data Field Header.
 *
 * \param tc_df_header the telecommand's Data Field Header
 *
 * \return the telecommand's Source ID
 */
// TODO: Declare function ccsds_pus_tc_get_sourceID
uint8_t ccsds_pus_tc_get_sourceID(uint32_t data_field_header);

#endif /* INCLUDE_CCSDS_PUS_FORMAT_H_ */
