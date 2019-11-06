/**********************************************************
* @file mac_test.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/


#include "typedef.h"

//#include "threads.h"

#include "mac_defs.h"
#include "mac.h"
#include "log.h"
#include "mytask.h"
#include "msg_handler.h"

bool g_run_enable = false;
uint32_t run_time = 0;

extern void init_rrc_sim();
extern void *rrc_thread();
extern void *rlc_thread();

uint32_t syncT()
{
	return run_time;
}

void *mac_thread()
{
    uint32_t timer_fd_4 = 0;
    uint32_t period_us_4 = 4000;

    uint32_t timer_fd_1 = 0;
    uint32_t period_us_1 = 1000;

    if (!make_timer(period_us_4, &timer_fd_4, true))
    {
		LOG_ERROR(MAC, "[TEST]: run_thread make 4ms periodic timer error");
		return NULL;
	}

    while(g_run_enable)
	{
		thread_wakeup(MAC_TASK, PERIODIC_4MS);

		if (!make_timer(period_us_1, &timer_fd_1, false))
		{
			LOG_ERROR(MAC, "[TEST]: run_thread make 1ms timer error");
			return NULL;
		}

		wait_period(timer_fd_1);
		
		stop_timer(timer_fd_1);

		thread_wakeup(MAC_TASK, PERIODIC_1MS);


		run_time++;
		if (run_time >= 20)
		{
			g_run_enable = false;
			break;
		}

		if (g_run_enable) 
		{
			wait_period(timer_fd_4);
		}
	}

	return 0;
}

void *mac_pre_thread()
{
	while(thread_wait(MAC_TASK, PERIODIC_4MS))
	{
		run_period();
	}
	return NULL;
}

void *mac_main_thread()
{
	while(thread_wait(MAC_TASK, PERIODIC_1MS))
	{
		run_scheduler();
	}
	return NULL;
}

void create_task()
{
	create_new_thread(MAC_TASK, mac_thread, NULL);
	create_new_thread(MAC_PRE_TASK, mac_pre_thread, NULL);
	create_new_thread(MAC_PRE_TASK, mac_main_thread, NULL);
	create_new_thread(RRC_TASK, rrc_thread, NULL);
	create_new_thread(RLC_TASK, rlc_thread, NULL);
}

void init_mac_sim()
{
	init_mac();
	init_thread(MAC_TASK);//used to sync
	message_int(MAC_TASK, ENONBLOCK);
	message_int(MAC_PRE_TASK, ENONBLOCK);
	message_int(MAC_PRE_TASK, ENONBLOCK);
}

void init_sim()
{
	g_run_enable = true;
	run_time = 0;
	init_mac_sim();
	init_rrc_sim();

	//for (uint32_t i = 0; i < MAX_TASK; i++)
	//{
	//	message_int((task_id)i);
	//}
}

int main()
{
	LOG_INFO(MAC, "[TEST]: MAC main start");
	
	init_sim();
	create_task();

	while (g_run_enable)
	{

	}
	return 1;
}
