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

#define _Assert_Exit_                          
{                                              
    fprintf(stderr, "\nExiting execution\n");       
    exit(1);
}


#define _Assert_(cOND,comp, aCTION, fORMAT, aRGS...)             
do {                                                       
    if (!(cOND)) {        
    	LOG_ERROR(comp,fORMAT,aRGS...)
#if 0
        fprintf(stderr, "\nAssertion ("#cOND") failed!\n"   
                "In %s() %s:%d\n" fORMAT,                   
                __FUNCTION__, __FILE__, __LINE__, ##aRGS);  
#endif 
        aCTION;                                            
    }						
} while(0)

#define AssertFatal(cOND, comp,fORMAT, aRGS...)          _Assert_(cOND,comp, _Assert_Exit_, fORMAT, ##aRGS)


#endif

