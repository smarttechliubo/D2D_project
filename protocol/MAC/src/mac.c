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
#include "mac_config.h"
#include "pre_schedule.h"
#include "d2d_message_type.h"
#include "interface_rrc_mac.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

bool g_timing_sync = false;

//extern context_s g_context;


void init_mac()
{
	
	mac_info_s *mac = (mac_info_s*) malloc(sizeof(mac_info_s));
    bzero(mac, sizeof(mac_info_s));

	mac->status = STATUS_NONE;
	mac->frame = INVALID_U32;
	mac->subframe = INVALID_U32;
	mac->cce_bits = 0;

	g_context.mac = mac;
	g_timing_sync = false;

}

void mac_clean()
{
	free(g_context.mac);
}

void mac_pre_handler()
{
	frame_t frame;
	sub_frame_t sub_frame;

	if (g_timing_sync == false)
		return;

	frame = g_context.frame;
	sub_frame = g_context.subframe;

	frame = (frame + (sub_frame + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
	sub_frame = (sub_frame + TIMING_ADVANCE) % MAX_SUBSFN;

	g_context.mac->cce_bits = 0;

	handle_rrc_msg();

	pre_schedule(frame, sub_frame, g_context.mac);

}

extern uint32_t syncT();

void syncTime()//TODO: sync
{
    // 1. get timing sync with PHY	
	if (g_timing_sync == false)
    {
		uint32_t time = syncT();// TODO: sync
		if (time != 0xFFFFFFFF)
		{
			g_context.frame = time >> 16;
			g_context.subframe = time&0xFFFF;
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
			uint32_t time = syncT();
			if (time != 0xFFFFFFFF)
			{
				if (time != g_context.frame*MAX_SUBSFN + g_context.subframe)
				{
					g_context.frame = time >> 16;
					g_context.subframe = time&0xFFFF;
					LOG_WARN(MAC, "Timing sync loast!!");
				}
			}
			else
		    {
				LOG_WARN(MAC, "Timing sync failed with PHY");
			}
		}
	}
}

// TODO: register interrupt function on platform, for now, just run with period timer 
// interrupt function
void run_period()
{
	LOG_INFO(MAC, "[TEST]: run_period");
	syncTime();		
	mac_pre_handler();
}

void run_scheduler()
{
	LOG_INFO(MAC, "[TEST]: run_scheduler");
	//handle_phy_msg();
}
