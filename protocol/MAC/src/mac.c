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
int32_t g_sync_old_sfn = -1;

bool  g_pdcch_send = false;
ULONG g_start_time = 0;
ULONG g_end_time = 0;
ULONG g_diff_time = 0;
ULONG g_run_period_time = 0;
ULONG g_run_scheduler_time = 0;
uint32_t g_pdcch_num = 0;
uint32_t g_pusch_num = 0;
bool  g_pdcch_send;

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
int sysSFN = 0;

int32_t get_sysSfn()
{
	int sfn = OSP_atomicGet(&sysSFN);
	return sfn;
}

void sysTimingUpdate()
{
	int time = (g_context.frame << 2) | (g_context.subframe&0x3);

	//if (sysSFN == 4095)
	//{
		OSP_atomicSet(&sysSFN, time);
	//}
	//else
	//{
		//OSP_atomicInc(&sysSFN);
	//}
}

void update_sfn()
{
    frame_t frame = g_context.frame;
    sub_frame_t subframe = g_context.subframe;

	subframe++;

	g_context.subframe = (subframe % MAX_SUBSFN);

	if (g_context.subframe == 0)
	{
		frame++;
		g_context.frame = (frame % MAX_SFN);
	}
}

void syncTime()//TODO: sync
{
	int time = 0;
    frame_t frame = g_context.frame;
    sub_frame_t subframe = g_context.subframe;

	if (g_timing_sync == false)
	{
		time = sfn_sync();//get_sysSfn();

		if ((time >= 0) && (time != g_sync_old_sfn))
		{
			g_context.frame = time >> 2;
			g_context.subframe = time & 0x3;

			g_timing_sync = true;
			g_sync_old_sfn = time;

			LOG_INFO(MAC, "syncTime: first SFN SYNC frame:%u, subframe:%u", 
				g_context.frame, g_context.subframe);
		}
		else
		{
			LOG_WARN(MAC, "syncTime: PHY not ready or time is not udpated ");
		}
	}
	else
	{
		if ((g_context.frame % 8 == 0) && (g_context.subframe % MAX_SUBSFN == 2))
		{
			time = sfn_sync();

			if ((time >= 0) && (time != g_sync_old_sfn))
			{
				frame = time >> 2;
				subframe = time & 0x3;
				g_sync_old_sfn = time;

				if (frame != g_context.frame || subframe != g_context.subframe)
				{
					LOG_ERROR(MAC, "syncTime: SFN SYNC fail,  sfn:%u, subsfn:%u, sfn:%u, subsfn:%u",
						frame, subframe, g_context.frame, g_context.subframe);

					g_context.frame = frame;
					g_context.subframe = subframe;

					g_timing_sync = false;
				}
				else
				{
					LOG_INFO(MAC, "syncTime: SFN SYNC ,  sfn:%u, subsfn:%u", frame, subframe);
				}
			}
			else 
			{
				LOG_ERROR(MAC, "syncTime: SFN SYNC fail,  time:%d, old sfn:%d, frame:%u, subframe:%u",
					time, g_sync_old_sfn, g_context.frame, g_context.subframe);

				g_timing_sync = false;
			}
		}
	}

}

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
	g_context.frame = 0;
	g_context.subframe = 0;

	g_context.mac = init_mac_info();
	g_timing_sync = false;
}

void mac_clean()
{
	mem_free((char*)g_context.mac);

	LOG_INFO(MAC, "mac_clean");
}

int32_t pre_check(const sub_frame_t subframe)
{
	int32_t ret = 0;

	if (g_context.mac->status == ESTATUS_ACTIVE)
	{
		if (g_context.mac->mode == 0)
		{
			if (g_context.mac->subframe_config == 0)
			{
				if (subframe <= 1)
				{
					ret = 1;
				}
				else
				{
					ret = 0;
				}
			}
			else if (g_context.mac->subframe_config == 1)
			{
				if (subframe <= 2)
				{
					ret = 1;
				}
				else
				{
					ret = 0;
				}
			}
		}
		else if (g_context.mac->mode == 1)
		{
			if (g_context.mac->subframe_config == 0)
			{
				if (subframe <= 1)
				{
					ret = 0;
				}
				else
				{
					ret = 1;
				}
			}
			else if (g_context.mac->subframe_config == 1)
			{
				if (subframe <= 2)
				{
					ret = 0;
				}
				else
				{
					ret = 1;
				}
			}
		}
	}
	else
	{
		ret = 0;
	}

	return ret;
}

void mac_pre_handler(frame_t frame, sub_frame_t subframe)
{
	g_context.mac->frame = frame;
	g_context.mac->subframe = subframe;
	g_context.mac->cce_bits = 0;

	pre_schedule(frame, subframe, g_context.mac);
}

void rcv_msg_handler(msgDef* msg)
{
	task_id taskId = 0;
	
	taskId = get_SrcId(msg);
	
	switch (taskId)
	{
		case TASK_D2D_RRC:
#ifdef MAC_TEST
		case TASK_D2D_MAC://TODO:TEST only
#endif
		{
			handle_rrc_msg(msg);
			break;
		}
		case TASK_D2D_RLC_TX:
		{
			handle_buffer_status_ind(msg);
			break;
		}
#ifdef MAC_TEST
		case TASK_D2D_MAC_SCH://TODO:TEST only
#endif
		case TASK_D2D_PHY_RX:
		{
			handle_phy_msg(msg);
			break;
		}
		default:
		{
			LOG_ERROR(MAC, "msg_handler, unknown task msg taskId:%u", taskId);
			break;
		}
	}
}


int32_t init_mac_period()
{
	//void* pTimer = _timerCreate(TASK_D2D_MAC, 1, 400,0);
	//int32_t ret = _timerStart(pTimer);

	//LOG_INFO(MAC,"init_mac_period pTimer is %p, ret:%d\r\n", pTimer,ret);

	//(void)_RegTimer4ms();
	//setFrameOffsetTime(1);

	init_mac();

	return 0;
}

extern void handle_phy_rx(const      frame_t frame, const sub_frame_t subframe);

// interrupt function
void run_period(msgDef* msg)
{
	bool isTimer = is_timer(msg);

	if (isTimer)
	{
		g_start_time = getOspCycel();
	}

	LOG_ERROR(MAC, "run_period current time, isTimer:%u, frame：%u，subframe:%u, count_ue:%u",  
		isTimer, g_context.frame, g_context.subframe, g_context.mac->count_ue);

	if (!isTimer)
	{
		rcv_msg_handler(msg);
	}
	else
	{
		//mac_pre_handler();
	}

	message_free(msg);

	if (isTimer)
		g_run_period_time = getOspCycel();
}

int32_t init_mac_scheduler()
{
	//void* pTimer = _timerCreate(TASK_D2D_MAC_SCH, 1, 400, 100);
	//int32_t ret = _timerStart(pTimer);

	//setFrameOffsetTime(1);

	//LOG_INFO(MAC,"init_mac_scheduler pTimer is %p, ret:%d\r\n", pTimer,ret);

	//(void)_RegTimer4ms();


	return 0;
}

void run_scheduler()
{
	int ret = 0;
	sub_frame_t subframe = 0;
	frame_t frame = 0;

	while(1)
	{
		ret = Osp_FrameIrqBlock();

		update_sfn();//syncTime();

		syncTime();
		sysTimingUpdate();

		subframe = (g_context.subframe + MAC_SCH_TIMING_ADVANCE) % MAX_SUBSFN;
		frame = (g_context.frame + (g_context.subframe + MAC_SCH_TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;

		g_run_scheduler_time = getOspCycel();

		if (g_timing_sync)
		{
			mac_pre_handler(frame, subframe);
#ifdef MAC_DEBUG
			LOG_ERROR(MAC, "run_scheduler,current sfn:%u, subsfn:%u, AIR sfn:%u, subsfn:%u", 
				g_context.frame, g_context.subframe,g_context.mac->frame, g_context.mac->subframe);
#endif

			//if(pre_check(subframe))
			{
				mac_scheduler();
			}

			frame = (frame + (subframe + 1) / MAX_SUBSFN) % MAX_SFN;
			subframe = (subframe + 1) % MAX_SUBSFN;

			//if (pre_check(subframe))
			{
				handle_buffer_status_req(frame, subframe);
			}

#ifdef MAC_TEST
			frame = (g_context.frame + (g_context.subframe + 1) / MAX_SUBSFN) % MAX_SFN;
			subframe = (g_context.subframe + 1) % MAX_SUBSFN;

			handle_phy_tx(frame, subframe);// for FPGA

			handle_phy_rx(g_context.frame, g_context.subframe);
#endif
		}

		g_end_time = getOspCycel();

		if (g_pdcch_send == true)
		{
			g_diff_time = g_end_time - g_start_time;
			ULONG run_period_time = g_run_period_time - g_start_time;
			ULONG run_scheduler_time = g_end_time - g_run_scheduler_time;
			ULONG run_scheduler_offset = g_run_scheduler_time - g_start_time;

			LOG_ERROR(MAC, " pdcch+pusch:%u,pdcch_num:%u, pusch:%u, MAC process time : %llu, run_period_time:%llu, run_scheduler_offset:%llu, run_scheduler_time:%llu", 
				g_pdcch_num+g_pusch_num,g_pdcch_num, g_pusch_num,g_diff_time, run_period_time, run_scheduler_offset, run_scheduler_time);
		}

		g_pdcch_send = false;
	}
}

#if 0
int32_t init_mac_period()
{
	//void* pTimer = _timerCreate(TASK_D2D_MAC, 1, 400,0);
	//int32_t ret = _timerStart(pTimer);

	//LOG_INFO(MAC,"init_mac_period pTimer is %p, ret:%d\r\n", pTimer,ret);

	//(void)_RegTimer4ms();

	init_mac();

	return 0;
}


// interrupt function
void run_period(msgDef* msg)
{
	bool isTimer = is_timer(msg);

	if (isTimer)
	{
		g_start_time = getOspCycel();
		update_sfn();//syncTime();
	}

	LOG_ERROR(MAC, "run_period current time, isTimer:%u, frame：%u，subframe:%u, count_ue:%u",  
		isTimer, g_context.frame, g_context.subframe, g_context.mac->count_ue);

	if (!isTimer)
	{
		rcv_msg_handler(msg);
	}
	else
	{
		mac_pre_handler(msg);
	}

	message_free(msg);

	if (isTimer)
		g_run_period_time = getOspCycel();
}

int32_t init_mac_scheduler()
{
	//void* pTimer = _timerCreate(TASK_D2D_MAC_SCH, 1, 400, 100);
	//int32_t ret = _timerStart(pTimer);

	//setFrameOffsetTime(1);

	//LOG_INFO(MAC,"init_mac_scheduler pTimer is %p, ret:%d\r\n", pTimer,ret);

	//(void)_RegTimer4ms();


	return 0;
}

void run_scheduler(msgDef* msg)
{
	bool isTimer = is_timer(msg);

#ifdef MAC_DEBUG
	LOG_ERROR(MAC, "run_scheduler， frame:%u, subframe:%u, isTimer:%u, g_timing_sync:%u", 
		g_context.mac->frame, g_context.mac->subframe, isTimer, g_timing_sync);
#endif

	int32_t ret = 0;

	sub_frame_t subframe = (g_context.subframe + MAC_SCH_TIMING_ADVANCE) % MAX_SUBSFN;
	frame_t frame = (g_context.frame + (subframe + MAC_SCH_TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;

	ret = pre_check(subframe);

	if (isTimer)
	{
		g_run_scheduler_time = getOspCycel();
		syncTime();
	}

	if (!isTimer)
	{
		sch_msg_handler(msg);
	}
	else if(ret > 0)
	{
		mac_scheduler();
	}

	frame = (frame + (subframe + 1) / MAX_SUBSFN) % MAX_SFN;
	subframe = (subframe + 1) % MAX_SUBSFN;

	if (pre_check(subframe))
	{
		handle_buffer_status_req(frame, subframe);
	}

	g_end_time = getOspCycel();


	message_free(msg);

	if (g_pdcch_send == true)
	{
		g_diff_time = g_end_time - g_start_time;
		ULONG run_period_time = g_run_period_time - g_start_time;
		ULONG run_scheduler_time = g_end_time - g_run_scheduler_time;
		ULONG run_scheduler_offset = g_run_scheduler_time - g_start_time;

		LOG_ERROR(MAC, "MAC process time : %llu, run_period_time:%llu, run_scheduler_time:%llu", 
			g_diff_time, run_period_time, run_scheduler_time);

		if (g_diff_time > 300000 || run_period_time > 80000 || run_scheduler_time > 150000)
		{
			LOG_ERROR(MAC, "CMD MAC process time : %llu, run_period_time:%llu, run_scheduler_offset:%llu, run_scheduler_time:%llu", 
				g_diff_time, run_period_time, run_scheduler_offset, run_scheduler_time);
		}

		//pdcch_num++;

		LOG_CMD(MAC, " pdcch+pusch:%u,pdcch_num:%u, pusch:%u, MAC process time : %llu, run_period_time:%llu, run_scheduler_offset:%llu, run_scheduler_time:%llu", 
			g_pdcch_num+g_pusch_num,g_pdcch_num, g_pusch_num,g_diff_time, run_period_time, run_scheduler_offset, run_scheduler_time);
	}

	g_pdcch_send = false;
}
#endif
