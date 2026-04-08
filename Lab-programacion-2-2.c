/*
 * main.c
 * Proyecto-programacion-laboratorio-intento-ayudado-de-gemini
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

int main() {
    int tcs_fd;
    int tms_fd;
    uint8_t read_byte;
    uint8_t ntcs;
    uint8_t tc = 0;
    uint16_t tm_count = 0; // Contador de telemetrías enviadas

    // 1. Abrimos los dos archivos (entrada y salida)
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

    // Leemos el primer byte para saber cuántos telecomandos hay
    read(tcs_fd, &read_byte, 1);
    ntcs = read_byte;

    // 2. Bucle principal
    while (tc < ntcs) {
        uint8_t source_ID;
        uint16_t tc_packet_id;
        uint16_t tc_packet_seq_ctrl;
        uint16_t tc_packet_err_ctrl;
        uint16_t tc_packet_len;
        uint32_t tc_df_header;
        uint8_t tc_bytes[256];
        uint16_t i = 0;
        uint16_t nbytes = 0;

        printf("\n--- Procesando Telecomando %d de %d ---\n", tc + 1, ntcs);

        // --- LECTURA DEL TELECOMANDO ---
        read(tcs_fd, &read_byte, 1); tc_bytes[0] = read_byte; tc_packet_id = read_byte << 8;
        read(tcs_fd, &read_byte, 1); tc_bytes[1] = read_byte; tc_packet_id = tc_packet_id | read_byte;

        read(tcs_fd, &read_byte, 1); tc_bytes[2] = read_byte; tc_packet_seq_ctrl = read_byte << 8;
        read(tcs_fd, &read_byte, 1); tc_bytes[3] = read_byte; tc_packet_seq_ctrl = tc_packet_seq_ctrl | read_byte;

        read(tcs_fd, &read_byte, 1); tc_bytes[4] = read_byte; tc_packet_len = read_byte << 8;
        read(tcs_fd, &read_byte, 1); tc_bytes[5] = read_byte; tc_packet_len = tc_packet_len | read_byte;

        read(tcs_fd, &read_byte, 1); tc_bytes[6] = read_byte; tc_df_header = read_byte << 24;
        read(tcs_fd, &read_byte, 1); tc_bytes[7] = read_byte; tc_df_header = tc_df_header | (read_byte << 16);
        read(tcs_fd, &read_byte, 1); tc_bytes[8] = read_byte; tc_df_header = tc_df_header | (read_byte << 8);
        read(tcs_fd, &read_byte, 1); tc_bytes[9] = read_byte; tc_df_header = tc_df_header | read_byte;
        source_ID = read_byte; // El Destination ID de la TM será este source_ID

        // Leer datos de la aplicación
        while (i < tc_packet_len - 5) {
            read(tcs_fd, &read_byte, 1);
            tc_bytes[10+i] = read_byte;
            i = i + 1;
        }

        read(tcs_fd, &read_byte, 1); tc_packet_err_ctrl = read_byte << 8;
        read(tcs_fd, &read_byte, 1); tc_packet_err_ctrl = tc_packet_err_ctrl | read_byte;

        // --- CÁLCULO DEL CRC ---
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

        // --- ESCRITURA DE LA TELEMETRÍA (PARTE 2.2) ---
        uint8_t byte_to_write;
        uint16_t tm_packet_id = 0x0B2C;
        uint16_t tm_seq_ctrl = 0xC000 | (tm_count & 0x3FFF); // Combinamos Flags con nuestro contador

        if (crc_value == tc_packet_err_ctrl) {
            printf("Expected CRC: 0x%X, Calculated CRC: 0x%X -> OK\n", tc_packet_err_ctrl, crc_value);
            
            // --- TELEMETRÍA DE ACEPTACIÓN ---
            // 1. Packet ID (0x0B2C)
            byte_to_write = tm_packet_id >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tm_packet_id & 0xFF; write(tms_fd, &byte_to_write, 1);
            
            // 2. Sequence Control
            byte_to_write = tm_seq_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tm_seq_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);
            
            // 3. Length (0x0007)
            byte_to_write = 0x00; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x07; write(tms_fd, &byte_to_write, 1);
            
            // 4. Data Field Header (Subtype 1)
            byte_to_write = 0x10; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x01; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x01; write(tms_fd, &byte_to_write, 1);
            byte_to_write = source_ID; write(tms_fd, &byte_to_write, 1);
            
            // 5. Source Data (Solo ID y Seq)
            byte_to_write = tc_packet_id >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_id & 0xFF; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_seq_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_seq_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);

        } else {
            printf("Expected CRC: 0x%X, Calculated CRC: 0x%X -> FAIL\n", tc_packet_err_ctrl, crc_value);
            
            // --- TELEMETRÍA DE RECHAZO ---
            // 1. Packet ID (0x0B2C)
            byte_to_write = tm_packet_id >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tm_packet_id & 0xFF; write(tms_fd, &byte_to_write, 1);
            
            // 2. Sequence Control
            byte_to_write = tm_seq_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tm_seq_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);
            
            // 3. Length (0x000D -> Porque es más largo)
            byte_to_write = 0x00; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x0D; write(tms_fd, &byte_to_write, 1);
            
            // 4. Data Field Header (Subtype 2)
            byte_to_write = 0x10; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x01; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x02; write(tms_fd, &byte_to_write, 1);
            byte_to_write = source_ID; write(tms_fd, &byte_to_write, 1);
            
            // 5. Source Data (ID, Seq, Error, CRC rx, CRC calc)
            byte_to_write = tc_packet_id >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_id & 0xFF; write(tms_fd, &byte_to_write, 1);
            
            byte_to_write = tc_packet_seq_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_seq_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);
            
            // Error Code 2
            byte_to_write = 0x00; write(tms_fd, &byte_to_write, 1);
            byte_to_write = 0x02; write(tms_fd, &byte_to_write, 1);
            
            // Received CRC
            byte_to_write = tc_packet_err_ctrl >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = tc_packet_err_ctrl & 0xFF; write(tms_fd, &byte_to_write, 1);
            
            // Calculated CRC
            byte_to_write = crc_value >> 8; write(tms_fd, &byte_to_write, 1);
            byte_to_write = crc_value & 0xFF; write(tms_fd, &byte_to_write, 1);
        }

        // Incrementamos contadores al final de cada vuelta
        tm_count = tm_count + 1; 
        tc = tc + 1;
    }

    // 3. Limpiamos y cerramos
    close(tcs_fd);
    close(tms_fd);
    
    printf("\n--- PROCESO TERMINADO --- \nArchivo 'multiple-tms.bin' generado con exito.\n");

    return 0;
}