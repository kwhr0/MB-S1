/** @file z80_consts.h

	Skelton for retropc emulator

	@par Origin
	MAME 0.145
	@author Takeda.Toshiya
	@date   2012.02.15-

	@note Modify for MB-S1 By Sasaji on 2019.10.21 -

	@brief [ Z80 constant values ]
*/

#ifndef _Z80_CONSTS_H_ 
#define _Z80_CONSTS_H_

#define CF	0x01
#define NF	0x02
#define PF	0x04
#define VF	PF
#define XF	0x08
#define HF	0x10
#define YF	0x20
#define ZF	0x40
#define SF	0x80

/* int_state */
#define Z80_STATE_EI		0x0001
#define Z80_STATE_HALT		0x0004
#define Z80_STATE_RESET		0x0008
#define Z80_STATE_NMI		0x0010
#define Z80_STATE_INTR		0x0020
#define Z80_STATE_BUSREQ	0x0040
#define Z80_STATE_BUSACK	0x0080
/* for debug */
#define Z80_STATE_BUSREQ_REL	0x400000
#define Z80_STATE_BUSACK_REL	0x800000

#endif /* _Z80_CONSTS_H_ */

