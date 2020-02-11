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
#include "mac_osp_interface.h"

bool g_timing_sync = false;

mac_info_s* init_mac_info()
{
	uint32_t i = 0;

	mac_info_s *mac = (mac_info_s*) mem_alloc(sizeof(mac_info_s));
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
	mac->min_rbs_per_ue = 1;

	for (; i < MAX_RBS; i++)
	{
		mac->rb_available[i] = 1;
	}

	for(uint32_t j = 0; j < MAX_UE; j++)
	{
		mac->ue[j].active = false;
		mac->ue[j].ueIndex = INVALID_U16;
	}

	return mac;
}

void init_mac()
{
	g_context.mac = init_mac_info();
	g_context.macd = init_mac_info();
	g_timing_sync = false;
}

void mac_clean()
{
	mem_free((char*)g_context.mac);
	mem_free((char*)g_context.macd);

	LOG_INFO(MAC, "mac_clean");
}

int32_t pre_check(const sub_frame_t subframe)
{
	if ((g_context.mac->status == ESTATUS_ACTIVE) && (subframe == 0 || subframe == 1))
	{
		return 0;
	}
	else if((g_context.macd->status == ESTATUS_ACTIVE) && (subframe == 2 || subframe == 3))
	{
		return 1;
	}

	return -1;
}

void mac_pre_handler(msgDef *msg)
{
	frame_t frame;
	sub_frame_t subframe;

	if (g_timing_sync == false)
		return;

	frame = g_context.frame;
	subframe = g_context.subframe;

	frame = (frame + (subframe + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
	subframe = (subframe + TIMING_ADVANCE) % MAX_SUBSFN;

	g_context.mac->frame = frame;
	g_context.mac->subframe = subframe;
	g_context.mac->cce_bits = 0;

	g_context.macd->frame = frame;
	g_context.macd->subframe = subframe;
	g_context.macd->cce_bits = 0;

	int32_t ret = pre_check(subframe);

	g_sch_mac = ((ret == 0) ? g_context.mac : g_context.macd);

	if (ret == 0)
	{
		pre_schedule(frame, subframe, g_context.mac);
	}
	else
	{
		//pre_schedule(frame, subframe, g_context.macd);
	}

}

void msg_handler(msgDef* msg)
{
	//msgDef* msg = NULL;
	//uint32_t msg_len = 0;
	task_id taskId = 0;

	taskId = get_SrcId(msg);

	switch (taskId)
	{
		case TASK_D2D_RLC_TX:
		{
			handle_buffer_status_ind(msg);
			break;
		}
		case TASK_D2D_PHY_RX:
		{
			handle_phy_msg(msg);
			break;
		}
		default:
		{
			LOG_ERROR(MAC, "msg_handler, unknown task msg msg_id:%u", taskId);
			break;
		}
	}
}

/*
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
*/
extern	int32_t get_sysSfn();

void syncTime()//TODO: sync
{

	int time = get_sysSfn();

	g_context.frame = time >> 2;
	g_context.subframe = time % MAX_SUBSFN;

	if (g_timing_sync == false)
		g_timing_sync = true;
/*
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
	*/
}

int32_t init_mac_period()
{
	void* pTimer;
	int32_t ret;
	
	(void)_RegTimer4ms();

	pTimer = _timerCreate(TASK_D2D_MAC, 1, 4,0);
	ret = _timerStart(pTimer);

	LOG_INFO(MAC,"init_mac_period pTimer is %p, ret:%d\r\n", pTimer,ret);

	init_mac();

	g_sch_mac = g_context.mac;

	msgDef* msg = NULL;

	rrc_mac_initial_req *req;
	msgSize msg_size = sizeof(rrc_mac_initial_req);

	msg = new_message(RRC_MAC_INITIAL_REQ, TASK_D2D_RRC, TASK_D2D_MAC, msg_size);

	if (msg != NULL)
	{
		req = (rrc_mac_initial_req*)message_ptr(msg);
		req->cellId = 					 0;
		req->bandwith =					 1;
		req->pdcch_config.rb_num =		 2;
		req->pdcch_config.rb_start_index = 2;
		req->subframe_config =			 0;
		req->mode =   0;

		if (message_send(TASK_D2D_MAC, msg, sizeof(msgDef)))
		{
			LOG_INFO(RRC, "LGC: rrc_mac_initial_req send");
		}
	}

	rrc_mac_connnection_setup* setup;
	msg_size = sizeof(rrc_mac_connnection_setup);

	msg = new_message(RRC_MAC_CONNECT_SETUP_CFG_REQ, TASK_D2D_RRC, TASK_D2D_MAC, msg_size);

	if (msg != NULL)
	{
		setup = (rrc_mac_connnection_setup*)message_ptr(msg);

		setup->mode = 0;
		setup->ue_index = 1;
		setup->maxHARQ_Tx = 4;
		setup->max_out_sync = 4;
		setup->logical_channel_num = 1;
		setup->logical_channel_config[0].chan_type = DTCH;
		setup->logical_channel_config[0].priority = 15;
		setup->logical_channel_config[0].logical_channel_id = 3;

		if (message_send(TASK_D2D_MAC, msg, sizeof(msgDef)))
		{
			LOG_INFO(RRC, "LGC: RRC_MAC_CONNECT_SETUP_CFG_REQ send");
		}
	}

	return 0;
}


// interrupt function
void run_period(msgDef* msg)
{
	bool isTimer = is_timer(msg);

	if (isTimer)
	{
		syncTime();
	}

	LOG_INFO(MAC, "run_period current time, isTimer:%u, frame：%u，subframe:%u",  
		isTimer, g_context.frame, g_context.subframe);

	if (!isTimer)
	{
		handle_rrc_msg(msg);
	}
	else
	{
		mac_pre_handler(msg);
	}

	message_free(msg);
}

int32_t init_mac_scheduler()
{
	void* pTimer;
	int32_t ret;

	(void)_RegTimer1ms();

	pTimer = _timerCreate(TASK_D2D_MAC_SCH, 1, 4, 1);
	ret = _timerStart(pTimer);

	LOG_INFO(MAC,"init_mac_scheduler pTimer is %p, ret:%d\r\n", pTimer,ret);

	return 0;
}

void run_scheduler(msgDef* msg)
{
	sub_frame_t subframe = (g_context.subframe + TIMING_ADVANCE) % MAX_SUBSFN;

	bool isTimer = is_timer(msg);
	int32_t ret = pre_check(subframe);

	LOG_INFO(MAC, "run_scheduler， frame:%u, subframe:%u, isTimer:%u", 
		g_context.mac->frame, g_context.mac->subframe, isTimer);
	
	if (!isTimer)
	{
		msg_handler(msg);
	}
	else if(ret >= 0)
	{			
		mac_scheduler();
	}

	message_free(msg);
}

