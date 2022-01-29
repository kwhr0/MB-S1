/** @file common.h

	Skelton for retropc emulator

	@author Takeda.Toshiya
	@date   2006.08.18 -

	@brief [ common header ]
*/

#ifndef COMMON_H
#define COMMON_H

#if defined(USE_WIN)
#include "osd/windows/win_common.h"
#elif defined(USE_SDL) || defined(USE_SDL2)
#include "osd/SDL/sdl_common.h"
#elif defined(USE_QT)
#include "osd/qt/qt_common.h"
#elif defined(USE_WX) || defined(USE_WX2)
#include "osd/wxwidgets/wxw_common.h"
#endif

/* endian */
#if (defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)) \
 || (defined(BYTE_ORDER) && (BYTE_ORDER == BIG_ENDIAN))
	/* big endian */
#define USE_BIG_ENDIAN 1
#endif

typedef union {
	struct {
#ifdef USE_BIG_ENDIAN
		uint8_t h, l;
#else
		uint8_t l, h;
#endif
	} b;
	uint16_t u16;
	int16_t s16;
} pair16_t;

typedef union {
	struct {
#ifdef USE_BIG_ENDIAN
		uint8_t h3, h2, h, l;
#else
		uint8_t l, h, h2, h3;
#endif
	} b;
	struct {
#ifdef USE_BIG_ENDIAN
		int8_t h3, h2, h, l;
#else
		int8_t l, h, h2, h3;
#endif
	} sb;
	struct {
#ifdef USE_BIG_ENDIAN
		uint16_t h, l;
#else
		uint16_t l, h;
#endif
	} w;
	struct {
#ifdef USE_BIG_ENDIAN
		int16_t h, l;
#else
		int16_t l, h;
#endif
	} sw;
	uint32_t u32;
	int32_t s32;
	uint32_t d;
	int32_t sd;
} pair32_t;

typedef union {
	struct {
#ifdef USE_BIG_ENDIAN
		uint8_t h7, h6, h5, h4, h3, h2, h, l;
#else
		uint8_t l, h, h2, h3, h4, h5, h6, h7;
#endif
	} b;
	uint64_t u64;
	int64_t s64;
} pair64_t;

/// unused parameter
#define UNUSED_PARAM(x)

/// rgb color
/// @attention 32bit color only. 16bit color was no longer supported.
#define _RGB888    32

#if defined(_RGB555)
#define RGB_COLOR(r, g, b) ((uint16_t)(((uint16_t)(r) & 0xf8) << 7) | (uint16_t)(((uint16_t)(g) & 0xf8) << 2) | (uint16_t)(((uint16_t)(b) & 0xf8) >> 3))
typedef uint16_t scrntype;
#elif defined(_RGB565)
#define RGB_COLOR(r, g, b) ((uint16_t)(((uint16_t)(r) & 0xf8) << 8) | (uint16_t)(((uint16_t)(g) & 0xfc) << 3) | (uint16_t)(((uint16_t)(b) & 0xf8) >> 3))
typedef uint16_t scrntype;
#elif defined(_RGB888)
#define RGB_COLOR(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | ((uint32_t)(b) << 0))
typedef uint32_t scrntype;
#endif

#ifdef USE_BIG_ENDIAN
inline uint16_t swap16(uint16_t x);
inline uint32_t swap32(uint32_t x);
inline uint64_t swap64(uint64_t x);
#define Int16_LE(x) (int16_t)swap16((uint16_t)(x))
#define Int32_LE(x) (int32_t)swap32((uint32_t)(x))
#define Int64_LE(x) (int64_t)swap64((uint64_t)(x))
#define Uint16_LE(x) swap16(x)
#define Uint32_LE(x) swap32(x)
#define Uint64_LE(x) swap64(x)
#else
#define Int16_LE(x) (x)
#define Int32_LE(x) (x)
#define Int64_LE(x) (x)
#define Uint16_LE(x) (x)
#define Uint32_LE(x) (x)
#define Uint64_LE(x) (x)
#endif

uint16_t conv_to_uint16_le(uint8_t *);
uint32_t conv_to_uint32_le(uint8_t *);
void conv_from_uint16_le(uint8_t *, uint16_t);
void conv_from_uint32_le(uint8_t *, uint32_t);

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef SWAP
#define SWAP(TYPE,a,b) { TYPE tmp = (a); (a) = (b); (b) = tmp; } 
#endif

#ifndef _MAX_PATH
#define _MAX_PATH	(260)
#endif

#define TO_BCD_LO(v)	((v) % 10)
#define TO_BCD_HI(v)	(int)(((v) % 100) / 10)

// null pointer
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#define nullptr ((void *)0)
#endif

typedef struct stVmRectWH {
	int x;
	int y;
	int w;
	int h;
} VmRectWH;

typedef struct stVmPoint {
	int x;
	int y;
} VmPoint;

typedef struct stVmSize {
	int w;
	int h;
} VmSize;

typedef struct stVmRect {
	int left;
	int top;
	int right;
	int bottom;
} VmRect;

typedef struct stVmMatrix {
	int x[2];
	int y[2];
} VmMatrix;

#define RECT_IN(st, xx, yy, ww, hh) { \
	st.x = (xx); \
	st.y = (yy); \
	st.w = (ww); \
	st.h = (hh); \
}

#define POINT_IN(st, xx, yy) { \
	st.x = (xx); \
	st.y = (yy); \
}

#define SIZE_IN(st, ww, hh) { \
	st.w = (ww); \
	st.h = (hh); \
}

#define MATRIX_IN(st, x0, y0, x1, y1) { \
	st.x[0] = (x0); \
	st.y[0] = (y0); \
	st.x[1] = (x1); \
	st.y[1] = (y1); \
}

#ifdef USE_AUDIO_U8
typedef uint8_t audio_sample_t;
#else
typedef int16_t audio_sample_t;
#endif

#endif /* COMMON_H */
