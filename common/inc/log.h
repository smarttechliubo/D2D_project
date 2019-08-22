/*********************************************************
*	copyright: 
*	fileName: main.c
*	date: 2019.3.30
*	author: Liu Guicheng
*	mail: 
*********************************************************/

#ifndef LOG_H_
#define LOG_H_

typedef enum
{
    ERROR = 1,
    WARN  = 2,
    INFO  = 3,
    DEBUG = 4
} LogLevel;

typedef enum 
{
	MAIN,
    PHY,
    MAC,
    RLC,
    PDCP,
    RRC,
    NAS
} comp_name_t;

void log_info(const char* filename, int line, comp_name_t comp, LogLevel level, const char* fmt, ...) __attribute__((format(printf,5,6)));

#define LOG_ERROR(comp, format, ...) log_info(__FILE__, __LINE__, comp, ERROR, format, ## __VA_ARGS__)
#define LOG_WARN(comp, format, ...)  log_info(__FILE__, __LINE__, comp, WARN, format, ## __VA_ARGS__)
#define LOG_INFO(comp, format, ...)  log_info(__FILE__, __LINE__, comp, INFO, format, ## __VA_ARGS__)
#define LOG_DEBUG(comp, format, ...) log_info(__FILE__, __LINE__, comp, DEBUG, format, ## __VA_ARGS__)

#endif

