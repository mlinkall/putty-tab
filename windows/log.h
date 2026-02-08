/*
 * tab.h - the PuTTY(tel)/pterm main program, which runs a PuTTY
 */

#ifndef LOG_H
#define LOG_H

#include <time.h>
#include <stdarg.h>
#include "putty.h"

#define PUTTY_LOG_FILE_NAME "DBG.txt"

#define PUTTY_MAX_BUFFER 1024 * 1024

#define PUTTY_LOG_FTL 1
#define PUTTY_LOG_ERR 2
#define PUTTY_LOG_WARNING 3
#define PUTTY_LOG_NOTICE 4
#define PUTTY_LOG_INFO 5
#define PUTTY_LOG_DEBUG 6

static int anaDefaultLevel = PUTTY_LOG_DEBUG;

///< function
static inline
void PUTTY_LOG_INIT()
{
	FILE* fp = fopen(PUTTY_LOG_FILE_NAME, "a");
	if(fp != NULL)
	{
		fclose(fp);
	}
}

static inline
char * PUTTY_TIME(char buffer[30])
{
	//memset(buffer, 0, 30);
	time_t t2 = time(0);
	struct tm* t = localtime(&t2);
	snprintf(buffer, 30, "%04d-%02d-%02d %02d:%02d:%02d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	buffer[29] = 0;
	return buffer;
}

static inline
void PUTTY_TRACE (char* szFile, int nLine,const char * fmt, ...)
{
	char current[30] = {0,};

	PUTTY_TIME(current);

	char buffer[MAX_PATH];
	va_list args;

	memset(buffer, 0, sizeof(buffer));
	va_start(args, fmt);
#ifdef WIN32
	_vsnprintf(buffer, sizeof(buffer), fmt, args);
#else
	vsnprintf(buffer, sizeof(buffer), fmt, args);
#endif
	va_end(args);

	TRACE("%s_%s:%d_%s\n",
		PUTTY_TIME(current), szFile, nLine, buffer);
}

static inline
void putty_log(int nLevel, char* szFile, int nLine, const char *fmt, ...)
{
	char buffer[MAX_PATH];
	va_list args;

	if (nLevel > anaDefaultLevel)
	{
		return;
	}
	memset(buffer, 0, sizeof(buffer));
	va_start(args, fmt);
#ifdef WIN32
	_vsnprintf(buffer, sizeof(buffer), fmt, args);
#else
	vsnprintf(buffer, sizeof(buffer), fmt, args);
#endif
	va_end(args);

#if _MSC_VER != 1200 //VC6.0
	FILE* fp = fopen(PUTTY_LOG_FILE_NAME, "a");
	if(fp != NULL)
	{
#endif
		time_t t2 = time(0);
		struct tm* t = localtime(&t2);
#if _MSC_VER != 1200 //VC6.0
		fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d %d %s:%d %s\n",
				t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,
				nLevel, szFile, nLine, buffer);
#else
		printf("%s\n", buffer);
#endif
#if _MSC_VER != 1200 //VC6.0
		fclose(fp);
	}
#endif
}

#define PUTTY_LOG(level,fmt,...) \
	putty_log(level, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif // LOG_H
