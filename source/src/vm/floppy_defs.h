/** @file floppy_defs.h

	Skelton for retropc emulator

	@author Sasaji
	@date   2020.11.05

	@brief [ floppy drive definition ]
*/

#ifndef FLOPPY_DEFS_H
#define FLOPPY_DEFS_H

#include "vm_defs.h"

#ifdef USE_SIG_FLOPPY_ACCESS
#define SIG_FLOPPY_ACCESS			0
#endif
#define SIG_FLOPPY_READ_ID			1
//#define SIG_FLOPPY_READ_ID_TRACK_NUM		2
//#define SIG_FLOPPY_READ_ID_HEAD_NUM		3
#define SIG_FLOPPY_READ				6
#define SIG_FLOPPY_READ_TRACK		7
#define SIG_FLOPPY_WRITE			8
#define SIG_FLOPPY_WRITE_TRACK		9
#define SIG_FLOPPY_WRITEDELETE		10
#define SIG_FLOPPY_WRITEPROTECT		11
#define SIG_FLOPPY_STEP				12	// step and dirc
#define SIG_FLOPPY_HEADLOAD			13
#define SIG_FLOPPY_READY			14
#define SIG_FLOPPY_TRACK0			15
#define SIG_FLOPPY_INDEX			16
#define SIG_FLOPPY_DELETED			17

#ifdef SET_CURRENT_TRACK_IMMEDIATELY
#define SIG_FLOPPY_CURRENTTRACK		18
#endif
#define SIG_FLOPPY_SECTOR_NUM		19
#define SIG_FLOPPY_SECTOR_SIZE		20
#define SIG_FLOPPY_TRACK_SIZE		21

// for 5inch mini floppy
#define SIG_FLOPPY_IRQ				26
#define SIG_FLOPPY_DRQ				27

#define SIG_FLOPPY_DENSITY			28

#endif /* FLOPPY_DEFS_H */

