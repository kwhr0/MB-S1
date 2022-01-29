/** @file keyboard.h

	HITACHI MB-S1 Emulator 'EmuB-S1'
	Skelton for retropc emulator

	@author Sasaji
	@date   2011.06.08 -

	@brief [ keyboard ]
*/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../vm_defs.h"
//#include "../../emu.h"
#include "../device.h"
//#include "../../config.h"
//#include "keyrecord.h"

#ifdef _DEBUG
/* #define _DEBUG_KEYBOARD */
#endif

#define SIG_KEYBOARD_HSYNC		1

#define SIG_KEYBOARD_PIA_PA		2
#define SIG_KEYBOARD_PIA_PB		3

#define ADDR_KB_NMI		0xffc8
#define ADDR_LP_FLG		0xffcb
#define ADDR_KEYBOARD	0xffe0
#define ADDR_LPEN_BL	0xffd5

class EMU;
class KEYRECORD;

/**
	@brief keyboard
*/
class KEYBOARD : public DEVICE
{
private:
	DEVICE *d_cpu,*d_disp,*d_board;
#ifdef USE_KEY_RECORD
	KEYRECORD *reckey;
#endif
	DEVICE *d_pia;

	uint8_t* key_stat;
	uint8_t  scan_code;
	uint8_t  key_scan_code;
//	uint8_t  key_native_code;
	uint8_t  key_pressed;

	uint8_t kb_mode;
	uint8_t kb_nmi;	// break key

	// light pen
	int*	mouse_stat;
	uint8_t	lpen_flg;
	uint8_t	lpen_flg_prev;
	bool	lpen_bl;

#ifdef USE_JOYSTICK
	uint32_t *joy_stat;
#ifdef USE_PIAJOYSTICK
	int		joy_pia_sel;
	uint8_t   joy_pia[2];
#endif
#endif

	int	counter;	// keyboard counter
	int remain_count;
	int remain_count_max;

	int  *key_mod;
	bool pause_pressed;
	bool altkey_pressed;
	bool modesw_pressed;
	bool powersw_pressed;

	uint8_t vm_key_stat[KEYBIND_KEYS];

#ifdef _DEBUG_KEYBOARD
	int frame_counter;
	int event_counter;
#endif

	uint32_t scan2key_map[KEYBIND_KEYS][KEYBIND_ASSIGN];
	uint32_t scan2key_preset_map[KEYBIND_PRESETS][KEYBIND_KEYS][KEYBIND_ASSIGN];
	uint32_t scan2joy_map[KEYBIND_KEYS][KEYBIND_ASSIGN];
	uint32_t scan2joy_preset_map[KEYBIND_PRESETS][KEYBIND_KEYS][KEYBIND_ASSIGN];
	uint32_t sjoy2joy_map[KEYBIND_JOYS][KEYBIND_ASSIGN];
	uint32_t sjoy2joy_preset_map[KEYBIND_PRESETS][KEYBIND_JOYS][KEYBIND_ASSIGN];
//	bool now_autokey;

	int hsync_register_id;	// keyboad clock

	//for resume
#pragma pack(1)
	struct vm_state_st {
		uint8_t kb_mode;
		uint8_t kb_nmi;
		uint8_t lpen_flg;
		uint8_t lpen_flg_prev;
		uint8_t lpen_bl;

		uint8_t key_pressed;

		char  reserved1[2];

		int   counter;
		int   remain_count;
	};
#pragma pack()

	void clear_scan_code(int);
	void update_scan_code(int);
	void update_special_key();
#ifdef USE_LIGHTPEN
	void update_light_pen();
#endif
	void update_keyboard();
	void update_joy_pia();

	inline bool pressing_key(int);

	bool load_cfg_file();
	void save_cfg_file();
	bool load_ini_file();
	void save_ini_file();
public:
	KEYBOARD(VM* parent_vm, EMU* parent_emu, const char* identifier) : DEVICE(parent_vm, parent_emu, identifier)
	{
		set_class_name("KEYBOARD");
	}
	~KEYBOARD() {}

	// common functions
	void initialize();
	void reset();
	void release();
	void write_signal(int id, uint32_t data, uint32_t mask);
	void event_frame();

	void write_io8(uint32_t addr, uint32_t data);
	uint32_t read_io8(uint32_t addr);

	// unique functions
	void set_context_cpu(DEVICE* device) {
		d_cpu = device;
	}
	void set_context_disp(DEVICE* device) {
		d_disp = device;
	}
	void set_context_board(DEVICE* device) {
		d_board = device;
	}
#ifdef USE_KEY_RECORD
	void set_keyrecord(KEYRECORD *value) {
		reckey = value;
	}
#endif
	void set_context_pia(DEVICE* device) {
		d_pia = device;
	}
	uint8_t get_kb_mode();
	void update_system_key();
	void system_key_down(int code);
	void system_key_up(int code);

	void event_callback(int event_id, int err);

	void save_keybind();

	void update_config();

#ifdef USE_KEY_RECORD
	bool play_reckey(const _TCHAR* filename);
	bool record_reckey(const _TCHAR* filename);
	void stop_reckey(bool stop_play = true, bool stop_record = true);
#endif

	void save_state(FILEIO *fio);
	void load_state(FILEIO *fio);

#ifdef USE_DEBUGGER
	uint32_t debug_read_io8(uint32_t addr);
	void debug_event_frame();
	bool debug_write_reg(uint32_t reg_num, uint32_t data);
	void debug_regs_info(_TCHAR *buffer, size_t buffer_len);
#endif
};

#endif /* KEYBOARD_H */

