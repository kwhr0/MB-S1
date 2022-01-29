/** @file sdl_screen.cpp

	Skelton for retropc emulator
	SDL edition

	@author Sasaji
	@date   2012.02.21

	@brief [ sdl screen ]

	@note
	This code is based on win32_screen.cpp of the Common Source Code Project.
	Author : Takeda.Toshiya
*/

#include "../../emu_osd.h"
#include "../../vm/vm.h"
#include "../../gui/gui.h"
#include "../../config.h"
#ifdef USE_MESSAGE_BOARD
#include "../../msgboard.h"
#endif
#include "sdl_csurface.h"
#include "../../video/rec_video.h"
//#include "sdl_utils.h"

void EMU_OSD::EMU_SCREEN()
{
#ifndef USE_SDL2
	screen = NULL;
#else
	window = NULL;
	renderer = NULL;
	texture = NULL;
#ifdef USE_OPENGL
	glcontext = NULL;
#endif
#endif

	screen_flags = 0;

#ifdef USE_OPENGL
	mix_texture_name = 0;
	src_tex_l = src_tex_t = 0.0;
	src_tex_r = src_tex_b = 1.0;
	src_pyl_l = src_pyl_t = -1.0;
	src_pyl_r = src_pyl_b = 1.0;
	opengl = NULL;
	next_use_opengl = 0;
#endif
}

///
/// initialize screen
///
void EMU_OSD::initialize_screen()
{
	EMU::initialize_screen();

#ifdef USE_OPENGL
	next_use_opengl = config.use_opengl;

	initialize_opengl();
#endif

#ifdef USE_MESSAGE_BOARD
	msgboard = new MsgBoard(this);
	if (msgboard) {
		msgboard->InitScreen(pixel_format, screen_size.w, screen_size.h);
		msgboard->SetMessagePos(4 + reMix.x, -4 - reMix.y, 2);
		msgboard->SetInfoPos(-4 - reMix.x, 4 + reMix.y, 1);
		msgboard->SetVisible(FLG_SHOWMSGBOARD ? true : false);
	}
#endif
	if (gui) {
		gui->CreateLedBox(res_path.GetN(), pixel_format);
	}
}

///
/// release screen
///
void EMU_OSD::release_screen()
{
#ifdef USE_OPENGL
	release_opengl();
#endif
	if (gui) {
		gui->ReleaseLedBox();
	}

	EMU::release_screen();
}

#if defined(USE_SDL)
///
/// create / recreate window on SDL1
///
/// @param[in] disp_no : (unused)
/// @param[in] x       : (unused)
/// @param[in] y       : (unused)
/// @param[in] width   : window width
/// @param[in] height  : window height
/// @param[in] flags   : on SDL_SetVideoMode
/// @return true / false
bool EMU_OSD::create_screen(int disp_no, int x, int y, int width, int height, uint32_t flags)
{
	bool rc = true;
	VmRect margin = { 0, 0, 0, 0 };
	VmRectWH screen_rect;

	lock_screen();

	/* SDL1 */
	if (now_screenmode == NOW_FULLSCREEN) {
		flags |= SDL_WINDOW_FULLSCREEN;
	} else {
		flags &= ~SDL_WINDOW_FULLSCREEN;
		margin = display_margin;
	}
	flags &= ~SDL_WINDOW_OPENGL;
#ifdef USE_OPENGL
	if (use_opengl) {
		flags |= SDL_WINDOW_OPENGL;
		release_texture();
		if (opengl) opengl->Terminate();
	}
#endif

	// (re)create window
	screen = SDL_SetVideoMode(margin.left + width + margin.right, margin.top + height + margin.bottom, _RGB888, flags);
	if (screen == NULL) {
		logging->out_logf(LOG_ERROR, _T("SDL_SetVideoMode: %s."), SDL_GetError());
		rc = false;
		goto FIN;
	}
	screen_flags = screen->flags;
	if (flags != screen_flags) {
		logging->out_debugf(_T("SDL: different flags 0x%x -> 0x%x."), flags, screen_flags);
	}

	pixel_format->ConvFrom(*screen->format);

	screen_rect.w = screen->w;
	screen_rect.h = screen->h;


#ifdef _DEBUG_LOG
	logging->out_debugf(_T("screen_flags:0x%x"),screen_flags);
	logging->out_debugf(_T("             width:%d height:%d"), width, height);
	logging->out_debugf(_T("screen_rect: width:%d height:%d"), screen_rect.w, screen_rect.h);
	logging->out_debugf(_T("screen->pitch:%d"), screen->pitch);
//	logging->out_debugf(_T("pixel_format->BitsPerPixel:%d"),pixel_format->BitsPerPixel());
//	logging->out_debugf(_T("pixel_format->BytesPerPixel:%d"),pixel_format->BytesPerPixel());
	logging->out_debugf(_T("pixel_format->Rmask:%x, Gmask:%x, Bmask:%x, Amask:%x"),pixel_format->Rmask,pixel_format->Gmask,pixel_format->Bmask,pixel_format->Amask);
	logging->out_debugf(_T("pixel_format->Rshift:%d, Gshift:%d, Bshift:%d, Ashift:%d"),pixel_format->Rshift,pixel_format->Gshift,pixel_format->Bshift,pixel_format->Ashift);
//	logging->out_debugf(_T("pixel_format->Rloss:%d, Gloss:%d, Bloss:%d, Aloss:%d"),pixel_format->Rloss,pixel_format->Gloss,pixel_format->Bloss,pixel_format->Aloss);
//	logging->out_debugf(_T("screen->format->colorkey:%d"),pixel_format->colorkey);
//	logging->out_debugf(_T("screen->format->alpha:%d"),pixel_format->alpha);
#endif

#ifdef USE_OPENGL
	if (flags & SDL_WINDOW_OPENGL) {
		// set pixel format
		pixel_format->PresetRGBA();
		if (!opengl) {
#ifdef USE_OPENGL_WH_ORTHO
			opengl = COpenGL::Create(1);
#else
			opengl = COpenGL::Create();
#endif
		}

		opengl->Initialize();

		// setting polygon
		set_opengl_poly(screen_rect.w, screen_rect.h);

		// create texture
		create_texture();

		// create buffer if need
		opengl->CreateBuffer(src_pyl_l, src_pyl_t, src_pyl_r, src_pyl_b, src_tex_l, src_tex_t, src_tex_r, src_tex_b);
	} else
#endif /* USE_OPENGL */
	{
		use_opengl = 0;
	}

	// create gui window
	if (gui->CreateWidget(screen, screen_rect.w, screen_rect.h) == -1) {
		rc = false;
		goto FIN;
	}

FIN:
	unlock_screen();

	return rc;
}

#elif defined(USE_SDL2)
///
/// create / recreate window on SDL2
///
/// @param[in] disp_no : (unused)
/// @param[in] x       : window position x
/// @param[in] y       : window position y
/// @param[in] width   : window width
/// @param[in] height  : window height
/// @param[in] flags   : (unused)
/// @return true / false
bool EMU_OSD::create_screen(int disp_no, int x, int y, int width, int height, uint32_t flags)
{
	bool rc = true;
	VmRect margin = { 0, 0, 0, 0 };
	VmRectWH screen_rect;

	lock_screen();

	/* SDL2 */
	SDL_RendererInfo info;
#ifdef _DEBUG_LOG
	int nums = SDL_GetNumRenderDrivers();
	for(int i = 0; i < nums; i++) {
		SDL_GetRenderDriverInfo(i, &info);
		logging->out_debugf(_T("RenderDriver: %d %s"), i, info.name);
	}
#endif

	flags &= ~SDL_WINDOW_OPENGL;
#ifdef USE_OPENGL
	if (use_opengl) {
		flags |= SDL_WINDOW_OPENGL;
		release_texture();
		set_opengl_attr();
	}
	if (glcontext) {
		SDL_GL_DeleteContext(glcontext);
		glcontext = NULL;
	}
#endif
	if (window) {
		if (now_screenmode == NOW_FULLSCREEN) {
			flags |= SDL_WINDOW_FULLSCREEN;
		} else {
			flags &= ~SDL_WINDOW_FULLSCREEN;
			margin = display_margin;
		}

		if (texture) {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}
		if (renderer) {
			SDL_DestroyRenderer(renderer);
			renderer = NULL;
		}
		if ((flags & SDL_WINDOW_FULLSCREEN) == 0) {
			// go window mode. So, set window size.
			screen_flags = SDL_GetWindowFlags(window);
			if (screen_flags & SDL_WINDOW_FULLSCREEN) {
				// fullscreen mode to window mode
				if (SDL_SetWindowFullscreen(window, flags) < 0) {
					logging->out_logf(LOG_WARN, _T("SDL_SetWindowFullscreen: %s."), SDL_GetError());
				}
				SDL_SetWindowPosition(window, window_dest.x, window_dest.y);

				gui->ScreenModeChanged(false);

			}
			// change window size
			SDL_SetWindowSize(window, margin.left + width + margin.right, margin.top + height + margin.bottom);
		} else {
			// go fullscreen mode. So, set screen mode
			SDL_DisplayMode mode;
			SDL_Rect re;
			if (SDL_GetCurrentDisplayMode(disp_no, &mode) < 0) {
				logging->out_logf(LOG_WARN, _T("SDL_GetCurrentDisplayMode: %s."), SDL_GetError());
			}
			if (SDL_GetDisplayBounds(disp_no, &re) < 0) {
				logging->out_logf(LOG_WARN, _T("SDL_GetDisplayBounds: %s."), SDL_GetError());
			}
			mode.w = margin.left + width + margin.right;
			mode.h = margin.top + height + margin.bottom;
			mode.format = 0;
			mode.refresh_rate = 0;
//			mode.driverdata = NULL;
			if (SDL_SetWindowDisplayMode(window, &mode) < 0) {
				logging->out_logf(LOG_WARN, _T("SDL_SetWindowDisplayMode: %s."), SDL_GetError());
			}
			SDL_GetWindowPosition(window, &window_dest.x, &window_dest.y);
			SDL_SetWindowPosition(window, re.x, re.y);

			gui->ScreenModeChanged(true);

			if (SDL_SetWindowFullscreen(window, flags) < 0) {
				logging->out_logf(LOG_WARN, _T("SDL_SetWindowFullscreen: %s."), SDL_GetError());
			}
		}
	} else {
		//
		// create window
		//

		SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
//		SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "1");

#ifndef GUI_USE_FOREIGN_WINDOW
		if (screen_mode.WithinDisp(x, y) < 0) {
			x = SDL_WINDOWPOS_UNDEFINED;
			y = SDL_WINDOWPOS_UNDEFINED;
		}

		window = SDL_CreateWindow(DEVICE_NAME, x, y,
			margin.left + width + margin.right, margin.top + height + margin.bottom, flags);
		if (window == NULL) {
			logging->out_logf(LOG_ERROR, _T("SDL_CreateWindow: %s."), SDL_GetError());
			rc = false;
			goto FIN;
		}
#else
		void *data = gui->GetWindowData();
		if (data != NULL) {
			window = SDL_CreateWindowFrom(data);
		}
		if (window == NULL) {
			logging->out_logf(LOG_ERROR, _T("SDL_CreateWindowFrom: %s."), SDL_GetError());
			rc = false;
			goto FIN;
		}
#endif
		if (pixel_format == NULL) {
			logging->out_logf(LOG_ERROR, _T("SDL_AllocFormat: %s."), SDL_GetError());
			rc = false;
			goto FIN;
		}
		pixel_format->ConvFrom(SDL_GetWindowPixelFormat(window));
	}
	screen_flags = SDL_GetWindowFlags(window);
//	screen = SDL_GetWindowSurface(window);
//	if (screen == NULL) {
//		logging->out_logf(LOG_ERROR, _T("SDL_GetWindowSurface: %s."), SDL_GetError());
//		rc = false;
//		goto FIN;
//	}
	SDL_GetWindowSize(window, &screen_rect.w, &screen_rect.h);

#ifdef _DEBUG_LOG
	logging->out_debugf(_T("screen_flags:0x%x"),screen_flags);
	logging->out_debugf(_T("             width:%d height:%d"), width, height);
	logging->out_debugf(_T("screen_rect: width:%d height:%d"), screen_rect.w, screen_rect.h);
//	logging->out_debugf(_T("screen->pitch:%d"), screen->pitch);
//	logging->out_debugf(_T("pixel_format->BitsPerPixel:%d"),pixel_format->BitsPerPixel());
//	logging->out_debugf(_T("pixel_format->BytesPerPixel:%d"),pixel_format->BytesPerPixel());
	logging->out_debugf(_T("pixel_format->Rmask:%x, Gmask:%x, Bmask:%x, Amask:%x"),pixel_format->Rmask,pixel_format->Gmask,pixel_format->Bmask,pixel_format->Amask);
	logging->out_debugf(_T("pixel_format->Rshift:%d, Gshift:%d, Bshift:%d, Ashift:%d"),pixel_format->Rshift,pixel_format->Gshift,pixel_format->Bshift,pixel_format->Ashift);
//	logging->out_debugf(_T("pixel_format->Rloss:%d, Gloss:%d, Bloss:%d, Aloss:%d"),pixel_format->Rloss,pixel_format->Gloss,pixel_format->Bloss,pixel_format->Aloss);
#endif

#ifdef USE_OPENGL
//	if (flags & SDL_WINDOW_OPENGL) {
	if (use_opengl != 0 && (screen_flags & SDL_WINDOW_OPENGL) != 0) {
		glcontext = SDL_GL_CreateContext(window);
		if (glcontext == NULL) {
			logging->out_logf(LOG_WARN, _T("SDL_GL_CreateContext: %s."), SDL_GetError());
		}

		if (!opengl) {
			opengl = COpenGL::Create();
		}

		// set pixel format
		pixel_format->PresetRGBA();

		opengl->Initialize();

		// setting polygon
		set_opengl_poly(screen_rect.w, screen_rect.h);

		// create texture
		create_texture();

		// create buffer if need
		opengl->CreateBuffer(src_pyl_l, src_pyl_t, src_pyl_r, src_pyl_b, src_tex_l, src_tex_t, src_tex_r, src_tex_b);
	} else
#endif /* USE_OPENGL */
	{
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer == NULL) {
			logging->out_logf(LOG_ERROR, _T("SDL_CreateRenderer: %s."), SDL_GetError());
			rc = false;
			goto FIN;
		}
		SDL_GetRendererInfo(renderer, &info);
		logging->out_logf(LOG_DEBUG, _T("Renderer name: %s"), info.name);

		if (!create_sdl_texture()) {
			rc = false;
			goto FIN;
		}
	}

	// create gui window
	if (gui->CreateWidget(window, screen_rect.w, screen_rect.h) == -1) {
		rc = false;
		goto FIN;
	}

FIN:
	unlock_screen();

	return rc;
}
#endif

///
/// create / recreate offline surface
///
bool EMU_OSD::create_offlinesurface()
{
	if (!pixel_format) return false;
#if defined(USE_SDL2) || defined(USE_WX2)
	if (!window) return true;
#else
	if (!screen) return true;
#endif

	if (sufOrigin && !sufOrigin->IsEnable()) {
		if (!sufOrigin->Create(screen_size.w, screen_size.h, *pixel_format)) {
			logging->out_log(LOG_ERROR, _T("EMU_OSD::create_offlinesurface sufOrigin failed."));
			return false;
		}
	}
#ifdef USE_SCREEN_ROTATE
	if (sufRotate) {
		sufRotate->Release();
		if (!sufRotate->Create(source_size.w, source_size.h, *pixel_format)) {
			logging->out_log(LOG_ERROR, _T("EMU_OSD::create_offlinesurface sufRotate failed."));
			return false;
		}
	}
#endif
	if (sufMixed) {
		sufMixed->Release();
		if (!sufMixed->Create(mixed_max_size.w, mixed_max_size.h, *pixel_format)) {
			logging->out_log(LOG_ERROR, _T("EMU_OSD::create_offlinesurface sufMixed failed."));
			return false;
		}
	}
#ifdef USE_SMOOTH_STRETCH
	if (sufStretch1 && sufStretch2) {
		sufStretch1->Release();
		sufStretch2->Release();
		stretch_screen = false;
		if(stretch_power.w != 1 || stretch_power.h != 1) {
			sufStretch1->Create(source_size.w * stretch_power.w, source_size.h * stretch_power.h);
			SetStretchBltMode(sufStretch1->GetDC(), COLORONCOLOR);

			sufStretch2->Create(stretched_size.w, stretched_size.h);
			SetStretchBltMode(sufStretch2->GetDC(), COLORONCOLOR);

			stretch_screen = true;
		}
	}
#endif

#if defined(USE_SDL2) || defined(USE_WX2)
	if (!create_sdl_texture()) {
		return false;
	}
#endif

	disable_screen &= ~DISABLE_SURFACE;

	return true;
}

#if defined(USE_SDL2) || defined(USE_WX2)
bool EMU_OSD::create_sdl_texture()
{
	if (sufMixed->IsEnable() && renderer != NULL && texture == NULL) {
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
//		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING,
			sufMixed->Width(), sufMixed->Height());
		if (!texture) {
			logging->out_logf(LOG_ERROR, _T("SDL_CreateTexture: %s."), SDL_GetError());
			return false;
		}
	}
	return true;
}
#endif

/// setting window or fullscreen size
///
/// @param [in] width : new width or -1 set current width
/// @param [in] height : new height or -1 set current height
/// @param [in] power : magnify x 10
/// @param [in] now_window : true:window / false:fullscreen
void EMU_OSD::set_display_size(int width, int height, int power, bool now_window)
{
//	bool display_size_changed = false;
	bool stretch_changed = false;

	if(width != -1 && (display_size.w != width || display_size.h != height)) {
		display_size.w = width;
		display_size.h = height;
//		display_size_changed = true;
		stretch_changed = true;
	}

#ifdef USE_SCREEN_ROTATE
	VmRectWH prev_source_size = source_size;

	if(config.monitor_type & 1) {
		stretch_changed |= (source_size.w != screen_size.h);
		stretch_changed |= (source_size.h != screen_size.w);
		stretch_changed |= (source_aspect_size.w != screen_aspect_size.h);
		stretch_changed |= (source_aspect_size.h != screen_aspect_size.w);

		source_size.w = screen_size.h;
		source_size.h = screen_size.w;
		source_aspect_size.w = screen_aspect_size.h;
		source_aspect_size.h = screen_aspect_size.w;
	} else
#endif
	{
		stretch_changed |= (source_size.w != screen_size.w);
		stretch_changed |= (source_size.h != screen_size.h);
		stretch_changed |= (source_aspect_size.w != screen_aspect_size.w);
		stretch_changed |= (source_aspect_size.h != screen_aspect_size.h);

		source_size = screen_size;
		source_aspect_size = screen_aspect_size;
	}

	// fullscreen and stretch screen
	if(config.stretch_screen && !now_window && display_size.w >= source_size.w && display_size.h >= source_size.h) {
		if (config.stretch_screen == 1) {
			// fit to full screen
			mixed_size = source_size;
			if (mixed_ratio.w < mixed_ratio.h) {
				mixed_size.h = mixed_size.h * mixed_ratio.w / mixed_ratio.h;
			} else {
				mixed_size.w = mixed_size.w * mixed_ratio.h / mixed_ratio.w;
			}
			mixed_size.x = (source_size.w - mixed_size.w) / 2;
			mixed_size.y = (source_size.h - mixed_size.h) / 2;
			mixed_size.y = adjust_y_position(mixed_size.h, mixed_size.y);

			stretched_size.w = (display_size.h * source_aspect_size.w) / source_aspect_size.h;
			stretched_size.h = display_size.h;
			stretched_dest_real.x = - mixed_size.x * display_size.h / source_aspect_size.h;
			stretched_dest_real.y = - mixed_size.y * display_size.h / source_aspect_size.h;
			if(stretched_size.w > display_size.w) {
				stretched_size.w = display_size.w;
				stretched_size.h = (display_size.w * source_aspect_size.h) / source_aspect_size.w;
				stretched_dest_real.x = - mixed_size.x * display_size.w / source_aspect_size.w;
				stretched_dest_real.y = - mixed_size.y * display_size.w / source_aspect_size.w;
			}
			if (mixed_ratio.w < mixed_ratio.h) {
				stretched_dest_real.y = stretched_dest_real.y * mixed_ratio.h / mixed_ratio.w;
			} else {
				stretched_dest_real.x = stretched_dest_real.x * mixed_ratio.w / mixed_ratio.h;
			}
			stretched_size.x = (display_size.w - stretched_size.w) / 2;
			stretched_size.y = (display_size.h - stretched_size.h) / 2;
			stretched_dest_real.x += stretched_size.x;
			stretched_dest_real.y += stretched_size.y;

		} else {
			// fit text area to full screen (cut off padding area)
			VmSize min_size;
			SIZE_IN(min_size, LIMIT_MIN_WINDOW_WIDTH, LIMIT_MIN_WINDOW_HEIGHT);
#ifdef USE_SCREEN_ROTATE
			if(config.monitor_type & 1) {
				SWAP(int, min_size.w, min_size.h);
			}
#endif
			VmSize mixed_rsize, mixed_rrsize;
			mixed_size = source_size;
			SIZE_IN(mixed_rsize, mixed_size.w, mixed_size.h);
			mixed_rrsize = mixed_rsize;

			VmSize min_rsize, min_rrsize;
			min_rsize = min_size;
			min_rrsize = min_size;
			if (mixed_ratio.w < mixed_ratio.h) {
				min_rsize.h = min_rsize.h * mixed_ratio.h / mixed_ratio.w;
				min_rrsize.h = min_rrsize.h * mixed_ratio.w / mixed_ratio.h;
				mixed_rsize.h = mixed_rsize.h * mixed_ratio.h / mixed_ratio.w;
				mixed_rrsize.h = mixed_rrsize.h * mixed_ratio.w / mixed_ratio.h;
			} else {
				min_rsize.w = min_rsize.w * mixed_ratio.w / mixed_ratio.h;
				min_rrsize.w = min_rrsize.w * mixed_ratio.h / mixed_ratio.w;
				mixed_rsize.w = mixed_rsize.w * mixed_ratio.w / mixed_ratio.h;
				mixed_rrsize.w = mixed_rrsize.w * mixed_ratio.h / mixed_ratio.w;
			}

			double magx = (double)display_size.w / min_rsize.w;
			double magy = (double)display_size.h / min_rsize.h;

			bool mag_based_w = (magx < magy);

			stretched_dest_real.x = 0;
			stretched_dest_real.y = 0;
			if(mag_based_w) {
				// magnify = display_size.w / min_rsize.w

				mixed_size.x = (mixed_size.w - min_size.w) / 2;
				mixed_size.w = min_size.w;
				stretched_size.x = 0;
				stretched_size.w = display_size.w;
				stretched_dest_real.x = (display_size.w - (mixed_rsize.w * display_size.w / min_rsize.w)) / 2;

				if ((mixed_size.h * min_rsize.h * display_size.w / min_rsize.w / min_size.h) >= display_size.h) {
					int mh = display_size.h * min_rrsize.h * min_rsize.w / display_size.w / min_size.h;
					mixed_size.y = (mixed_size.h - mh) / 2;
					mixed_size.h = mh;
					stretched_size.y = 0;
					stretched_size.h = display_size.h;
					stretched_dest_real.y = (display_size.h - (mixed_size.h * display_size.w / min_rsize.w)) / 2;
				} else {
					int mh = mixed_rrsize.h;
					int sh = mixed_size.h * display_size.w / min_rsize.w;
					mixed_size.y = (mixed_size.h - mh) / 2;
					mixed_size.h = mh;
					stretched_size.y = (display_size.h - sh) / 2;
					stretched_size.h = sh;
					stretched_dest_real.y = stretched_size.y;
				}

			} else {
				// magnify = display_size.h / min_rsize.h

				mixed_size.y = (mixed_size.h - min_size.h) / 2;
				mixed_size.h = min_size.h;
				stretched_size.y = 0;
				stretched_size.h = display_size.h;
				stretched_dest_real.y = (display_size.h - (mixed_rsize.h * display_size.h / min_rsize.h)) / 2;

				if ((mixed_size.w * min_rsize.w * display_size.h / min_rsize.h / min_size.w) >= display_size.w) {
					int mw = display_size.w * min_rrsize.w * min_rsize.h / display_size.h / min_size.w;
					mixed_size.x = (mixed_size.w - mw) / 2;
					mixed_size.w = mw;
					stretched_size.x = 0;
					stretched_size.w = display_size.w;
					stretched_dest_real.x = (display_size.w - (mixed_rsize.w * display_size.h / min_rsize.h)) / 2;
				} else {
					int mw = mixed_rrsize.w;
					int sw = mixed_size.w * display_size.h / min_rsize.h;
					mixed_size.x = (mixed_size.w - mw) / 2;
					mixed_size.w = mw;
					stretched_size.x = (display_size.w - sw) / 2;
					stretched_size.w = sw;
					stretched_dest_real.x = stretched_size.x;
				}

			}
		}
	}
	// window or non-streach mode
	else {
		for(int n = 0; n <= 1; n++) {
			if (n == 0) {
				mixed_size.w = display_size.w * 10 / power;
				mixed_size.h = display_size.h * 10 / power;
			} else {
				mixed_size = source_size;
			}
			if (mixed_ratio.w < mixed_ratio.h) {
				mixed_size.h = mixed_size.h * mixed_ratio.w / mixed_ratio.h;
			} else {
				mixed_size.w = mixed_size.w * mixed_ratio.h / mixed_ratio.w;
			}
			mixed_size.x = (source_size.w - mixed_size.w) / 2;
			mixed_size.y = (source_size.h - mixed_size.h) / 2;
			if (mixed_size.x >= 0 && mixed_size.y >= 0) {
				break;
			}
		}
		mixed_size.y = adjust_y_position(mixed_size.h, mixed_size.y);

		stretched_size.w = source_aspect_size.w * power / 10;
		stretched_size.h = source_aspect_size.h * power / 10;
		stretched_size.x = (display_size.w - stretched_size.w) / 2;
		stretched_size.y = (display_size.h - stretched_size.h) / 2;
		stretched_dest_real.x = - mixed_size.x * power / 10;
		stretched_dest_real.y = - mixed_size.y * power / 10;
		if (mixed_ratio.w < mixed_ratio.h) {
			stretched_dest_real.y = stretched_dest_real.y * mixed_ratio.h / mixed_ratio.w;
		} else {
			stretched_dest_real.x = stretched_dest_real.x * mixed_ratio.w / mixed_ratio.h;
		}
		if (stretched_size.x < 0) {
			stretched_size.x = 0;
			stretched_size.w = display_size.w;
		}
		if (stretched_size.y < 0) {
			stretched_size.y = 0;
			stretched_size.h = display_size.h;
		}
		stretched_dest_real.x += stretched_size.x;
		stretched_dest_real.y += stretched_size.y;
	}

	// sufMixed size normalize 2^n for opengl
	mixed_max_size.w = 0;
	mixed_max_size.h = 0;
	// 65536,32768,16384,8192,4096,2048,1024,512,256,128,64,32,16
	for(int i=65536; i>=16; i = (i >> 1)) {
		if (source_size.w <= i) mixed_max_size.w = i;
		if (source_size.h <= i) mixed_max_size.h = i;
	}

	reMix = mixed_size;
	reSuf = stretched_size;

#ifdef USE_OPENGL
	src_tex_l = (GLfloat)mixed_size.x / mixed_max_size.w;
	src_tex_t = (GLfloat)mixed_size.y / mixed_max_size.h;
	src_tex_r = (GLfloat)(mixed_size.x + mixed_size.w) / mixed_max_size.w;
	src_tex_b = (GLfloat)(mixed_size.y + mixed_size.h) / mixed_max_size.h;
	if (stretched_size.w < display_size.w) src_pyl_r = (GLfloat)stretched_size.w / display_size.w;
	else src_pyl_r = 1.0f;
	if (stretched_size.h < display_size.h) src_pyl_b = (GLfloat)stretched_size.h / display_size.h;
	else src_pyl_b = 1.0f;
	src_pyl_l = -src_pyl_r;
	src_pyl_t = -src_pyl_b;

	rePyl.left = stretched_size.x;
	rePyl.top = stretched_size.y;
	rePyl.right = stretched_size.x + stretched_size.w;
	rePyl.bottom = stretched_size.y + stretched_size.h;
#endif

	change_rec_video_size(config.screen_video_size);

	first_invalidate = true;
	screen_size_changed = false;
#ifdef _DEBUG_LOG
	logging->out_debugf(_T("set_display_size: w:%d h:%d power:%d %s"),width,height,power,now_window ? _T("window") : _T("fullscreen"));
	logging->out_debugf(_T("         display: w:%d h:%d"),display_size.w, display_size.h);
	logging->out_debugf(_T("          screen: w:%d h:%d"), screen_size.w, screen_size.h);
	logging->out_debugf(_T("   screen aspect: w:%d h:%d"), screen_aspect_size.w, screen_aspect_size.h);
	logging->out_debugf(_T("          source: w:%d h:%d"), source_size.w, source_size.h);
	logging->out_debugf(_T("   source aspect: w:%d h:%d"), source_aspect_size.w, source_aspect_size.h);
	logging->out_debugf(_T("           mixed: w:%d h:%d"), mixed_size.w, mixed_size.h);
	logging->out_debugf(_T("         stretch: w:%d h:%d"), stretched_size.w, stretched_size.h);
	logging->out_debugf(_T("      mixed dest: x:%d y:%d"), mixed_size.x, mixed_size.y);
	logging->out_debugf(_T("    stretch dest: x:%d y:%d"), stretched_size.x, stretched_size.y);
	logging->out_debugf(_T(" stretch dest re: x:%d y:%d"), stretched_dest_real.x, stretched_dest_real.y);
#ifdef USE_OPENGL
	logging->out_debugf(_T("     src polygon: l:%d t:%d r:%d b:%d"), rePyl.left, rePyl.top, rePyl.right, rePyl.bottom);
	logging->out_debugf(_T("     src polygon: l:%.5f t:%.5f r:%.5f b:%.5f"), src_pyl_l, src_pyl_t, src_pyl_r, src_pyl_b);
	logging->out_debugf(_T("     src texture: l:%.5f t:%.5f r:%.5f b:%.5f"), src_tex_l, src_tex_t, src_tex_r, src_tex_b);
#endif
#endif
	if (now_window) {
		stretched_size.x += display_margin.left;
		stretched_size.y += display_margin.top;
		reSuf.x += display_margin.left;
		reSuf.y += display_margin.top;
#ifdef _DEBUG_LOG
		logging->out_debugf(_T(" margin      suf: l:%d t:%d w:%d h:%d"),reSuf.x,reSuf.y,reSuf.w,reSuf.h);
#endif
#ifdef USE_OPENGL
//		reTex.x += display_margin.left;
//		reTex.y += display_margin.top;
		rePyl.left += display_margin.left;
		rePyl.top += display_margin.top;
		rePyl.right += display_margin.left;
		rePyl.bottom += display_margin.top;
#ifdef _DEBUG_LOG
		logging->out_debugf(_T("     src polygon: l:%d t:%d r:%d b:%d"), rePyl.left, rePyl.top, rePyl.right, rePyl.bottom);
		logging->out_debugf(_T("     src polygon: l:%.5f t:%.5f r:%.5f b:%.5f"), src_pyl_l, src_pyl_t, src_pyl_r, src_pyl_b);
		logging->out_debugf(_T("     src texture: l:%.5f t:%.5f r:%.5f b:%.5f"), src_tex_l, src_tex_t, src_tex_r, src_tex_b);
#endif
#endif
	}

	lock_screen();

#ifdef USE_SCREEN_ROTATE
	if(config.monitor_type) {
		sufSource = sufRotate;
	} else
#endif
	{
		sufSource = sufOrigin;
	}

#ifdef USE_SCREEN_ROTATE
	if ((source_size.w != prev_source_size.w && source_size.h != prev_source_size.h) || stretch_changed) {
		create_offlinesurface();
	}
#endif

	// send display size to vm
	set_vm_display_size();

	if (gui) {
		gui->SetLedBoxPosition(now_window, mixed_size.x, mixed_size.y, mixed_size.w, mixed_size.h, config.led_pos | (is_fullscreen() ? 0x10 : 0));
	}
#ifdef USE_MESSAGE_BOARD
	if (msgboard) {
		msgboard->SetSize(source_size.w, source_size.h);
		msgboard->SetMessagePos(4 + mixed_size.x,  - 4 - source_size.h + mixed_size.y + mixed_size.h, 2);
		msgboard->SetInfoPos(-4 - mixed_size.x, 4 + mixed_size.y, 1);
	}
#endif

	unlock_screen();
}

///
/// draw src screen from virtual machine
///
void EMU_OSD::draw_screen()
{
	// don't draw screen before new screen size is applied to buffers
	if(screen_size_changed) {
		return;
	}

	lock_screen();

	if (sufOrigin->Lock()) {
		if (!config.now_power_off) {
			vm->draw_screen();
		} else {
			fill_gray();
		}
		sufOrigin->Unlock();
	}

	// screen size was changed in vm->draw_screen()
	if(screen_size_changed) {
		unlock_screen();
		return;
	}

#ifdef USE_SCREEN_ROTATE
	// rotate screen
	// right turn
	// src and dst should be the same size
	if(config.monitor_type) {
		int rtype = (config.monitor_type & 3);
		VmSize ss, ds;
		SIZE_IN(ss, screen_size.w, screen_size.h);
		SIZE_IN(ds, source_size.w, source_size.h);
		{
			int nw0 = rotate_matrix[rtype].x[0] < 0 ? (ss.w - 1) : 0;
			int nh0 = rotate_matrix[rtype].y[0] < 0 ? (ss.h - 1) : 0;
			int nw1 = rotate_matrix[rtype].x[1] < 0 ? (ss.w - 1) : 0;
			int nh1 = rotate_matrix[rtype].y[1] < 0 ? (ss.h - 1) : 0;
			for(int sy = 0; sy < ss.h; sy++) {
				scrntype* src = sufOrigin->GetBuffer() + ss.w * sy;
				for(int sx = 0; sx < ss.w; sx++) {
					int dx = nw0 + rotate_matrix[rtype].x[0] * sx + nh0 + rotate_matrix[rtype].y[0] * sy; 
					int dy = nw1 + rotate_matrix[rtype].x[1] * sx + nh1 + rotate_matrix[rtype].y[1] * sy;
					scrntype* dst = sufRotate->GetBuffer() + ds.w * dy + dx;
					*dst = *src;
					src++;
				}
			}
		}
	}
#endif

	{
		if (FLG_SHOWLEDBOX && gui) {
			gui->DrawLedBox(sufSource);
		}
	}

	unlock_screen();
}

///
/// copy src screen to mix screen and overlap a message
///
/// @return false: cannot mix (no allocate mix surface)
bool EMU_OSD::mix_screen()
{
	if (disable_screen) return false;

	lock_screen();

	sufSource->Blit(*sufMixed);

	unlock_screen();

	if (gui) {
		gui->UpdateIndicator(update_led());
	}
#ifdef USE_MESSAGE_BOARD
	if (msgboard && FLG_SHOWMSGBOARD) {
		msgboard->Draw(sufMixed);
	}
#endif

#ifdef USE_OPENGL
//	if (screen_flags & SDL_WINDOW_OPENGL) {
	if (use_opengl > 0) {
		if (mix_texture_name > 0) {
			if (first_invalidate) {
#ifdef USE_OPENGL_WH_ORTHO
				if (opengl->Version() <= 1) {
					src_pyl_l = (GLfloat)rePyl.left;
					src_pyl_t = (GLfloat)rePyl.top;
					src_pyl_r = (GLfloat)rePyl.right;
					src_pyl_b = (GLfloat)rePyl.bottom;
				}
#endif
				opengl->SetTexturePos(src_pyl_l, src_pyl_t, src_pyl_r, src_pyl_b, src_tex_l, src_tex_t, src_tex_r, src_tex_b);
				first_invalidate = false;
			}

			// draw texture using screen pixel buffer
			opengl->Draw(sufMixed);

			if (!self_invalidate) {
				// call this only once at first.
				change_opengl_attr();
			}
		}
	} else
#endif
	{
#ifdef USE_SDL2
		/* SDL2 */
		if (first_invalidate) {
			first_invalidate = false;
		}
		// fill black on screen
		if (renderer) {
			SDL_RenderClear(renderer);
			SDL_UpdateTexture(texture, NULL, sufMixed->GetBuffer(), sufMixed->BytesPerLine());
			// render screen
//			if(stretched_size.w == mixed_size.w && stretched_size.h == mixed_size.h) {
			CSurface::Render(*renderer, *texture, reMix, reSuf);
//			SDL_UTILS::render_copy(renderer, texture, reMix, reSuf);
//			} else {
//				SDL_UTILS::render_copy(renderer, texture, reMix, reSuf);
//			}
		}
#else
		/* SDL1 */
		// fill black on screen
		if (first_invalidate) {
			SDL_Rect re;
			re.x = 0; re.y = 0; re.w = screen->w; re.h = screen->h;
			SDL_FillRect(screen, &re, SDL_MapRGB(screen->format, 0, 0, 0));
			first_invalidate = false;
		}
		// render screen
		if(stretched_size.w == mixed_size.w && stretched_size.h == mixed_size.h) {
			sufMixed->Blit(reMix, *screen, reSuf);
//			SDL_UTILS::blit_surface(sufMixed->Get(), &reMix, screen, &reSuf);
		} else {
			sufMixed->StretchBlit(reMix, *screen, reSuf);
//			SDL_UTILS::zoomin_surface(sufMixed->Get(), &reMix, screen, &reSuf);
		}
#endif
	}

	// invalidate window
	self_invalidate = true;
//	skip_frame = false;

	return true;
}

///
/// post request screen updating to draw it on main thread
///
void EMU_OSD::need_update_screen()
{
	// nothing to do
}

///
/// pointer on source screen
///
/// @return pointer on source screen
scrntype* EMU_OSD::screen_buffer(int y)
{
	return sufOrigin->GetBuffer(y);
}

///
/// offset on source screen
///
/// @return offset on source screen
int EMU_OSD::screen_buffer_offset()
{
	return screen_size.w;
}

///
/// render screen and display window
///
/// @note must be called by main thread
void EMU_OSD::update_screen()
{
//	lock_screen();

	if (mix_screen()) {

#ifdef USE_OPENGL
		if (screen_flags & SDL_WINDOW_OPENGL) {
			gui->MixSurface();
#ifndef USE_SDL2
			SDL_GL_SwapBuffers();
#else
			if (window) SDL_GL_SwapWindow(window);
#endif
		} else
#endif
		{
			// GuiMixScreen include SDL_UpdateRect process.
			if (!gui->MixSurface()) {
#ifndef USE_SDL2
//				SDL_UpdateRect(screen, 0, 0, display_size.w, display_size.h);
				SDL_UpdateRect(screen, 0, 0, 0, 0);
#else
//				if (window) SDL_UpdateWindowSurfaceRects(window, NULL, 0);
				if (renderer) SDL_RenderPresent(renderer);
#endif
			}
		}
	}
//	unlock_screen();
}

///
/// capture current screen and save to a file
///
void EMU_OSD::capture_screen()
{
	int size = config.screen_video_size;
	rec_video->Capture(CAPTURE_SCREEN_TYPE, rec_video_stretched_size, sufSource, rec_video_size[size]);
}

///
/// start recording video
///
bool EMU_OSD::start_rec_video(int type, int fps_no, bool show_dialog)
{
#ifdef USE_REC_VIDEO
	int size = config.screen_video_size;
	return rec_video->Start(type, fps_no, rec_video_size[size], sufSource, show_dialog);
#else
	return false;
#endif
}

///
/// record video
///
void EMU_OSD::record_rec_video()
{
#ifdef USE_REC_VIDEO
	if (rec_video->IsRecordFrame()) {
		int size = config.screen_video_size;
		rec_video->Record(rec_video_stretched_size, sufSource, rec_video_size[size]);
	}
#endif
}

/// store window position to ini file
void EMU_OSD::resume_window_placement()
{
#ifdef USE_SDL2
	if (now_screenmode == NOW_FULLSCREEN) {
		config.window_position_x = window_dest.x;
		config.window_position_y = window_dest.y;
	} else {
		int x = 0;
		int y = 0;
		SDL_GetWindowPosition(window, &x, &y);
		config.window_position_x = x;
		config.window_position_y = y;
	}
#endif
}

/// change window size / switch over fullscreen and window
/// @param[in] mode 0 - 7: window size  8 -:  fullscreen size  -1: switch over  -2: shift window mode
void EMU_OSD::change_screen_mode(int mode)
{
//	logging->out_debugf(_T("change_screen_mode: mode:%d cwmode:%d pwmode:%d w:%d h:%d"),mode,config.window_mode,prev_window_mode,desktop_size.w,desktop_size.h);
//	if (mode == config.window_mode) return;
	if (now_resizing) {
		// ignore events
		return;
	}

	if (mode == -1) {
		// switch over fullscreen and window
		if (now_screenmode != NOW_WINDOW) {
			// go window mode
			mode = prev_window_mode;
		}
	} else if (mode == -2) {
		// shift window mode 
		if (now_screenmode != NOW_WINDOW) {
			// no change
			return;
		} else {
			mode = ((config.window_mode + 1) % window_mode.Count());
		}
	}
	if (now_screenmode != NOW_FULLSCREEN) {
		prev_window_mode = config.window_mode;
	}
//	logging->out_debugf(_T("change_screen_mode: mode:%d cwmode:%d pwmode:%d w:%d h:%d"),mode,config.window_mode,prev_window_mode,desktop_size.w,desktop_size.h);
	set_window(mode, desktop_size.w, desktop_size.h);
	if (!create_screen(config.disp_device_no, 0, 0, config.screen_width, config.screen_height, screen_flags)) {
		exit(1);
	}
	first_change_screen = false;
}

/// setting window or fullscreen size to display
/// @param [in] mode 0 .. 7 window mode / 8 .. 23 fullscreen mode / -1 want to go fullscreen, but unknown mode
/// @param [in] cur_width  current desktop width if mode is -1
/// @param [in] cur_height current desktop height if mode is -1
void EMU_OSD::set_window(int mode, int cur_width, int cur_height)
{
	logging->out_debugf(_T("set_window: mode:%d"), mode);

	now_resizing = true;

	if(mode >= 0 && mode < 8) {
		// go window
		if (mode >= window_mode.Count()) mode = 0;
		const CWindowMode *wm = window_mode.Get(mode);
		int width = wm->width;
		int height = wm->height;
#ifdef USE_SCREEN_ROTATE
		if (config.monitor_type & 1) {
			int v = width;
			width = height;
			height = v;
		}
#endif

		config.window_mode = mode;
		config.disp_device_no = 0;
		config.screen_width = width;
		config.screen_height = height;
		window_mode_power = wm->power;

		now_screenmode = NOW_WINDOW;
#ifndef USE_SDL2
		gui->ScreenModeChanged(false);
#endif
		logging->out_debugf(_T("set_window: f->w mode:%d w:%d h:%d nf:%d"), mode, width, height, (int)now_screenmode);

		// set screen size to emu class
		set_display_size(width, height, window_mode_power, true);
	}
	else if(now_screenmode != NOW_FULLSCREEN) {
		// go fullscreen

		// get current position of window
#ifndef USE_SDL2
		window_dest.x = window_dest.y = 0;
#else
		SDL_GetWindowPosition(window, &window_dest.x, &window_dest.y);
#endif

		const CVideoMode *sm = NULL;
		int disp_no = 0;
		int width = 0;
		int height = 0;
//		int left = 0;
//		int top = 0;

		if (mode >= 8) {
			// check mode number is valid
			if (!screen_mode.GetMode((mode - 8) / VIDEO_MODE_MAX, (mode - 8) % VIDEO_MODE_MAX)) {
				mode = -1;
			}
		}

		if(mode == -1) {
			// search current monitor
			disp_no = screen_mode.WithinDisp(window_dest.x, window_dest.y);
			if (disp_no < 0) {
				disp_no = 0;
			}
			// get width and height
			const CDisplayDevice *dd = screen_mode.GetDisp(disp_no);
//			left = dd->re.x;
//			top = dd->re.y;
			width = dd->re.w;
			height = dd->re.h;

			// matching width and height
			int find = screen_mode.FindMode(disp_no, width, height);
			if (find >= 0) {
				mode = find + (disp_no * VIDEO_MODE_MAX) + 8;
			} else {
				mode = 8;
			}
		} else {
			disp_no = (mode - 8) / VIDEO_MODE_MAX;
			sm = screen_mode.GetMode(disp_no, (mode - 8) % VIDEO_MODE_MAX);

			const CDisplayDevice *dd = screen_mode.GetDisp(disp_no);
//			left =  dd->re.x;
//			top = dd->re.y;
			width = sm ? sm->width : dd->re.w;
			height = sm ? sm->height : dd->re.h;
		}

		config.window_mode = mode;
		config.disp_device_no = disp_no;
		config.screen_width = width;
		config.screen_height = height;

		now_screenmode = NOW_FULLSCREEN;
#ifndef USE_SDL2
		gui->ScreenModeChanged(true);
#endif
		logging->out_debugf(_T("set_window: w->f mode:%d w:%d h:%d nf:%d"), mode, width, height, (int)now_screenmode);

		// set screen size to emu class
		set_display_size(width, height, 10, false);
	} else {
		// now fullscreen
		if (mode >= screen_mode.CountMode(0) + 8) {
			mode = -1;
		}
		const CVideoMode *sm = screen_mode.GetMode(0, mode - 8);
		int width = sm ? sm->width : cur_width;
		int height = sm ? sm->height : cur_height;

		logging->out_debugf(_T("set_window: f->f mode:%d w:%d h:%d nf:%d"), mode, width, height, (int)now_screenmode);

		// set screen size to emu class
		set_display_size(width, height, 10, false);
	}
	now_resizing = false;
}

// ----------
#ifdef USE_OPENGL

void EMU_OSD::initialize_opengl()
{
}

void EMU_OSD::create_texture()
{
#ifdef USE_SDL2
	if (!window) return;
	if (SDL_GL_SetSwapInterval(use_opengl == 1 ? 1 : 0) < 0) {
		logging->out_logf(LOG_WARN, _T("SDL_GL_SetSwapInterval: %s."), SDL_GetError());
	}
#endif
	if (mix_texture_name != 0) return;

	// create
	mix_texture_name = opengl->CreateTexture(config.gl_filter_type);

	opengl->ClearScreen();

#ifndef USE_SDL2
	SDL_GL_SwapBuffers();
#else
	SDL_GL_SwapWindow(window);
#endif
}

void EMU_OSD::change_opengl_attr()
{
	if (!use_opengl) {
		return;
	}

	opengl->SetTextureFilter(config.gl_filter_type);
}

void EMU_OSD::release_opengl()
{
	if (use_opengl) {
		release_texture();
#ifdef USE_SDL2
		if (glcontext) {
			SDL_GL_DeleteContext(glcontext);
		}
#endif
	}
	delete opengl;
	opengl = NULL;
}

void EMU_OSD::release_texture()
{
	if (opengl) {
		mix_texture_name = opengl->ReleaseTexture();
	}
}

void EMU_OSD::set_opengl_attr()
{
	// set OpenGL atributes
#if defined(_RGB555)
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 0 );
	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 16 );
#elif defined(_RGB565)
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 0 );
	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 16 );
#elif defined(_RGB888)
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 0 );
	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
#endif
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
//		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 0 );
#ifdef __WIN32__
	// if windows, multisample must be spcify
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 1 );
#endif
	if (opengl && use_opengl != 0) {
#ifdef USE_SDL2
		if (SDL_GL_SetSwapInterval( use_opengl == 1 ? 1 : 0 ) < 0) {
			logging->out_logf(LOG_WARN, _T("SDL_GL_SetSwapInterval: %s."), SDL_GetError());
		}
#else
		if (SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, use_opengl == 1 ? 1 : 0 ) < 0) {
			logging->out_logf(LOG_WARN, _T("SDL_GL_SWAP_CONTROL: %s."), SDL_GetError());
		}
#endif
	}
}

void EMU_OSD::set_opengl_poly(int width, int height)
{
#ifdef _DEBUG_LOG
	int v;
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &v); logging->out_debugf(_T("SDL_GL_RED_SIZE:%d"), v);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &v); logging->out_debugf(_T("SDL_GL_GREEN_SIZE:%d"), v);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &v); logging->out_debugf(_T("SDL_GL_BLUE_SIZE:%d"), v);
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &v); logging->out_debugf(_T("SDL_GL_ALPHA_SIZE:%d"), v);
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &v); logging->out_debugf(_T("SDL_GL_DEPTH_SIZE:%d"), v);
	SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &v); logging->out_debugf(_T("SDL_GL_BUFFER_SIZE:%d"), v);
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &v); logging->out_debugf(_T("SDL_GL_DOUBLEBUFFER:%d"), v);
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &v); logging->out_debugf(_T("SDL_GL_MULTISAMPLEBUFFERS:%d"), v);
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &v); logging->out_debugf(_T("SDL_GL_MULTISAMPLESAMPLES:%d"), v);
#ifndef USE_SDL2
	SDL_GL_GetAttribute(SDL_GL_SWAP_CONTROL, &v); logging->out_logf(LOG_DEBUG, _T("SDL_GL_SWAP_CONTROL:%d"), v);
#else
	logging->out_logf(LOG_DEBUG, _T("SDL_GL_GetSwapInterval:%d"), SDL_GL_GetSwapInterval());
#endif
#endif

	if (opengl) opengl->InitViewport(width, height);
}

#endif // USE_OPENGL
