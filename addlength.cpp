/*
==========================================================================
addlength.cpp : CSV format converter

Use to load CSV with newline character in the string field.
It is can be converted such data into a row that is inserted LENGTH_DIGIT digits
row-length fields at the each row-head and add comma at the each row-tail.
In order to make it readable using SQL*Loader.

Usage:
addlength <src_file> <dst_file>

How to build:
nmake EXE="addlength.exe" OBJS="addlength.obj" CPPFLAGS="/nologo /EHsc /Zi /O2"
--------------------------------------------------------------------------
Copyright (c) 2012 PLUMSIX Co.,Ltd.

Permission is hereby granted, free of charge, to any person obtaining 
a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
==========================================================================
*/

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "windows.h"

#define LENGTH_DIGIT    10  /* same as 'n' in the "VAR n" parameter option.  */
#define ENCLOSURE       '"'

#define BOM             "\xEF\xBB\xBF"
#define LEN_BOM         (sizeof(BOM) -1)
#define LEN_BUF         1024

#if 0
#define FGETC fgetc
#define FWRITE fwrite 
#else
#define FGETC _fgetc_nolock
#define FWRITE _fwrite_nolock
#endif

// c: testing conditionÅAm: messageÅAr: return value
#define CHECK_ERROR(c,m,r) \
if (c) \
{ \
	printf("%s - %s rc=%d, rows=%l64d, clms=%l64d\n", r == 0 ? "Usage" : "Error", m, r, rows, clms); \
	rtn = r; \
	goto err; \
} \

inline void vPubCharToBuffer(unsigned char *dst, const int& ch, long& cols)
{
	*dst = ch;
	cols++;
}

static int WriteBuffer(long& cols, unsigned char *dst, FILE *fp)
{
	char lb[LENGTH_DIGIT + 1];
	if (dst[cols - 3] != ',' && dst[cols - 2] == '\r')
	{
		dst[cols - 2] = ',';
		dst[cols - 1] = '\r';
		dst[cols - 0] = '\n';
		cols++;
	}
	sprintf(lb, "%0*ld", LENGTH_DIGIT, cols - LENGTH_DIGIT);
	memcpy(dst, lb, LENGTH_DIGIT);
	return FWRITE(dst, cols, 1, fp);
}

int main(int argc, char* argv[])
{
	int rtn;
	FILE *fin = 0;
	FILE *fout = 0;
	void *bom_probe = 0;
	unsigned char *data_buf = 0;
	size_t rc;
	long cols;
	int ch;
	__int64 rows = 0, clms = 1;
	__int64 bytes = 0;
	LARGE_INTEGER liFreq, liTick0, liTick1;

	CHECK_ERROR(argc < 3, "addlength <src_file> <dst_file>.", 0);

	bom_probe = new unsigned char [LEN_BOM];
	CHECK_ERROR(!bom_probe, "Faiild to allocate memory.", 1);

	data_buf = new unsigned char [LEN_BUF];
	CHECK_ERROR(!data_buf, "Faiild to allocate memory.", 1);

	fin = fopen(argv[1], "rb");
	CHECK_ERROR(fin == NULL, "Faiild to open the file.", 2);

	rc = fread(bom_probe, LEN_BOM, 1, fin);
	CHECK_ERROR(rc < 1, "Faiild to read the file.", 3);

	if (memcmp(BOM, bom_probe, LEN_BOM) != 0)
	{
		fseek(fin, 0, SEEK_SET);
		fout = fopen(argv[2], "wb");
		CHECK_ERROR(fout == NULL, "Faiild to open the file.", 9);
	}
	else 
	{
		fout = fopen(argv[2], "wb");
		CHECK_ERROR(fout == NULL, "Faiild to open the file.", 10);
		rc = fwrite(BOM, LEN_BOM, 1, fout);
		CHECK_ERROR(rc < 1, "Faiild to write the file.\n", 4);
	}

	::QueryPerformanceFrequency(&liFreq);
	::QueryPerformanceCounter(&liTick0);

	cols = LENGTH_DIGIT;
	while ((ch = FGETC(fin)) != EOF)
	{
		vPubCharToBuffer(&data_buf[cols], ch, cols);
		CHECK_ERROR(LEN_BUF <= cols, "Line buffer was over flowed.", 5);
		if (ch == ENCLOSURE) // Start of a string.
		{
			while ((ch = FGETC(fin)) != ENCLOSURE)
			{
				CHECK_ERROR(ch == EOF, "Encountered EOF before closing quote.", 7);
				vPubCharToBuffer(&data_buf[cols], ch, cols);
				CHECK_ERROR(LEN_BUF <= cols, "Line buffer was over flowed. ", 8);
			}
			vPubCharToBuffer(&data_buf[cols], ch, cols);
			CHECK_ERROR(LEN_BUF <= cols, "Line buffer was over flowed. ", 11);
		}
		else if (ch == '\n')
		{
			rc = WriteBuffer(cols, data_buf, fout);
			CHECK_ERROR(rc < 1, "Faiild to write the file.\n", 6);
			bytes += cols;
			cols = LENGTH_DIGIT;
			rows++;
			clms = 1;
		}
		else if (ch == ',')
		{
			clms++;
		}
	}
	// Write the data that is remained between last LF and EOF.
	if (cols > LENGTH_DIGIT)
	{
		rc = WriteBuffer(cols, data_buf, fout);
		CHECK_ERROR(rc < 1, "Faiild to write the file.\n", 12);
		bytes += cols;
	}

	::QueryPerformanceCounter(&liTick1);

	double fElapsed = (((liTick1.QuadPart - liTick0.QuadPart) * 1000) / liFreq.QuadPart) / 1000.0;
	printf("Infomation - Completed successfully. Wrote %I64d records and %I64d bytes in %.2f seconds.\n", rows, bytes, fElapsed);
	rtn = 0;

err:
	if (data_buf) delete [] data_buf;
	if (bom_probe) delete [] bom_probe;
	if (fin) fclose(fin);
	if (fout) fclose(fout);
	return rtn;
}
