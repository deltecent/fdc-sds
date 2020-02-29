#ifndef	_DRIVE_H
#define	_DRIVE_H

#include <stdint.h>

#define	MAX_DRIVES	16
#define	MAX_TRACKS	77
#define	MAX_TRACK_LEN	(137*32)
#define MAX_DISK_SIZE	MAX_TRACK_LEN * MAX_TRACKS
#define	MAX_PATH	128

typedef struct DRVSTAT {
	int		fd;
	char		filename[MAX_PATH];
	uint8_t		mounted;
	uint8_t		readonly;
	uint8_t		hdld;
	uint16_t	track;
} drvstat_t;

#define	LSB(b)		(b & 0xff)
#define	MSB(b)		((b & 0xff00) >> 8)
#define	WORD(lsb,msb)	((msb << 8) | lsb)

#define	FDC_OK		0x00
#define	FDC_NOT_READY	0x01
#define	FDC_CHKSUM_ERR	0x02
#define	FDC_WRITE_ERR	0x03

extern int	fdc_errno;

/*
** FDC+ Command / Response Block
*/
typedef struct CRBLOCK {
	char		cmd[4];

	union {
		struct {
			uint8_t	lsb1;
			uint8_t	msb1;
		};
		uint16_t	param1;
	};

	union {
		struct {
			uint8_t	lsb2;
			uint8_t	msb2;
		};
		uint16_t	param2;
	};
} crblk_t;

extern drvstat_t drvstat[MAX_DRIVES];
extern uint8_t trackbuf[MAX_TRACK_LEN];

extern int mountDrive(int drive, char *filename);
extern int unmountDrive(int drive);
extern void unmountAll();
extern int writeProtect(int drive, int flag);
extern int readTrack(int drive, int track, int length, void *buffer);
extern int writeTrack(int drive, int track, int length, void *buffer);

#endif
