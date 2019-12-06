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
#include "tasks_def.h"

#define PERIODIC_4MS 4
#define PERIODIC_1MS 1

typedef enum
{
	ETASK_A,
	ETASK_B,
	ETASK_MAX
}task_id_sim;

typedef struct task_list_s 
{
	task_id_sim taskId;
	pthread_t thread;
	//pthread_mutex_t mutex;
	pthread_cond_t cond_4ms;
	pthread_mutex_t mutex_4ms;
	pthread_cond_t cond_1ms;
	pthread_mutex_t mutex_1ms;
}task_info;

task_id_sim get_task_id_sim(const task_id taskId);
void init_thread(task_id taskId);
bool thread_wait(task_id taskId, uint32_t periodic);
bool thread_wakeup(task_id taskId, uint32_t periodic);
bool create_new_thread(task_id taskId, void *(*start_routine) (void*), void *arg);
void wait_period(uint32_t timer_fd);
bool make_timer(uint32_t period_us, uint32_t* timer_fd, bool periodic);
bool stop_timer(uint32_t timer_fd);
bool close_timer(uint32_t timer_fd);
bool restart_timer(uint32_t period_us, uint32_t timer_fd);


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MYTASK_H_ */
