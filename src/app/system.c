#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>
#include <memory.h>

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
