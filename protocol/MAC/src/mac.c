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
#include <pthread.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "mac_vars.h"
#include "mac_defs.h"
#include "mac_ra.h"
#include "mac_rx.h"
#include "pre_schedule.h"
#include "d2d_message_type.h"
#include "interface_rrc_mac.h"

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

#ifdef TASK_MAC
    //pthread_create(&g_mac.thread, NULL, run_thread, g_mac.arg);
#endif
}

void mac_clean()
{
	free(g_context.mac);
}

bool mac_release()
{
	return true;
}

/****************************************/
//TODO: Temp define
//bool syncTime(frame_t *frame, sub_frame_t *subframe);
uint32_t syncT()
{
	return 0;
}

/****************************************/
void rrc_mac_config(const rrc_mac_initial_req *req)
{
	mac_info_s *mac = g_context.mac;
	if(mac != NULL)
	{
		mac->mode = req->mode;
		mac->cellId = req->cellId;
		mac->bandwith = req->bandwith;
		mac->subframe_config = req->subframe_config;
		mac->rb_num = req->pdcch_config.rb_num;
		mac->rb_start_index = req->pdcch_config.rb_start_index;
		
		mac->max_rbs_per_ue = MAX_RBS;
		mac->status = STATUS_INIT;
	}
	else
	{
		LOG_ERROR(MAC, "MAC config error");
	}

	init_ra(mac->cellId);
}

void rrc_mac_bcch_req(const rrc_mac_bcch_para_config_req *req)
{
	mac_info_s *mac = g_context.mac;
	if (req->flag == 1)
	{
		if (req->mib.pdcch_config.rb_num > 0 && 
			(req->mib.pdcch_config.rb_start_index >= 0 && req->mib.pdcch_config.rb_start_index <= 3))
		{
			mac->common_channel.mib_size = MIB_PDU_SIZE;
			mac->common_channel.bch_info.rb_num = req->mib.pdcch_config.rb_num;
			mac->common_channel.bch_info.rb_start_index = req->mib.pdcch_config.rb_start_index;
		}
		else
		{
			LOG_ERROR(MAC, "BCCH MIB parameter incorrect, PDCCH rb_num:%d, rb_start_index:%d", 
				req->mib.pdcch_config.rb_num, req->mib.pdcch_config.rb_start_index);
		}
	}
	else if (req->flag == 2)
	{
		mac->common_channel.sib_size = req->sib.size;
		memcpy(mac->common_channel.sib_pdu, req->sib.sib_pdu, req->sib.size);
	}
	else
	{
		LOG_ERROR(MAC, "BCCH got invalid BCCH info, flag:%d", req->flag);
	}

}

void handle_rrc_msg()
{
	message_type_t msg_type = RRC_MAC_INITIAL_REQ; // TODO: add msg header handler function
	rrc_mac_initial_req *req; //TODO: add msg body handler function

	switch (msg_type)
	{
		case RRC_MAC_INITIAL_REQ:
		{
			rrc_mac_config(req);
			break;
		}
		case RRC_MAC_RELEASE_REQ:
		{
			mac_release();
			break;
		}
		case RRC_MAC_CONNECT_SETUP_CFG_REQ:
			break;
		case RRC_MAC_BCCH_PARA_CFG_REQ:
		{
			//rrc_mac_bcch_req(req);
			break;
		}
		case RRC_MAC_BCCH_SIB1_REQ:
			break;
		default:
			break;
	}
}

void mac_pre_handler()
{
	frame_t frame;
	sub_frame_t sub_frame;

	if (g_timing_sync == false || g_context.mac->status < STATUS_SYNC)
		return;

	frame = g_context.frame;
	sub_frame = g_context.subframe;

	frame = (frame + (sub_frame + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
	sub_frame = (sub_frame + TIMING_ADVANCE) % MAX_SUBSFN;

	handle_rrc_msg();

	pre_schedule(frame, sub_frame, g_context.mac);

}

void syncTime()//TODO: sync
{
    // 1. get timing sync with PHY	
    //frame_t frame;
    //sub_frame_t subframe;
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
	syncTime();		
	mac_pre_handler();
}

void run_scheduler()
{
	handle_phy_msg();
}

