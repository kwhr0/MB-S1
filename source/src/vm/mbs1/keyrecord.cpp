/** @file keyrecord.cpp

	Skelton for retropc emulator

	@author Sasaji
	@date   2015.09.12

	@brief [ key recording ]
*/

#include "keyrecord.h"
#include "../../emu.h"
#include "../device.h"
#include "../../config.h"
#include "../../fileio.h"
#include "../../utility.h"
#include "../../version.h"

#ifdef USE_KEY_RECORD
#define KEY_RECORD_SYSTEM_CODE	0x1000
#define KEY_RECORD_MAX			0x2000
#define KEY_RECORD_HEADER		"KEYRECORD_MBS1"
#define KEY_RECORD_HEADER_L3	"KEYRECORD_BML3MK5"
#endif

KEYRECORD::KEYRECORD(EMU* parent_emu)
{
	emu = parent_emu;
	mouse_stat = emu->mouse_buffer();

	fkro = new FILEIO();
	fkri = new FILEIO();
	config.reckey_recording = false;
	config.reckey_playing = false;
	key_rec_sum_clock = 0;
	memset(vm_key_recp_stat, 0, sizeof(vm_key_recp_stat));
	memset(vm_key_recr_stat, 0, sizeof(vm_key_recr_stat));
	memset(&mouse_recp_stat, 0, sizeof(mouse_recp_stat));
	memset(&mouse_recr_stat, 0, sizeof(mouse_recr_stat));
	memset(joypia_recp_stat, 0, sizeof(joypia_recp_stat));
	memset(joypia_recr_stat, 0, sizeof(joypia_recr_stat));
#ifdef USE_LIGHTPEN
	memset(lpen_recp_stat, 0, sizeof(lpen_recp_stat));
	memset(lpen_recr_stat, 0, sizeof(lpen_recr_stat));
#endif
#ifdef _DEBUG_KEYRECORD
	memset(vm_key_dbg_stat, 0, sizeof(vm_key_dbg_stat));
#endif
	clock_scale = 0;
}

KEYRECORD::~KEYRECORD()
{
	stop_reckey();

	delete fkro;
	delete fkri;
}

// ----------------------------------------------------------------------------
void KEYRECORD::reading_keys(int num)
{
#ifdef USE_KEY_RECORD
	if (config.reckey_playing) {
		uint64_t now_clk = dev->get_current_clock();
		int cols = -1;
		uint64_t clk = 0;
		int type = -1;
		int code[8];
		char *p = NULL;

		while(config.reckey_playing) {
			if (0x30 <= rec_key_rec_buff[0] && rec_key_rec_buff[0] <= 0x39) {
//#ifndef __MINGW32__
				cols = sscanf(rec_key_rec_buff,"%llu:%d:",&clk,&type);
//#else
//				cols = sscanf(rec_key_rec_buff,"%I64u:%d:",&clk,&type);
//#endif
				clk <<= clock_scale;

				if ((int64_t)clk + key_rec_sum_clock < 0) {
					clk = 0;
				} else {
					clk += key_rec_sum_clock;
				}
//				logging->out_logf(LOG_DEBUG,_T("RecKey:n:%llu:%llu"),now_clk,clk);
				if ((now_clk + 1) < clk) {
					break;
				}
//				logging->out_logf(LOG_DEBUG,_T("RecKey:keyin"));

				if (cols == 2 && clk > 0) {
					p = strchr(rec_key_rec_buff, ':');
					p = strchr(p+1, ':');
					switch(type) {
					case 1:
						// key
						cols = sscanf(p,":%x:%d",&code[0],&code[1]);
						if (cols == 2 && code[0] >= 0) {
							int code0 = (code[0] << 1) + 1;
#ifdef _DEBUG_KEYRECORD
//#ifndef __MINGW32__
							logging->out_logf(LOG_DEBUG, _T("RecKey%d %02x nc:%llu %c %llu ks:%02x nk:%d %c %d %s")
//#else
//							logging->out_logf(LOG_DEBUG, _T("RecKey%d %02x nc:%I64u %c %I64u ks:%02x nk:%d %c %d %s")
//#endif
								, num, code[0]
								, now_clk, (now_clk == clk ? _T('=') : _T('!')), clk
								, *key_scan_code_ptr
								, *counter_ptr, (*counter_ptr == code0 ? _T('=') : _T('!')), code0
								, (code[1] & 1) ? _T("ON") : _T("OFF"));
#endif
							if (code[0] < KEYBIND_KEYS) {
								// normal key
								vm_key_recp_stat[code[0]]=(code[1] & 1);
								if (num == 0 && (clk < now_clk || code0 < (*counter_ptr))) {
									// adjust timing
									*counter_ptr = code0;
									*key_scan_code_ptr = ((code0 >> 1) & ((*kb_mode_ptr & 0x08) ? 0x07 : 0x7f));

									logging->out_debugf(_T("RecKey%d %02x adjust k:%d ks:%02x"), num, code[0], *counter_ptr, *key_scan_code_ptr);
								}
							} else if (code[0] >= KEY_RECORD_SYSTEM_CODE && code[0] < KEY_RECORD_MAX) {
								if (code[1] & 1) {
									// global key
									if (clock_scale && (code[0] & 0xfff) == 'M') {
										// Mode Switch
										code[0] = (code[0] & 0xf000) | 'm';
									}
									emu->system_key_down(code[0] & 0xfff);
									emu->execute_global_keys(code[0] & 0xfff, 2);
								}
							}
						}
						break;
					case 2:
#ifdef USE_LIGHTPEN
						// lightpen
						cols = sscanf(p,":%d:%d:%x",&code[0],&code[1],&code[2]);
						if (cols == 3) {
							memcpy(lpen_recp_stat, code, sizeof(lpen_recp_stat));
						}
#endif
						break;
					case 3:
						// mouse
						cols = sscanf(p, ":%d:%d:%d:%d:%x:%x:%x:%x"
							,&code[0],&code[1],&code[2],&code[3],&code[4],&code[5],&code[6],&code[7]
						);
						if (cols == 8) {
							mouse_recp_stat[0].pos = code[0];
							mouse_recp_stat[0].dir = code[1];
							mouse_recp_stat[1].pos = code[2];
							mouse_recp_stat[1].dir = code[3];
							mouse_recp_stat[0].btn = code[4];
							mouse_recp_stat[0].prev_btn = code[5];
							mouse_recp_stat[1].btn = code[6];
							mouse_recp_stat[1].prev_btn = code[7];
						}
						break;
					case 4:
						// joystick on PIA
						cols = sscanf(p, ":%x:%x"
							,&code[0],&code[1]
						);
#ifdef _DEBUG_KEYRECORD
//#ifndef __MINGW32__
						logging->out_logf(LOG_DEBUG, _T("RecKey%d %llu %c %llu j0:%02x j1:%02x")
//#else
//						logging->out_logf(LOG_DEBUG, _T("RecKey%d %I64u %c %I64u j0:%02x j1:%02x")
//#endif
							, num
							, now_clk, (now_clk == clk ? _T('=') : _T('!')), clk
							, code[0], code[1]
						);
#endif
						if (cols == 2) {
							joypia_recp_stat[0] = (uint8_t)code[0];
							joypia_recp_stat[1] = (uint8_t)code[1];
						}
						break;
					}
				}
			}
			// read next data
			if (fkri->Fgets(rec_key_rec_buff, sizeof(rec_key_rec_buff)) == NULL) {
				stop_reckey(true, false);
				break;
			}
		}
	}
#endif /* USR_KEY_RECORD */
}

bool KEYRECORD::recording_keys(int code, bool pressed)
{
#ifdef USE_KEY_RECORD
	if (config.reckey_playing) {
		if (pressed != true) {
			// record key pressed ?
			if (vm_key_recp_stat[code]) {
				pressed = true;
			}
		}
#ifdef _DEBUG_KEYRECORD
		if ((vm_key_dbg_stat[code] != 0) != pressed) {
//#ifndef __MINGW32__
			logging->out_logf(LOG_DEBUG, _T("   Key0 %02x  c:%llu kc:%d kr:%d %s")
//#else
//			logging->out_logf(LOG_DEBUG, _T("   Key0 %02x  c:%I64u kc:%d kr:%d %s")
//#endif
				, code, dev->get_current_clock()
				, *counter_ptr, *remain_count_ptr, pressed ? _T("ON") : _T("OFF"));
			vm_key_dbg_stat[code] = (pressed ? 1 : 0);
		}
#endif
	}

	if (config.reckey_recording) {
		if (pressed && vm_key_recr_stat[code] == 0) {
//#ifndef __MINGW32__
			sprintf(rec_key_tmp_buff,"%llu:1:%04x:1\n"
//#else
//			sprintf(rec_key_tmp_buff,"%I64u:1:%04x:1\n"
//#endif
				, dev->get_current_clock(), code);
			fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
			vm_key_recr_stat[code] = 1;
		} else if (!pressed && vm_key_recr_stat[code] != 0) {
//#ifndef __MINGW32__
			sprintf(rec_key_tmp_buff,"%llu:1:%04x:0\n"
//#else
//			sprintf(rec_key_tmp_buff,"%I64u:1:%04x:0\n"
//#endif
				, dev->get_current_clock(), code);
			fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
			vm_key_recr_stat[code] = 0;
		}
	}
#endif /* USR_KEY_RECORD */
	return pressed;
}

void KEYRECORD::recording_system_keys(int code, int pressed)
{
#ifdef USE_KEY_RECORD
	if (config.reckey_recording) {
		code |= KEY_RECORD_SYSTEM_CODE;
		if (code < 0x2000) {
//#ifndef __MINGW32__
			sprintf(rec_key_tmp_buff,"%llu:1:%04x:%d\n"
//#else
//			sprintf(rec_key_tmp_buff,"%I64u:1:%04x:%d\n"
//#endif
				, dev->get_current_clock(), code, pressed);
			fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
		}
	}
#endif /* USR_KEY_RECORD */
}

void KEYRECORD::recording_mouse_status(t_mouse_stat *mst)
{
#ifdef USE_KEY_RECORD
	if (config.reckey_recording) {
		if (memcmp(mst, mouse_recr_stat, sizeof(mouse_recr_stat)) != 0) {
//#ifndef __MINGW32__
			sprintf(rec_key_tmp_buff,"%llu:3:%d:%d:%d:%d:%x:%x:%x:%x\n"
//#else
//			sprintf(rec_key_tmp_buff,"%I64u:3:%d:%d:%d:%d:%x:%x:%x:%x\n"
//#endif
				, dev->get_current_clock()
				,mst[0].pos,mst[0].dir,mst[1].pos,mst[1].dir,mst[0].btn,mst[0].prev_btn,mst[1].btn,mst[1].prev_btn
			);
			fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
			memcpy(mouse_recr_stat, mst, sizeof(mouse_recr_stat));
		}
	}
#endif /* USR_KEY_RECORD */
}

void KEYRECORD::recording_joypia_status(uint8_t *joystat)
{
#ifdef USE_KEY_RECORD
	if (config.reckey_playing) {
		joystat[0] = (joystat[0] & 0xf ? 0 : joypia_recp_stat[0] & 0xf) | (joypia_recp_stat[0] & 0xf0);
		joystat[1] = (joystat[1] & 0xf ? 0 : joypia_recp_stat[1] & 0xf) | (joypia_recp_stat[1] & 0xf0);
	}
	if (config.reckey_recording) {
		if (memcmp(joystat, joypia_recr_stat, sizeof(joypia_recr_stat)) != 0) {
//#ifndef __MINGW32__
			sprintf(rec_key_tmp_buff,"%llu:4:%02x:%02x\n"
//#else
//			sprintf(rec_key_tmp_buff,"%I64u:4:%02x:%02x\n"
//#endif
				, dev->get_current_clock()
				,joystat[0],joystat[1]
			);
			fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
			memcpy(joypia_recr_stat, joystat, sizeof(joypia_recr_stat));
		}
	}
#endif /* USR_KEY_RECORD */
}

#ifdef USE_LIGHTPEN
void KEYRECORD::recording_lightpen_status()
{
#ifdef USE_KEY_RECORD
	if (config.reckey_recording) {
		if (mouse_stat[2] != lpen_recr_stat[2]) {
//#ifndef __MINGW32__
			sprintf(rec_key_tmp_buff,"%llu:2:%d:%d:%x\n"
//#else
//			sprintf(rec_key_tmp_buff,"%I64u:2:%d:%d:%x\n"
//#endif
				, dev->get_current_clock(), mouse_stat[0], mouse_stat[1], mouse_stat[2] & 3);
			fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
			memcpy(lpen_recr_stat, mouse_stat, sizeof(lpen_recr_stat));
		}
	}
#endif /* USR_KEY_RECORD */
}
#endif /* USE_LIGHTPEN */

// ----------------------------------------------------------------------------
bool KEYRECORD::play_reckey(const _TCHAR* filename)
{
#ifdef USE_KEY_RECORD
	stop_reckey(true, false);

	char *p = NULL;
	int cols;
	int version;
	uint64_t start_clock;

	fkri->Fopen(filename ,FILEIO_READ_ASCII);
	if (fkri->IsOpened()) {
		clock_scale = 0;
		// check header
		p = fkri->Fgets(rec_key_tmp_buff, sizeof(rec_key_tmp_buff));
		if (p == NULL || strncmp(rec_key_tmp_buff, KEY_RECORD_HEADER, strlen(KEY_RECORD_HEADER)) != 0) {
			if (p == NULL || strncmp(rec_key_tmp_buff, KEY_RECORD_HEADER_L3, strlen(KEY_RECORD_HEADER_L3)) != 0) {
				// error
				logging->out_log_x(LOG_ERROR, CMsg::This_record_key_file_is_not_supported);
				goto FIN;
			} else {
				logging->out_logf_x(LOG_WARN, CMsg::The_record_key_file_for_VSTR_is_no_longer_supported, _T("BML3MK5"));
				clock_scale = 1;
			}
		}
		p = fkri->Fgets(rec_key_tmp_buff, sizeof(rec_key_tmp_buff));
		cols = sscanf(rec_key_tmp_buff, "Version:%d", &version);
		if (p == NULL || cols != 1 || version != 1) {
			// error
			logging->out_log_x(LOG_ERROR, CMsg::Record_key_file_is_invalid_version);
			goto FIN;
		}
		p = fkri->Fgets(rec_key_tmp_buff, sizeof(rec_key_tmp_buff));
//#ifndef __MINGW32__
		cols = sscanf(rec_key_tmp_buff, "StartClock:%llu", &start_clock);
//#else
//		cols = sscanf(rec_key_tmp_buff, "StartClock:%I64u", &start_clock);
//#endif
		if (p == NULL || cols != 1) {
			// error
			logging->out_log_x(LOG_ERROR, CMsg::Record_key_file_has_invalid_parameter);
			goto FIN;
		}
//		logging->out_logf(LOG_DEBUG, _T("RecordKey StartClock:%llu"), start_clock);
		start_clock <<= clock_scale;

		_TCHAR base_path[_MAX_PATH];

		UTILITY::get_dir_and_basename(filename, base_path, NULL);
		logging->out_debugf(_T("BasePath:%s"),base_path);

		// parse optional parameters
		_TCHAR *state_file = NULL;
		_TCHAR *tape_file = NULL;
		_TCHAR *disk_file[MAX_DRIVE];
		int disk_bank_num[MAX_DRIVE];
		bool tape_playing = true;
		int drv = 0;
		int major = 0;
		int minor = 0;
		int revision = 0;

		for(drv = 0; drv < MAX_DRIVE; drv++) {
			disk_file[drv] = NULL;
		}

		for(;;) {
			p = fkri->Fgets(rec_key_tmp_buff, sizeof(rec_key_tmp_buff));
			if (p == NULL || p[0] == '\r' || p[0] == '\n') {
				break;
			}
			if (strncmp(rec_key_tmp_buff, "StateFile:", 10) == 0) {
				get_file_path(base_path, &state_file, NULL);
				logging->out_debugf(_T("StateFile:%s"),state_file);
			} else if (strncmp(rec_key_tmp_buff, "TapeFile:", 9) == 0) {
				get_file_path(base_path, &tape_file, NULL);
				logging->out_debugf(_T("TapeFile:%s"),tape_file);
			} else if (strncmp(rec_key_tmp_buff, "TapeType:", 9) == 0) {
				if (strncmp(&rec_key_tmp_buff[9], "Rec", 3) == 0) {
					tape_playing = false;
				}
			} else if (sscanf(rec_key_tmp_buff, "Disk%dFile:", &drv) == 1) {
				if (0 <= drv && drv < MAX_DRIVE) {
					get_file_path(base_path, &disk_file[drv], &disk_bank_num[drv]);
					logging->out_debugf(_T("Disk%dFile:%s:%d"),drv,disk_file[drv],disk_bank_num[drv]);
				}
			} else if (sscanf(rec_key_tmp_buff, "EmulatorVersion:%d.%d.%d", &major, &minor, &revision) == 3) {
				if (clock_scale != 1) {
					logging->out_logf_x(LOG_INFO, CMsg::The_version_of_the_emulator_used_for_recording_is_VDIGIT_VDIGIT_VDIGIT, major, minor, revision);
				}
			}
		}

		// open files
		if (state_file) {
			emu->load_state(state_file);
			// set mode
			if (clock_scale) {
				emu->set_parami(VM::ParamSysMode, REG_SYS_MODE);
			}
		}
		if (tape_file) {
			if (tape_playing) {
				emu->play_datarec(tape_file);
			} else {
				emu->rec_datarec(tape_file);
			}
		}
		for(drv = 0; drv < MAX_DRIVE; drv++) {
			if (disk_file[drv]) {
				emu->open_disk_by_bank_num(drv, disk_file[drv], disk_bank_num[drv], 0, false);
			} else {
				emu->close_disk(drv);
			}
		}

		delete [] state_file;
		delete [] tape_file;
		for(drv = 0; drv < MAX_DRIVE; drv++) {
			delete [] disk_file[drv];
		}

		// adjust start clock
		key_rec_sum_clock = dev->get_current_clock() - start_clock;
		// check ok
		config.reckey_playing = true;

		// read first data
		memset(rec_key_rec_buff, 0, sizeof(rec_key_rec_buff));
		fkri->Fgets(rec_key_rec_buff, sizeof(rec_key_rec_buff));
//#ifndef __MINGW32__
		logging->out_debugf(_T("RecKeyStart: c:%llu s:%llu"), dev->get_current_clock(), start_clock);
//#else
//		logging->out_debugf(_T("RecKeyStart: c:%I64u s:%I64u"), dev->get_current_clock(), start_clock);
//#endif
	}
FIN:
#endif /* USR_KEY_RECORD */
	return config.reckey_playing;
}

bool KEYRECORD::record_reckey(const _TCHAR* filename)
{
#ifdef USE_KEY_RECORD
	stop_reckey(false, true);

	fkro->Fopen(filename ,FILEIO_WRITE_ASCII);
	if (fkro->IsOpened()) {
		config.reckey_recording = true;

		_TCHAR base_path[_MAX_PATH];
		char keyname[128];

		UTILITY::get_dir_and_basename(filename, base_path, NULL);

		// write header
		sprintf(rec_key_tmp_buff, "%s\n", KEY_RECORD_HEADER);
		fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
		sprintf(rec_key_tmp_buff, "Version:%d\n", 1);
		fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
//#ifndef __MINGW32__
		sprintf(rec_key_tmp_buff, "StartClock:%llu\n", dev->get_current_clock());
//#else
//		sprintf(rec_key_tmp_buff, "StartClock:%I64u\n", dev->get_current_clock());
//#endif
		fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
		sprintf(rec_key_tmp_buff, "EmulatorVersion:%d.%d.%d\n", APP_VER_MAJOR, APP_VER_MINOR, APP_VER_REV);
		fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);

		set_relative_path("StateFile", base_path, config.saved_state_path);

		if (set_relative_path("TapeFile", base_path, config.opened_datarec_path)) {
			if (emu->datarec_opened(true)) {
				sprintf(rec_key_tmp_buff, "TapeType:Play\n");
				fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
			} else if (emu->datarec_opened(false)) {
				sprintf(rec_key_tmp_buff, "TapeType:Rec\n");
				fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
			}
		}

		for(int drv = 0; drv < MAX_DRIVE; drv++) {
			sprintf(keyname, "Disk%dFile", drv);
			set_relative_path(keyname, base_path, config.opened_disk_path[drv]);
		}

		sprintf(rec_key_tmp_buff, "\n");
		fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);
	}
#endif /* USR_KEY_RECORD */
	return config.reckey_recording;
}

void KEYRECORD::stop_reckey(bool stop_play, bool stop_record)
{
#ifdef USE_KEY_RECORD
	if (stop_play && fkri) {
		fkri->Fclose();
		config.reckey_playing = false;
		memset(vm_key_recp_stat, 0, sizeof(vm_key_recp_stat));
		memset(mouse_recp_stat, 0, sizeof(mouse_recp_stat));
#ifdef USE_LIGHTPEN
		memset(lpen_recp_stat, 0, sizeof(lpen_recp_stat));
#endif
	}
	if (stop_record && fkro) {
		fkro->Fclose();
		config.reckey_recording = false;
		memset(vm_key_recr_stat, 0, sizeof(vm_key_recr_stat));
		memset(mouse_recr_stat, 0, sizeof(mouse_recr_stat));
#ifdef USE_LIGHTPEN
		memset(lpen_recr_stat, 0, sizeof(lpen_recr_stat));
#endif
	}
#endif /* USR_KEY_RECORD */
}

bool KEYRECORD::set_relative_path(const char *key, const _TCHAR *base_path, CRecentPath &path)
{
	if (path.path.Length() <= 0) return false;

	_TCHAR npath[_MAX_PATH];
	char mpath[_MAX_PATH];

	UTILITY::tcscpy(npath, _MAX_PATH, path.path);
	UTILITY::make_relative_path(base_path, npath);
	if (path.num > 0) config.set_number_in_path(npath, _MAX_PATH, path.num);
	UTILITY::cconv_from_native_path(npath, mpath, _MAX_PATH);
	sprintf(rec_key_tmp_buff, "%s:%s\n", key, mpath);

	fkro->Fwrite(rec_key_tmp_buff, strlen(rec_key_tmp_buff), 1);

	return true;
}

void KEYRECORD::get_file_path(const _TCHAR *base_path, _TCHAR **file_path, int *bank_num)
{
	if (!(*file_path)) (*file_path) = new _TCHAR[_MAX_PATH];
	memset((*file_path), 0, sizeof(_TCHAR) * _MAX_PATH);
	const char *ps = strchr(rec_key_tmp_buff, ':') + 1;
	UTILITY::conv_to_native_path(ps, (*file_path), _MAX_PATH);
	if (bank_num) {
		(*bank_num) = 0;
		config.get_number_in_path((*file_path), bank_num);
	}
	UTILITY::convert_path_separator(*file_path);
	UTILITY::make_absolute_path(base_path, *file_path);
}
