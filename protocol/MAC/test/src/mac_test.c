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
/*
bool g_run_enable = false;
uint32_t run_time = 0;

extern void init_rrc_sim();
extern void init_rlc_sim();
extern void init_phy_sim();
extern void *rrc_thread();
extern void *rlc_thread();
extern void *phy_tx_thread();
extern void *phy_rx_thread();
extern void mac_clean();

uint32_t g_systemTing = 0;


uint32_t syncT()
{
	return run_time;
}

void timing_update()
{
	run_time++;
	
	if (run_time%10 == 4)
	{
		run_time = run_time + 6;
	}
	
	if (run_time == 100)
	{
		run_time = 0;
		g_run_enable = 0;
	}
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

    while(1)
	{
		thread_wakeup(TASK_D2D_MAC, PERIODIC_4MS);

		if (!make_timer(period_us_1, &timer_fd_1, false))
		{
			LOG_ERROR(MAC, "[TEST]: run_thread make 1ms timer error");
			return NULL;
		}

		wait_period(timer_fd_1);
		
		close_timer(timer_fd_1);

		thread_wakeup(TASK_D2D_MAC, PERIODIC_1MS);

		wait_period(timer_fd_4);

		timing_update();

	}

	return 0;
}

void *mac_pre_thread()
{
	while(thread_wait(TASK_D2D_MAC, PERIODIC_4MS))
	{
		run_period();
	}
	return NULL;
}

void *mac_main_thread()
{
	while(thread_wait(TASK_D2D_MAC, PERIODIC_1MS))
	{
		run_scheduler();
	}

	return NULL;
}

void create_task()
{
	create_new_thread(TASK_D2D_MAC, mac_thread, NULL);
	create_new_thread(TASK_D2D_MAC, mac_pre_thread, NULL);
	create_new_thread(TASK_D2D_MAC_SCH, mac_main_thread, NULL);
	create_new_thread(TASK_D2D_RRC, rrc_thread, NULL);
	create_new_thread(TASK_D2D_RLC, rlc_thread, NULL);
	create_new_thread(TASK_D2D_PHY_TX, phy_tx_thread, NULL);
	create_new_thread(TASK_D2D_PHY_RX, phy_rx_thread, NULL);
}

void init_mac_sim()
{
	init_mac();
	init_thread(TASK_D2D_MAC);//used to sync
}

void mac_test_clean()
{
	mac_clean();
	message_close();
}

void init_sim(const uint16_t mode)
{
	g_run_enable = true;
	run_time = 0;

	//init_mac_sim();
	init_rrc_sim(mode);
	init_rlc_sim(mode);
	init_phy_sim(mode);

}
int gdb=0;
int main_______(int argc,char *argv[])
{
	LOG_INFO(MAC, "[TEST]: MAC main start");

	uint16_t mode = 0; //0:source, 1:destination
	while(gdb);
	if (argc >= 2)
	{
		mode = (*argv[1] == '1') ? 1 : 0;
		
		LOG_INFO(MAC, "[TEST]: MAC main start mode:%u", mode);
	}
	
	init_sim(mode);
	create_task();

	while (g_run_enable)
	{

	}

	mac_test_clean();

	return 1;
}
*/
