/*
 * main.c
 *
 *  Created on: Feb 11, 2026
 *      Author: atcsol
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;


int main() {

	int tc_fd;
	uint8_t read_byte;



	uint8_t ntcs;
	uint8_t tc = 0;
	uint8_t source_ID;
	uint16_t tc_packet_id;
	uint16_t tc_packet_seq_ctrl;
	uint16_t tc_packet_err_ctrl;

	/*tc_fd = open("single-tc.bin", O_RDONLY);*/
	tc_fd = open("multiple-tcs.bin", O_RDONLY);

	read(tc_fd, &read_byte, 1);
	ntcs = read_byte;
	// Read Packet ID and store it into tc_packet_id
	while (tc < ntcs) {


				uint16_t tc_packet_len;
		uint32_t tc_df_header;

		uint16_t APID;
		uint8_t sec_flags;
		uint16_t sec_count;
		uint8_t ACK;
		uint8_t service_type;
		uint8_t service_subtype;

		uint8_t tc_bytes[256];

		uint16_t i = 0;
		uint16_t nbytes = 0;

		read(tc_fd, &read_byte, 1);        // Read MSB
		tc_bytes[0] = read_byte;
		tc_packet_id = read_byte;
		tc_packet_id = tc_packet_id << 8;  // Shift it 8 bits to the left

		read(tc_fd, &read_byte, 1);               // Read LSB
		tc_bytes[1] = read_byte;
		tc_packet_id = tc_packet_id | read_byte;  // OR the MSB and the LSB

		printf("Packet ID: 0x%X\n", tc_packet_id);

		// segunda parte
		read(tc_fd, &read_byte, 1);        // Read MSB
		tc_bytes[2] = read_byte;
		tc_packet_seq_ctrl = read_byte;
		tc_packet_seq_ctrl = tc_packet_seq_ctrl << 8;  // Shift it 8 bits to the left

		read(tc_fd, &read_byte, 1);               // Read LSB
		tc_bytes[3] = read_byte;
		tc_packet_seq_ctrl = tc_packet_seq_ctrl | read_byte;  // OR the MSB and the LSB

		printf("Packet Sequence Control: 0x%X\n", tc_packet_seq_ctrl);

		// tercera parte
		read(tc_fd, &read_byte, 1);        // Read MSB
		tc_bytes[4] = read_byte;
		tc_packet_len = read_byte;
		tc_packet_len = tc_packet_len << 8;  // Shift it 8 bits to the left

		read(tc_fd, &read_byte, 1);               // Read LSB
		tc_bytes[5] = read_byte;
		tc_packet_len = tc_packet_len | read_byte;  // OR the MSB and the LSB

		printf("Packet Length: 0x%X\n", tc_packet_len);

		// cuarta parte
		read(tc_fd, &read_byte, 1);        // Read MSB
		tc_bytes[6] = read_byte;
		tc_df_header = read_byte;
		tc_df_header = tc_df_header << 8;  // Shift it 8 bits to the left

		read(tc_fd, &read_byte, 1);               // Read LSB
		tc_bytes[7] = read_byte;
		tc_df_header = tc_df_header | read_byte;  // OR the MSB and the LSB
		tc_df_header = tc_df_header << 8;  // Shift it 8 bits to the left

		read(tc_fd, &read_byte, 1);               // Read LSB
		tc_bytes[8] = read_byte;
		tc_df_header = tc_df_header | read_byte;  // OR the MSB and the LSB
		tc_df_header = tc_df_header << 8;  // Shift it 8 bits to the left

		read(tc_fd, &read_byte, 1);               // Read LSB
		tc_bytes[9] = read_byte;
		tc_df_header = tc_df_header | read_byte;  // OR the MSB and the LSB
		printf("Data Field Header: 0x%X\n", tc_df_header);


		//El while para guardar en el array el app data

		while (i < tc_packet_len - 5) {

			// TODO: Read byte from file
			read(tc_fd, &read_byte, 1);
			// TODO: Store byte into tc_bytes[10+i]
			tc_bytes[10+i] = read_byte;
			// TODO: Increment index variable (i = i + 1)
			i=i+1;

		}

		// quinta parte
		read(tc_fd, &read_byte, 1);        // Read MSB
		tc_packet_err_ctrl = read_byte;
		tc_packet_err_ctrl = tc_packet_err_ctrl << 8;  // Shift it 8 bits to the left

		read(tc_fd, &read_byte, 1);               // Read LSB
		tc_packet_err_ctrl = tc_packet_err_ctrl | read_byte;  // OR the MSB and the LSB

		printf("Packet Error Control: 0x%X\n", tc_packet_err_ctrl);

		//Algoritmo para calcular el CRC
		uint16_t crc_value = 0xFFFF;
		nbytes = 5+tc_packet_len;
		for (uint16_t i = 0; i < nbytes; i++ ) {

			crc_value = crc_value ^ (tc_bytes[i] << 8);

			for (uint8_t j = 0; j < 8; j++) {

				if ((crc_value & 0x8000) != 0) {
					crc_value = (crc_value << 1) ^ 0x1021;
				} else {
					crc_value = crc_value << 1;
				}
			}
		}

		// Segunda parte del trabajo (Practica 1º, parte 2)
		APID = tc_packet_id << 5;
		APID = APID >> 5;
		printf("APID: 0x%X\n", APID);

		sec_flags = tc_packet_seq_ctrl >>14;
		printf("Secuence flags: 0x%X\n", sec_flags);

		sec_count = tc_packet_seq_ctrl <<2;
		sec_count = sec_count >>2;
		printf("Secuence count: %X\n", sec_count);

		ACK = tc_df_header >>24;
		ACK = ACK <<4;
		ACK = ACK >>4;

		printf("ACK: 0x%X\n", ACK);

		service_type = tc_df_header >>16;
		printf("Service type: %d\n", service_type);

		service_subtype = tc_df_header >>8;
		printf("Service subtype: %d\n", service_subtype);

		source_ID = tc_df_header;
		printf("Source ID: 0x%X\n", source_ID);

		if (crc_value == tc_packet_err_ctrl)
			printf("Expected CRC value 0x%X, Calculated CRC value 0x%X: OK\n",
					tc_packet_err_ctrl, crc_value);
		else
			printf("Expected CRC value 0x%X, Calculated CRC value 0x%X: FAIL\n",
					tc_packet_err_ctrl, crc_value);



		// Aqui empieza la práctica de varios telecomandos//


		// TODO: Increment index variable (tc = tc + 1)
		tc = tc + 1;

	}

	close(tc_fd);
	return 0;




	//Parte corespondiente a escribir un telecomando

    int tm_fd;

    uint8_t byte_to_write;

    uint16_t tm_packet_id = 0;

    tm_fd = open("single-tm.bin", O_WRONLY | O_CREAT | O_TRUNC, 0664);

    // Set Data Field Header Flag
    tm_packet_id = tm_packet_id | (1 << 11);

    // Set APID
    tm_packet_id = tm_packet_id | (0x32C);

    // Write MSB of TM Packet ID
    byte_to_write = (tm_packet_id & 0xFF00) >> 8;
    write(tm_fd, &byte_to_write, 1);

    // Write LSB of TM Packet ID
    byte_to_write = (tm_packet_id & 0x00FF);
    write(tm_fd, &byte_to_write, 1);

    /*Como queremos hacer que el paquete sea  "3" (2bit) y "0" (14 bit)
    lo dividimos en un paquete que sea 1100 (4 bit), que es 12
    en hexadecimal, y otros 3 (4 bit) que sean 0 quedando como
    resultado la misma cadena de números */

    byte_to_write = 0x12;
	write(tm_fd, &byte_to_write, 1);

	byte_to_write = 0x0;
	write(tm_fd, &byte_to_write, 1);
	write(tm_fd, &byte_to_write, 1);
	write(tm_fd, &byte_to_write, 1);
	/* El packet length debera ser igual a los 32 bits
	del source data +3 bits =35 bits (en hexadecimal es 23)*/


	byte_to_write = 0x23;
	write(tm_fd, &byte_to_write, 1);


	/* Ahora completamos el apartado del packet data field, que volvemos a
	 agrupar y dividir en conjuntos de 8 bits para */

	byte_to_write = 0x10;
	write(tm_fd, &byte_to_write, 1);

	byte_to_write = 0x1;
	write(tm_fd, &byte_to_write, 1);

	byte_to_write = 0x1;
	write(tm_fd, &byte_to_write, 1);

	byte_to_write = source_ID;
	write(tm_fd, &byte_to_write, 1);

	byte_to_write = tc_packet_id;
	write(tm_fd, &byte_to_write, 2);

	byte_to_write = tc_packet_seq_ctrl;
	write(tm_fd, &byte_to_write, 2);

    close(tm_fd);







}
