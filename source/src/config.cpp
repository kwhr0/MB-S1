/** @file config.cpp

	Skelton for retropc emulator

	@author Takeda.Toshiya
	@date   2006.08.18 -

	@note
	Modified for BML3MK5 by Sasaji at 2011.06.17
	Modified for MBS1 by Sasaji at 2015.08.16

	@brief [ config ]
*/

#include <math.h>
#include <ctype.h>
#include "config.h"
#include "fileio.h"
#include "emu.h"
#include "clocale.h"
#include "msgs.h"
#include "utility.h"
#include "simple_ini.h"

extern EMU *emu;

Config *pconfig = NULL;

//
// definition for ini file
//
#define SECTION_MAIN	_T("")
#define SECTION_CONTROL	_T("control")
#define SECTION_TAPE	_T("tape")
#define SECTION_FDD		_T("fdd")
#define SECTION_SCREEN	_T("screen")
#define SECTION_SOUND	_T("sound")
#define SECTION_PRINTER	_T("printer")
#define SECTION_STATE	_T("state")
#define SECTION_AUTOKEY	_T("autokey")
#define SECTION_MSGBOARD _T("messageboard")
#define SECTION_MENU	_T("menu")
#define SECTION_ROM     _T("rom")
#define SECTION_COMM    _T("comm")
#define SECTION_SNAPSHOT _T("snapshot")
#define SECTION_FONT	_T("font")
#define SECTION_LEDBOX	_T("ledbox") // add version2 12 on bml3mk5 / version2 4 on mbs1
#ifdef USE_DEBUGGER
#define SECTION_DEBUGGER _T("debugger")
#endif
#if defined(_MBS1) && defined(USE_Z80B_CARD)
#define SECTION_Z80BCARD _T("z80bcard")
#endif

//
//
//
void CDirPath::SetFromPath(const _TCHAR *file_path)
{
	_TCHAR dir_path[_MAX_PATH];

	UTILITY::get_dir_and_basename(file_path, dir_path, NULL);

	CTchar::Set(dir_path);
}

void CDirPath::Set(const _TCHAR *dir_path)
{
	_TCHAR path[_MAX_PATH];
	if (dir_path) {
		UTILITY::tcscpy(path, _MAX_PATH, dir_path);
		UTILITY::convert_path_separator(path);
		UTILITY::add_path_separator(path);
	} else {
		path[0] = _T('\0');
	}
	CTchar::Set(path);
}

//
//
//
void CFilePath::Set(const _TCHAR *file_path)
{
	_TCHAR path[_MAX_PATH];
	if (file_path) {
		UTILITY::tcscpy(path, _MAX_PATH, file_path);
		UTILITY::convert_path_separator(path);
	} else {
		path[0] = _T('\0');
	}
	CTchar::Set(path);
}

//
//
//
CRecentPath::CRecentPath()
	: path()
{
	this->num = 0;
}

CRecentPath::CRecentPath(const CRecentPath &src)
	: path(src.path)
{
	this->num = src.num;
}

CRecentPath::CRecentPath(const _TCHAR *srcpath, int srcnum)
	: path(srcpath)
{
	this->num = srcnum;
}

CRecentPath::~CRecentPath()
{
}

void CRecentPath::Set(const _TCHAR *srcpath, int srcnum)
{
	path.Set(srcpath);
	num = srcnum;
}

void CRecentPath::Clear()
{
	path.Clear();
	num = 0;
}

bool CRecentPath::Match(const _TCHAR *tagpath, int tagnum)
{
	return path.MatchString(tagpath) && (num == tagnum);
}

//
//
//
CRecentPathList::CRecentPathList()
	: CPtrList<CRecentPath>(MAX_HISTORY + 1)
{
	updated = false;
}

CRecentPathList::~CRecentPathList()
{
}

bool CRecentPathList::Match(const _TCHAR *tagpath, int tagnum)
{
	bool match = false;
	for(int i=0; i<Count(); i++) {
		if (Item(i)->Match(tagpath, tagnum)) {
			match = true;
			break;
		}
	}
	return match;
}

void CRecentPathList::Update(const _TCHAR *tagpath, int tagnum)
{
	int idx = Count() - 1;
	for(; idx >= 0; idx--) {
		if (Item(idx)->Match(tagpath, tagnum)) {
			break;
		}
	}
	if (idx >= 0) {
		// swap
		if (idx > 0) {
			CRecentPath *p = EraseFromList(idx);
			Insert(0, p);
		}
	} else {
		// insert to top
		if (Count() > 0) {
			Insert(0, new CRecentPath(tagpath, tagnum));
			Delete(MAX_HISTORY);
		} else {
			Add(new CRecentPath(tagpath, tagnum));
		}
	}
	updated = true;
}

//
//
//
Config::Config()
{
	ini = NULL;
}

Config::~Config()
{

}

void Config::initialize()
{
	// initial settings
	version1 = FILE_VERSION;
	version2 = CONFIG_VERSION;

#ifdef USE_FD1
#if defined(USE_FD8) || defined(USE_FD7)
	for (int i=0; i<8; i++) {
#elif defined(USE_FD6) || defined(USE_FD5)
	for (int i=0; i<6; i++) {
#else
	for (int i=0; i<4; i++) {
#endif
		recent_disk_path[i].updated = true;
	}
#endif

#ifdef USE_DATAREC
	recent_datarec_path.updated = true;
#endif

	window_mode = 0;
	window_position_x = 0x7fffffff;
	window_position_y = 0x7fffffff;
	stretch_screen = 0;
//	cutout_screen = false;
	pixel_aspect = 0;
	capture_type = 0;

	sound_frequency = 6;	// 48KHz
	sound_latency = 1;	// 100msec

	cpu_power = 1;
	now_power_off = false;
	use_power_off = false;

#ifdef USE_DIPSWITCH
	dipswitch = DIPSWITCH_DEFAULT;
#endif
#if defined(USE_MONITOR_TYPE) || defined(USE_SCREEN_ROTATE)
	monitor_type = 0;
#endif
#if defined(_MBS1)
	sys_mode = 0x07;
	mem_nowait = false;
	tvsuper = 0;		// digital
#endif
#ifdef USE_SCANLINE
	scan_line = 0;
#endif
#if defined(_BML3MK5) || defined(_MBS1)
	sync_irq = true;
	afterimage = 0;
#ifdef USE_KEEPIMAGE
	keepimage = 0;
#endif
	exram_size_num = 1;	// 64KB
	disptmg_skew = 0;
	curdisp_skew = 0;
#ifdef USE_DIRECT3D
	use_direct3d = 0;
	d3d_filter_type = 0;
#endif
#ifdef USE_OPENGL
	use_opengl = 0;
	gl_filter_type = 1;
#endif

	fdd_type = 0;
	mount_fdd = 0x3;
#if defined(_MBS1)
	io_port = IOPORT_MSK_KANJI | IOPORT_MSK_EXPSG | IOPORT_MSK_KEYBD | IOPORT_MSK_MOUSE;
#if defined(USE_Z80B_CARD)
	io_port |= IOPORT_MSK_Z80BCARD;
#endif
#else
	io_port = IOPORT_MSK_KANJI | IOPORT_MSK_PSG6;
#endif
	show_dlg = MSK_INSIDELEDBOX | MSK_SHOWMSGBOARD | MSK_SHOWLEDBOX | MSK_SHOWMSG_UNDEFOP;

	original = 0;

	delay_fdd = 0;
	check_fdmedia = 0;

	fps_no = -1;

	disp_device_no = 0;
	screen_width = 0;
	screen_height = 0;

#if defined(USE_WIN)
	msgboard_info_fontsize = 18;
	msgboard_msg_fontsize = 9;
#elif defined(USE_SDL) || defined(USE_SDL2)
	msgboard_info_fontname.Set(_T("ipagp.ttf"));
	msgboard_msg_fontname.Set(_T("ipagp.ttf"));
	msgboard_info_fontsize = 22;
	msgboard_msg_fontsize = 12;
#if defined(GUI_TYPE_AGAR)
	menu_fontname.Set(_T("ipagp.ttf"));
	menu_fontsize = 12;
#endif
#else
	msgboard_info_fontsize = 22;
	msgboard_msg_fontsize = 12;
#endif

	realmode_datarec = false;

	screen_video_size = 0;

	volume = 80;
	beep_volume = 80;
	psg6_volume = 80;
	psg9_volume = 80;
	relay_volume = 80;
	cmt_volume = 50;
	fdd_volume = 80;
	mute = false;
	beep_mute = false;
	psg6_mute = false;
	psg9_mute = false;
	relay_mute = false;
	cmt_mute = false;
	fdd_mute = false;
#if defined(_MBS1)
	psg_volume = 80;
	psgexfm_volume = 80;
	psgexssg_volume = 80;
	opnfm_volume = 80;
	opnssg_volume = 80;
	psg_mute = false;
	psgexfm_mute = false;
	psgexssg_mute = false;
	opnfm_mute = false;
	opnssg_mute = false;

	opn_clock = 0;
	opn_irq = 0;
	use_opn_expsg = false;
#endif

	for(int i=0; i<MAX_PRINTER; i++) {
		printer_server_host[i].Set(_T("localhost"));
		printer_server_port[i] = 10200;
		printer_online[i] = true;
		printer_direct[i] = false;
		printer_delay[i] = 1.0;
	}

	for(int i=0; i<MAX_COMM; i++) {
		comm_dipswitch[i] = 1;
		comm_server_host[i].Set(_T("localhost"));
		comm_server_port[i] = 6890 + i;
		comm_server[i] = false;
//		comm_connect[i] = false;
		comm_through[i] = false;
		comm_binary[i] = false;
	}
	comm_uart_baudrate = 9600;
	comm_uart_databit = 8;	// 7 or 8
	comm_uart_parity = 0;
	comm_uart_stopbit = 1; // 1:1bit 2:2bit
	comm_uart_flowctrl = 0;

	led_pos = 0;
	memset(led_dist, 0, sizeof(led_dist));

	reckey_recording = false;
	reckey_playing = false;

	wav_reverse = 0;
	wav_half = 1;
	wav_correct = 0;
	wav_correct_type = 0;
	wav_correct_amp[0] = 1000;
	wav_correct_amp[1] = 1000;

	wav_sample_rate = 3;
	wav_sample_bits = 0;
#endif
#ifdef _HC80
	device_type = 2;	// Nonintelligent ram disk
#endif
#ifdef _PC8801MA
	boot_mode = 2;	// V2 mode, 4MHz
	cpu_clock_low = true;
#endif
#ifdef USE_DIRECTINPUT
	use_direct_input = 0;
#endif
#ifdef USE_DEBUGGER
	debugger_imm_start = 0;
	debugger_server_host.Set(_T("localhost"));
	debugger_server_port = 54321;
#endif
#ifdef USE_PERFORMANCE_METER
	show_pmeter = false;
#endif
#if defined(_MBS1) && defined(USE_Z80B_CARD)
	z80b_card_out_irq = 0;
#endif
}

void Config::load(const _TCHAR *file)
{
	// initial settings
	initialize();

	ini_file.Set(file);

	//
	load_ini_file(ini_file);
}

void Config::save()
{
	//
	save_ini_file(ini_file);
}

void Config::release()
{
	delete ini;
	ini = NULL;
}

bool Config::load_ini_file(const _TCHAR *ini_file)
{
	_TCHAR section[100];
	_TCHAR key[100];
	_TCHAR base_path[_MAX_PATH];
	_TCHAR base_name[_MAX_PATH];
	long valuel;
	bool valueb;

	if (ini != NULL) {
		return true;
	}

	ini = new CSimpleIni();
//#ifdef _UNICODE
//	ini->SetUnicode(true);
//#endif
	UTILITY::get_dir_and_basename(ini_file, base_path, base_name);

	// load ini file
	bool rc;
	if ((rc = ini->LoadFile(ini_file)) != true) {
		logging->out_logf_x(LOG_WARN, CMsg::VSTR_couldn_t_be_loaded, base_name);
		return false;
	}

	// check config file version
	if (_tcscmp(ini->GetValue(SECTION_MAIN, _T("Name"), _T("")), _T(CLASS_NAME)) != 0) {
		logging->out_logf_x(LOG_WARN, CMsg::VSTR_is_invalid_file, base_name);
		return false;
	}

	version1 = (int)ini->GetLongValue(SECTION_MAIN, _T("Version1"));
	if(version1 != FILE_VERSION) {
		logging->out_logf_x(LOG_WARN, CMsg::VSTR_is_invalid_version, base_name);
		initialize();
		return false;
	}
	version2 = (int)ini->GetLongValue(SECTION_MAIN, _T("Version2"));
	if(version2 != CONFIG_VERSION) {
		logging->out_logf_x(LOG_INFO, CMsg::VSTR_is_old_version, base_name);
	}

	valuel = ini->GetLongValue(SECTION_CONTROL, _T("CpuPower"), cpu_power);
	if (0 <= valuel && valuel <= 5) {
		cpu_power = (uint8_t)valuel;
	}
	now_power_off = ini->GetBoolValue(SECTION_CONTROL, _T("NowPowerOff"), now_power_off);
	use_power_off = ini->GetBoolValue(SECTION_CONTROL, _T("UsePowerOff"), use_power_off);

#ifdef USE_DIPSWITCH
	dipswitch = (uint8_t)(ini->GetLongValue(SECTION_CONTROL, _T("DipSwitch"), dipswitch) & 0xff);
#endif

#ifdef USE_FD1
	get_dirpath_value(SECTION_FDD, _T("Path"), initial_disk_path);
	delay_fdd = (uint8_t)(ini->GetLongValue(SECTION_FDD, _T("IgnoreDelay"), delay_fdd) & 0xff);
	check_fdmedia = (uint8_t)(ini->GetLongValue(SECTION_FDD, _T("CheckMedia"), check_fdmedia) & 0xff);

#if defined(USE_FD8) || defined(USE_FD7)
	for (int i=0; i<8; i++) {
#elif defined(USE_FD6) || defined(USE_FD5)
	for (int i=0; i<6; i++) {
#else
	for (int i=0; i<4; i++) {
#endif
#if defined(_MBS1)
		UTILITY::stprintf(section, 100, _T("%s%d"), SECTION_FDD, i);
#else
		if (version2 < 8) {
			// old version is start base 1
			UTILITY::stprintf(section, 100, _T("%s%d"), SECTION_FDD, i+1);
		} else {
			UTILITY::stprintf(section, 100, _T("%s%d"), SECTION_FDD, i);
		}
#endif
		for (int j=0; j<MAX_HISTORY; j++) {
			UTILITY::stprintf(key, 100, _T("File%d"), j+1);
			get_recentpath_value(section, key, recent_disk_path[i]);
		}
		recent_disk_path[i].updated = true;

		valueb = ini->GetBoolValue(section, _T("MountWhenStartUp"), (mount_fdd & (1 << i)) != 0);
		mount_fdd = (valueb ? mount_fdd | (1 << i) : mount_fdd & ~(1 << i));
	}
#endif
#ifdef USE_DATAREC
	get_dirpath_value(SECTION_TAPE, _T("Path"), initial_datarec_path);
	for (int j=0; j<MAX_HISTORY; j++) {
		UTILITY::stprintf(key, 100, _T("File%d"), j+1);
		get_recentpath_value(SECTION_TAPE, key, recent_datarec_path);
	}
	recent_datarec_path.updated = true;
	realmode_datarec = ini->GetBoolValue(SECTION_TAPE, _T("RealMode"), realmode_datarec);
#if defined(_BML3MK5) || defined(_MBS1)
	wav_reverse = ini->GetBoolValue(SECTION_TAPE, _T("LoadWavReverse"), wav_reverse);
	wav_half = ini->GetBoolValue(SECTION_TAPE, _T("LoadWavHalf"), wav_half);
	wav_correct = ini->GetBoolValue(SECTION_TAPE, _T("LoadWavCorrect"), wav_correct);
	valuel = ini->GetLongValue(SECTION_TAPE, _T("LoadWavCorrectType"), wav_correct_type);
	if (0 <= valuel && valuel <= 1) {
		wav_correct_type = (uint8_t)valuel;
	}
	valuel = ini->GetLongValue(SECTION_TAPE, _T("LoadWavCorrectAmp0"), wav_correct_amp[0]);
	if (100 <= valuel && valuel <= 5000) {
		wav_correct_amp[0] = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_TAPE, _T("LoadWavCorrectAmp1"), wav_correct_amp[1]);
	if (100 <= valuel && valuel <= 5000) {
		wav_correct_amp[1] = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_TAPE, _T("SaveWavSampleRate"), wav_sample_rate);
	if (0 <= valuel && valuel <= 3) {
		wav_sample_rate = (uint8_t)valuel;
	}
	valuel = ini->GetLongValue(SECTION_TAPE, _T("SaveWavSampleBits"), wav_sample_bits);
	if (0 <= valuel && valuel <= 1) {
		wav_sample_bits = (uint8_t)valuel;
	}
#endif
#endif

	window_mode = (int)ini->GetLongValue(SECTION_SCREEN, _T("WindowMode"), window_mode);
	window_position_x = (int)ini->GetLongValue(SECTION_SCREEN, _T("WindowPositionX"), window_position_x);
	window_position_y = (int)ini->GetLongValue(SECTION_SCREEN, _T("WindowPositionY"), window_position_y);
#if defined(_BML3MK5) || defined(_MBS1)
	disp_device_no = (int)ini->GetLongValue(SECTION_SCREEN, _T("DisplayDevice"), disp_device_no);
	screen_width = (int)ini->GetLongValue(SECTION_SCREEN, _T("ScreenWidth"), screen_width);
	screen_height = (int)ini->GetLongValue(SECTION_SCREEN, _T("ScreenHeight"), screen_height);
#endif
#if defined(_MBS1)
	if (version2 >= 5)
#else
	if (version2 >= 13)
#endif
	{
		valuel = ini->GetLongValue(SECTION_SCREEN, _T("StretchScreen"), stretch_screen);
		if (0 <= valuel && valuel <= 2) {
			stretch_screen = (uint8_t)valuel;
		}
	} else {
		stretch_screen = ini->GetBoolValue(SECTION_SCREEN, _T("StretchScreen")) ? 1 : 0;
		stretch_screen = ini->GetBoolValue(SECTION_SCREEN, _T("CutoutScreen")) ? 2 : stretch_screen;
	}
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("PixelAspect"), pixel_aspect);
	if (0 <= valuel && valuel <= 2) {
		pixel_aspect = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("CaptureType"), capture_type);
	if (0 <= valuel && valuel <= 1) {
		capture_type = (int)valuel;
	}

	sound_frequency = (int)ini->GetLongValue(SECTION_SOUND,  _T("FrequencyNo"), sound_frequency);
	sound_latency = (int)ini->GetLongValue(SECTION_SOUND,  _T("LatencyNo"), sound_latency);

#if defined(USE_MONITOR_TYPE) || defined(USE_SCREEN_ROTATE)
	monitor_type = ini->GetLongValue(SECTION_SCREEN,  _T("MonitorType"), monitor_type);
	monitor_type &= 3;
#endif
#ifdef USE_SCANLINE
#if defined(_MBS1)
	if (version2 >= 5)
#else
	if (version2 >= 13)
#endif
	{
		valuel = ini->GetLongValue(SECTION_SCREEN, _T("ScanLine"), scan_line);
		if (0 <= valuel && valuel <= 3) {
			scan_line = (uint8_t)valuel;
		}
	} else {
		scan_line = ini->GetBoolValue(SECTION_SCREEN, _T("ScanLine")) ? 1 : 0;
	}
#endif

#ifdef _MBS1
	sys_mode = (uint8_t)ini->GetLongValue(SECTION_CONTROL, _T("SystemMode"), sys_mode);
	mem_nowait = ini->GetBoolValue(SECTION_CONTROL, _T("MemoryNoWait"), mem_nowait);
	tvsuper = (ini->GetBoolValue(SECTION_SCREEN, _T("AnalogRGB"), false) ? 0x10 : 0);
#endif
#if defined(_BML3MK5) || defined(_MBS1)
	sync_irq = ini->GetBoolValue(SECTION_CONTROL, _T("SyncIRQ"), sync_irq);
	valuel = ini->GetLongValue(SECTION_CONTROL, _T("ExtendedRamSize"), exram_size_num);
#if defined(_MBS1)
	if (0 <= valuel && valuel <= 4)
#else
	if (0 <= valuel && valuel <= 1)
#endif
	{
		exram_size_num = (uint8_t)valuel;
	}

	valuel = ini->GetLongValue(SECTION_SCREEN, _T("AfterImage"), afterimage);
	if (0 <= valuel && valuel <= 2) {
		afterimage = (uint8_t)valuel;
	}
#ifdef USE_KEEPIMAGE
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("KeepImage"), keepimage);
	if (0 <= valuel && valuel <= 2) {
		keepimage = (uint8_t)valuel;
	}
#endif
#ifdef USE_DIRECT3D
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("UseDirect3D"), use_direct3d);
	if (0 <= valuel && valuel <= 2) {
		use_direct3d = (uint8_t)valuel;
	}
	ini->Delete(SECTION_SCREEN, _T("UseOpenGL"));	// not use in this app
#endif
#ifdef USE_OPENGL
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("UseOpenGL"), use_opengl);
	if (0 <= valuel && valuel <= 2) {
		use_opengl = (uint8_t)valuel;
	}
	ini->Delete(SECTION_SCREEN, _T("UseDirect3D"));	// not use in this app
#endif
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("DisptmgSkew"), disptmg_skew);
#if defined(_MBS1)
	if (-2 <= valuel && valuel <= 2)
#else
	if (0 <= valuel && valuel <= 2)
#endif
	{
		disptmg_skew = (int8_t)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("CurdispSkew"), curdisp_skew);
#if defined(_MBS1)
	if (-2 <= valuel && valuel <= 2)
#else
	if (0 <= valuel && valuel <= 2)
#endif
	{
		curdisp_skew = (int8_t)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("VideoSize"), screen_video_size);
	if (0 <= valuel && valuel <= 1) {
		screen_video_size = (uint8_t)valuel;
	}
#ifdef USE_DIRECT3D
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("D3DFilterType"), d3d_filter_type);
	if (0 <= valuel && valuel <= 2) {
		d3d_filter_type = (uint8_t)valuel;
	}
	ini->Delete(SECTION_SCREEN, _T("GLFilterType"));	// not use in this app
#endif
#ifdef USE_OPENGL
	valuel = ini->GetLongValue(SECTION_SCREEN, _T("GLFilterType"), gl_filter_type);
	if (0 <= valuel && valuel <= 1) {
		gl_filter_type = (uint8_t)valuel;
	}
	ini->Delete(SECTION_SCREEN, _T("D3DFilterType"));	// not use in this app
#endif
	get_str_value(SECTION_SCREEN, _T("Language"), language);

	valuel = ini->GetLongValue(SECTION_SOUND,  _T("Volume"), volume);
	if (0 <= valuel && valuel <= 100) {
		volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_CONTROL, _T("FddType"), fdd_type);
	if (0 <= valuel && valuel <= 3) {
		fdd_type = (int)valuel;
	}
#if defined(_MBS1)
	valuel = ini->GetLongValue(SECTION_CONTROL, _T("IoPort"), io_port);
	if (version2 < 0x02) {
		valuel |= IOPORT_MSK_EXPSG; // use ex psg
	}
	if (version2 < 0x06) {
		valuel |= (IOPORT_MSK_KEYBD | IOPORT_MSK_MOUSE); // use keyboard and mouse
	}
	io_port = (int)(valuel & IOPORT_MSK_ALL);
#else
	io_port = ini->GetLongValue(SECTION_CONTROL, _T("IoPort"), io_port) & IOPORT_MSK_ALL;
#endif
	original = (int)ini->GetLongValue(SECTION_CONTROL, _T("OriginalSettings"), original);

	valueb = ini->GetBoolValue(SECTION_CONTROL, _T("ShowMessage"), FLG_SHOWMSGBOARD ? true : false);
	show_dlg = valueb ? (show_dlg | MSK_SHOWMSGBOARD) : (show_dlg & ~MSK_SHOWMSGBOARD);
#if defined(_MBS1)
	if (version2 < 0x03)
#else
	if (version2 < 15)
#endif
	{
		valueb = ini->GetBoolValue(SECTION_CONTROL, _T("UseJoystick"), FLG_USEJOYSTICK ? true : false);
		show_dlg = valueb ? (show_dlg | MSK_USEJOYSTICK) : (show_dlg & ~MSK_USEJOYSTICK);
	} else {
		valuel = ini->GetLongValue(SECTION_CONTROL, _T("UseJoystick"), FLG_USEJOYSTICK ? 1 : (FLG_USEPIAJOYSTICK ? 2 : 0));
		show_dlg = (valuel == 1 ? show_dlg | MSK_USEJOYSTICK : (valuel == 2 ? show_dlg | MSK_USEPIAJOYSTICK : show_dlg));
	}
#if defined(_MBS1)
	valueb = ini->GetBoolValue(SECTION_CONTROL, _T("EnableMouse"), FLG_USEMOUSE ? true : false);
	show_dlg = valueb ? (show_dlg | MSK_USEMOUSE) : (show_dlg & ~MSK_USEMOUSE);
#else
	valueb = ini->GetBoolValue(SECTION_CONTROL, _T("EnableLightpen"), FLG_USELIGHTPEN ? true : false);
	show_dlg = valueb ? (show_dlg | MSK_USELIGHTPEN) : (show_dlg & ~MSK_USELIGHTPEN);
#endif
	valueb = ini->GetBoolValue(SECTION_CONTROL, _T("ShowMessageUndefOpcode"), FLG_SHOWMSG_UNDEFOP ? true : false);
	show_dlg = valueb ? (show_dlg | MSK_SHOWMSG_UNDEFOP) : (show_dlg & ~MSK_SHOWMSG_UNDEFOP);

	valuel = ini->GetLongValue(SECTION_CONTROL, _T("FpsNo"), fps_no);
	if (-1 <= valuel && valuel <= 6) {
		fps_no = (int)valuel;
	}

	get_dirpath_value(SECTION_PRINTER, _T("Path"), initial_printer_path);
	get_dirpath_value(SECTION_STATE, _T("Path"), initial_state_path);
	for (int j=0; j<MAX_HISTORY; j++) {
		UTILITY::stprintf(key, 100, _T("File%d"), j+1);
		get_recentpath_value(SECTION_STATE, key, recent_state_path);
	}
	recent_state_path.updated = true;
#ifdef USE_AUTO_KEY
	get_dirpath_value(SECTION_AUTOKEY, _T("Path"), initial_autokey_path);
#endif
	get_str_value(SECTION_MSGBOARD, _T("InfoFontName"), msgboard_info_fontname);
	get_str_value(SECTION_MSGBOARD, _T("MessageFontName"), msgboard_msg_fontname);
	ini->Delete(SECTION_MSGBOARD, _T("FontName"));	// no longer support

	valuel = ini->GetLongValue(SECTION_MSGBOARD, _T("InfoFontSize"), msgboard_info_fontsize);
	if (1 <= valuel && valuel <= 60) {
		msgboard_info_fontsize = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_MSGBOARD, _T("MessageFontSize"), msgboard_msg_fontsize);
	if (1 <= valuel && valuel <= 60) {
		msgboard_msg_fontsize = (int)valuel;
	}
#if defined(GUI_TYPE_AGAR)
	get_str_value(SECTION_MENU, _T("FontName"), menu_fontname);
	valuel = ini->GetLongValue(SECTION_MENU, _T("FontSize"), menu_fontsize);
	if (1 <= valuel && valuel <= 60) {
		menu_fontsize = (int)valuel;
	}
#endif

	get_dirpath_value(SECTION_ROM, _T("Path"), rom_path);

	valuel = ini->GetLongValue(SECTION_SOUND, _T("BeepVolume"), beep_volume);
	if (0 <= valuel && valuel <= 100) {
		beep_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("6VoicePsgVolume"), psg6_volume);
	if (0 <= valuel && valuel <= 100) {
		psg6_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("9VoicePsgVolume"), psg9_volume);
	if (0 <= valuel && valuel <= 100) {
		psg9_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("RelayVolume"), relay_volume);
	if (0 <= valuel && valuel <= 100) {
		relay_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("CmtVolume"), cmt_volume);
	if (0 <= valuel && valuel <= 100) {
		cmt_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("FddVolume"), fdd_volume);
	if (0 <= valuel && valuel <= 100) {
		fdd_volume = (int)valuel;
	}
#if defined(_MBS1)
	valuel = ini->GetLongValue(SECTION_SOUND, _T("PsgVolume"), psg_volume);
	if (0 <= valuel && valuel <= 100) {
		psg_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("ExPsgFmVolume"), psgexfm_volume);
	if (0 <= valuel && valuel <= 100) {
		psgexfm_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("ExPsgSSGVolume"), psgexssg_volume);
	if (0 <= valuel && valuel <= 100) {
		psgexssg_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("OPNFmVolume"), opnfm_volume);
	if (0 <= valuel && valuel <= 100) {
		opnfm_volume = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("OPNSSGVolume"), opnssg_volume);
	if (0 <= valuel && valuel <= 100) {
		opnssg_volume = (int)valuel;
	}
#endif
	mute = ini->GetBoolValue(SECTION_SOUND, _T("Mute"), mute);
	beep_mute = ini->GetBoolValue(SECTION_SOUND, _T("BeepMute"), beep_mute);
	psg6_mute = ini->GetBoolValue(SECTION_SOUND, _T("6VoicePsgMute"), psg6_mute);
	psg9_mute = ini->GetBoolValue(SECTION_SOUND, _T("9VoicePsgMute"), psg9_mute);
	relay_mute = ini->GetBoolValue(SECTION_SOUND, _T("RelayMute"), relay_mute);
	cmt_mute = ini->GetBoolValue(SECTION_SOUND, _T("CmtMute"), cmt_mute);
	fdd_mute = ini->GetBoolValue(SECTION_SOUND, _T("FddMute"), fdd_mute);
#if defined(_MBS1)
	psg_mute = ini->GetBoolValue(SECTION_SOUND, _T("PsgMute"), psg_mute);
	psgexfm_mute = ini->GetBoolValue(SECTION_SOUND, _T("ExPsgFmMute"), psgexfm_mute);
	psgexssg_mute = ini->GetBoolValue(SECTION_SOUND, _T("ExPsgSSGMute"), psgexssg_mute);
	opnfm_mute = ini->GetBoolValue(SECTION_SOUND, _T("OPNFmMute"), opnfm_mute);
	opnssg_mute = ini->GetBoolValue(SECTION_SOUND, _T("OPNSSGMute"), opnssg_mute);

	valuel = ini->GetLongValue(SECTION_SOUND, _T("OPNClock"), opn_clock);
	if (0 <= valuel && valuel <= 3) {
		opn_clock = (int)valuel;
	}
	valuel = ini->GetLongValue(SECTION_SOUND, _T("OPNInterrupt"), opn_irq);
	if (0 <= valuel && valuel <= 2) {
		opn_irq = (int)valuel;
	}
	use_opn_expsg = ini->GetBoolValue(SECTION_SOUND, _T("UseOPNOnExPsg"), use_opn_expsg);
#endif

	for (int i=0; i<MAX_PRINTER; i++) {
		UTILITY::stprintf(section, 100, _T("%s%d"), SECTION_PRINTER, i);

		get_str_value(section, _T("ServerHost"), printer_server_host[i]);
		valuel = ini->GetLongValue(section, _T("ServerPort"), printer_server_port[i]);
		if (0 <= valuel && valuel <= 65535) {
			printer_server_port[i] = (int)valuel;
		}
		double valued = ini->GetDoubleValue(section, _T("DelayMsec"), printer_delay[i]);
		if (0.1 <= valued && valued <= 1000.0) {
			printer_delay[i] = valued;
		}
		printer_online[i] = ini->GetBoolValue(section, _T("Online"), printer_online[i]);
	}

	valuel = ini->GetLongValue(SECTION_COMM, _T("BaudRate"), comm_uart_baudrate);
	if (valuel > 0) comm_uart_baudrate = (int)valuel;
	valuel = ini->GetLongValue(SECTION_COMM, _T("DataBit"), comm_uart_databit);
	if (valuel >= 7 && valuel <= 8) comm_uart_databit = (int)valuel;
	valuel = ini->GetLongValue(SECTION_COMM, _T("Parity"), comm_uart_parity);
	if (valuel >= 0 && valuel <= 2) comm_uart_parity = (int)valuel;
	valuel = ini->GetLongValue(SECTION_COMM, _T("StopBit"), comm_uart_stopbit);
	if (valuel >= 1 && valuel <= 2) comm_uart_stopbit = (int)valuel;
	valuel = ini->GetLongValue(SECTION_COMM, _T("FlowControl"), comm_uart_flowctrl);
	if (valuel >= 0 && valuel <= 2) comm_uart_flowctrl = (int)valuel;

	for (int i=0; i<MAX_COMM; i++) {
		UTILITY::stprintf(section, 100, _T("%s%d"), SECTION_COMM, i);
		valuel = ini->GetLongValue(section, _T("DipSwitch"), comm_dipswitch[i]);
		if (1 <= valuel && valuel <= 4) {
			comm_dipswitch[i] = (int)valuel;
		}
		get_str_value(section, _T("ServerHost"), comm_server_host[i]);
		valuel = ini->GetLongValue(section, _T("ServerPort"), comm_server_port[i]);
		if (0 <= valuel && valuel <= 65535) {
			comm_server_port[i] = (int)valuel;
		}
		comm_through[i] = ini->GetBoolValue(section, _T("ThroughMode"), comm_through[i]);
	}
	get_dirpath_value(SECTION_SNAPSHOT, _T("Path"), snapshot_path);

#if defined(_MBS1)
	if (version2 >= 4)
#else
	if (version2 >= 12)
#endif
	{
		valueb = ini->GetBoolValue(SECTION_LEDBOX, _T("Show"), FLG_SHOWLEDBOX ? true : false);
		show_dlg = valueb ? (show_dlg | MSK_SHOWLEDBOX) : (show_dlg & ~MSK_SHOWLEDBOX);
		valueb = ini->GetBoolValue(SECTION_LEDBOX, _T("Inside"), FLG_INSIDELEDBOX ? true : false);
		show_dlg = valueb ? (show_dlg | MSK_INSIDELEDBOX) : (show_dlg & ~MSK_INSIDELEDBOX);
		valuel = ini->GetLongValue(SECTION_LEDBOX, _T("Position"), led_pos);
		if (0 <= valuel && valuel <= 3) {
			led_pos = (uint8_t)valuel;
		}
		led_dist[0].x = (int)ini->GetLongValue(SECTION_LEDBOX, _T("DistanceOnWindowX"), led_dist[0].x);
		led_dist[0].y = (int)ini->GetLongValue(SECTION_LEDBOX, _T("DistanceOnWindowY"), led_dist[0].y);
		led_dist[1].x = (int)ini->GetLongValue(SECTION_LEDBOX, _T("DistanceOnFullscreenX"), led_dist[1].x);
		led_dist[1].y = (int)ini->GetLongValue(SECTION_LEDBOX, _T("DistanceOnFullscreenY"), led_dist[1].y);
	} else {
		valueb = ini->GetBoolValue(SECTION_CONTROL, _T("ShowLED"), FLG_SHOWLEDBOX ? true : false);
		show_dlg = valueb ? (show_dlg | MSK_SHOWLEDBOX) : (show_dlg & ~MSK_SHOWLEDBOX);
		valueb = ini->GetBoolValue(SECTION_CONTROL, _T("InsideLED"), FLG_INSIDELEDBOX ? true : false);
		show_dlg = valueb ? (show_dlg | MSK_INSIDELEDBOX) : (show_dlg & ~MSK_INSIDELEDBOX);
		valuel = ini->GetLongValue(SECTION_CONTROL, _T("LEDPosition"), led_pos);
		if (0 <= valuel && valuel <= 3) {
			led_pos = (uint8_t)valuel;
		}
	}
#if defined(USE_WIN)
	get_filepath_value(SECTION_FONT, _T("File"), font_path);
#endif
#if defined(USE_SDL) || defined(USE_SDL2)
	get_dirpath_value(SECTION_FONT, _T("Path"), font_path);
#endif

	if (fdd_type == FDD_TYPE_3FDD) {
		// use 3inch fdd
		io_port = (io_port & ~IOPORT_MSK_FDDALL) | IOPORT_MSK_3FDD;
	} else if (fdd_type == FDD_TYPE_5FDD
			|| fdd_type == FDD_TYPE_58FDD)
	{
		// use 5inch/8inch fdd
		io_port = (io_port & ~IOPORT_MSK_FDDALL) | IOPORT_MSK_5FDD;
	}

	// convert from old version data
#if defined(_MBS1)
	if (version2 < 7)
#else
	if (version2 < 8) {
		cpu_power++;
	}
	if (version2 < 14)
#endif
	{
		volume = conv_volume(volume);
		beep_volume = conv_volume(beep_volume);
		psg6_volume = conv_volume(psg6_volume);
		psg9_volume = conv_volume(psg9_volume);
		relay_volume = conv_volume(relay_volume);
		cmt_volume = conv_volume(cmt_volume);
		fdd_volume = conv_volume(fdd_volume);
#if defined(_MBS1)
		psg_volume = conv_volume(psg_volume);
#endif
	}

#endif
#ifdef USE_DIRECTINPUT
	valueb = (use_direct_input != 0);
	use_direct_input = ini->GetBoolValue(SECTION_CONTROL, _T("UseDirectInput"), valueb) ? 1 : 0;
#endif
#ifdef USE_DEBUGGER
	valuel = ini->GetLongValue(SECTION_DEBUGGER, _T("ImmediateStart"), debugger_imm_start);
	if (0 <= valuel && valuel <= 1) {
		debugger_imm_start = (int)valuel;
	}
	get_str_value(SECTION_DEBUGGER, _T("ServerHost"), debugger_server_host);
	valuel = ini->GetLongValue(SECTION_DEBUGGER, _T("ServerPort"), debugger_server_port);
	if (0 <= valuel && valuel <= 65535) {
		debugger_server_port = (int)valuel;
	}
#endif
#ifdef USE_PERFORMANCE_METER
	show_pmeter = ini->GetBoolValue(SECTION_CONTROL, _T("ShowPerformanceMeter"), show_pmeter);
#endif

#if defined(_MBS1) && defined(USE_Z80B_CARD)
	valuel = ini->GetLongValue(SECTION_Z80BCARD, _T("OutputInterrupt"), z80b_card_out_irq);
	if (0 <= valuel && valuel <= 1) {
		z80b_card_out_irq = (int)valuel;
	}
	if (version2 < 10) {
		io_port |= IOPORT_MSK_Z80BCARD;
	}
#endif

	version2 = CONFIG_VERSION;

	logging->out_logf_x(LOG_INFO, CMsg::VSTR_was_loaded, base_name);

	return true;
}

int Config::conv_volume(int vol)
{
	return (int)(100.0 *log((double)(vol+1))/log((double)151.0));
}

void Config::save_ini_file(const _TCHAR *ini_file)
{
	_TCHAR comment[100];
	_TCHAR section[100];
	_TCHAR key[100];
	_TCHAR base_path[_MAX_PATH];
	_TCHAR base_name[_MAX_PATH];
	_TCHAR buf[_MAX_PATH];

	if (ini == NULL) {
		return;
	}

	UTILITY::get_dir_and_basename(ini_file, base_path, base_name);

	// section
	UTILITY::stprintf(comment, 100, _T("; %s config file"), _T(DEVICE_NAME));

	// values
	ini->SetValue(SECTION_MAIN, _T("Name"), _T(CLASS_NAME), comment);
	ini->SetValue(SECTION_MAIN, _T("Encording"), _T("UTF-8"));
	ini->SetLongValue(SECTION_MAIN, _T("Version1"), version1, NULL, true);
	ini->SetLongValue(SECTION_MAIN, _T("Version2"), version2);

	ini->SetLongValue(SECTION_CONTROL, _T("CpuPower"), cpu_power);
	ini->SetBoolValue(SECTION_CONTROL, _T("NowPowerOff"), now_power_off);
	ini->SetBoolValue(SECTION_CONTROL, _T("UsePowerOff"), use_power_off);

#ifdef USE_DIPSWITCH
	ini->SetLongValue(SECTION_CONTROL, _T("DipSwitch"), dipswitch, NULL, true);
#endif


#ifdef USE_FD1
	ini->SetValue(SECTION_FDD, _T("Path"), conv_from_npath(initial_disk_path));
	ini->SetLongValue(SECTION_FDD, _T("IgnoreDelay"), delay_fdd);
	ini->SetLongValue(SECTION_FDD, _T("CheckMedia"), check_fdmedia);

#if defined(USE_FD8) || defined(USE_FD7)
	for (int i=0; i<8; i++) {
#elif defined(USE_FD6) || defined(USE_FD5)
	for (int i=0; i<6; i++) {
#else
	for (int i=0; i<4; i++) {
#endif
		UTILITY::stprintf(section, 100, _T("%s%d"), SECTION_FDD, i);
		for (int j=0; j<MAX_HISTORY && j<recent_disk_path[i].Count(); j++) {
			UTILITY::stprintf(key, 100, _T("File%d"), j+1);
			UTILITY::tcscpy(buf, _MAX_PATH, recent_disk_path[i][j]->path);
			set_number_in_path(buf, _MAX_PATH, recent_disk_path[i][j]->num);
			ini->SetValue(section, key, conv_from_npath(buf));
		}
		ini->SetBoolValue(section, _T("MountWhenStartUp"), (mount_fdd & (1 << i)) != 0);
	}
#endif
#ifdef USE_DATAREC
	ini->SetValue(SECTION_TAPE, _T("Path"), conv_from_npath(initial_datarec_path));
	for (int j=0; j<MAX_HISTORY && j<recent_datarec_path.Count(); j++) {
		UTILITY::stprintf(key, 100, _T("File%d"), j+1);
		ini->SetValue(SECTION_TAPE, key, conv_from_npath(recent_datarec_path[j]->path));
	}
	ini->SetBoolValue(SECTION_TAPE, _T("RealMode"), realmode_datarec);
#if defined(_BML3MK5) || defined(_MBS1)
	ini->SetBoolValue(SECTION_TAPE, _T("LoadWavReverse"), wav_reverse);
	ini->SetBoolValue(SECTION_TAPE, _T("LoadWavHalf"), wav_half);
	ini->SetBoolValue(SECTION_TAPE, _T("LoadWavCorrect"), wav_correct);
	ini->SetLongValue(SECTION_TAPE, _T("LoadWavCorrectType"), wav_correct_type);
	ini->SetLongValue(SECTION_TAPE, _T("LoadWavCorrectAmp0"), wav_correct_amp[0]);
	ini->SetLongValue(SECTION_TAPE, _T("LoadWavCorrectAmp1"), wav_correct_amp[1]);
	ini->SetLongValue(SECTION_TAPE, _T("SaveWavSampleRate"), wav_sample_rate);
	ini->SetLongValue(SECTION_TAPE, _T("SaveWavSampleBits"), wav_sample_bits);
#endif
#endif

	ini->SetLongValue(SECTION_SCREEN, _T("WindowMode"), window_mode);
	ini->SetLongValue(SECTION_SCREEN, _T("WindowPositionX"), window_position_x);
	ini->SetLongValue(SECTION_SCREEN, _T("WindowPositionY"), window_position_y);
#if defined(_BML3MK5) || defined(_MBS1)
	ini->SetLongValue(SECTION_SCREEN, _T("DisplayDevice"), disp_device_no);
	ini->SetLongValue(SECTION_SCREEN, _T("ScreenWidth"), screen_width);
	ini->SetLongValue(SECTION_SCREEN, _T("ScreenHeight"), screen_height);
#endif
	ini->SetLongValue(SECTION_SCREEN, _T("StretchScreen"), stretch_screen);
	ini->Delete(SECTION_SCREEN, _T("CutoutScreen"));
	ini->SetLongValue(SECTION_SCREEN, _T("PixelAspect"), pixel_aspect);
	ini->SetLongValue(SECTION_SCREEN, _T("CaptureType"), capture_type);

	ini->SetLongValue(SECTION_SOUND,  _T("FrequencyNo"), sound_frequency);
	ini->SetLongValue(SECTION_SOUND,  _T("LatencyNo"), sound_latency);

#if defined(USE_MONITOR_TYPE) || defined(USE_SCREEN_ROTATE)
	ini->SetLongValue(SECTION_SCREEN,  _T("MonitorType"), monitor_type);
#endif
#ifdef USE_SCANLINE
	ini->SetLongValue(SECTION_SCREEN, _T("ScanLine"), scan_line);
#endif

#ifdef _MBS1
	ini->SetLongValue(SECTION_CONTROL, _T("SystemMode"), sys_mode, NULL, true);
	ini->SetBoolValue(SECTION_CONTROL, _T("MemoryNoWait"), mem_nowait);
	ini->SetBoolValue(SECTION_SCREEN, _T("AnalogRGB"), (tvsuper & 0x10) != 0);
#endif
#if defined(_BML3MK5) || defined(_MBS1)
	ini->SetBoolValue(SECTION_CONTROL, _T("SyncIRQ"), sync_irq);
	ini->SetLongValue(SECTION_CONTROL, _T("ExtendedRamSize"), exram_size_num);

	ini->SetLongValue(SECTION_SCREEN, _T("AfterImage"), afterimage);
#ifdef USE_KEEPIMAGE
	ini->SetLongValue(SECTION_SCREEN, _T("KeepImage"), keepimage);
#endif
#ifdef USE_DIRECT3D
	ini->SetLongValue(SECTION_SCREEN, _T("UseDirect3D"), use_direct3d);
#endif
#ifdef USE_OPENGL
	ini->SetLongValue(SECTION_SCREEN, _T("UseOpenGL"), use_opengl);
#endif
	ini->SetLongValue(SECTION_SCREEN, _T("DisptmgSkew"), disptmg_skew);
	ini->SetLongValue(SECTION_SCREEN, _T("CurdispSkew"), curdisp_skew);
	ini->SetLongValue(SECTION_SCREEN, _T("VideoSize"), screen_video_size);
#ifdef USE_DIRECT3D
	ini->SetLongValue(SECTION_SCREEN, _T("D3DFilterType"), d3d_filter_type);
#endif
#ifdef USE_OPENGL
	ini->SetLongValue(SECTION_SCREEN, _T("GLFilterType"), gl_filter_type);
#endif
	ini->SetValue(SECTION_SCREEN, _T("Language"), language);

	ini->SetLongValue(SECTION_SOUND,  _T("Volume"), volume);
	ini->SetLongValue(SECTION_CONTROL, _T("FddType"), fdd_type);
	ini->SetLongValue(SECTION_CONTROL, _T("IoPort"), io_port, NULL, true);
	ini->SetLongValue(SECTION_CONTROL, _T("OriginalSettings"), original, NULL, true);

	ini->SetBoolValue(SECTION_CONTROL, _T("ShowMessage"), FLG_SHOWMSGBOARD ? true : false);
	ini->SetLongValue(SECTION_CONTROL, _T("UseJoystick"), FLG_USEJOYSTICK ? 1 : (FLG_USEPIAJOYSTICK ? 2 : 0));
#if defined(_MBS1)
	ini->SetBoolValue(SECTION_CONTROL, _T("EnableMouse"), FLG_USEMOUSE ? true : false);
#else
	ini->SetBoolValue(SECTION_CONTROL, _T("EnableLightpen"), FLG_USELIGHTPEN ? true : false);
#endif
	ini->SetBoolValue(SECTION_CONTROL, _T("ShowMessageUndefOpcode"), FLG_SHOWMSG_UNDEFOP ? true : false);

	ini->SetLongValue(SECTION_CONTROL, _T("FpsNo"), fps_no);

	ini->SetValue(SECTION_PRINTER, _T("Path"), conv_from_npath(initial_printer_path));
	ini->SetValue(SECTION_STATE, _T("Path"), conv_from_npath(initial_state_path));
	for (int j=0; j<MAX_HISTORY && j<recent_state_path.Count(); j++) {
		UTILITY::stprintf(key, 100, _T("File%d"), j+1);
		ini->SetValue(SECTION_STATE, key, conv_from_npath(recent_state_path[j]->path));
	}
#ifdef USE_AUTO_KEY
	ini->SetValue(SECTION_AUTOKEY, _T("Path"), conv_from_npath(initial_autokey_path));
#endif
	ini->SetValue(SECTION_MSGBOARD, _T("InfoFontName"), conv_from_npath(msgboard_info_fontname));
	ini->SetValue(SECTION_MSGBOARD, _T("MessageFontName"), conv_from_npath(msgboard_msg_fontname));
	ini->SetLongValue(SECTION_MSGBOARD, _T("InfoFontSize"), msgboard_info_fontsize);
	ini->SetLongValue(SECTION_MSGBOARD, _T("MessageFontSize"), msgboard_msg_fontsize);
#if defined(GUI_TYPE_AGAR)
	ini->SetValue(SECTION_MENU, _T("FontName"), conv_from_npath(menu_fontname));
	ini->SetLongValue(SECTION_MENU, _T("FontSize"), menu_fontsize);
#endif
	ini->SetValue(SECTION_ROM, _T("Path"), conv_from_npath(rom_path));
#endif

	ini->SetLongValue(SECTION_SOUND, _T("BeepVolume"), beep_volume);
	ini->SetLongValue(SECTION_SOUND, _T("6VoicePsgVolume"), psg6_volume);
	ini->SetLongValue(SECTION_SOUND, _T("9VoicePsgVolume"), psg9_volume);
	ini->SetLongValue(SECTION_SOUND, _T("RelayVolume"), relay_volume);
	ini->SetLongValue(SECTION_SOUND, _T("CmtVolume"), cmt_volume);
	ini->SetLongValue(SECTION_SOUND, _T("FddVolume"), fdd_volume);
#if defined(_MBS1)
	ini->SetLongValue(SECTION_SOUND, _T("PsgVolume"), psg_volume);
	ini->SetLongValue(SECTION_SOUND, _T("ExPsgFmVolume"), psgexfm_volume);
	ini->SetLongValue(SECTION_SOUND, _T("ExPsgSSGVolume"), psgexssg_volume);
	ini->SetLongValue(SECTION_SOUND, _T("OPNFmVolume"), opnfm_volume);
	ini->SetLongValue(SECTION_SOUND, _T("OPNSSGVolume"), opnssg_volume);
#endif
	ini->SetBoolValue(SECTION_SOUND, _T("Mute"), mute);
	ini->SetBoolValue(SECTION_SOUND, _T("BeepMute"), beep_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("6VoicePsgMute"), psg6_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("9VoicePsgMute"), psg9_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("RelayMute"), relay_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("CmtMute"), cmt_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("FddMute"), fdd_mute);
#if defined(_MBS1)
	ini->SetBoolValue(SECTION_SOUND, _T("PsgMute"), psg_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("ExPsgFmMute"), psgexfm_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("ExPsgSSGMute"), psgexssg_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("OPNFmMute"), opnfm_mute);
	ini->SetBoolValue(SECTION_SOUND, _T("OPNSSGMute"), opnssg_mute);

	ini->SetLongValue(SECTION_SOUND, _T("OPNClock"), opn_clock);
	ini->SetLongValue(SECTION_SOUND, _T("OPNInterrupt"), opn_irq);
	ini->SetBoolValue(SECTION_SOUND, _T("UseOPNOnExPsg"), use_opn_expsg);
#endif

	for(int i=0; i<MAX_PRINTER; i++) {
		UTILITY::stprintf(section, 100, _T("%s%d"), SECTION_PRINTER, i);
		ini->SetValue(section, _T("ServerHost"), printer_server_host[i]);
		ini->SetLongValue(section, _T("ServerPort"), printer_server_port[i]);
		ini->SetDoubleValue(section, _T("DelayMsec"), printer_delay[i]);
		ini->SetBoolValue(section, _T("Online"), printer_online[i]);
	}

	ini->SetLongValue(SECTION_COMM, _T("BaudRate"), comm_uart_baudrate);
	ini->SetLongValue(SECTION_COMM, _T("DataBit"), comm_uart_databit);
	ini->SetLongValue(SECTION_COMM, _T("Parity"), comm_uart_parity);
	ini->SetLongValue(SECTION_COMM, _T("StopBit"), comm_uart_stopbit);
	ini->SetLongValue(SECTION_COMM, _T("FlowControl"), comm_uart_flowctrl);

	for(int i=0; i<MAX_COMM; i++) {
		UTILITY::stprintf(section, 100, _T("%s%d"), SECTION_COMM, i);
		ini->SetLongValue(section, _T("DipSwitch"), comm_dipswitch[i]);
		ini->SetValue(section, _T("ServerHost"), comm_server_host[i]);
		ini->SetLongValue(section, _T("ServerPort"), comm_server_port[i]);
		ini->SetBoolValue(section, _T("ThroughMode"), comm_through[i]);
	}

	ini->SetValue(SECTION_SNAPSHOT, _T("Path"), conv_from_npath(snapshot_path));

	ini->SetBoolValue(SECTION_LEDBOX, _T("Show"), FLG_SHOWLEDBOX ? true : false);
	ini->SetBoolValue(SECTION_LEDBOX, _T("Inside"), FLG_INSIDELEDBOX ? true : false);
	ini->SetLongValue(SECTION_LEDBOX, _T("Position"), led_pos);
	ini->SetLongValue(SECTION_LEDBOX, _T("DistanceOnWindowX"), led_dist[0].x);
	ini->SetLongValue(SECTION_LEDBOX, _T("DistanceOnWindowY"), led_dist[0].y);
	ini->SetLongValue(SECTION_LEDBOX, _T("DistanceOnFullscreenX"), led_dist[1].x);
	ini->SetLongValue(SECTION_LEDBOX, _T("DistanceOnFullscreenY"), led_dist[1].y);

	// delete old key
	ini->Delete(SECTION_CONTROL, _T("ShowLED"));
	ini->Delete(SECTION_CONTROL, _T("InsideLED"));
	ini->Delete(SECTION_CONTROL, _T("LEDPosition"));

#if defined(USE_WIN)
	ini->SetValue(SECTION_FONT, _T("File"), conv_from_npath(font_path));
#endif
#if defined(USE_SDL) || defined(USE_SDL2)
	ini->SetValue(SECTION_FONT, _T("Path"), conv_from_npath(font_path));
#endif
#ifdef USE_DIRECTINPUT
	ini->SetBoolValue(SECTION_CONTROL, _T("UseDirectInput"), (use_direct_input & 1) != 0);
#endif
#ifdef USE_DEBUGGER
	ini->SetLongValue(SECTION_DEBUGGER, _T("ImmediateStart"), debugger_imm_start);
	ini->SetValue(SECTION_DEBUGGER, _T("ServerHost"), debugger_server_host);
	ini->SetLongValue(SECTION_DEBUGGER, _T("ServerPort"), debugger_server_port);
#endif
#ifdef USE_PERFORMANCE_METER
	ini->SetBoolValue(SECTION_CONTROL, _T("ShowPerformanceMeter"), show_pmeter);
#endif
#if defined(_MBS1) && defined(USE_Z80B_CARD)
	ini->SetLongValue(SECTION_Z80BCARD, _T("OutputInterrupt"), z80b_card_out_irq);
#endif

	// save ini file
	if (ini->SaveFile(ini_file) == true) {
		logging->out_logf_x(LOG_INFO, CMsg::VSTR_was_saved, base_name);
	} else {
		logging->out_logf_x(LOG_WARN, CMsg::VSTR_couldn_t_be_saved, base_name);
	}
}

const _TCHAR *Config::conv_to_npath(const _TCHAR *path)
{
#if defined(USE_WIN)
	static _TCHAR npath[_MAX_PATH];
#if defined(_MBS1)
	UTILITY::conv_to_native_path(path, npath, _MAX_PATH);
	return npath;
#else
	if (version2 >= 9) {
		UTILITY::conv_to_native_path(path, npath, _MAX_PATH);
		return npath;
	}
#endif
#endif
	return path;
}

const _TCHAR *Config::conv_from_npath(const _TCHAR *npath)
{
#if defined(USE_WIN)
	static _TCHAR path[_MAX_PATH];
	if (npath) {
		UTILITY::conv_from_native_path(npath, path, _MAX_PATH);
	} else {
		path[0]=_T('\0');
	}
	return path;
#else
	return npath;
#endif
}

bool Config::get_number_in_path(_TCHAR *path, int *number)
{
	int len = (int)_tcslen(path);
	bool match = false;
	bool digit = false;
	int pos = len - 1;
	int num = 0;

	// get the number in the end of path
	// ex: "foo.bar:123" -> ":123"
	for (; pos >= 0 && pos >= len - 4; pos--) {
		if (!_istdigit(path[pos]) && path[pos] != _T(':')) {
			break;
		}
		if (_istdigit(path[pos])) {
			digit = true;
		}
		if (digit && path[pos] == _T(':')) {
			match = true;
			break;
		}
	}
	if (match) {
		_stscanf(&path[pos + 1], _T("%d"), &num);
		for(; pos < len; pos++) path[pos] = _T('\0');
	}
	if (number) *number = num;
	return match;
}

bool Config::set_number_in_path(_TCHAR *path, size_t size, int number)
{
	// 1 .. 9999
	if (number <= 0 || number > 9999) {
		return false;
	}

	_TCHAR buf[8];

	UTILITY::stprintf(buf, 8, _T(":%d"), number);
	UTILITY::tcscat(path, size, buf);
	return true;
}

void Config::get_str_value(const _TCHAR *section, const _TCHAR *key, CTchar &str)
{
	_TCHAR buf[_MAX_PATH];

	*buf = _T('\0');
	const _TCHAR *p = ini->GetValue(section, key, str);
	if (p) {
		UTILITY::tcscpy(buf, _MAX_PATH, conv_to_npath(p));
	}
	str.Set(buf);
}

void Config::get_dirpath_value(const _TCHAR *section, const _TCHAR *key, CDirPath &path)
{
	_TCHAR buf[_MAX_PATH];

	*buf = _T('\0');
	const _TCHAR *p = ini->GetValue(section, key, path);
	if (p) {
		UTILITY::tcscpy(buf, _MAX_PATH, conv_to_npath(p));
	}
	path.Set(buf);
}

void Config::get_filepath_value(const _TCHAR *section, const _TCHAR *key, CFilePath &path)
{
	_TCHAR buf[_MAX_PATH];

	*buf = _T('\0');
	const _TCHAR *p = ini->GetValue(section, key, path);
	if (p) {
		UTILITY::tcscpy(buf, _MAX_PATH, conv_to_npath(p));
	}
	path.Set(buf);
}

void Config::get_recentpath_value(const _TCHAR *section, const _TCHAR *key, CRecentPathList &pathlist)
{
	_TCHAR buf[_MAX_PATH];
	int num = 0;

	*buf = _T('\0');
	const _TCHAR *p = ini->GetValue(section, key);
	if (p) {
		UTILITY::tcscpy(buf, _MAX_PATH, conv_to_npath(p));
		get_number_in_path(buf, &num);
		UTILITY::convert_path_separator(buf);
		if (!pathlist.Match(buf, num)) {
			pathlist.Add(new CRecentPath(buf, num));
		}
	}
}
