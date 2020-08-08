/******************************************************************
 * @file  test_time.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年10月17日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年10月17日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h> 
#include <sys/time.h>

#if 0 
unsigned long rdtsc(void)
 {
	 unsigned int low,high;
	 asm volatile("rdtsc":"=a"(low),"=d"(high));
	 return low|(unsigned long )high<<32;
 }


unsigned long  GetCpuCycle( )
{
	unsigned long  gettime;

	gettime = rdtsc();
	return gettime;
}
#endif 


struct timeval  GetTimeUs( )
{
	struct timeval  gettime;

    gettimeofday(&gettime,NULL);
	return gettime;
}



 
 
 
/**************************function******************************/
