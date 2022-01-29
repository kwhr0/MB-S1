/** @file fileio.h

	Skelton for retropc emulator

	@author Takeda.Toshiya
	@date   2006.08.18 -

	@brief [ file i/o ]
*/

#ifndef FILEIO_H
#define FILEIO_H

#include "common.h"
#include <stdio.h>

#define FILEIO_READ_BINARY		1
#define FILEIO_WRITE_BINARY		2
#define FILEIO_READ_WRITE_BINARY	3
#define FILEIO_READ_WRITE_NEW_BINARY	4
#define FILEIO_READ_ASCII		5
#define FILEIO_WRITE_ASCII		6
#define FILEIO_READ_WRITE_ASCII		7
#define FILEIO_READ_WRITE_NEW_ASCII	8
#define FILEIO_SEEK_SET			0
#define FILEIO_SEEK_CUR			1
#define FILEIO_SEEK_END			2

/**
	@brief file i/o
*/
class FILEIO
{
private:
	FILE* fp;

	static int GetAttribute(const _TCHAR *filename, int *flags);

public:
	FILEIO();
	~FILEIO();

	static bool IsFileExists(const _TCHAR *filename);
	static bool IsFileProtected(const _TCHAR *filename);
	static bool IsDirExists(const _TCHAR *dirname);
	static void RemoveFile(const _TCHAR *filename);

	bool Fopen(const _TCHAR *filename, int mode);
	void Fclose();
	bool IsOpened() { return (fp != NULL); }
	uint32_t FileLength();

	bool FgetBool();
	void FputBool(bool val);
	uint8_t FgetUint8();
	void FputUint8(uint8_t val);
#if 0
	uint16_t FgetUint16();
	void FputUint16(uint16_t val);
	uint32_t FgetUint32();
	void FputUint32(uint32_t val);
	uint64_t FgetUint64();
	void FputUint64(uint64_t val);
#endif
	int8_t FgetInt8();
	void FputInt8(int8_t val);
#if 0
	int16_t FgetInt16();
	void FputInt16(int16_t val);
	int32_t FgetInt32();
	void FputInt32(int32_t val);
	int64_t FgetInt64();
	void FputInt64(int64_t val);
#endif
	float FgetFloat();
	void FputFloat(float val);
	double FgetDouble();
	void FputDouble(double val);

	uint16_t FgetUint16_LE();
	void FputUint16_LE(uint16_t val);
	uint32_t FgetUint32_LE();
	void FputUint32_LE(uint32_t val);
	uint64_t FgetUint64_LE();
	void FputUint64_LE(uint64_t val);
	int16_t FgetInt16_LE();
	void FputInt16_LE(int16_t val);
	int32_t FgetInt32_LE();
	void FputInt32_LE(int32_t val);
	int64_t FgetInt64_LE();
	void FputInt64_LE(int64_t val);

	uint16_t FgetUint16_BE();
	void FputUint16_BE(uint16_t val);
	uint32_t FgetUint32_BE();
	void FputUint32_BE(uint32_t val);
	uint64_t FgetUint64_BE();
	void FputUint64_BE(uint64_t val);
	int16_t FgetInt16_BE();
	void FputInt16_BE(int16_t val);
	int32_t FgetInt32_BE();
	void FputInt32_BE(int32_t val);
	int64_t FgetInt64_BE();
	void FputInt64_BE(int64_t val);

	int Fgetc();
	int Fputc(int c);
	char    *Fgets(char *buffer, int max_size);
#ifdef _UNICODE
	wchar_t *Fgets(wchar_t *buffer, int max_size);
#endif
	int Fputs(const char *buffer);
#ifdef _UNICODE
	int Fputs(const wchar_t *buffer);
#endif
	int Fsets(int c, int max_size);
	int Fprintf(const char *format, ...);
#ifdef _UNICODE
	int Fprintf(const wchar_t *format, ...);
#endif
	size_t Fread(void* buffer, size_t size, size_t count);
	size_t Fwrite(const void* buffer, size_t size, size_t count);
	uint32_t Fseek(long offset, int origin);
	uint32_t Ftell();
	int Flush();
	int Ftruncate(size_t size);
	FILE *GetFile() { return fp; }
	void SetFile(FILE *fp_) { fp = fp_; }
};

#endif /* FILEIO_H */
