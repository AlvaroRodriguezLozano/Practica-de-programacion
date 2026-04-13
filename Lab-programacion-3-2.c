#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "basic_types.h"
#include "ccsds_pus_format.h"
#include "ccsds_pus_stdio.h"
#include "serialize.h"
#include "crc.h"

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

    for (uint8_t tc = 0; tc < ntcs; tc = tc + 1) {
                               
    uint16_t tc_packet_id;
    uint16_t tc_packet_seq_ctrl;
    uint16_t tc_packet_len;
    uint32_t tc_df_header;
    uint16_t tc_packet_err_ctrl;

    uint16_t crc_value;
    uint8_t nbytes = 0;

    uint8_t tc_bytes[256];

    // Read telecommand from file
    nbytes = ccsds_pus_tc_read(tcs_fd, tc_bytes);
    
    // Deserialize primary fields
    ccsds_pus_tc_get_fields(tc_bytes, &tc_packet_id,
            &tc_packet_seq_ctrl,
            &tc_packet_len,
            &tc_df_header,
            &tc_packet_err_ctrl);

    // Calculate CRC
    // We need to calculate the CRC with nbytes - 2, since the vector
    // ALSO STORES the Packet Error Control field
    crc_value = cal_crc_16(tc_bytes, nbytes - 2);
    
    // KEEP THE REST OF THE CODE AS IS
    //
    //
        // Tus variables calculadas
       // Llamamos a la función externa para obtener el APID
        ccsds_pus_tmtc_print_packet_header_fields(tc_packet_id);
        ccsds_pus_tmtc_print_packet_seq_ctrl_fields(tc_packet_seq_ctrl);
        ccsds_pus_tc_print_df_header_fields(tc_df_header);
        uint8_t source_ID = ccsds_pus_tc_get_sourceID(tc_df_header);

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
    }
    close(tcs_fd);
    close(tms_fd);
    return 0;
}