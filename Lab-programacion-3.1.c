#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "basic_types.h"
#include "ccsds_pus_format.h"
#include "ccsds_pus_stdio.h"

int main() {
    int tcs_fd;
    int tms_fd;
    uint8_t read_byte;
    uint8_t ntcs;
    uint8_t tc = 0;
    uint16_t tm_count = 0;

    tcs_fd = open("multiple-tcs.bin", O_RDONLY);
    if (tcs_fd == -1) {
        printf("ERROR: No encuentro el archivo multiple-tcs.bin.\n");
        return 1;
    }

    tms_fd = open("multiple-tms.bin", O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (tms_fd == -1) {
        printf("ERROR: No he podido crear el archivo multiple-tms.bin.\n");
        close(tcs_fd);
        return 1;
    }

    read(tcs_fd, &read_byte, 1);
    ntcs = read_byte;

    while (tc < ntcs) {
        uint16_t tc_packet_id;
        uint16_t tc_packet_seq_ctrl;
        uint16_t tc_packet_err_ctrl;
        uint16_t tc_packet_len;
        uint32_t tc_df_header;

        uint16_t APID;
        uint8_t sec_flags;
        uint16_t sec_count;
        uint8_t ACK;
        uint8_t service_type;
        uint8_t service_subtype;
        uint8_t source_ID;

        uint8_t tc_bytes[256];
        uint16_t i = 0;
        uint16_t nbytes = 0;

        // primera parte
        read(tcs_fd, &read_byte, 1);
        tc_bytes[0] = read_byte;
        tc_packet_id = read_byte;
        tc_packet_id = tc_packet_id << 8;

        read(tcs_fd, &read_byte, 1);
        tc_bytes[1] = read_byte;
        tc_packet_id = tc_packet_id | read_byte;

        printf("\nPacket ID: 0x%X\n", tc_packet_id);

        // segunda parte
        read(tcs_fd, &read_byte, 1);
        tc_bytes[2] = read_byte;
        tc_packet_seq_ctrl = read_byte;
        tc_packet_seq_ctrl = tc_packet_seq_ctrl << 8;

        read(tcs_fd, &read_byte, 1);
        tc_bytes[3] = read_byte;
        tc_packet_seq_ctrl = tc_packet_seq_ctrl | read_byte;

        printf("Packet Sequence Control: 0x%X\n", tc_packet_seq_ctrl);

        // tercera parte
        read(tcs_fd, &read_byte, 1);
        tc_bytes[4] = read_byte;
        tc_packet_len = read_byte;
        tc_packet_len = tc_packet_len << 8;

        read(tcs_fd, &read_byte, 1);
        tc_bytes[5] = read_byte;
        tc_packet_len = tc_packet_len | read_byte;

        printf("Packet Length: 0x%X\n", tc_packet_len);

        // cuarta parte
        read(tcs_fd, &read_byte, 1);
        tc_bytes[6] = read_byte;
        tc_df_header = read_byte;
        tc_df_header = tc_df_header << 8;

        read(tcs_fd, &read_byte, 1);
        tc_bytes[7] = read_byte;
        tc_df_header = tc_df_header | read_byte;
        tc_df_header = tc_df_header << 8;

        read(tcs_fd, &read_byte, 1);
        tc_bytes[8] = read_byte;
        tc_df_header = tc_df_header | read_byte;
        tc_df_header = tc_df_header << 8;

        read(tcs_fd, &read_byte, 1);
        tc_bytes[9] = read_byte;
        tc_df_header = tc_df_header | read_byte;
        
        printf("Data Field Header: 0x%X\n", tc_df_header);

        // App data
        while (i < tc_packet_len - 5) {
            read(tcs_fd, &read_byte, 1);
            tc_bytes[10+i] = read_byte;
            i = i + 1;
        }

        // quinta parte
        read(tcs_fd, &read_byte, 1);
        tc_packet_err_ctrl = read_byte;
        tc_packet_err_ctrl = tc_packet_err_ctrl << 8;

        read(tcs_fd, &read_byte, 1);
        tc_packet_err_ctrl = tc_packet_err_ctrl | read_byte;

        printf("Packet Error Control: 0x%X\n", tc_packet_err_ctrl);

        // Algoritmo para calcular el CRC
        uint16_t crc_value = 0xFFFF;
        nbytes = 5 + tc_packet_len;
        for (uint16_t j = 0; j < nbytes; j++ ) {
            crc_value = crc_value ^ (tc_bytes[j] << 8);
            for (uint8_t k = 0; k < 8; k++) {
                if ((crc_value & 0x8000) != 0) {
                    crc_value = (crc_value << 1) ^ 0x1021;
                } else {
                    crc_value = crc_value << 1;
                }
            }
        }

        // Tus variables calculadas
       // Llamamos a la función externa para obtener el APID
        ccsds_pus_tmtc_print_packet_header_fields(tc_packet_id);

        ccsds_pus_tmtc_print_packet_seq_ctrl_fields(tc_packet_seq_ctrl);

        ccsds_pus_tc_print_df_header_fields(tc_df_header);

        source_ID = ccsds_pus_tc_get_sourceID(tc_df_header);

        // --- ESCRITURA DE LA TELEMETRÍA ---
        uint8_t byte_to_write;
        uint16_t tm_packet_id = 0x0B2C;
        uint16_t tm_seq_ctrl = 0xC000 | (tm_count & 0x3FFF);

        if (crc_value == tc_packet_err_ctrl) {
            printf("Expected CRC value 0x%X, Calculated CRC value 0x%X: OK\n\n", tc_packet_err_ctrl, crc_value);

            // Telemetría Aceptación
            byte_to_write = tm_packet_id >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tm_packet_id & 0xFF; write(tms_fd, &byte_to_write, 1);

            byte_to_write = tm_seq_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tm_seq_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);

            byte_to_write = 0x00; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x07; write(tms_fd, &byte_to_write, 1);

            byte_to_write = 0x10; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x01; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x01; write(tms_fd, &byte_to_write, 1);
            byte_to_write = source_ID; write(tms_fd, &byte_to_write, 1);

            byte_to_write = tc_packet_id >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_id & 0xFF; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_seq_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_seq_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);

        } else {
            printf("Expected CRC value 0x%X, Calculated CRC value 0x%X: FAIL\n\n", tc_packet_err_ctrl, crc_value);

            // Telemetría Rechazo
            byte_to_write = tm_packet_id >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tm_packet_id & 0xFF; write(tms_fd, &byte_to_write, 1);

            byte_to_write = tm_seq_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tm_seq_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);

            byte_to_write = 0x00; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x0D; write(tms_fd, &byte_to_write, 1);

            byte_to_write = 0x10; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x01; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x02; write(tms_fd, &byte_to_write, 1);
            byte_to_write = source_ID; write(tms_fd, &byte_to_write, 1);

            byte_to_write = tc_packet_id >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_id & 0xFF; write(tms_fd, &byte_to_write, 1);

            byte_to_write = tc_packet_seq_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_seq_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);

            byte_to_write = 0x00; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x02; write(tms_fd, &byte_to_write, 1);

            byte_to_write = tc_packet_err_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_err_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);

            byte_to_write = crc_value >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = crc_value & 0xFF; write(tms_fd, &byte_to_write, 1);
        }

        tm_count = tm_count + 1;
        tc = tc + 1;
    }
    close(tcs_fd);
    close(tms_fd);
    return 0;
}