
#ifndef _DB_UTIL_H_
#define _DB_UTIL_H_
#include <common.h>

#define DB_TEXT_FORMAT_UTF8  0

extern void tg_exit(int status);

extern UINT32 db_get_ticks();

#define DB_DEBUG    1


#if  DB_DEBUG    
#ifdef WIN32
extern void db_log(const char *format, ...);
#define DB_PRINTF(format, ...) db_log ( format, ##__VA_ARGS__)
#else
#define DB_PRINTF(format, ...) printf ( format, ##__VA_ARGS__)
#endif
#else

#define DB_PRINTF(format, ...) 
#endif


#define DB_ASSERT(condition)			do {	\
		if(!(condition))\
			tg_exit(0);\
		}while(0)

#endif




