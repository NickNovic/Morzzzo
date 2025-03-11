#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "system.h"

#define SERIAL_PORT "/dev/ttyUSB0"


unsigned long* get_sizes (char *pattern, int pattern_lenght) {
	unsigned long *sizes = malloc(pattern_lenght * sizeof(unsigned long)); /* Contains sizes of 1's chunks */
	int sizes_counter = 0;
	
	/* This flag represents if bit before current was 1 */
	bool was_one_flag = false;
	/* Counter of 1's, what are going one by one */
	unsigned long ones_counter = 0;
	/*
	 * I think here is 3 because on device i set it up, so I send 8 bits of data, 
	 * stop bit and null-terminator, so 8 + 1 + 8 = 17.
	 * First bit contains data
	 * */
	char buffer[3];
	
	int serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	
	if (serial_fd == -1) {
		perror("Error openning serial port: ");
		return NULL;
	}

	setup_termios(serial_fd);
	while (sizes_counter < pattern_lenght) {
		int num_bytes = read(serial_fd, buffer, sizeof(buffer));
		if (num_bytes <= 0) {
			continue;
		}

		if (buffer[0] == '0' && was_one_flag) {
			sizes[sizes_counter] = ones_counter;
			sizes_counter++;
			was_one_flag = false;
			ones_counter = 0;
			continue;
		}
		
		if (buffer[0] == '1') {
			was_one_flag = true;
			ones_counter++;
			continue;
		}
	}
	
	printf("device is calibrated\n");
	close(serial_fd);
	return sizes;
}

