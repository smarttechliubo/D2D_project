/*********************************************************
*	copyright: 
*	fileName: log.c
*	date: 2019.3.30
*	author: Liu Guicheng
*	mail: 
*********************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include "log.h"

#ifndef LOGLEVEL
#define LOGLEVEL WARN
#endif

// 使用了GNU C扩展语法，只在gcc（C语言）生效，
// g++的c++版本编译不通过
static const char* s_loginfo[] = {
    [ERROR] = "ERROR",
    [WARN]  = "WARN",
    [INFO]  = "INFO",
    [DEBUG] = "DEBUG"
};

static const char* s_comp[] = {
	[MAIN] = "MAIN",
	[DRIVER] = "DRIVER",
    [PHY] = "PHY",
    [MAC]  = "MAC",
    [RLC]  = "RLC",
    [RLC_TX]  = "RLC_TX",
    [RLC_RX]  = "RLC_RX",
    [PDCP] = "PDCP",
    [RRC]  = "RRC",
    [NAS] = "NAS",
    [IP] = "IP"
};


static void get_timestamp(char *buffer)
{
    time_t t;
    struct tm *p;
    struct timeval tv;
    int len;
    int millsec;

    t = time(NULL);
    p = localtime(&t);

    gettimeofday(&tv, NULL);
   // millsec = (int)(tv.tv_usec / 1000);
    millsec = (int)(tv.tv_usec);
    /* 时间格式：[2011-11-15 12:47:34:888] */
    len = snprintf(buffer, 36, "[%04d-%02d-%02d %02d:%02d:%02d:%06d(us)] ",
        p->tm_year+1900, p->tm_mon+1,
        p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, millsec);

    buffer[len] = '\0';
}

void log_info(const char* filename, int line, comp_name_t comp, LogLevel level, const char* fmt, ...)
{

	int syslog_level =0; 
#ifndef USE_SYSLOG 
    if(level > LOGLEVEL)
        return;
#else 
	switch(level)
	{
		case ERROR:
			syslog_level = LOG_ERR; 
			break; 
		case WARN: 
			syslog_level = LOG_WARNING ; 
			break; 
		case INFO:
			syslog_level = LOG_NOTICE; 
			break; 
		case DEBUG:
			syslog_level = LOG_NOTICE; 
			break; 
        default: 
        	syslog_level = LOG_ERR; 
			break; 
	}

#endif 



    va_list arg_list;
    char buf[1024];
    memset(buf, 0, 1024);
    va_start(arg_list, fmt);
    vsnprintf(buf, 1024, fmt, arg_list);
    char time[36] = {0};


    // 去掉*可能*存在的目录路径，只保留文件名
    const char* tmp = strrchr(filename, '/');
    if (!tmp) tmp = filename;
    else tmp++;

#ifndef USE_SYSLOG

    get_timestamp(time);


    //printf("%s, [%s], [%s:%d] %s\n", time, s_comp[comp], tmp, line, buf);

    #ifdef  LOG_PRINTF_ALL 
    printf("%s [%s] [%s]  [%s:%d]  %s\n", time, s_comp[comp], s_loginfo[level], tmp, line, buf);
	#else 
	printf( "%s: [%s][%s] %s\n" , time, s_loginfo[level],s_comp[comp], buf);
	#endif 
#else 
	syslog(syslog_level," [%s] [%s]  [%s,%d] %s\n",  s_comp[comp], s_loginfo[level], tmp,line,buf);
#endif 
		
    va_end(arg_list);

}





