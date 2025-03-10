#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>
#include <memory.h>

#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD B9600

int setup_termios(int serial_fd){
	struct termios tty;
	memset(&tty, 0, sizeof(tty));

	if (tcgetattr(serial_fd, &tty) != 0) {
		perror("Error getting termina attributes");
		close(serial_fd);
		return 1;
	}

	cfsetospeed(&tty, BAUD);
	cfsetispeed(&tty, BAUD);

	/*
	 * Configure serial communication mode
	 * */
	tty.c_cflag &= ~PARENB; /* Disable parity bit */
	
	tty.c_cflag &= ~CSTOPB; /* Use one stop bit (disabling using of two bits */
	
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8; /* Setting character size to 8 bits */
	
	tty.c_cflag &= ~CRTSCTS; /* Disable hardware flow control RTS/CTS */

	tty.c_cflag |= CREAD | CLOCAL; /* Enabling reciver (CREAD) and ignoring modem control lines(CLOCAL) */
	
	/*
	 * Configure Input & Output behavior
	 * */
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE);// | ISIG); 
	/* 
	 * 1. Disable canonical mode so input is read byte by byte instead of waiting for newline.
	 * 2. Disable echo(and echoe) so terminal don't print recived chars 
	 * 3. Disable signal characters (ISIG) so special characters (like Ctrl + C) don't terminate program */
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);/* Disable software flow control XON/XOFF */
	tty.c_oflag &= ~OPOST;/* Disable output processing (data will be sent raw) */

	if(tcsetattr(serial_fd, TCSANOW, &tty) != 0) { /* TCSANOW makes change take effect immediately */
		perror("Error setting terminal attributes");
		close(serial_fd);
		return 1;
	}

	return 0;
}

/* Barrier is calculated as average of sizes 
 * @param=sizes
 * @param=pattern_lenght
 * */
unsigned long calculate_barrier (unsigned long *sizes, int pattern_lenght) {
	unsigned long barrier = 0;
	unsigned long long summ = 0;
	for (int i = 0; i < pattern_lenght; i++) {
		summ += sizes[i];
	}
	barrier = summ / pattern_lenght;
	return barrier;			
}

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

unsigned long calibrate(void) {
	/* Represents barrier value */
	unsigned long barrier = 0;

	char pattern[] = "-.-.";
	/* 1 is ofset because of \0 */
	int pattern_lenght = sizeof(pattern) / sizeof(pattern[0]) - 1;

	printf("%s \n", pattern);
	
	unsigned long *sizes = get_sizes(pattern, pattern_lenght);
	barrier = calculate_barrier(sizes, pattern_lenght);
	free(sizes);
	return barrier;
}

int main(){
	unsigned long barrier = calibrate();
	
	int serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	int res = setup_termios(serial_fd);
	
	if (res != 0) {
		perror("Termios setup error");
	}

	char buffer[3];
	unsigned long ones_counter = 0;
	bool was_one_flag = false;
	
	while (1) {
		int num_bytes = read(serial_fd, buffer, sizeof(buffer) - 1);
		
		if (num_bytes <= 0) {
			continue;
		}
		
		if (buffer[0] == '1') {
			ones_counter++;
			was_one_flag = true;
			continue;
		}

		if (buffer[0] == '0' && was_one_flag) {
			if (ones_counter > barrier) {
				printf("-");
			} else {
				printf(".");
			}
			fflush(stdout);
			ones_counter = 0;
			was_one_flag = false;
			continue;
		}
	}

	close(serial_fd);
}
