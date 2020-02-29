#ifndef _DISPLAY_H
#define	_DISPLAY_H

#include <ncurses.h>

extern char dispbuf[128];

extern void displayInit();
extern void displayReset();
extern int displayGetch();
extern void displayPort(char *port);
extern void displayBaud(int baud);
extern void displayCommand(char *command);
extern void displayBlock(int drive, int track, int length);
extern void displayError(char *string, int err);
extern void displayDebug(char *string);
extern void displayHead(int drive, int head);
extern void displayTrack(int drive, int track);
extern void displayMount(int drive, char *path);
extern void displayRO(int drive, int wp);
extern void displayBuffer(char *string, void *buffer, int length);

#endif

