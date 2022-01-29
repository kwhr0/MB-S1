/** @file sdl_input.h

	Skelton for retropc emulator
	SDL edition

	@author Sasaji
	@date   2012.02.21

	@brief [ sdl input ]

*/

#ifndef SDL_INPUT_H
#define SDL_INPUT_H

#include "sdl_emu.h"
#include "../../vm/vm_defs.h"
#include "../../keycode.h"

#if defined(USE_SDL2)

const uint8_t scancode2keycode[256] = {
	/* 0x00 - 0x0f */
	0,
	0,
	0,
	0,
	KEYCODE_A,
	KEYCODE_B,
	KEYCODE_C,
	KEYCODE_D,
	KEYCODE_E,
	KEYCODE_F,
	KEYCODE_G,
	KEYCODE_H,
	KEYCODE_I,
	KEYCODE_J,
	KEYCODE_K,
	KEYCODE_L,
	/* 0x10 - 0x1f */
	KEYCODE_M,
	KEYCODE_N,
	KEYCODE_O,
	KEYCODE_P,
	KEYCODE_Q,
	KEYCODE_R,
	KEYCODE_S,
	KEYCODE_T,
	KEYCODE_U,
	KEYCODE_V,
	KEYCODE_W,
	KEYCODE_X,
	KEYCODE_Y,
	KEYCODE_Z,
	KEYCODE_1,
	KEYCODE_2,
	/* 0x20 - 0x2f */
	KEYCODE_3,
	KEYCODE_4,
	KEYCODE_5,
	KEYCODE_6,
	KEYCODE_7,
	KEYCODE_8,
	KEYCODE_9,
	KEYCODE_0,
	KEYCODE_RETURN,
	KEYCODE_ESCAPE,
	KEYCODE_BACKSPACE,
	KEYCODE_TAB,
	KEYCODE_SPACE,
	KEYCODE_MINUS,
	KEYCODE_CARET, //<-JP KEYCODE_EQUALS,
	KEYCODE_AT, //<-JP KEYCODE_LBRACKET,
	/* 0x30 - 0x3f */
	KEYCODE_LBRACKET, //<-JP KEYCODE_RBRACKET,
	KEYCODE_RBRACKET, //<-JP KEYCODE_BACKSLASH,
	0, // KEYCODE_NONUSHASH,
	KEYCODE_SEMICOLON,
	KEYCODE_COLON, //<-JP // KEYCODE_APOSTROPHE,
	KEYCODE_GRAVE,
	KEYCODE_COMMA,
	KEYCODE_PERIOD,
	KEYCODE_SLASH,
	KEYCODE_CAPSLOCK,
	KEYCODE_F1,
	KEYCODE_F2,
	KEYCODE_F3,
	KEYCODE_F4,
	KEYCODE_F5,
	KEYCODE_F6,
	/* 0x40 - 0x4f */
	KEYCODE_F7,
	KEYCODE_F8,
	KEYCODE_F9,
	KEYCODE_F10,
	KEYCODE_F11,
	KEYCODE_F12,
#if defined(__APPLE__) && defined(__MACH__)
	KEYCODE_F13,
	KEYCODE_F14,
	KEYCODE_F15,
#else
	0, // KEYCODE_PRINTSCREEN,
	KEYCODE_SCROLLLOCK,
	KEYCODE_PAUSE,
#endif
	KEYCODE_INSERT,
	KEYCODE_HOME,
	KEYCODE_PAGEUP,
	KEYCODE_DELETE,
	KEYCODE_END,
	KEYCODE_PAGEDOWN,
	KEYCODE_RIGHT,
	/* 0x50 - 0x5f */
	KEYCODE_LEFT,
	KEYCODE_DOWN,
	KEYCODE_UP,
	KEYCODE_NUMLOCK,
	KEYCODE_KP_DIVIDE,
	KEYCODE_KP_MULTIPLY,
	KEYCODE_KP_MINUS,
	KEYCODE_KP_PLUS,
	KEYCODE_KP_ENTER,
	KEYCODE_KP_1,
	KEYCODE_KP_2,
	KEYCODE_KP_3,
	KEYCODE_KP_4,
	KEYCODE_KP_5,
	KEYCODE_KP_6,
	KEYCODE_KP_7,
	/* 0x60 - 0x6f */
	KEYCODE_KP_8,
	KEYCODE_KP_9,
	KEYCODE_KP_0,
	KEYCODE_KP_PERIOD,
	0, // KEYCODE_NONUSBACKSLASH,
	KEYCODE_MENU,
	KEYCODE_POWER,
	KEYCODE_KP_EQUALS,
	KEYCODE_F13,
	KEYCODE_F14,
	KEYCODE_F15,
	KEYCODE_F16,
	KEYCODE_F17,
	KEYCODE_F18,
	KEYCODE_F19,
	KEYCODE_F20,
	/* 0x70 - 0x7f */
	KEYCODE_F21,
	KEYCODE_F22,
	0, // KEYCODE_F23,
	0, // KEYCODE_F24,
	0, // KEYCODE_EXECUTE,
	KEYCODE_HELP,
	KEYCODE_MENU,
	KEYCODE_SELECT,
	0, // KEYCODE_STOP,
	0, // KEYCODE_AGAIN,
	KEYCODE_UNDO,
	0, // KEYCODE_CUT,
	0, // KEYCODE_COPY,
	0, // KEYCODE_PASTE,
	0, // KEYCODE_FIND,
	0, // KEYCODE_MUTE,
	/* 0x80 - 0x8f */
	0, // KEYCODE_VOLUMEUP,
	0, // KEYCODE_VOLUMEDOWN,
	0, // KEYCODE_LOCKINGCAPSLOCK
	0, // KEYCODE_LOCKINGNUMLOCK
	0, // KEYCODE_LOCKINGSCROLLLOCK
	0, // KEYCODE_KP_COMMA,
	0, // KEYCODE_KP_EQUALSAS400,
	KEYCODE_UNDERSCORE, //<-JP INTERNATIONAL1,
	KEYCODE_KATAHIRA, // INTERNATIONAL2,
	KEYCODE_BACKSLASH, //<-JP KEYCODE_INTERNATIONAL3,
	KEYCODE_HENKAN, // INTERNATIONAL4,
	KEYCODE_MUHENKAN, // INTERNATIONAL5,
	0, // KEYCODE_INTERNATIONAL6,
	0, // KEYCODE_INTERNATIONAL7,
	0, // KEYCODE_INTERNATIONAL8,
	0, // KEYCODE_INTERNATIONAL9,
	/* 0x90 - 0x9f */
	0, // KEYCODE_LANG1, /**< Hangul/English toggle */
	0, // KEYCODE_LANG2, /**< Hanja conversion */
	0, // KEYCODE_LANG3, /**< Katakana */
	0, // KEYCODE_LANG4, /**< Hiragana */
	0, // KEYCODE_LANG5, /**< Zenkaku/Hankaku */
	0, // KEYCODE_LANG6, /**< reserved */
	0, // KEYCODE_LANG7, /**< reserved */
	0, // KEYCODE_LANG8, /**< reserved */
	0, // KEYCODE_LANG9, /**< reserved */
	KEYCODE_FUNCTION, // KEYCODE_ALTERASE, /**< Erase-Eaze */
	KEYCODE_SYSREQ,
	0, // KEYCODE_CANCEL,
	KEYCODE_CLEAR,
	0, // KEYCODE_PRIOR,
	0, // KEYCODE_RETURN2,
	0, // KEYCODE_SEPARATOR,
	/* 0xa0 - 0xaf */
	0, // KEYCODE_OUT
	0, // KEYCODE_OPER
	0, // KEYCODE_CLEARAGAIN
	0, // KEYCODE_CRSEL
	0, // KEYCODE_EXSEL
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* 0xb0 - 0xbf */
	0, // KEYCODE_KP_00
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* 0xc0 - 0xcf */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* 0xd0 - 0xdf */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* 0xe0 - 0xef */
	KEYCODE_LCTRL,
	KEYCODE_LSHIFT,
	KEYCODE_LALT, /**< alt, option */
	KEYCODE_LGUI, /**< windows, command (apple), meta */
	KEYCODE_RCTRL,
	KEYCODE_RSHIFT,
	KEYCODE_RALT, /**< alt gr, option */
	KEYCODE_RGUI, /**< windows, command (apple), meta */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* 0xf0 - 0xff */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};


#else /* !USE_SDL2 */

#if defined(_WIN32)

const uint8_t scancode2keycode[128] = {
	/* 0x00 - 0x0f */
	0,
	KEYCODE_ESCAPE,		// ESC
	KEYCODE_1,			// 1 !
	KEYCODE_2,			// 2 "
	KEYCODE_3,			// 3 #
	KEYCODE_4,			// 4 $
	KEYCODE_5,			// 5 %
	KEYCODE_6,			// 6 &
	KEYCODE_7,			// 7 '
	KEYCODE_8,			// 8 (
	KEYCODE_9,			// 9 )
	KEYCODE_0,			// 0
	KEYCODE_MINUS,		// - = (JP) - _ (US)
	KEYCODE_CARET,		// ~ ^ (JP) + = (US)
	KEYCODE_BACKSPACE,	// BackSpace
	KEYCODE_TAB,		// Tab
	/* 0x10 - 0x1f */
	KEYCODE_Q,			// Q
	KEYCODE_W,			// W
	KEYCODE_E,			// E
	KEYCODE_R,			// R
	KEYCODE_T,			// T
	KEYCODE_Y,			// Y
	KEYCODE_U,			// U
	KEYCODE_I,			// I
	KEYCODE_O,			// O
	KEYCODE_P,			// P
	KEYCODE_AT,			// @ ` (JP) [ { (US)
	KEYCODE_LBRACKET,	// [ { (JP) ] } (US)
	KEYCODE_RETURN,		// Enter
	KEYCODE_LCTRL,		// Left Ctrl
	KEYCODE_A,			// A
	KEYCODE_S,			// S
	/* 0x20 - 0x2f */
	KEYCODE_D,			// D
	KEYCODE_F,			// F
	KEYCODE_G,			// G
	KEYCODE_H,			// H
	KEYCODE_J,			// J
	KEYCODE_K,			// K
	KEYCODE_L,			// L
	KEYCODE_SEMICOLON,	// ; + (JP) : ; (US)
	KEYCODE_COLON,		// : * (JP) " ' (US)
	KEYCODE_GRAVE,		// Kanji `
	KEYCODE_LSHIFT,		// Left Shift
	KEYCODE_RBRACKET,	// ] } (JP) | \ (US)
	KEYCODE_Z,			// Z
	KEYCODE_X,			// X
	KEYCODE_C,			// C
	KEYCODE_V,			// V
	/* 0x30 - 0x3f */
	KEYCODE_B,			// B
	KEYCODE_N,			// N
	KEYCODE_M,			// M
	KEYCODE_COMMA,		// , <
	KEYCODE_PERIOD,		// . >
	KEYCODE_SLASH,		// / ?
	KEYCODE_RSHIFT,		// Right Shift
	KEYCODE_KP_MULTIPLY,// num *
	KEYCODE_LALT,		// Left Alt
	KEYCODE_SPACE,		// Space
	0,
	KEYCODE_F1,			// F1
	KEYCODE_F2,			// F2
	KEYCODE_F3,			// F3
	KEYCODE_F4,			// F4
	KEYCODE_F5,			// F5
	/* 0x40 - 0x4f */
	KEYCODE_F6,			// F6
	KEYCODE_F7,			// F7
	KEYCODE_F8,			// F8
	KEYCODE_F9,			// F9
	KEYCODE_F10,		// F10
	KEYCODE_PAUSE,		// Pause / Break
	KEYCODE_SCROLLLOCK,	// Scroll Lock
	KEYCODE_KP_7,		// num 7
	KEYCODE_KP_8,		// num 8
	KEYCODE_KP_9,		// num 9
	KEYCODE_KP_MINUS,	// num -
	KEYCODE_KP_4,		// num 4
	KEYCODE_KP_5,		// num 5
	KEYCODE_KP_6,		// num 6
	KEYCODE_KP_PLUS,	// num +
	KEYCODE_KP_1,		// num 1
	/* 0x50 - 0x5f */
	KEYCODE_KP_2,		// num 2
	KEYCODE_KP_3,		// num 3
	KEYCODE_KP_0,		// num 0
	KEYCODE_KP_PERIOD,	// num .
	0,
	0,
	0,
	KEYCODE_F11,		// F11
	KEYCODE_F12,		// F12
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* 0x60 - 0x6f */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* 0x70 - 0x7f */
	KEYCODE_KATAHIRA,	// Katakana / Hiragana (JP)
	0,
	0,
	KEYCODE_UNDERSCORE,	// _
	0,
	0,
	0,
	0,
	0,
	KEYCODE_HENKAN,		// Henkan (JP)
	0,
	KEYCODE_MUHENKAN,	// Muhenkan (JP)
	0,
	KEYCODE_BACKSLASH,	// Yen
	0,
	0
};

typedef struct st_sc2kc {
	short sc;
	short kc;
} sc2kc_t;

const sc2kc_t scancode2keycode2[] = {
	{ 0x11c, KEYCODE_KP_ENTER },// num Enter
	{ 0x11d, KEYCODE_RCTRL },	// Right ctrl
	{ 0x12a, KEYCODE_SYSREQ },	// Print Screen / SysRq
	{ 0x135, KEYCODE_KP_DIVIDE },// num /
	{ 0x138, KEYCODE_RALT },	// Right Alt
	{ 0x145, KEYCODE_NUMLOCK },	// Num Lock
	{ 0x147, KEYCODE_HOME },	// Home
	{ 0x148, KEYCODE_UP },		// Up allow
	{ 0x149, KEYCODE_PAGEUP },	// Page Up
	{ 0x14b, KEYCODE_LEFT },	// Left allow
	{ 0x14d, KEYCODE_RIGHT },	// Right allow
	{ 0x14f, KEYCODE_END },		// End
	{ 0x150, KEYCODE_DOWN },	// Down allow
	{ 0x151, KEYCODE_PAGEDOWN },// Page Down
	{ 0x152, KEYCODE_INSERT },	// Insert
	{ 0x153, KEYCODE_DELETE },	// Delete
	{ 0x15b, KEYCODE_LSUPER },	// Left Win
	{ 0x15c, KEYCODE_RSUPER },	// Right Win
	{ 0x15d, KEYCODE_MENU },	// Application
	{ -1, -1 }
};

#elif defined(__APPLE__) && defined(__MACH__)

const uint8_t scancode2keycode[128] = {
	/* 0x00 - 0x0f */
	KEYCODE_A,			// A
	KEYCODE_S,			// S
	KEYCODE_D,			// D
	KEYCODE_F,			// F
	KEYCODE_H,			// H
	KEYCODE_G,			// G
	KEYCODE_Z,			// Z
	KEYCODE_X,			// X
	KEYCODE_C,			// C
	KEYCODE_V,			// V
	0,
	KEYCODE_B,			// B
	KEYCODE_Q,			// Q
	KEYCODE_W,			// W
	KEYCODE_E,			// E
	KEYCODE_R,			// R
	/* 0x10 - 0x1f */
	KEYCODE_Y,			// Y
	KEYCODE_T,			// T
	KEYCODE_1,			// 1 !
	KEYCODE_2,			// 2 "
	KEYCODE_3,			// 3 #
	KEYCODE_4,			// 4 $
	KEYCODE_6,			// 6 &
	KEYCODE_5,			// 5 %
	KEYCODE_CARET,		// ~ ^ (JP) + = (US)
	KEYCODE_9,			// 9 )
	KEYCODE_7,			// 7 '
	KEYCODE_MINUS,		// - = (JP) - _ (US)
	KEYCODE_8,			// 8 (
	KEYCODE_0,			// 0
	KEYCODE_LBRACKET,	// [ { (JP) ] } (US)
	KEYCODE_O,			// O
	/* 0x20 - 0x2f */
	KEYCODE_U,			// U
	KEYCODE_AT,			// @ ` (JP) [ { (US)
	KEYCODE_I,			// I
	KEYCODE_P,			// P
	KEYCODE_RETURN,		// Enter
	KEYCODE_L,			// L
	KEYCODE_J,			// J
	KEYCODE_COLON,		// : * (JP) " ' (US)
	KEYCODE_K,			// K
	KEYCODE_SEMICOLON,	// ; + (JP) : ; (US)
	KEYCODE_RBRACKET,	// ] } (JP) | \ (US)
	KEYCODE_COMMA,		// , <
	KEYCODE_SLASH,		// / ?
	KEYCODE_N,			// N
	KEYCODE_M,			// M
	KEYCODE_PERIOD,		// . >
	/* 0x30 - 0x3f */
	KEYCODE_TAB,		// Tab
	KEYCODE_SPACE,		// Space
	KEYCODE_GRAVE,		// Kanji `
	KEYCODE_BACKSPACE,	// BackSpace
	0, //KEYCODE_IB_ENTER,		// iBook Enter
	KEYCODE_ESCAPE,		// ESC
	KEYCODE_RGUI,		// Right command
	KEYCODE_LGUI,		// Left command
	KEYCODE_LSHIFT,		// Left Shift
	KEYCODE_CAPSLOCK,	// CapsLock
	KEYCODE_LALT,		// Left Alt
	KEYCODE_LCTRL,		// Left Ctrl
	KEYCODE_RSHIFT,		// Right Shift
	KEYCODE_RALT,		// Right Alt
	KEYCODE_RCTRL,		// Right Ctrl
	KEYCODE_FUNCTION,	// fn
	/* 0x40 - 0x4f */
	KEYCODE_F17,		// F17
	KEYCODE_KP_PERIOD,	// num .
	0,
	KEYCODE_KP_MULTIPLY,// num *
	0,
	KEYCODE_KP_PLUS,	// num +
	0,
	KEYCODE_NUMLOCK,	// clear
	0,
	0,
	0,
	KEYCODE_KP_DIVIDE,	// num /
	KEYCODE_KP_ENTER,	// num enter
	KEYCODE_KP_MINUS,	// num -
	0,
	KEYCODE_F18,		// F18
	/* 0x50 - 0x5f */
	KEYCODE_F19,		// F19
	KEYCODE_KP_EQUALS,	// num equal
	KEYCODE_KP_0,		// num 0
	KEYCODE_KP_1,		// num 1
	KEYCODE_KP_2,		// num 2
	KEYCODE_KP_3,		// num 3
	KEYCODE_KP_4,		// num 4
	KEYCODE_KP_5,		// num 5
	KEYCODE_KP_6,		// num 6
	KEYCODE_KP_7,		// num 7
	0,
	KEYCODE_KP_8,		// num 8
	KEYCODE_KP_9,		// num 9
	KEYCODE_BACKSLASH,	// yen(backslash)
	KEYCODE_UNDERSCORE,	// _
	0,
	/* 0x60 - 0x6f */
	KEYCODE_F5,			// F5
	KEYCODE_F6,			// F6
	KEYCODE_F7,			// F7
	KEYCODE_F3,			// F3
	KEYCODE_F8,			// F8
	KEYCODE_F9,			// F9
	KEYCODE_EISU,		// eisu
	KEYCODE_F11,		// F11
	KEYCODE_KANA,		// kana
	KEYCODE_F13,		// F13
	KEYCODE_F16,		// F16
	KEYCODE_F14,		// F14
	0,
	KEYCODE_F10,		// F10
	KEYCODE_MENU,		// menu
	KEYCODE_F12,		// F12
	/* 0x70 - 0x7f */
	0,
	KEYCODE_F15,		// F15
	KEYCODE_INSERT,		// insert
	KEYCODE_HOME,		// home
	KEYCODE_PAGEUP,		// page up
	KEYCODE_DELETE,		// delete x
	KEYCODE_F4,			// F4
	KEYCODE_END,		// end
	KEYCODE_F2,			// F2
	KEYCODE_PAGEDOWN,	// page down
	KEYCODE_F1,			// F1
	KEYCODE_LEFT,		// left
	KEYCODE_RIGHT,		// right
	KEYCODE_DOWN,		// down
	KEYCODE_UP,			// up
	KEYCODE_POWER		// power

//	KEYCODE_PAUSE,		// Pause / Break
//	KEYCODE_SCROLLLOCK,	// Scroll Lock
//	KEYCODE_KATAHIRA,	// Katakana / Hiragana (JP)
//	KEYCODE_UNDERSCORE,	// _
};

typedef struct st_ks2kc {
	short ks;
	short kc;
} ks2kc_t;

const ks2kc_t keysym2keycode[] = {
	{ 97,  KEYCODE_A },
	{ 301, KEYCODE_CAPSLOCK },
	{ 303, KEYCODE_RSHIFT },
	{ 304, KEYCODE_LSHIFT },
	{ 305, KEYCODE_RCTRL },
	{ 306, KEYCODE_LCTRL },
	{ 307, KEYCODE_RALT },
	{ 308, KEYCODE_LALT },
	{ 309, KEYCODE_RGUI },
	{ 310, KEYCODE_LGUI },
	{ -1, -1 }
};

#else

const uint8_t sdlcode2keycode[128] = {
	/* 0x00 - 0x0f */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	KEYCODE_BACKSPACE,
	KEYCODE_TAB,
	0,
	0,
	KEYCODE_CLEAR,
	KEYCODE_RETURN,
	0,
	0,
	/* 0x10 - 0x1f */
	0,
	0,
	0,
	KEYCODE_PAUSE,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	KEYCODE_ESCAPE,
	0,
	0,
	0,
	0,
	/* 0x20 - 0x2f */
	KEYCODE_SPACE,
	0,
	0,
	0,
	0,
	0,
	0,
	KEYCODE_QUOTE,
	KEYCODE_LPAREN,
	KEYCODE_RPAREN,
	KEYCODE_ASTERISK,
	KEYCODE_PLUS,
	KEYCODE_COMMA,
	KEYCODE_MINUS,
	KEYCODE_PERIOD,
	KEYCODE_SLASH,
	/* 0x30 - 0x3f */
	KEYCODE_0,
	KEYCODE_1,
	KEYCODE_2,
	KEYCODE_3,
	KEYCODE_4,
	KEYCODE_5,
	KEYCODE_6,
	KEYCODE_7,
	KEYCODE_8,
	KEYCODE_9,
	KEYCODE_COLON,
	KEYCODE_SEMICOLON,
	KEYCODE_LESS,
	KEYCODE_EQUALS,
	KEYCODE_GREATER,
	KEYCODE_QUESTION,
	/* 0x40 - 0x4f */
	KEYCODE_AT,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* 0x50 - 0x5f */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	KEYCODE_LBRACKET,
	0,	// determine using scancode // KEYCODE_BACKSLASH,
	KEYCODE_RBRACKET,
	KEYCODE_CARET,
	0,	// determine using scancode // KEYCODE_UNDERSCORE,
	/* 0x60 - 0x6f */
	KEYCODE_GRAVE,
	KEYCODE_A,
	KEYCODE_B,
	KEYCODE_C,
	KEYCODE_D,
	KEYCODE_E,
	KEYCODE_F,
	KEYCODE_G,
	KEYCODE_H,
	KEYCODE_I,
	KEYCODE_J,
	KEYCODE_K,
	KEYCODE_L,
	KEYCODE_M,
	KEYCODE_N,
	KEYCODE_O,
	/* 0x70 - 0x7f */
	KEYCODE_P,
	KEYCODE_Q,
	KEYCODE_R,
	KEYCODE_S,
	KEYCODE_T,
	KEYCODE_U,
	KEYCODE_V,
	KEYCODE_W,
	KEYCODE_X,
	KEYCODE_Y,
	KEYCODE_Z,
	0,
	0,
	0,
	0,
	KEYCODE_DELETE
};

typedef struct st_sc2kc {
	short sc;
	short kc;
} sc2kc_t;

#if defined(__FreeBSD__)
const sc2kc_t scancode2keycode[] = {
	{ 0x31, KEYCODE_GRAVE },
	{ 0x42, KEYCODE_CAPSLOCK },
	{ 0x81, KEYCODE_HENKAN },
	{ 0x83, KEYCODE_MUHENKAN },
	{ 0x84, KEYCODE_BACKSLASH },
	{ 0xd0, KEYCODE_KATAHIRA },
	{ 0xd3, KEYCODE_UNDERSCORE },
	{ -1, -1 }
};
#else
	/* linux */
const sc2kc_t scancode2keycode[] = {
	{ 0x31, KEYCODE_GRAVE },
	{ 0x42, KEYCODE_CAPSLOCK },
	{ 0x61, KEYCODE_UNDERSCORE },
	{ 0x64, KEYCODE_HENKAN },
	{ 0x65, KEYCODE_KATAHIRA },
	{ 0x66, KEYCODE_MUHENKAN },
	{ 0x84, KEYCODE_BACKSLASH },
	{ -1, -1 }
};
#endif

#endif

const uint8_t sdlcode2keycode2[0x50] = {
	/* 0x100 - 0x10f */
	KEYCODE_KP_0,
	KEYCODE_KP_1,
	KEYCODE_KP_2,
	KEYCODE_KP_3,
	KEYCODE_KP_4,
	KEYCODE_KP_5,
	KEYCODE_KP_6,
	KEYCODE_KP_7,
	KEYCODE_KP_8,
	KEYCODE_KP_9,
	KEYCODE_KP_PERIOD,
	KEYCODE_KP_DIVIDE,
	KEYCODE_KP_MULTIPLY,
	KEYCODE_KP_MINUS,
	KEYCODE_KP_PLUS,
	KEYCODE_KP_ENTER,
	/* 0x110 - 0x11f */
	KEYCODE_KP_EQUALS,
	KEYCODE_UP,
	KEYCODE_DOWN,
	KEYCODE_RIGHT,
	KEYCODE_LEFT,
	KEYCODE_INSERT,
	KEYCODE_HOME,
	KEYCODE_END,
	KEYCODE_PAGEUP,
	KEYCODE_PAGEDOWN,
	KEYCODE_F1,
	KEYCODE_F2,
	KEYCODE_F3,
	KEYCODE_F4,
	KEYCODE_F5,
	KEYCODE_F6,
	/* 0x120 - 0x12f */
	KEYCODE_F7,
	KEYCODE_F8,
	KEYCODE_F9,
	KEYCODE_F10,
	KEYCODE_F11,
	KEYCODE_F12,
	KEYCODE_F13,
	KEYCODE_F14,
	KEYCODE_F15,
	0,
	0,
	0,
	KEYCODE_NUMLOCK,
	KEYCODE_CAPSLOCK,
	KEYCODE_SCROLLLOCK,
	KEYCODE_RSHIFT,
	/* 0x130 - 0x13f */
	KEYCODE_LSHIFT,
	KEYCODE_RCTRL,
	KEYCODE_LCTRL,
	KEYCODE_RALT,
	KEYCODE_LALT,
	KEYCODE_RGUI,
	KEYCODE_LGUI,
	KEYCODE_RSUPER,
	KEYCODE_LSUPER,
	KEYCODE_MODE,
	KEYCODE_COMPOSE,
	KEYCODE_HELP,
	KEYCODE_PRINT,
	KEYCODE_SYSREQ,
	0, // KEYCODE_BREAK,
	KEYCODE_MENU,
	/* 0x140 - 0x14f */
	KEYCODE_POWER,
	KEYCODE_EURO,
	KEYCODE_UNDO,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

#endif /* !USE_SDL2 */

#ifdef USE_AUTO_KEY

#define AUTO_KEY_SHIFT	0x87
#define AUTO_KEY_RETURN	0xaf
#define AUTO_KEY_KANA	0x8a
#define AUTO_KEY_GRAPH	0x8b
#define AUTO_KEY_NONE	0xff
#define AUTO_KEY_MASK		0x00ff
#define AUTO_KEY_SHIFT_MASK	0x0100
#define AUTO_KEY_GRAPH_MASK	0x0200
#define AUTO_KEY_UPPER_MASK	0x0400
#define AUTO_KEY_LOWER_MASK	0x0800

static const int autokey_table[256] = {
	// 0x100: shift
	// 0x200: graph
	// 0x400: alphabet
	// 0x800: ALPHABET
	// 0x1000: katakana
	// 0x2000: hiragana
	// use vm key scan code 0x80 - 0xfe
	0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x0af,0x000,0x000,0x0af,0x000,0x000,
	0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x08c,0x083,0x085,0x081,0x084,
	0x080,0x19a,0x19b,0x197,0x191,0x199,0x192,0x190,0x193,0x19c,0x1b6,0x1b4,0x0c3,0x096,0x0cc,0x0c4,
	0x094,0x09a,0x09b,0x097,0x091,0x099,0x092,0x090,0x093,0x09c,0x0b6,0x0b4,0x1c3,0x196,0x1cc,0x1c4,
	0x0a6,0x4b8,0x4c9,0x4cb,0x4bb,0x4ab,0x4b1,0x4b9,0x4b2,0x4a3,0x4b0,0x4b3,0x4bc,0x4c0,0x4c2,0x4ac,
	0x4a4,0x4a8,0x4a1,0x4ba,0x4a9,0x4a0,0x4c1,0x4aa,0x4ca,0x4a2,0x4c8,0x0a5,0x09f,0x0b5,0x095,0x1c6,
	0x000,0x8b8,0x8c9,0x8cb,0x8bb,0x8ab,0x8b1,0x8b9,0x8b2,0x8a3,0x8b0,0x8b3,0x8bc,0x8c0,0x8c2,0x8ac,
	0x8a4,0x8a8,0x8a1,0x8ba,0x8a9,0x8a0,0x8c1,0x8aa,0x8ca,0x8a2,0x8c8,0x000,0x000,0x000,0x000,0x000,
	// hiragana1
	0x2c8,0x2ca,0x2cb,0x2c1,0x2c9,0x2c2,0x2c0,0x2c3,0x2cc,0x2bc,0x2ac,0x29c,0x293,0x2a3,0x2b3,0x2bd,
	0x2cd,0x28d,0x2be,0x2b7,0x2b4,0x2bf,0x2cf,0x2c4,0x29d,0x28e,0x2c7,0x2ce,0x2a1,0x2a9,0x2b1,0x2b9,
	// katakana
	0x029a,0x11cc,0x11a5,0x11b5,0x11c3,0x11c4,0x1194,0x1197,0x11ab,0x1191,0x1199,0x1192,0x1190,0x1193,0x119c,0x11c8,
	0x109f,0x1097,0x10ab,0x1091,0x1099,0x1092,0x10a9,0x10b9,0x10b2,0x10b6,0x10c9,0x10ca,0x10bb,0x10a1,0x10a4,0x10cb,
	0x10a8,0x10b8,0x10c8,0x10aa,0x10ba,0x10a0,0x10a3,0x109a,0x10c3,0x10b3,0x10b1,0x10c1,0x109b,0x1095,0x1096,0x10b0,
	0x10c2,0x10b5,0x10c4,0x10c0,0x1090,0x1093,0x109c,0x10ac,0x10bc,0x10cc,0x10b4,0x10c6,0x1094,0x10a2,0x10a6,0x10a5,
	// hiragana2
	0x296,0x2a6,0x2b6,0x2c6,0x2a2,0x2a0,0x2b2,0x2b0,0x2a8,0x2b8,0x2aa,0x2ba,0x2ab,0x2bb,0x2c5,0x28f,
	0x2a7,0x29f,0x29b,0x297,0x291,0x299,0x292,0x290,0x2ad,0x294,0x2a4,0x295,0x2a5,0x20a2,0x2b5,0x000,
};
#endif

#endif	/* SDL_INPUT_H */
