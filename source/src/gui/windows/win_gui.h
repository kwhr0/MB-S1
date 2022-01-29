/** @file win_gui.h

	HITACHI BASIC MASTER LEVEL3 Mark5 / MB-S1 Emulator 'EmuB-6892/EmuB-S1'
	Skelton for retropc emulator

	@author Sasaji
	@date   2014.03.24 -

	@brief [ gui for windows ]
*/

#ifndef WIN_GUI_H
#define WIN_GUI_H

#include "../../common.h"
#include "../../depend.h"
#include <Windows.h>
#include "../gui_base.h"
#include "winfont.h"
#include "win_configbox.h"
#include "win_keybindbox.h"
#include "win_volumebox.h"
#include "win_recvidbox.h"
#include "win_recaudbox.h"
#include "win_aboutbox.h"
#ifdef USE_DEBUG_SOUND_FILTER
#include "win_sndfilterbox.h"
#endif

#ifdef GUI_TYPE_WINDOWS

/// Show menus and dialogs on Windows
class GUI : public GUI_BASE
{
private:
	HINSTANCE hInstance;
	CFont *font;
//	GUI_WIN::ConfigBox *configbox;
//	GUI_WIN::KeybindBox *keybindbox;
//	GUI_WIN::VolumeBox *volumebox;
	GUI_WIN::RecVideoBox *recvidbox;
	GUI_WIN::RecAudioBox *recaudbox;
//	GUI_WIN::AboutBox *aboutbox;

	/// menu
	bool now_menu;
	bool now_menuloop;

	static HMENU get_sub_menu(HMENU hMenu, int id);
	static HMENU get_sub_menu_new(HMENU hMenu, int id);
	void update_recent_menu(HMENU hMenu, int menu_id, CRecentPathList &list);
	void update_multivolume_menu(HMENU hMenu, int drv, int menu_id);

	void update_control_menu(HMENU hMenu);
	void update_fdd_menu(HMENU hMenu, int drv, int ID_OPEN_FD, int ID_CHANGE_FD, int ID_WRITEPROTECT_FD, int ID_RECENT_FD, int ID_SELECT_D88_BANK);
	void update_tape_menu(HMENU hMenu);
	void update_screen_menu(HMENU hMenu);
	void update_sound_menu(HMENU hMenu);
	void update_device_menu(HMENU hMenu);
	void update_option_menu(HMENU hMenu);

#if defined(USE_SDL) || defined(USE_SDL2)
	WNDPROC OrigProcPtr;
	/// Hook event procedure on main window and process message before on SDL.
	static LRESULT CALLBACK MessageProcStatic(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	LRESULT MessageProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
#endif

public:
	GUI(int argc, char **argv, EMU *new_emu);
	virtual ~GUI();
#if defined(USE_SDL)
	virtual int CreateWidget(SDL_Surface *screen, int width, int height);
#endif

	virtual int CreateMenu();

	virtual void ShowMenu();
	virtual void HideMenu();

#if defined(USE_SDL) || defined(USE_SDL2)
	virtual void PreProcessEvent();
	virtual void PostProcessEvent();
	virtual bool NeedUpdateScreen();
#else
	virtual LRESULT ProcessEvent(UINT iMsg, WPARAM wParam, LPARAM lParam);
#endif

	virtual void ScreenModeChanged(bool fullscreen);

	void SetWindowHandle(HWND new_hwnd) { hWindow = new_hwnd; }

	virtual void SetFocusToMainWindow();

	virtual bool ShowRecordVideoDialog(int fps_num);
	virtual bool ShowRecordAudioDialog(void);
	virtual bool ShowRecordVideoAndAudioDialog(int fps_num);

	virtual bool ShowAboutDialog();

#ifdef USE_EMU_INHERENT_SPEC
	virtual bool ShowConfigureDialog();
	virtual bool ShowKeybindDialog();
	virtual bool ShowVolumeDialog();
#ifdef USE_DEBUG_SOUND_FILTER
	virtual bool ShowSndFilterDialog();
#endif
#endif
#ifdef USE_DATAREC
	virtual bool ShowLoadDataRecDialog();
	virtual bool ShowSaveDataRecDialog();
	virtual void set_datarec_file_menu(HMENU hMenu, UINT uItem);
#endif
#ifdef USE_FD1
	virtual bool ShowOpenFloppyDiskDialog(int drv);
	virtual int  ShowSelectFloppyDriveDialog(int drv);
	virtual bool ShowOpenBlankFloppyDiskDialog(int drv, uint8_t type);
	virtual void GetMultiVolumeStr(int num, const _TCHAR *name, _TCHAR *str, size_t slen);
	void set_disk_file_menu(HMENU hMenu, UINT uItem, int drv);
#ifdef USE_EMU_INHERENT_SPEC
	void set_disk_side_menu(HMENU hMenu, UINT uItem, int drv);
#endif
	void set_file_name_for_menu(bool ok, CMsg::Id ok_prefix, CMsg::Id ng_prefix, const _TCHAR *path, int num, _TCHAR *str);
#endif
#ifdef USE_CART1
	virtual bool ShowOpenCartridgeDialog(int drv);
#endif
#ifdef USE_QD1
	virtual bool ShowOpenQuickDiskDialog(int drv);
#endif
#ifdef USE_MEDIA
	virtual bool ShowOpenMediaDialog();
#endif
#ifdef USE_BINARY_FILE1
	virtual bool ShowLoadBinaryDialog(int drv);
	virtual bool ShowSaveBinaryDialog(int drv);
#endif

#ifdef USE_AUTO_KEY
	virtual bool ShowOpenAutoKeyDialog();
	virtual bool StartAutoKey(void);
#endif

#ifdef USE_EMU_INHERENT_SPEC
	virtual bool ShowSaveStateDialog(bool cont);
	virtual bool ShowLoadStateDialog();
#ifdef USE_KEY_RECORD
	virtual bool ShowPlayRecKeyDialog();
	virtual bool ShowRecordRecKeyDialog();
	virtual bool ShowRecordStateAndRecKeyDialog();
#endif
#endif

#ifdef USE_PRINTER
	virtual bool ShowSavePrinterDialog(int drv);
#endif

#ifndef USE_SDL2
	virtual bool OpenDroppedFile(void *param);
#endif

	void update_menu(HMENU hMenu, int pos);
	void translate_menu(HMENU hMenu, int depth);

	virtual bool ShowVirtualKeyboard();

//#if defined(USE_SDL) || defined(USE_SDL2)
//	static const char *gettext_acp(const char *src);
//	static const char *cmsg_acp(CMsg::Id id);
//#endif
};

#endif /* GUI_TYPE_WINDOWS */

#if (defined(USE_SDL) || defined(USE_SDL2)) && defined(GUI_TYPE_WINDOWS) && !defined(NOT_USE_GETTEXT_ACP)
# ifdef USE_GETTEXT
#  ifdef _tgettext
#   undef _
#   undef _tgettext
#  endif
//#  define _(text) GUI::gettext_acp(text)
//#  define _tgettext GUI::gettext_acp
//#  define CMSGM(x) GUI::cmsg_acp(CMsg::x)
//#  define CMSGVM(x) GUI::cmsg_acp(x)
#  define _(text) clocale->GetText(text, CLocale::CODE_ACP)
#  define _tgettext(text) clocale->GetText(text, CLocale::CODE_ACP)
#  define CMSGM(x) clocale->GetText(gMessages.Get(CMsg::x, false), CLocale::CODE_ACP)
#  define CMSGVM(x) clocale->GetText(gMessages.Get(x, false), CLocale::CODE_ACP)
# else
#  define CMSGM(x) CMSG(x)
#  define CMSGVM(x) CMSGV(x)
# endif
#else
#  define CMSGM(x) CMSG(x)
#  define CMSGVM(x) CMSGV(x)
#endif

#endif /* WIN_GUI_H */
