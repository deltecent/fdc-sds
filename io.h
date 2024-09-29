#ifndef	_IO_H
#define	_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

#ifndef B76800
#define	B76800	76800
#endif

#ifndef B403200
#define	B403200	403200
#endif

#ifndef B460800
#define	B460800	460800
#endif

extern int openPort(char *device, int baud);
extern int closePort();
extern int recvByte(uint8_t *byte, int tsecs);
extern int recvBuf(void *buffer, int length, int tsecs);
extern int sendBuf(void *buffer, int length, int tsecs);
extern uint16_t calcChksum(void *buffer, int length);

#endif
