/** @file keyrecord.h

	Skelton for retropc emulator

	@author Sasaji
	@date   2015.09.12

	@brief [ key recording ]
*/

#ifndef KEYRECORD_H
#define KEYRECORD_H

#include "../vm_defs.h"
#include "../../common.h"
//#include "../../emu.h"
//#include "../device.h"
//#include "../../config.h"
//#include "../../fileio.h"

#ifdef _DEBUG
#define _DEBUG_KEYRECORD
#endif

class EMU;
class DEVICE;
class FILEIO;
class CRecentPath;

/// for mouse
typedef struct st_mst {
		int pos;		///< x/y axis
		uint8_t dir;		///< axis direction

		uint8_t btn;		///< button left/right  b7:on/off b6:trigger on/off
		uint8_t prev_btn;
} t_mouse_stat;

/// record a motion of key pressing/releasing
class KEYRECORD
{
private:
	EMU *emu;
	DEVICE *dev;

	int *mouse_stat;

	uint8_t *key_scan_code_ptr;
	int *counter_ptr;
	int *remain_count_ptr;
	uint8_t *kb_mode_ptr;

	// key record
	FILEIO *fkro, *fkri;
	uint8_t vm_key_recp_stat[KEYBIND_KEYS];
	uint8_t vm_key_recr_stat[KEYBIND_KEYS];
#ifdef USE_LIGHTPEN
	int   lpen_recp_stat[3];
	int   lpen_recr_stat[3];
#endif
	t_mouse_stat mouse_recp_stat[2];
	t_mouse_stat mouse_recr_stat[2];
	uint8_t joypia_recp_stat[2];
	uint8_t joypia_recr_stat[2];
	char  rec_key_tmp_buff[512];
	char  rec_key_rec_buff[512];
	int64_t key_rec_sum_clock;

	int clock_scale;

#ifdef _DEBUG
	uint8_t vm_key_dbg_stat[KEYBIND_KEYS];
#endif

	bool set_relative_path(const char *key, const _TCHAR *base_path, CRecentPath &path);
	void get_file_path(const _TCHAR *base_path, _TCHAR **file_path, int *bank_num);

public:
	KEYRECORD(EMU* parent_emu);
	~KEYRECORD();

	void reading_keys(int num);
	bool recording_keys(int code, bool pressed);
	void recording_system_keys(int code, int pressed);
	void recording_mouse_status(t_mouse_stat *mst);
	void recording_joypia_status(uint8_t *joy);
#ifdef USE_LIGHTPEN
	void recording_lightpen_status();
#endif

	bool play_reckey(const _TCHAR* filename);
	bool record_reckey(const _TCHAR* filename);
	void stop_reckey(bool stop_play = true, bool stop_record = true);

	void set_context(DEVICE* value) {
		dev = value;
	}
	void set_key_scan_code_ptr(uint8_t *value) {
		key_scan_code_ptr = value;
	}
	void set_counter_ptr(int *value) {
		counter_ptr = value;
	}
	void set_remain_count_ptr(int *value) {
		remain_count_ptr = value;
	}
	void set_kb_mode_ptr(uint8_t *value) {
		kb_mode_ptr = value;
	}
#ifdef USE_LIGHTPEN
	inline int get_lpen_recp_stat(int num) {
		return lpen_recp_stat[num];
	}
#endif
	inline t_mouse_stat *get_mouse_recp_stat() {
		return mouse_recp_stat;
	}
	inline uint8_t get_vm_key_recp_stat(int code) {
		return vm_key_recp_stat[code];
	}
};

#endif /* KEYRECORD_H */
