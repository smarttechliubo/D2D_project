<<<<<<< HEAD
/*********************************************************
*	copyright: 
*	fileName: main.c
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
#include "osp_ex.h"


#ifndef LOG_H_
#define LOG_H_


#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"



typedef enum
{
    ERROR = ERR_DEBUG_LEVEL,
    WARN  = WARNING_DEBUG_LEVEL,
    INFO  = RUN_DEBUG_LEVEL,
    DEBUG = DBG_DEBUG_LEVEL
} LogLevel;

typedef enum 
{
	NOT_USING
} comp_name_t;

#define MAIN "[MAIN]"
#define DRIVER "[DRIVER]"
#define PHY "[PHY]"
#define MAC "[MAC]"
#define RLC "[RLC]"
#define RLC_TX "[RLC_TX]"
#define RLC_RX "[RLC_RX]"
#define PDCP "[PDCP]"
#define RRC "[RRC]"
#define NAS "[NAS]"
#define DUMMY "[DUMMY]"
#define IP "[IP]"


void log_info(const char* filename, int line, comp_name_t comp, LogLevel level, const char* fmt, ...) __attribute__((format(printf,5,6)));

#if 0
#define LOG_ERROR(comp, format, ...) log_info(__FILE__, __LINE__, comp, ERROR, format, ## __VA_ARGS__)
#define LOG_WARN(comp, format, ...)  log_info(__FILE__, __LINE__, comp, WARN, format, ## __VA_ARGS__)
#define LOG_INFO(comp, format, ...)  log_info(__FILE__, __LINE__, comp, INFO, format, ## __VA_ARGS__)
#define LOG_DEBUG(comp, format, ...) log_info(__FILE__, __LINE__, comp, DEBUG, format, ## __VA_ARGS__)
#else
#define LOG_ERROR(comp, format, ...) DebugOutWithTime(ERROR,  "[ERROR]" comp  ": [%s:%d] "format"\n", __FILE__, __LINE__,  ## __VA_ARGS__)
#define LOG_WARN(comp, format, ...)  DebugOutWithTime(WARN,  "[WARN]" comp  ": [%s:%d] "format"\n", __FILE__, __LINE__,  ## __VA_ARGS__)
#define LOG_INFO(comp, format, ...)  DebugOutWithTime(INFO,  "[INFO]" comp  ": [%s:%d] "format"\n", __FILE__, __LINE__,  ## __VA_ARGS__)
#define LOG_DEBUG(comp, format, ...) DebugOutWithTime(DEBUG,  "[DEBUG]" comp  ": [%s:%d] "format"\n", __FILE__, __LINE__,  ## __VA_ARGS__)
#endif

#define _Assert_Exit_  {                       \
	exit(1);                                     \
	}



#define  _Assert_(cOND,comp, aCTION, fORMAT, ...)   \
if (!(cOND)) \
{ \
			LOG_ERROR(comp,fORMAT,##__VA_ARGS__);                \
			aCTION                         \
}   \


#define AssertFatal(cOND, comp,fORMAT, ...)          _Assert_(cOND,comp, _Assert_Exit_, fORMAT, ##__VA_ARGS__)


#endif

=======
/*********************************************************
*	copyright: 
*	fileName: main.c
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
#include "osp_ex.h"
#include <unistd.h>


#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)


#ifndef LOG_H_
#define LOG_H_


#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"



typedef enum
{
	CMD = CMD_DEBUG_LEVEL,
    ERROR = ERR_DEBUG_LEVEL,
    WARN  = WARNING_DEBUG_LEVEL,
    INFO  = RUN_DEBUG_LEVEL,
    DEBUG = DBG_DEBUG_LEVEL
} LogLevel;

typedef enum 
{
	NOT_USING
} comp_name_t;

#define MAIN "[MAIN]"
#define DRIVER "[DRIVER]"
#define PHY "[PHY]"
#define MAC "[MAC]"
#define RLC "[RLC]"
#define RLC_TX "[RLC_TX]"
#define RLC_RX "[RLC_RX]"

#define PDCP "[PDCP]"
#define RRC "[RRC]"
#define NAS "[NAS]"
#define DUMMY "[DUMMY]"
#define IP_UDP "[IP_UDP]"
#define IP_MSG "[IP_MSG]"





void log_info(const char* filename, int line, comp_name_t comp, LogLevel level, const char* fmt, ...) __attribute__((format(printf,5,6)));

#if 0
#define LOG_ERROR(comp, format, ...) log_info(__FILE__, __LINE__, comp, ERROR, format, ## __VA_ARGS__)
#define LOG_WARN(comp, format, ...)  log_info(__FILE__, __LINE__, comp, WARN, format, ## __VA_ARGS__)
#define LOG_INFO(comp, format, ...)  log_info(__FILE__, __LINE__, comp, INFO, format, ## __VA_ARGS__)
#define LOG_DEBUG(comp, format, ...) log_info(__FILE__, __LINE__, comp, DEBUG, format, ## __VA_ARGS__)
#else
#define LOG_CMD(comp, format, ...)   DebugOutWithTime(CMD,  "[CMD]" comp  ": [%s:%d] "format"\n", __FILENAME__, __LINE__,  ## __VA_ARGS__)
#define LOG_ERROR(comp, format, ...) DebugOutWithTime(ERROR,  "[ERROR]" comp  ": [%s:%d] "format"\n", __FILENAME__, __LINE__,  ## __VA_ARGS__)
#define LOG_WARN(comp, format, ...)  DebugOutWithTime(WARN,  "[WARN]" comp  ": [%s:%d] "format"\n", __FILENAME__, __LINE__,  ## __VA_ARGS__)
#define LOG_INFO(comp, format, ...)  DebugOutWithTime(INFO,  "[INFO]" comp  ": [%s:%d] "format"\n", __FILENAME__, __LINE__,  ## __VA_ARGS__)
#define LOG_DEBUG(comp, format, ...) DebugOutWithTime(DEBUG,  "[DEBUG]" comp  ": [%s:%d] "format"\n", __FILENAME__, __LINE__,  ## __VA_ARGS__)
#endif

#define _Assert_Exit_  {                       \
	fflush(stdout);                            \
	sleep(10);                                   \
	exit(1);                                   \
	}



#define  _Assert_(cOND,comp, aCTION, fORMAT, ...)   \
if (!(cOND)) \
{ \
			LOG_ERROR(comp,fORMAT,##__VA_ARGS__);                \
			aCTION                         \
}   \


#define AssertFatal(cOND, comp,fORMAT, ...)          _Assert_(cOND,comp, _Assert_Exit_, fORMAT, ##__VA_ARGS__)


#endif

>>>>>>> master
