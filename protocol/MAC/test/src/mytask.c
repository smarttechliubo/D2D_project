/**********************************************************
* @file mytask.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/06
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

#include "mytask.h"
#include "log.h"


task_info mytask[MAX_TASK];

bool create_new_thread(task_id_t taskId, void *(*start_routine) (void*), void *arg)
{
	int policy;
	
    if ((taskId >= MAX_TASK) || (pthread_create(&mytask[taskId].thread, NULL, start_routine, arg) != 0))
    {
		LOG_ERROR(MAC, "[TEST]: create task error, taskId:%u",taskId);
		return false;
	}

    //pthread_setname_np(*thread, taskName);
	struct sched_param sparam;
	memset(&sparam, 0, sizeof(sparam));
	sparam.sched_priority = sched_get_priority_max(SCHED_FIFO)-10;
	policy = SCHED_FIFO ; 
	if (pthread_setschedparam(mytask[taskId].thread, policy, &sparam) != 0) 
	{
		//
	}
	LOG_INFO(MAC, "[TEST]: create task success");
	return true;
}



bool make_timer(uint32_t period_us, uint32_t* timer_fd, bool periodic)
{
    int32_t ret = -1; 
    uint32_t ns;
    uint32_t sec;
    struct itimerspec itval;

    /* Create the timer */
    ret = timerfd_create (CLOCK_MONOTONIC, 0);
    *timer_fd = ret;
    if (ret > 0) 
	{
        /* Make the timer periodic */
        sec = period_us/1e6;
        ns = (period_us - (sec * 1000000)) * 1000;
        itval.it_interval.tv_sec = periodic ? sec : 0;
        itval.it_interval.tv_nsec = periodic ? ns : 0;
        itval.it_value.tv_sec = sec;
        itval.it_value.tv_nsec = ns;
        ret = timerfd_settime (*timer_fd, 0, &itval, NULL); 

		if (ret < 0) 
		{
          perror("timerfd_settime");
		  return false;
        }
	}
	else 
	{
		perror("timerfd_create");
		return false;
    }
    return true;
}

bool stop_timer(uint32_t timer_fd)
{
    int32_t ret = -1; 
    struct itimerspec itval;

    /* Make the timer periodic */
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = 0;
    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = 0;
    ret = timerfd_settime (timer_fd, 0, &itval, NULL); 

	if (ret < 0) 
	{
      perror("timerfd_settime");
	  return false;
    }
    return true;
}

bool restart_timer(uint32_t period_us, uint32_t timer_fd)
{
	int32_t ret = -1; 
	uint32_t ns;
	uint32_t sec;
	struct itimerspec itval;

	/* Create the timer */
	/* Make the timer periodic */
	sec = period_us/1e6;
	ns = (period_us - (sec * 1000000)) * 1000;
	itval.it_interval.tv_sec = sec;
	itval.it_interval.tv_nsec = ns;
	itval.it_value.tv_sec = sec;
	itval.it_value.tv_nsec = ns;
	ret = timerfd_settime (timer_fd, 0, &itval, NULL); 

	if (ret < 0) 
	{
	  perror("timerfd_settime");
	  return false;
	}
	return true;
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

