/** @file fileio.cpp

	Skelton for retropc emulator

	@author Takeda.Toshiya
	@date   2006.08.18 -

	@brief [ file i/o ]
*/

#if defined(_WIN32) && !defined(NO_USE_WINAPI)
#include "depend.h"
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif
#if !defined(USE_WIN)
#include <sys/stat.h>
#endif
#include "fileio.h"
#include "utility.h"

FILEIO::FILEIO()
{
	fp = NULL;
}

FILEIO::~FILEIO()
{
	Fclose();
}

int FILEIO::GetAttribute(const _TCHAR *filename, int *flags)
{
#if defined(_WIN32) && !defined(NO_USE_WINAPI)
#if defined(USE_UTF8_ON_MBCS)
	// convert UTF-8 to MBCS string
	_TCHAR tfilename[_MAX_PATH];
	UTILITY::conv_to_native_path(filename, tfilename, _MAX_PATH);
#else
	const _TCHAR *tfilename = filename;
#endif
#if defined(USE_WIN)
	// use windows API
	DWORD attr = GetFileAttributes(tfilename);
	if(attr == -1) {
		return -1;
	}
	return (int)attr;
#else
	struct _stat fs;

	if (_tstat(tfilename, &fs)) {
		return -1;
	}
	return (int)fs.st_mode;
#endif
#else
#if defined(_UNICODE)
	// convert wchar_t to char
	char tfilename[_MAX_PATH];
	UTILITY::cconv_from_native_path(filename, tfilename, _MAX_PATH);
#else
	const char *tfilename = filename;
#endif

	struct stat fs;

	if (stat(tfilename, &fs)) {
		return -1;
	}
#if defined(__APPLE__) && defined(__MACH__)
	if (flags) {
		*flags = (int)fs.st_flags;
	}
#endif
	return (int)fs.st_mode;
#endif
}

bool FILEIO::IsFileExists(const _TCHAR *filename)
{
	int attr = GetAttribute(filename, NULL);
#if defined(_WIN32)
#if defined(USE_WIN)
	return (attr >= 0 && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	return (attr >= 0 && (attr & _S_IFREG) != 0);
#endif
#else
	return (attr >= 0 && (attr & S_IFREG) != 0);
#endif
}

bool FILEIO::IsFileProtected(const _TCHAR *filename)
{
	int flags = 0;
	int attr = GetAttribute(filename, &flags);
#if defined(_WIN32)
#if defined(USE_WIN)
	// use windows API
	return (attr >= 0 && (attr & FILE_ATTRIBUTE_READONLY) != 0);
#else
	return (attr >= 0 && (attr & _S_IWRITE) == 0);
#endif
#else
#if defined(__APPLE__) && defined(__MACH__)
	return (attr >= 0 && ((attr & S_IWUSR) == 0 || (flags & 2) != 0));
#else
	return (attr >= 0 && (attr & S_IWUSR) == 0);
#endif
#endif
}

bool FILEIO::IsDirExists(const _TCHAR *dirname)
{
	int attr = GetAttribute(dirname, NULL);
#if defined(_WIN32)
#if defined(USE_WIN)
	return (attr >= 0 && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else
	return (attr >= 0 && (attr & _S_IFDIR) != 0);
#endif
#else
	return (attr >= 0 && (attr & S_IFDIR) != 0);
#endif
}

void FILEIO::RemoveFile(const _TCHAR *filename)
{
#if defined(_WIN32) && !defined(NO_USE_WINAPI)
#if defined(USE_UTF8_ON_MBCS)
	// convert UTF-8 to MBCS string
	_TCHAR tfilename[_MAX_PATH];
	UTILITY::conv_to_native_path(filename, tfilename, _MAX_PATH);
#else
	const _TCHAR *tfilename = filename;
#endif
#if defined(USE_WIN)
	// use windows API
	DeleteFile(tfilename);
//	_tremove(tfilename);	// not supported on wince
#else
	_tunlink(tfilename);
#endif
#else
#if defined(_UNICODE)
	// convert wchar_t to char
	char tfilename[_MAX_PATH];
	UTILITY::cconv_from_native_path(filename, tfilename, _MAX_PATH);
#else
	const char *tfilename = filename;
#endif
	unlink(tfilename);
#endif
}

bool FILEIO::Fopen(const _TCHAR *filename, int mode)
{
	Fclose();

#if defined(_WIN32) || !defined(_UNICODE)
#if defined(USE_UTF8_ON_MBCS)
	// convert UTF-8 to MBCS string
	_TCHAR tfilename[_MAX_PATH];
	UTILITY::conv_to_native_path(filename, tfilename, _MAX_PATH);
#else
	const _TCHAR *tfilename = filename;
#endif
	switch(mode) {
	case FILEIO_READ_BINARY:
		return ((fp = _tfopen(tfilename, _T("rb"))) != NULL);
	case FILEIO_WRITE_BINARY:
		return ((fp = _tfopen(tfilename, _T("wb"))) != NULL);
	case FILEIO_READ_WRITE_BINARY:
		return ((fp = _tfopen(tfilename, _T("r+b"))) != NULL);
	case FILEIO_READ_WRITE_NEW_BINARY:
		return ((fp = _tfopen(tfilename, _T("w+b"))) != NULL);
	case FILEIO_READ_ASCII:
		return ((fp = _tfopen(tfilename, _T("r"))) != NULL);
	case FILEIO_WRITE_ASCII:
		return ((fp = _tfopen(tfilename, _T("w"))) != NULL);
	case FILEIO_READ_WRITE_ASCII:
		return ((fp = _tfopen(tfilename, _T("r+"))) != NULL);
	case FILEIO_READ_WRITE_NEW_ASCII:
		return ((fp = _tfopen(tfilename, _T("w+"))) != NULL);
	}
#else
	// convert wchar_t to char
	char tfilename[_MAX_PATH];
	UTILITY::cconv_from_native_path(filename, tfilename, _MAX_PATH);
	switch(mode) {
	case FILEIO_READ_BINARY:
		return ((fp = fopen(tfilename, "rb")) != NULL);
	case FILEIO_WRITE_BINARY:
		return ((fp = fopen(tfilename, "wb")) != NULL);
	case FILEIO_READ_WRITE_BINARY:
		return ((fp = fopen(tfilename, "r+b")) != NULL);
	case FILEIO_READ_WRITE_NEW_BINARY:
		return ((fp = fopen(tfilename, "w+b")) != NULL);
	case FILEIO_READ_ASCII:
		return ((fp = fopen(tfilename, "r")) != NULL);
	case FILEIO_WRITE_ASCII:
		return ((fp = fopen(tfilename, "w")) != NULL);
	case FILEIO_READ_WRITE_ASCII:
		return ((fp = fopen(tfilename, "r+")) != NULL);
	case FILEIO_READ_WRITE_NEW_ASCII:
		return ((fp = fopen(tfilename, "w+")) != NULL);
	}
#endif
	return false;
}

void FILEIO::Fclose()
{
	if(fp) {
		fclose(fp);
	}
	fp = NULL;
}

uint32_t FILEIO::FileLength()
{
	long pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	return (uint32_t)len;
}

/*
#define GET_VALUE(type) \
	uint8_t buffer[sizeof(type)]; \
	fread(buffer, sizeof(buffer), 1, fp); \
	return *(type *)buffer
*/

#define GET_VALUE(type) \
	type value; \
	fread((void *)&value, sizeof(value), 1, fp); \
	return value

#define PUT_VALUE(type, v) \
	fwrite(&v, sizeof(type), 1, fp)

bool FILEIO::FgetBool()
{
	return (fgetc(fp) == 1);
}

void FILEIO::FputBool(bool val)
{
	fputc(val ? 1 : 0, fp);
}

uint8_t FILEIO::FgetUint8()
{
	GET_VALUE(uint8_t);
}

void FILEIO::FputUint8(uint8_t val)
{
	PUT_VALUE(uint8_t, val);
}

#if 0
uint16_t FILEIO::FgetUint16()
{
	GET_VALUE(uint16_t);
}

void FILEIO::FputUint16(uint16_t val)
{
	PUT_VALUE(uint16_t, val);
}

uint32_t FILEIO::FgetUint32()
{
	GET_VALUE(uint32_t);
}

void FILEIO::FputUint32(uint32_t val)
{
	PUT_VALUE(uint32_t, val);
}

uint64_t FILEIO::FgetUint64()
{
	GET_VALUE(uint64_t);
}

void FILEIO::FputUint64(uint64_t val)
{
	PUT_VALUE(uint64_t, val);
}
#endif

int8_t FILEIO::FgetInt8()
{
	GET_VALUE(int8_t);
}

void FILEIO::FputInt8(int8_t val)
{
	PUT_VALUE(int8_t, val);
}

#if 0
int16_t FILEIO::FgetInt16()
{
	GET_VALUE(int16_t);
}

void FILEIO::FputInt16(int16_t val)
{
	PUT_VALUE(int16_t, val);
}

int32_t FILEIO::FgetInt32()
{
	GET_VALUE(int32_t);
}

void FILEIO::FputInt32(int32_t val)
{
	PUT_VALUE(int32_t, val);
}

int64_t FILEIO::FgetInt64()
{
	GET_VALUE(int64_t);
}

void FILEIO::FputInt64(int64_t val)
{
	PUT_VALUE(int64_t, val);
}
#endif

float FILEIO::FgetFloat()
{
	GET_VALUE(float);
}

void FILEIO::FputFloat(float val)
{
	PUT_VALUE(float, val);
}

double FILEIO::FgetDouble()
{
	GET_VALUE(double);
}

void FILEIO::FputDouble(double val)
{
	PUT_VALUE(double, val);
}

uint16_t FILEIO::FgetUint16_LE()
{
	pair16_t tmp;
	tmp.b.l = FgetUint8();
	tmp.b.h = FgetUint8();
	return tmp.u16;
}

void FILEIO::FputUint16_LE(uint16_t val)
{
	pair16_t tmp;
	tmp.u16 = val;
	FputUint8(tmp.b.l);
	FputUint8(tmp.b.h);
}

uint32_t FILEIO::FgetUint32_LE()
{
	pair32_t tmp;
	tmp.b.l  = FgetUint8();
	tmp.b.h  = FgetUint8();
	tmp.b.h2 = FgetUint8();
	tmp.b.h3 = FgetUint8();
	return tmp.u32;
}

void FILEIO::FputUint32_LE(uint32_t val)
{
	pair32_t tmp;
	tmp.u32 = val;
	FputUint8(tmp.b.l);
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.h2);
	FputUint8(tmp.b.h3);
}

uint64_t FILEIO::FgetUint64_LE()
{
	pair64_t tmp;
	tmp.b.l  = FgetUint8();
	tmp.b.h  = FgetUint8();
	tmp.b.h2 = FgetUint8();
	tmp.b.h3 = FgetUint8();
	tmp.b.h4 = FgetUint8();
	tmp.b.h5 = FgetUint8();
	tmp.b.h6 = FgetUint8();
	tmp.b.h7 = FgetUint8();
	return tmp.u64;
}

void FILEIO::FputUint64_LE(uint64_t val)
{
	pair64_t tmp;
	tmp.u64 = val;
	FputUint8(tmp.b.l);
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.h2);
	FputUint8(tmp.b.h3);
	FputUint8(tmp.b.h4);
	FputUint8(tmp.b.h5);
	FputUint8(tmp.b.h6);
	FputUint8(tmp.b.h7);
}

int16_t FILEIO::FgetInt16_LE()
{
	pair16_t tmp;
	tmp.b.l = FgetUint8();
	tmp.b.h = FgetUint8();
	return tmp.s16;
}

void FILEIO::FputInt16_LE(int16_t val)
{
	pair16_t tmp;
	tmp.s16 = val;
	FputUint8(tmp.b.l);
	FputUint8(tmp.b.h);
}

int32_t FILEIO::FgetInt32_LE()
{
	pair32_t tmp;
	tmp.b.l  = FgetUint8();
	tmp.b.h  = FgetUint8();
	tmp.b.h2 = FgetUint8();
	tmp.b.h3 = FgetUint8();
	return tmp.s32;
}

void FILEIO::FputInt32_LE(int32_t val)
{
	pair32_t tmp;
	tmp.s32 = val;
	FputUint8(tmp.b.l);
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.h2);
	FputUint8(tmp.b.h3);
}

int64_t FILEIO::FgetInt64_LE()
{
	pair64_t tmp;
	tmp.b.l  = FgetUint8();
	tmp.b.h  = FgetUint8();
	tmp.b.h2 = FgetUint8();
	tmp.b.h3 = FgetUint8();
	tmp.b.h4 = FgetUint8();
	tmp.b.h5 = FgetUint8();
	tmp.b.h6 = FgetUint8();
	tmp.b.h7 = FgetUint8();
	return tmp.s64;
}

void FILEIO::FputInt64_LE(int64_t val)
{
	pair64_t tmp;
	tmp.s64 = val;
	FputUint8(tmp.b.l);
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.h2);
	FputUint8(tmp.b.h3);
	FputUint8(tmp.b.h4);
	FputUint8(tmp.b.h5);
	FputUint8(tmp.b.h6);
	FputUint8(tmp.b.h7);
}

uint16_t FILEIO::FgetUint16_BE()
{
	pair16_t tmp;
	tmp.b.h = FgetUint8();
	tmp.b.l = FgetUint8();
	return tmp.u16;
}

void FILEIO::FputUint16_BE(uint16_t val)
{
	pair16_t tmp;
	tmp.u16 = val;
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.l);
}

uint32_t FILEIO::FgetUint32_BE()
{
	pair32_t tmp;
	tmp.b.h3 = FgetUint8();
	tmp.b.h2 = FgetUint8();
	tmp.b.h  = FgetUint8();
	tmp.b.l  = FgetUint8();
	return tmp.u32;
}

void FILEIO::FputUint32_BE(uint32_t val)
{
	pair32_t tmp;
	tmp.u32 = val;
	FputUint8(tmp.b.h3);
	FputUint8(tmp.b.h2);
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.l);
}

uint64_t FILEIO::FgetUint64_BE()
{
	pair64_t tmp;
	tmp.b.h7 = FgetUint8();
	tmp.b.h6 = FgetUint8();
	tmp.b.h5 = FgetUint8();
	tmp.b.h4 = FgetUint8();
	tmp.b.h3 = FgetUint8();
	tmp.b.h2 = FgetUint8();
	tmp.b.h  = FgetUint8();
	tmp.b.l  = FgetUint8();
	return tmp.u64;
}

void FILEIO::FputUint64_BE(uint64_t val)
{
	pair64_t tmp;
	tmp.u64 = val;
	FputUint8(tmp.b.h7);
	FputUint8(tmp.b.h6);
	FputUint8(tmp.b.h5);
	FputUint8(tmp.b.h4);
	FputUint8(tmp.b.h3);
	FputUint8(tmp.b.h2);
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.l);
}

int16_t FILEIO::FgetInt16_BE()
{
	pair16_t tmp;
	tmp.b.h = FgetUint8();
	tmp.b.l = FgetUint8();
	return tmp.s16;
}

void FILEIO::FputInt16_BE(int16_t val)
{
	pair16_t tmp;
	tmp.s16 = val;
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.l);
}

int32_t FILEIO::FgetInt32_BE()
{
	pair32_t tmp;
	tmp.b.h3 = FgetUint8();
	tmp.b.h2 = FgetUint8();
	tmp.b.h  = FgetUint8();
	tmp.b.l  = FgetUint8();
	return tmp.s32;
}

void FILEIO::FputInt32_BE(int32_t val)
{
	pair32_t tmp;
	tmp.s32 = val;
	FputUint8(tmp.b.h3);
	FputUint8(tmp.b.h2);
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.l);
}

int64_t FILEIO::FgetInt64_BE()
{
	pair64_t tmp;
	tmp.b.h7 = FgetUint8();
	tmp.b.h6 = FgetUint8();
	tmp.b.h5 = FgetUint8();
	tmp.b.h4 = FgetUint8();
	tmp.b.h3 = FgetUint8();
	tmp.b.h2 = FgetUint8();
	tmp.b.h  = FgetUint8();
	tmp.b.l  = FgetUint8();
	return tmp.s64;
}

void FILEIO::FputInt64_BE(int64_t val)
{
	pair64_t tmp;
	tmp.s64 = val;
	FputUint8(tmp.b.h7);
	FputUint8(tmp.b.h6);
	FputUint8(tmp.b.h5);
	FputUint8(tmp.b.h4);
	FputUint8(tmp.b.h3);
	FputUint8(tmp.b.h2);
	FputUint8(tmp.b.h);
	FputUint8(tmp.b.l);
}

int FILEIO::Fgetc()
{
	return fgetc(fp);
}

int FILEIO::Fputc(int c)
{
	return fputc(c, fp);
}

char *FILEIO::Fgets(char *buffer, int max_size)
{
	return fgets(buffer, max_size, fp);
}

#ifdef _UNICODE
wchar_t *FILEIO::Fgets(wchar_t *buffer, int max_size)
{
	return fgetws(buffer, max_size, fp);
}
#endif

int FILEIO::Fputs(const char *buffer)
{
	return fputs(buffer, fp);
}

#ifdef _UNICODE
int FILEIO::Fputs(const wchar_t *buffer)
{
	return fputws(buffer, fp);
}
#endif

int FILEIO::Fsets(int c, int max_size)
{
	int len = 0;
	while(len < max_size && fputc(c, fp) != EOF) {
		len++;
	}
	return len;
}

int FILEIO::Fprintf(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int len = vfprintf(fp, format, ap);
	va_end(ap);

	return len;
}

#ifdef _UNICODE
int FILEIO::Fprintf(const wchar_t *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int len = UTILITY::vftprintf(fp, format, ap);
	va_end(ap);

	return len;
}
#endif

size_t FILEIO::Fread(void* buffer, size_t size, size_t count)
{
	return fread(buffer, size, count, fp);
}

size_t FILEIO::Fwrite(const void* buffer, size_t size, size_t count)
{
	return fwrite(buffer, size, count, fp);
}

uint32_t FILEIO::Fseek(long offset, int origin)
{
	switch(origin) {
	case FILEIO_SEEK_CUR:
		return fseek(fp, offset, SEEK_CUR);
	case FILEIO_SEEK_END:
		return fseek(fp, offset, SEEK_END);
	case FILEIO_SEEK_SET:
		return fseek(fp, offset, SEEK_SET);
	}
	return 0xFFFFFFFF;
}

uint32_t FILEIO::Ftell()
{
	return (uint32_t)ftell(fp);
}

int FILEIO::Flush()
{
	return (int)fflush(fp);
}

int FILEIO::Ftruncate(size_t size)
{
#if defined(_WIN32) && !defined(NO_USE_WINAPI)
	int fd = _fileno(fp);
	return _chsize(fd, (long)size);
//	SetEndOfFile(hFile);
#else
	int fd = fileno(fp);
	return ftruncate(fd, (off_t)size);
#endif
}
