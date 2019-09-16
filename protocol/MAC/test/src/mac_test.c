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
#include "msg_queue.h"

bool g_run_enable = false;
uint32_t run_time = 0;

extern void init_rrc_sim();
extern void *rrc_thread();

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
		LOG_ERROR(MAC, "[TEST]: run_thread make periodic timer error");
		return NULL;
	}

	if (!make_timer(period_us_1, &timer_fd_1, false))
	{
		LOG_ERROR(MAC, "[TEST]: run_thread make 1ms timer error");
		return NULL;
	}
	stop_timer(timer_fd_1);

    while(g_run_enable)
	{
		restart_timer(period_us_1, timer_fd_1);
		
		run_period();

		wait_period(timer_fd_1);
		
		stop_timer(timer_fd_1);

		run_scheduler();


		run_time++;
		if (run_time >= 100)
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

void create_task()
{
	create_new_thread(MAC_TASK, mac_thread, NULL);
	create_new_thread(RRC_TASK, rrc_thread, NULL);
}

void init_mac_sim()
{
	init_mac();
	msgq_init(MAC_QUEUE);
}

void init_sim()
{
	g_run_enable = true;
	run_time = 0;
	init_mac_sim();
	init_rrc_sim();
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
