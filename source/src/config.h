/** @file config.h

	Skelton for retropc emulator

	@author Takeda.Toshiya
	@date   2006.08.18 -

	@note
	Modified for BML3MK5 by Sasaji at 2011.06.17
	Modified for MBS1 by Sasaji at 2015.08.16

	@brief [ config ]

	@note
	Config ini file uses UTF-8 as string code.
	Under windows: convert to MBCS(shift jis) code from UTF-8.
	Under mac/linux: always uses UTF-8, so LANG environment should be set UTF-8. 
*/

#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"
#include "vm/vm_defs.h"
#include "cchar.h"
#include "cptrlist.h"
//#ifdef _UNICODE
//#define SI_CONVERT_GENERIC	// use generic simple.ini on all platform
//#endif
//#include "SimpleIni.h"
//#include "simple_ini.h"

class CSimpleIni;

#define FILE_VERSION	0x30

#define IOPORT_MSK_5FDD		0x00001
#define IOPORT_MSK_3FDD		0x00002
#define IOPORT_MSK_PSG6		0x00004
#define IOPORT_MSK_EXPIA	0x00008
#define IOPORT_MSK_EXACIA	0x00010
#define IOPORT_MSK_PSG9		0x00020
#define IOPORT_MSK_KANJI	0x00040
#define IOPORT_MSK_EXPSG	0x00080	/* for mbs1 */
#define IOPORT_MSK_FDDALL	0x00003
#define IOPORT_MSK_OS9BD	0x00100	/* for mbs1 */
#define IOPORT_MSK_CM01		0x00200	/* for mbs1 */
#define IOPORT_MSK_KEYBD	0x00400	/* for mbs1 */
#define IOPORT_MSK_MOUSE	0x00800	/* for mbs1 */
#define IOPORT_MSK_FMOPN	0x01000	/* for mbs1 */
#define IOPORT_MSK_DISROMB	0x02000	/* for mbs1 */
#define IOPORT_MSK_DISIG	0x04000
#define IOPORT_MSK_RTC		0x08000
#define IOPORT_MSK_Z80BCARD	0x10000	/* for mbs1 */
#if defined(_MBS1)
#if defined(USE_Z80B_CARD)
#define IOPORT_MSK_ALL		0x1ffff
#else
#define IOPORT_MSK_ALL		0x0ffff
#endif
#else
#define IOPORT_MSK_ALL		0x0c07f
#endif
#define IOPORT_NUMS			17
#define IOPORT_USE_5FDD		(config.io_port & IOPORT_MSK_5FDD)
#define IOPORT_USE_3FDD		(config.io_port & IOPORT_MSK_3FDD)
#define IOPORT_USE_PSG6		(config.io_port & IOPORT_MSK_PSG6)
#define IOPORT_USE_EXPIA	(config.io_port & IOPORT_MSK_EXPIA)
#define IOPORT_USE_EXACIA	(config.io_port & IOPORT_MSK_EXACIA)
#define IOPORT_USE_PSG9		(config.io_port & IOPORT_MSK_PSG9)
#define IOPORT_USE_KANJI	(config.io_port & IOPORT_MSK_KANJI)
#define IOPORT_USE_EXPSG	(config.io_port & IOPORT_MSK_EXPSG)		/* for mbs1 */
#define IOPORT_USE_FDD		(config.io_port & IOPORT_MSK_FDDALL)
#define IOPORT_USE_OS9BD	(config.io_port & IOPORT_MSK_OS9BD)		/* for mbs1 */
#define IOPORT_USE_CM01		(config.io_port & IOPORT_MSK_CM01)		/* for mbs1 */
#define IOPORT_USE_KEYBD	(config.io_port & IOPORT_MSK_KEYBD)		/* for mbs1 */
#define IOPORT_USE_MOUSE	(config.io_port & IOPORT_MSK_MOUSE)		/* for mbs1 */
#define IOPORT_USE_FMOPN	(config.io_port & IOPORT_MSK_FMOPN)		/* for mbs1 */
#define IOPORT_USE_DISROMB	(config.io_port & IOPORT_MSK_DISROMB)	/* for mbs1 */
#define IOPORT_USE_DISIG	(config.io_port & IOPORT_MSK_DISIG)
#define IOPORT_USE_RTC		(config.io_port & IOPORT_MSK_RTC)
#define IOPORT_USE_Z80BCARD	(config.io_port & IOPORT_MSK_Z80BCARD)	/* for mbs1 */

enum en_ioport_pos {
	IOPORT_POS_5FDD = 0,
	IOPORT_POS_3FDD,
	IOPORT_POS_PSG6,
	IOPORT_POS_EXPIA,
	IOPORT_POS_EXACIA,
	IOPORT_POS_PSG9,
	IOPORT_POS_KANJI,
	IOPORT_POS_EXPSG,	/* for mbs1 */
	IOPORT_POS_OS9BD,	/* for mbs1 */
	IOPORT_POS_CM01,	/* for mbs1 */
	IOPORT_POS_KEYBD,	/* for mbs1 */
	IOPORT_POS_MOUSE,	/* for mbs1 */
	IOPORT_POS_FMOPN,	/* for mbs1 */
	IOPORT_POS_DISROMB,	/* for mbs1 */
	IOPORT_POS_DISIG,
	IOPORT_POS_RTC,
	IOPORT_POS_Z80BCARD /* for mbs1 */
};

#define MSK_SHOWLEDBOX		0x001
#define MSK_SHOWMSGBOARD	0x002
#define MSK_USEJOYSTICK		0x004
#define MSK_INSIDELEDBOX	0x008
#define MSK_LEDBOX_ALL		0x009
#define MSK_USELIGHTPEN		0x010
#define MSK_USEMOUSE		0x020
#define MSK_USEPIAJOYSTICK	0x040
#define MSK_USEJOYSTICK_ALL	0x044
#define MSK_SHOWMSG_UNDEFOP	0x080
#define MSK_SHOWDLG_ALL		0x0ff
#define FLG_SHOWLEDBOX		(config.show_dlg & MSK_SHOWLEDBOX)
#define FLG_SHOWMSGBOARD	(config.show_dlg & MSK_SHOWMSGBOARD)
#define FLG_USEJOYSTICK		(config.show_dlg & MSK_USEJOYSTICK)
#define FLG_INSIDELEDBOX	(config.show_dlg & MSK_INSIDELEDBOX)
#define FLG_LEDBOX_ALL		(config.show_dlg & MSK_LEDBOX_ALL)
#define FLG_USELIGHTPEN		(config.show_dlg & MSK_USELIGHTPEN)
#define FLG_USEMOUSE		(config.show_dlg & MSK_USEMOUSE)
#define FLG_USEPIAJOYSTICK	(config.show_dlg & MSK_USEPIAJOYSTICK)
#define FLG_USEJOYSTICK_ALL	(config.show_dlg & MSK_USEJOYSTICK_ALL)
#define FLG_SHOWMSG_UNDEFOP	(config.show_dlg & MSK_SHOWMSG_UNDEFOP)

#define MSK_ORIG_CURIG		0x001
#define MSK_ORIG_PAL8		0x002		/* for mbs1 */
#define MSK_ORIG_NOPAL64	0x004		/* for mbs1 */
#define MSK_ORIG_NOPAL		0x008		/* for mbs1 */
#define MSK_ORIG_LIMKEY		0x010
#define MSK_ORIG_FDMSK		0x020		/* for mbs1 */
#define MSK_ORIG_FDINSERT	0x040
#define MSK_ORIG_FDDRQ		0x080
#define MSK_ORIG_MOUSEEG	0x100		/* for mbs1 */
#define FLG_ORIG_CURIG		(config.original & MSK_ORIG_CURIG)
#define FLG_ORIG_PAL8		(config.original & MSK_ORIG_PAL8)		/* for mbs1 */
#define FLG_ORIG_NOPAL64	(config.original & MSK_ORIG_NOPAL64)	/* for mbs1 */
#define FLG_ORIG_NOPAL		(config.original & MSK_ORIG_NOPAL)		/* for mbs1 */
#define FLG_ORIG_MOUSEEG	(config.original & MSK_ORIG_MOUSEEG)	/* for mbs1 */
#define FLG_ORIG_LIMKEY		(config.original & MSK_ORIG_LIMKEY)
#define FLG_ORIG_FDMSK		(config.original & MSK_ORIG_FDMSK)		/* for mbs1 */
#define FLG_ORIG_FDINSERT	(config.original & MSK_ORIG_FDINSERT)
#define FLG_ORIG_FDDRQ		(config.original & MSK_ORIG_FDDRQ)

#define MSK_DELAY_FDSEARCH	0x01
#define MSK_DELAY_FDSEEK	0x02
#define FLG_DELAY_FDSEARCH	(config.delay_fdd & MSK_DELAY_FDSEARCH)
#define FLG_DELAY_FDSEEK	(config.delay_fdd & MSK_DELAY_FDSEEK)

#define MSK_CHECK_FDDENSITY	0x01
#define MSK_CHECK_FDMEDIA	0x02
#define FLG_CHECK_FDDENSITY	(config.check_fdmedia & MSK_CHECK_FDDENSITY)
#define FLG_CHECK_FDMEDIA	(config.check_fdmedia & MSK_CHECK_FDMEDIA)

#define FDD_TYPE_NOFDD		0
#define FDD_TYPE_3FDD		1
#define FDD_TYPE_5FDD		2
#define FDD_TYPE_58FDD		3
#define MSK_FDD_TYPE		0x3

/// directory path
class CDirPath : public CTchar
{
public:
	void SetFromPath(const _TCHAR *file_path);
	void Set(const _TCHAR *dir_path);
private:
	void Set(const CDirPath &) {}
	void Set(const _TCHAR *, int) {}
};

/// file path
class CFilePath : public CTchar
{
public:
	void Set(const _TCHAR *file_path);
};

#define MAX_HISTORY	20

/// recent path
class CRecentPath
{
public:
	CTchar path;
	int    num;
public:
	CRecentPath();
	CRecentPath(const CRecentPath &src);
	CRecentPath(const _TCHAR *srcpath, int srcnum);
	~CRecentPath();
	void Set(const _TCHAR *srcpath, int srcnum);
	void Clear();
	bool Match(const _TCHAR *tagpath, int tagnum);
};

/// recent path list
class CRecentPathList : public CPtrList<CRecentPath>
{
public:
	bool updated;
public:
	CRecentPathList();
	~CRecentPathList();
	bool Match(const _TCHAR *tagpath, int tagnum);
	void Update(const _TCHAR *tagpath, int tagnum);
};

/// @brief Read/Write config file
///
///	@note
///	Config ini file uses UTF-8 as string code.
///	Under windows: convert to MBCS(shift jis) code from UTF-8.
///	Under mac/linux: always uses UTF-8, so LANG environment should be set UTF-8. 
class Config
{
public:
	int version1;	// config file version
	int version2;

	// recent files
#ifdef USE_FD1
	CDirPath initial_disk_path;
#if defined(USE_FD8) || defined(USE_FD7)
	CRecentPathList recent_disk_path[8];
	CRecentPath opened_disk_path[8];
#elif defined(USE_FD6) || defined(USE_FD5)
	CRecentPathList recent_disk_path[6];
	CRecentPath opened_disk_path[6];
#else
	CRecentPathList recent_disk_path[4];
	CRecentPath opened_disk_path[4];
#endif
#endif
#ifdef USE_DATAREC
	CDirPath initial_datarec_path;
	CRecentPathList recent_datarec_path;
	CRecentPath opened_datarec_path;
#endif
#ifdef USE_CART1
	CDirPath initial_cart_path;
	CRecentPath recent_cart_path[1];
#endif
#ifdef USE_QD1
	CDirPath initial_quickdisk_path;
	CRecentPath recent_quickdisk_path[1];
#endif
#ifdef USE_MEDIA
	CDirPath initial_media_path;
	CRecentPath recent_media_path;
#endif
#ifdef USE_BINARY_FILE1
	CDirPath initial_binary_path;
	CRecentPath recent_binary_path[2];
#endif

	// screen
	int window_mode;
	int window_position_x;
	int window_position_y;
	uint8_t stretch_screen;
//	bool cutout_screen;
	int pixel_aspect;
	uint8_t capture_type;

	// sound
	int sound_frequency;
	int sound_latency;

	// virtual machine
	int cpu_power;
	bool now_power_off;
	bool use_power_off;
#ifdef USE_FD1
	bool ignore_crc;
#endif
#ifdef USE_DIPSWITCH
	uint8_t dipswitch;
#endif
#ifdef USE_BOOT_MODE
	int boot_mode;		// MZ-800, PASOPIA, PC-8801MA, PC-98DO
#endif
#ifdef USE_CPU_CLOCK_LOW
	bool cpu_clock_low;	// PC-8801MA, PC-9801E, PC-9801VM, PC-98DO
#endif
#if defined(_HC80) || defined(_PASOPIA) || defined(_PC8801MA)
	int device_type;
#endif
#if defined(USE_MONITOR_TYPE) || defined(USE_SCREEN_ROTATE)
	int monitor_type;
#endif
#ifdef USE_SCANLINE
	uint8_t scan_line;
#endif
#if defined(_MBS1)
	uint8_t sys_mode;
	bool mem_nowait;
	uint8_t tvsuper;
#endif
#if defined(_BML3MK5) || defined(_MBS1)
	bool sync_irq;
	int  afterimage;
#ifdef USE_KEEPIMAGE
	int  keepimage;
#endif
	uint8_t exram_size_num;
#ifdef USE_DIRECT3D
	uint8_t use_direct3d;
	uint8_t d3d_filter_type;
#endif
#ifdef USE_OPENGL
	uint8_t use_opengl;
	uint8_t gl_filter_type;
#endif
	int8_t  disptmg_skew;
	int8_t  curdisp_skew;
	int volume;
	int fdd_type;
	int mount_fdd;

	// CONFIG_VERSION >= 0x05 on bml3mk5
	// bit0: use fdd5  bit1: use fdd3  bit2: use psg  bit3: use ex pia
	// bit4: use ex acia  bit5: use 9psg  bit6: use kanji
	// bit7: use s1 ex psg (CONFIG_VERSION >= 0x02 on mbs1)
	// bit8: use OS/9  bit9: use comm  bit10: use keyboard  bit11: use mouse (on mbs1)
	// bit12: use fm opn (on mbs1)
	// bit13: disable booting from rom basic
	// bit14: disable ig ram
	// bit15: use rtc
	// bit16: use Z80B card
	int io_port;
	// bit0: show led  bit1: show msg  bit2: use joystick  bit3: inside led
	// bit4: enable lightpen bit5: enable mouse bit6: use pia joystick  bit7: show msg when undef opcode
	int show_dlg;
	// bit0: show cursor on IG  bit1: set dark gray on palette 8 (on mbs1)
	// bit2: not use palette in ANALOG 64 color mode  bit3: not use palette in ANALOG mode (on mbs1)
	// bit4: auto key release after few frames
	// bit5: ff00 mask (on mbs1)
	// bit6: check fd insert when get track  bit7: ignore drq when sector is not found
	// bit8: occur mouse IRQ when both edge (on mbs1)
	int original;

	// bit0: ignore delay by search sector
	// bit1: ignore delay by seek track
	uint8_t delay_fdd;

	// bit0: check a difference of density in floppy disk
	// bit0: check a difference of media in floppy disk
	uint8_t check_fdmedia;

	CDirPath initial_printer_path;
	CDirPath initial_state_path;
	CRecentPath saved_state_path;
	CRecentPathList recent_state_path;

	// CONFIG_VERSION >= 0x06 on bml3mk5
	int fps_no;

	int disp_device_no;
	int screen_width;
	int screen_height;

	CDirPath initial_autokey_path;
	CRecentPath opened_autokey_path;
	// support above params on cfg file
	CTchar msgboard_info_fontname;
	CTchar msgboard_msg_fontname;
	CDirPath rom_path;

	bool realmode_datarec;

	//
	uint8_t screen_video_size;
	uint8_t msgboard_info_fontsize;
	uint8_t msgboard_msg_fontsize;
#if defined(GUI_TYPE_AGAR)
	CTchar menu_fontname;
	uint8_t menu_fontsize;
#endif

	// CONFIG_VERSION >= 0x07 on bml3mk5
	int beep_volume;
	int psg6_volume;
	int psg9_volume;
	int relay_volume;
	int cmt_volume;
	int fdd_volume;
	bool mute;
	bool beep_mute;
	bool psg6_mute;
	bool psg9_mute;
	bool relay_mute;
	bool cmt_mute;
	bool fdd_mute;
#ifdef _MBS1
	int psg_volume;
	int psgexfm_volume;
	int psgexssg_volume;
	int opnfm_volume;
	int opnssg_volume;
	bool psg_mute;
	bool psgexfm_mute;
	bool psgexssg_mute;
	bool opnfm_mute;
	bool opnssg_mute;

	int opn_clock;
	int opn_irq;

	// use opn instead of psg on external psg port 
	bool use_opn_expsg;
#endif

	// CONFIG_VERSION >= 0x08 on bml3mk5
	CTchar printer_server_host[MAX_PRINTER];
	int    printer_server_port[MAX_PRINTER];
	bool   printer_direct[MAX_PRINTER];
	bool   printer_online[MAX_PRINTER];
	double printer_delay[MAX_PRINTER];

	int    comm_dipswitch[MAX_COMM];
	CTchar comm_server_host[MAX_COMM];
	int    comm_server_port[MAX_COMM];
	bool   comm_server[MAX_COMM];
//	bool   comm_connect[MAX_COMM];
	bool   comm_through[MAX_COMM];
	bool   comm_binary[MAX_COMM];

	int    comm_uart_baudrate;
	int    comm_uart_databit;
	int    comm_uart_parity;
	int    comm_uart_stopbit;
	int    comm_uart_flowctrl;

	CDirPath snapshot_path;

	uint8_t led_pos;
	VmPoint led_dist[2];

#if defined(USE_WIN)
	CFilePath font_path;
#endif
#if defined(USE_SDL) || defined(USE_SDL2) || defined(USE_WX) || defined(USE_WX2)
	CDirPath font_path;
#endif

	bool reckey_recording;
	bool reckey_playing;

	bool wav_reverse;
	bool wav_half;
	bool wav_correct;
	uint8_t wav_correct_type;
	int wav_correct_amp[2];

	uint8_t wav_sample_rate;
	uint8_t wav_sample_bits;

#endif
#ifdef USE_SOUND_DEVICE_TYPE
	int sound_device_type;
#endif

#ifdef USE_DIRECTINPUT
	// bit0:enable/disable bit2:available
	uint8_t use_direct_input;
#endif

#ifdef USE_DEBUGGER
	int    debugger_imm_start;
	CTchar debugger_server_host;
	int    debugger_server_port;
#endif

#ifdef USE_PERFORMANCE_METER
	bool   show_pmeter;
#endif

	CTchar language;

#if defined(_MBS1) && defined(USE_Z80B_CARD)
	int    z80b_card_out_irq;
#endif

public:
	Config();
	~Config();

	void initialize();
	void load(const _TCHAR *path);
	void save();
	void release();

	static bool get_number_in_path(_TCHAR *path, int *number);
	static bool set_number_in_path(_TCHAR *path, size_t size, int number);

private:
	CSimpleIni *ini;

	CTchar ini_file;

	bool load_ini_file(const _TCHAR *file);
	void save_ini_file(const _TCHAR *file);

	const _TCHAR *conv_to_npath(const _TCHAR *path);
	const _TCHAR *conv_from_npath(const _TCHAR *npath);
	void get_str_value(const _TCHAR *section, const _TCHAR *key, CTchar &str);
	void get_dirpath_value(const _TCHAR *section, const _TCHAR *key, CDirPath &path);
	void get_filepath_value(const _TCHAR *section, const _TCHAR *key, CFilePath &path);
	void get_recentpath_value(const _TCHAR *section, const _TCHAR *key, CRecentPathList &pathlist);

	static int conv_volume(int);
};

extern Config *pconfig;
#define config (*pconfig)

#endif /* CONFIG_H */

