/** @file rec_video_defs.h

	Skelton for retropc emulator

	@author Sasaji
	@date   2019.10.01 -

	@brief [ record video definition ]
*/

#ifndef RECORD_VIDEO_DEFS_H
#define RECORD_VIDEO_DEFS_H

// =====================================================================
#if defined(USE_WIN)
// windows native edition

#define USE_REC_VIDEO
#define USE_REC_VIDEO_VFW
#define USE_REC_VIDEO_MMF		1
#define USE_REC_VIDEO_FFMPEG	1
#define USE_CAP_SCREEN_WIN		1

#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
#define USE_REC_AUDIO_MMF		2
#define USE_REC_AUDIO_FFMPEG	2

// =====================================================================
#elif defined(USE_SDL) || defined(USE_SDL2)
// SDL edition

// -----------------------------------------------------
#if defined(_WIN32)
// windows

#define USE_REC_VIDEO
#define USE_REC_VIDEO_VFW
//#define USE_REC_VIDEO_MMF		1
#define USE_REC_VIDEO_FFMPEG	1
#define USE_CAP_SCREEN_WIN		1

#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
//#define USE_REC_AUDIO_MMF		2
#define USE_REC_AUDIO_FFMPEG	2

// -----------------------------------------------------
#elif (defined(__APPLE__) && defined(__MACH__))
// macosx
#include <AvailabilityMacros.h>

//#define USE_REC_VIDEO
#define USE_REC_VIDEO_AVKIT		0x01
#if !defined(MAC_OS_X_VERSION_10_13)
#define USE_REC_VIDEO_QTKIT		0x02
#endif
#define USE_REC_VIDEO_FFMPEG	1
#define USE_CAP_SCREEN_COCOA		1

//#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
#define USE_REC_AUDIO_AVKIT		0x10
#define USE_REC_AUDIO_FFMPEG	2

// -----------------------------------------------------
#else
//linux (UNIX)

#define USE_REC_VIDEO
#define USE_REC_VIDEO_FFMPEG	1
#define USE_CAP_SCREEN_LIBPNG	1

#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
#define USE_REC_AUDIO_FFMPEG	2

#endif

// =====================================================================
#elif defined(USE_WX) || defined(USE_WX2)
// wxWidgets edition

// -----------------------------------------------------
#if defined(_WIN32)
// windows

#define USE_REC_VIDEO
#define USE_REC_VIDEO_VFW
//#define USE_REC_VIDEO_MMF		1
#define USE_REC_VIDEO_FFMPEG	1
//#define USE_CAP_SCREEN_WIN		1
#define USE_CAP_SCREEN_WX		1

#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
//#define USE_REC_AUDIO_MMF		2
#define USE_REC_AUDIO_FFMPEG	2

// -----------------------------------------------------
#elif (defined(__APPLE__) && defined(__MACH__))
// macosx

#define USE_REC_VIDEO
#define USE_REC_VIDEO_AVKIT		0x01
#define USE_REC_VIDEO_QTKIT		0x02
#define USE_REC_VIDEO_FFMPEG	1
//#define USE_CAP_SCREEN_COCOA		1
#define USE_CAP_SCREEN_WX		1

#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
#define USE_REC_AUDIO_AVKIT		0x10
#define USE_REC_AUDIO_FFMPEG	2

// -----------------------------------------------------
#else
//linux (UNIX)

#define USE_REC_VIDEO
#define USE_REC_VIDEO_FFMPEG	1
//#define USE_CAP_SCREEN_LIBPNG	1
#define USE_CAP_SCREEN_WX		1

#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
#define USE_REC_AUDIO_FFMPEG	2

#endif

// =====================================================================
#elif defined(USE_QT)
// Qt edition

// -----------------------------------------------------
#if defined(_WIN32)
// windows

#define USE_REC_VIDEO
#define USE_REC_VIDEO_VFW
//#define USE_REC_VIDEO_MMF		1
#define USE_REC_VIDEO_FFMPEG	1
//#define USE_CAP_SCREEN_WIN		1
#define USE_CAP_SCREEN_QT		1

#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
//#define USE_REC_AUDIO_MMF		2
#define USE_REC_AUDIO_FFMPEG	2

// -----------------------------------------------------
#elif (defined(__APPLE__) && defined(__MACH__))
// macosx

//#define USE_REC_VIDEO
//#define USE_REC_VIDEO_AVKIT		0x01
//#define USE_REC_VIDEO_QTKIT		0x02
//#define USE_REC_VIDEO_FFMPEG	1
#define USE_CAP_SCREEN_QT		1

//#define USE_REC_AUDIO
//#define USE_REC_AUDIO_WAVE
//#define USE_REC_AUDIO_AVKIT		0x10
//#define USE_REC_AUDIO_FFMPEG	2

// -----------------------------------------------------
#else
//linux (UNIX)

#define USE_REC_VIDEO
#define USE_REC_VIDEO_FFMPEG	1
#define USE_CAP_SCREEN_QT		1

#define USE_REC_AUDIO
#define USE_REC_AUDIO_WAVE
#define USE_REC_AUDIO_FFMPEG	2

#endif

// =====================================================================
#endif

#if defined(USE_CAP_SCREEN_WIN) \
 || defined(USE_CAP_SCREEN_COCOA) \
 || defined(USE_CAP_SCREEN_LIBPNG) \
 || defined(USE_CAP_SCREEN_WX) \
 || defined(USE_CAP_SCREEN_QT)
#define USE_CAPTURE_SCREEN_PNG
#endif

#endif /* RECORD_VIDEO_DEFS_H */
