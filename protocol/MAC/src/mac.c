/**********************************************************
* @file mac.c
* @brief  mac handle function
*
* @author	guicheng.liu
* @date 	2019/06/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "mac_vars.h"
#include "mac_defs.h"
#include "mac_rx.h"
#include "mac_tx.h"
#include "mac_config.h"
#include "msg_handler.h"
#include "pre_schedule.h"
#include "d2d_message_type.h"
#include "interface_rrc_mac.h"
#include "intertask_interface.h"

bool g_timing_sync = false;

void init_mac()
{
	uint32_t i = 0;

	mac_info_s *mac = (mac_info_s*) malloc(sizeof(mac_info_s));
    bzero(mac, sizeof(mac_info_s));

	mac->status = ESTATUS_NONE;
	mac->frame = INVALID_U32;
	mac->subframe = INVALID_U32;
	mac->count_ue = 0;
	mac->cce_bits = 0;
	mac->num0 = 0;

	mac->rb_available[i++] = 0;
	mac->rb_available[i++] = 0;

	mac->alloc_pattern = EPATTERN_GREEDY;

	for (; i < MAX_RBS; i++)
	{
		mac->rb_available[i] = 1;
	}

	for(uint32_t j = 0; j < MAX_UE; j++)
	{
		mac->ue[j].active = false;
		mac->ue[j].ueIndex = INVALID_U16;
	}

	g_context.mac = mac;
	g_timing_sync = false;
}

void mac_clean()
{
	free(g_context.mac);

	LOG_INFO(MAC, "mac_clean");
}

void mac_pre_handler(msgDef *msg)
{
	LOG_INFO(MAC, "LGC: mac_pre_handler 1");

	frame_t frame;
	sub_frame_t subframe;

	if (!is_timer(msg))
	{
		handle_rrc_msg(msg);
	}
	else
	{
		if (g_timing_sync == false)
			return;

		frame = g_context.frame;
		subframe = g_context.subframe;

		frame = (frame + (subframe + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
		subframe = (subframe + TIMING_ADVANCE) % MAX_SUBSFN;

		g_context.mac->frame = frame;
		g_context.mac->subframe = subframe;
		g_context.mac->cce_bits = 0;

		if (!pre_check(subframe))
		{
			return;
		}

		pre_schedule(frame, subframe, g_context.mac);
	}
}

uint32_t system_run_time = 0;

void syncTimingUpdate()
{
	system_run_time++;
	
	if (system_run_time%10 == 4)
	{
		system_run_time = system_run_time + 6;
	}

	if (system_run_time == 10240)
		system_run_time = 0;
}

uint32_t get_syncTiming()
{
	 return system_run_time;
}


void syncTime()//TODO: sync
{
	uint32_t time = 0;

	// 1. get timing sync with PHY	
	if (g_timing_sync == false)
    {
		time = get_syncTiming();// TODO: sync

		if (time != 0xFFFFFFFF)
		{
			g_context.frame = time / 10;
			g_context.subframe = time % MAX_SUBSFN;
			g_timing_sync = true;
		}
		else
	    {
			LOG_WARN(MAC, "Timing sync fail!!");
		}
	}
	// 2. get period sync with PHY
	else if (g_timing_sync == true)
	{
	    g_context.subframe++;

		if (g_context.subframe == MAX_SUBSFN)
		{
		    g_context.subframe = 0;
			g_context.frame = (g_context.frame+1)%MAX_SFN;
		}
		
		if ((g_context.frame*10 + g_context.subframe)%TIMING_SYNC_PERIOD == 0)
		{
			time = get_syncTiming();

			if (time != 0xFFFFFFFF)
			{
				if (time != g_context.frame*10 + g_context.subframe)
				{
					LOG_WARN(MAC, "Timing sync loast!! time:%u, frame:%u, subframe:%u",
						time, g_context.frame, g_context.subframe);

					g_context.frame = time / 10;
					g_context.subframe = time % MAX_SUBSFN;
				}
			}
			else
		    {
				LOG_WARN(MAC, "Timing sync failed with PHY");
			}
		}
	}
}

int32_t init_mac_period()
{
	void* pTimer;
	int32_t ret;
	
	pTimer = OSP_timerCreateSim(TASK_D2D_MAC, 1, 4,0);
	ret = OSP_timerStart(pTimer);

	LOG_INFO(MAC,"init_mac_period pTimer is %p, ret:%d\r\n", pTimer,ret);

	init_mac();

	return 0;
}


// interrupt function
void run_period(msgDef* msg)
{
	syncTimingUpdate();//TODO: timing sync with phy
	syncTime();

	LOG_INFO(MAC, "run_period current time frame：%u，subframe:%u", g_context.frame, g_context.subframe);

	mac_pre_handler(msg);
}

int32_t init_mac_scheduler()
{
	void* pTimer;
	int32_t ret;
	
	pTimer = OSP_timerCreateSim(TASK_D2D_MAC_SCH, 1, 4, 1);
	ret = OSP_timerStart(pTimer);

	LOG_INFO(MAC,"init_mac_scheduler pTimer is %p, ret:%d\r\n", pTimer,ret);

	init_mac();

	return 0;
}

void run_scheduler(msgDef* msg)
{
	LOG_INFO(MAC, "run_scheduler， frame:%u, subframe:%u", g_context.mac->frame, g_context.mac->subframe);

	if (!is_timer(msg))
	{
		msg_handler(msg);
	}
	else
	{
		if (!pre_check(g_context.mac->subframe))
		{
			return;
		}

		mac_scheduler();
	}
}

