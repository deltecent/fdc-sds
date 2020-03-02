#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include "fdcsds.h"
#include "drive.h"
#include "display.h"

/*
** 137 bytes per sector, 32 sectors per track, 77 tracks
*/

drvstat_t drvstat[MAX_DRIVES];
uint8_t trackbuf[MAX_TRACK_LEN];
int fdc_errno;

int mountDrive(int drive, char *filename)
{
	if (drive >= MAX_DRIVES) {
		return -1;
	}

	strncpy(drvstat[drive].filename, filename, MAX_PATH);

	if ((drvstat[drive].fd = open(filename, O_RDWR)) == -1) {
		drvstat[drive].mounted = FALSE;
		displayMount(drive, "--ERROR--");
		displayError("MOUNT", errno);
	}
	else {
		drvstat[drive].mounted = TRUE;
		displayMount(drive, filename);
	}

	return drvstat[drive].fd;
}

int unmountDrive(int drive)
{
	if (drive >= MAX_DRIVES) {
		return -1;
	}

	if (drvstat[drive].mounted && drvstat[drive].fd != -1) {
		close(drvstat[drive].fd);

		drvstat[drive].mounted = FALSE;
		drvstat[drive].fd = -1;

		displayMount(drive, NULL);
	}

	return 0;
}

void unmountAll()
{
	int d;

	for (d = 0; d < MAX_DRIVES; d++) {
		unmountDrive(d);
	}
}

int writeProtect(int drive, int flag)
{
	if (drive >= MAX_DRIVES) {
		return -1;
	}

	if (drvstat[drive].mounted) {
		drvstat[drive].readonly = (flag) ? TRUE : FALSE;

		displayRO(drive, flag);
	}

	return (0);
}

int readTrack(int drive, int track, int length, void *buffer)
{
	off_t offset;
	int bytes;

	if (drive >= MAX_DRIVES) {
		displayError("INVALID DRIVE", 0);
		return -1;
	}

	if (drvstat[drive].mounted && drvstat[drive].fd == -1) {
		displayError("DISK NOT MOUNTED", 0);
		return -1;
	}

	offset = track * length;

	displayTrack(drive, track);

	/*
	** Seek to track
	*/
	if (lseek(drvstat[drive].fd, offset, SEEK_SET) != offset) {
		displayError("LSEEK", errno);
		return(-1);
	}

	/*
	** Read track
	*/
	if ((bytes = read(drvstat[drive].fd, buffer, length)) != length) {
		displayError("READ TRACK", errno);
	}

	return (bytes);
}

int writeTrack(int drive, int track, int length, void *buffer)
{
	off_t offset;
	int bytes;

	if (drive >= MAX_DRIVES) {
		displayError("INVALID DRIVE", 0);
		fdc_errno = FDC_NOT_READY;
		return (-1);
	}

	if (!drvstat[drive].mounted || drvstat[drive].fd == -1) {
		displayError("DISK NOT MOUNTED", 0);
		fdc_errno = FDC_NOT_READY;
		return (-1);
	}

	if (drvstat[drive].readonly) {
		displayError("DISK READ ONLY", 0);
		fdc_errno = FDC_WRITE_ERR;
		return (-1);
	}

	offset = track * length;

	displayTrack(drive, track);

	/*
	** Seek to track
	*/
	if (lseek(drvstat[drive].fd, offset, SEEK_SET) != offset) {
		displayError("LSEEK", errno);
		fdc_errno = FDC_WRITE_ERR;
		return (-1);
	}

	/*
	** Write track
	*/
	if ((bytes = write(drvstat[drive].fd, buffer, length)) != length) {
		displayError("WRITE TRACK", errno);
		fdc_errno = FDC_WRITE_ERR;
		return (-1);
	}

	fdc_errno = FDC_OK;

	return (bytes);
}

