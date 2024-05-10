#ifndef	_FDC_H
#define	_FDC_H

#include <stdlib.h>
#include <stdint.h>

#define	FDCSDS_NAME		"FDC+ Serial Drive Server"
#define	FDCSDS_COPYRIGHT	"(c) 2024 Deltec Enterprises LLC"

#ifndef TRUE
#define	TRUE	1
#endif

#ifndef FALSE
#define FALSE	!TRUE
#endif

extern int f_help;
extern int f_verbose;
extern int f_debug;

#endif
