/**********************************************************
* @file mytask.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/06
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MYTASK_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MYTASK_H_

#include <pthread.h>

#include "typedef.h"

typedef enum
{
	PHY_TASK,
	MAC_TASK,
	MAC_PRE_TASK,
	MAC_MAIN_TASK,
	RLC_TASK,
	RRC_TASK,
	MAX_TASK
}task_id_t;

typedef struct task_list_s 
{
	task_id_t taskId;
	pthread_t thread;
	pthread_mutex_t mutex;
}task_info;

bool create_new_thread(task_id_t taskId, void *(*start_routine) (void*), void *arg);
void wait_period(uint32_t timer_fd);
bool make_timer(uint32_t period_us, uint32_t* timer_fd, bool periodic);
bool stop_timer(uint32_t timer_fd);
bool restart_timer(uint32_t period_us, uint32_t timer_fd);


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MYTASK_H_ */
