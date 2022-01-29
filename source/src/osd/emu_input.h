/** @file emu_input.h

	Skelton for retropc emulator

	@author Sasaji
	@date   2019.10.01 -

	@brief [ emu input ]
*/

#ifndef EMU_INPUT_H
#define EMU_INPUT_H

#define KEY_KEEP_FRAMES 3

#if defined(USE_WIN)
#include "windows/win_input.h"
#elif defined(USE_SDL) || defined(USE_SDL2)
#include "SDL/sdl_input.h"
#elif defined(USE_WX) || defined(USE_WX2)
#include "wxwidgets/wxw_input.h"
#elif defined(USE_QT)
#include "qt/qt_input.h"
#endif

#endif	/* EMU_INPUT_H */
