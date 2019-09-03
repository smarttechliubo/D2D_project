/**********************************************************
* @file mac_test.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "typedef.h"

bool g_run_enable = false;

extern void run_period();

void init_mac_task()
{
	g_run_enable = true;
}

uint32_t make_periodic(uint32_t period_us, uint32_t* timer_fd)
{
    int32_t ret = -1; 
    uint32_t ns;
    uint32_t sec;
    struct itimerspec itval;

    /* Create the timer */
    ret = timerfd_create (CLOCK_MONOTONIC, 0);
    *timer_fd = ret;
    if (ret > 0) {
      /* Make the timer periodic */
      sec = period_us/1e6;
      ns = (period_us - (sec * 1000000)) * 1000;
      itval.it_interval.tv_sec = sec;
      itval.it_interval.tv_nsec = ns;
      itval.it_value.tv_sec = sec;
      itval.it_value.tv_nsec = ns;
      ret = timerfd_settime (*timer_fd, 0, &itval, NULL); 
      if (ret < 0) {
        perror("timerfd_settime");
      }
    } else {
      perror("timerfd_create");
    }
    return ret;
}


void wait_period(uint32_t timer_fd) 
{
    unsigned long long missed;
    int ret;

    /* Wait for the next timer event. If we have missed any the
        number is written to "missed" */
    ret = read (timer_fd, &missed, sizeof (missed));
    if (ret == -1)
    {
      perror ("read timer");
      return;
    }

    /* "missed" should always be >= 1, but just to be sure, check it is not 0 anyway */
    if (missed > 0) {
      //wakeups_missed += (missed - 1);
    }
}

void *run_thread() 
{
    uint32_t timer_fd = 0;
    uint32_t period_us = 1000;
    //uint32_t wakeups_missed = 0;
    if (make_periodic(period_us, &timer_fd))
      return NULL;
    
    while(g_run_enable) {
      run_period();
      if (g_run_enable) {
        wait_period(timer_fd);
      }
    }
	return 0;
}
#if 0
int main()
{
	init_mac_task();
}
#endif
