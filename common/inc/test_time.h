/******************************************************************
 * @file  test_time.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年10月17日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年10月17日        bo.liu       create file

*****************************************************************/

#ifndef     TEST_TIME_H
#define     TEST_TIME_H
 
 #include <sys/time.h>

 


extern  unsigned long GetCycleTime(); 
extern  struct timeval  GetTimeUs( ); 

 
#endif
