#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include "io.h"
#include "display.h"

static int fd;

static struct termios oldtio;
static struct termios newtio;

static char debug[80];

int openPort(char *device, int baud)
{
	if (device == NULL) {
		return (-1);
	}

	if ((fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK)) == -1) {
		return fd;
	}

	tcgetattr(fd, &oldtio); /* save current serial port settings */

	memset(&newtio, 0, sizeof(newtio)); /* clear struct for new port settings */
	newtio.c_cflag = CLOCAL | CS8 | CREAD;
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;

	/*
	** The FDC+ only supports 230.4K, 403.2K and 460.8K
	** Linux does not appear to support 403.2K
	*/
	switch (baud) {
#ifdef _APPLE_
		case 403200:
			cfsetspeed(&newtio, B403200);
			break;

#endif
		case 460800:
			cfsetspeed(&newtio, B460800);
			break;

		default:
			baud = 230400;
			cfsetspeed(&newtio, B230400);
			break;
	}

	tcsetattr(fd, TCSANOW, &newtio);

	tcflush(fd, TCIFLUSH);

	displayPort(device);
	displayBaud(baud);

	return fd;
}

int closePort()
{
	tcsetattr(fd, TCSANOW, &oldtio);

	return close(fd);
}

int recvByte(uint8_t *byte, int tsecs)
{
	int n;
	fd_set input;
	struct timeval timeout;

	FD_ZERO(&input);
	FD_SET(fd, &input);

	timeout.tv_sec = tsecs;
	timeout.tv_usec = 0;

	n = select(fd+1, &input, NULL, NULL, &timeout);

	if (n < 0) {		/* ERROR */
		displayError("SELECT", errno);

		return (-1);
	}
	else if (n==0) {	/* TIMEOUT */
		return (0);
	}

	n = read(fd, byte, 1);

	return(n);
}

int recvBuf(void *buffer, int length, int tsecs)
{
	int count = 0;
	int bytes;
	uint8_t lsb;
	uint8_t msb;
	uint16_t chksum;

	/*
	** Receive requested length
	*/
	while (length) {

		bytes = recvByte(buffer + count, tsecs);

		if (bytes > 0) {
			count += bytes;
			length -= bytes;
		}
		else {
			return(count);
		}
	}

	/*
	** Receive Checksum
	*/
	recvByte(&lsb, 1);
	recvByte(&msb, 1);
	chksum = (msb << 8) + lsb;

	if (chksum != calcChksum(buffer, count)) {
		displayError("CHECKSUM", 0);

		count = 0;
	}

	return count;
}

/*
** Send buffer and append checksum
*/
int sendBuf(void *buffer, int length, int tsecs)
{
	uint16_t chksum;
	uint8_t msb;
	uint8_t lsb;
	int bytes;
	int sent = 0;
	int n;
	fd_set output;
	struct timeval timeout;

	FD_ZERO(&output);
	FD_SET(fd, &output);

	timeout.tv_sec = tsecs;
	timeout.tv_usec = 0;

	while (sent < length) {
		n = select(fd+1, NULL, &output, NULL, &timeout);

		if (n == -1) {
			displayError("SELECT", errno);
			return -1;
		}

		bytes = write(fd, buffer + sent, length - sent);

		if (bytes == 0) {
			displayError("SERIAL WRITE TIMEOUT", errno);
			return -1;
		}
		else if (bytes == -1 && errno != EAGAIN) {
			displayError("SERIAL WRITE", errno);
			return -1;
		}

		sent += bytes;
	}

	chksum = calcChksum(buffer, length);

	lsb = chksum & 0x00ff;

	bytes = write(fd, &lsb, 1);

	if (bytes != 1 && errno != EAGAIN) {
		displayError("SERIAL WRITE", errno);
		return -1;
	}

	msb = (chksum & 0xff00) >> 8;

	bytes = write(fd, &msb, 1);

	if (bytes != 1 && errno != EAGAIN) {
		displayError("SERIAL WRITE", errno);
		return -1;
	}

	tcdrain(fd);

	return 0;
}

uint16_t calcChksum(void *buffer, int length)
{
	uint16_t chksum = 0;

	while (length--) {
		chksum += * (uint8_t *) buffer++;
	}

	return chksum;
}


