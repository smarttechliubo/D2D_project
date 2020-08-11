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


task_info mytask[ETASK_MAX];

task_id_sim get_task_id_sim(const task_id taskId)
{
	task_id_sim mqid = ETASK_MAX;
/*
	switch (taskId)
	{
		case TASK_D2D_RRC:
		{
			mqid = ERRC_TASK_SIM;
			break;
		}
		case TASK_D2D_RLC:
		{
			mqid = ERLC_TASK_SIM;
			break;
		}
		case TASK_D2D_MAC:
		{
			mqid = EMAC_TASK_SIM;
			break;
		}
		case TASK_D2D_MAC_SCH:
		{
			mqid = EMAC_SCH_TASK_SIM;
			break;
		}
		case TASK_D2D_PHY_TX:
		{
			mqid = EPHY_TX_TASK_SIM;
			break;
		}
		case TASK_D2D_PHY_RX:
		{
			mqid = EPHY_RX_TASK_SIM;
			break;
		}
		default:
			break;
	}
*/
	return mqid;
}

void init_thread(task_id taskId)
{
	task_id_sim id = get_task_id_sim(taskId);

	if (id >= ETASK_MAX)
	{
		LOG_ERROR(MAC, "init_thread, Wrong taskId:%u",taskId);
		return;
	}

	task_info* task = &mytask[id];

	pthread_mutex_init(&task->mutex_4ms,NULL);
	pthread_cond_init(&task->cond_4ms,NULL);

	pthread_mutex_init(&task->mutex_1ms,NULL);
	pthread_cond_init(&task->cond_1ms,NULL);
}

bool thread_wakeup(task_id taskId, uint32_t periodic)
{	
	task_id_sim id = get_task_id_sim(taskId);

	if (id >= ETASK_MAX)
	{
		LOG_ERROR(MAC, "thread_wakeup, Wrong taskId:%u",taskId);
		return false;
	}

	task_info* task = &mytask[id];

	if (periodic == PERIODIC_4MS)// 4ms periodic timer
	{
		pthread_mutex_lock(&task->mutex_4ms);
		pthread_cond_broadcast(&task->cond_4ms);
		pthread_mutex_unlock(&task->mutex_4ms);
	}
	else
	{
		pthread_mutex_lock(&task->mutex_1ms);
		pthread_cond_broadcast(&task->cond_1ms);
		pthread_mutex_unlock(&task->mutex_1ms);
	}

	return true;
}

bool thread_wait(task_id taskId, uint32_t periodic)
{
	task_id_sim id = get_task_id_sim(taskId);

	if (id >= ETASK_MAX)
	{
		LOG_ERROR(MAC, "thread_wait, Wrong taskId:%u",taskId);
		return false;
	}

	task_info* task = &mytask[id];

	if (periodic == PERIODIC_4MS)// 4ms periodic timer
	{
		pthread_mutex_lock(&task->mutex_4ms);
		pthread_cond_wait(&task->cond_4ms, &task->mutex_4ms);
		pthread_mutex_unlock(&task->mutex_4ms);
	}
	else
	{
		pthread_mutex_lock(&task->mutex_1ms);
		pthread_cond_wait(&task->cond_1ms, &task->mutex_1ms);
		pthread_mutex_unlock(&task->mutex_1ms);
	}

	return true;
}

bool create_new_thread(task_id taskId, void *(*start_routine) (void*), void *arg)
{
	task_id_sim id = get_task_id_sim(taskId);

	if (id >= ETASK_MAX)
	{
		LOG_ERROR(MAC, "create_new_thread, Wrong taskId:%u",taskId);
		return false;
	}

	int policy;
	
    if ((id >= ETASK_MAX) || (pthread_create(&mytask[id].thread, NULL, start_routine, arg) != 0))
    {
		LOG_ERROR(MAC, "[TEST]: create task error, taskId:%u",taskId);
		return false;
	}

    //pthread_setname_np(*thread, taskName);
	struct sched_param sparam;
	memset(&sparam, 0, sizeof(sparam));
	sparam.sched_priority = sched_get_priority_max(SCHED_FIFO)-10;
	policy = SCHED_FIFO ; 

	if (pthread_setschedparam(mytask[id].thread, policy, &sparam) != 0) 
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

bool close_timer(uint32_t timer_fd)
{
	close(timer_fd);

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
