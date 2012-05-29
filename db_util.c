
#include <common.h>
#include <printbuf.h>
#include "stdarg.h"
#ifdef WIN32
static CHAR s_db_printf_buf[1024];
void db_log(const char *format, ...)
{
    extern void HostPrintfSetColor(UINT8 color_layout_index);
    extern void HostPrintfClearColor();
    
    INT32 len=0;
    va_list     args;
    va_start( args, format );
	memset(s_db_printf_buf,0,1024);
    len=vsprintf(s_db_printf_buf,format,args);
    va_end(args);
    HostPrintfSetColor(3);
    output_2_console(s_db_printf_buf,strlen(s_db_printf_buf)+1);
    HostPrintfClearColor();
  
}
#endif


UINT32 db_get_ticks()
{
#ifndef WIN32
    	return RTT_stm_get_time_1us_resolution();
#else
    	return  UtaOsGetTicks();
#endif
}