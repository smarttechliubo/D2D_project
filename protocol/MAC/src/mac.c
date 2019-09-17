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
#include "mac_ra.h"
#include "mac_rx.h"
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

void mac_release_cfm()
{
	mac_rrc_release_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_release_cfm);
	msgDef* msg = (msgDef*)new_msg(msg_size);
	if (msg != NULL)
	{
		msg->header.msgId = MAC_RRC_INITIAL_CFM;
		msg->header.source = MAC_TASK;
		msg->header.destination = RRC_TASK;
		msg->header.msgSize = msg_size;

		cfm = (mac_rrc_release_cfm*)msg->data;
		cfm->status = 1;
		cfm->error_code = 0;

		if (msgSend(RRC_QUEUE, (char *)msg, msg_size + sizeof(msgDef)))
		{
		}

		msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "[TEST]: new mac message fail!");
	}
}

void mac_release(const rrc_mac_release_req *req)//TODO: mac reset ue release
{
	
	mac_release_cfm();
}


// MAC_TEST
void mac_config_cfm()
{
	mac_rrc_initial_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_initial_cfm);
	msgDef* msg = (msgDef*)new_msg(msg_size);
	if (msg != NULL)
	{
		msg->header.msgId = MAC_RRC_INITIAL_CFM;
		msg->header.source = MAC_TASK;
		msg->header.destination = RRC_TASK;
		msg->header.msgSize = msg_size;

		cfm = (mac_rrc_initial_cfm*)msg->data;
		cfm->status = 1;
		cfm->error_code = 0;

		if (msgSend(RRC_QUEUE, (char *)msg, msg_size + sizeof(msgDef)))
		{
		}

		msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "[TEST]: new mac message fail!");
	}
}

void mac_config(const rrc_mac_initial_req *req)
{
	mac_info_s *mac = g_context.mac;
	if(mac != NULL)
	{
		mac->mode = (mode_e)req->mode;
		mac->cellId = req->cellId;
		mac->bandwith = req->bandwith;
		mac->subframe_config = req->subframe_config;
		mac->rb_num = req->pdcch_config.rb_num;
		mac->rb_start_index = req->pdcch_config.rb_start_index;
		
		mac->max_rbs_per_ue = MAX_RBS;
		mac->status = STATUS_INIT;
		
		mac_config_cfm();
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
//MAC_TEST
	msgDef *msg = (msgDef *)malloc(MQ_MSGSIZE);
	uint32_t msg_len = msgRecv(MAC_QUEUE, (char *)msg, MQ_MSGSIZE);

	if (msg_len == 0)
	{
		LOG_INFO(MAC, "[TEST] NO RRC MSG");
		return;
	}

	switch (msg->header.msgId)
	{
		case RRC_MAC_INITIAL_REQ:
		{
			rrc_mac_initial_req *req = (rrc_mac_initial_req *)msg->data; //TODO: add msg body handler function

			LOG_INFO(MAC, "mac initial, cellId:%u, mode:%u, bw:%u", 
				req->cellId,req->mode,req->bandwith);

			mac_config(req);
			free(msg);
			break;
		}
		case RRC_MAC_RELEASE_REQ:
		{
			rrc_mac_release_req *req = (rrc_mac_release_req *)msg->data; //TODO: add msg body handler function

			LOG_INFO(MAC, "mac release, cellId:%u, ue_index:%u, releaseCause:%u", 
				req->cellId,req->ue_index,req->releaseCause);

			mac_release(req);
			free(msg);
			break;
		}
		case RRC_MAC_CONNECT_SETUP_CFG_REQ:
		{
			rrc_mac_connnection_setup *req = (rrc_mac_connnection_setup *)msg->data;

			LOG_INFO(MAC, "mac connect setup, ue_index:%u, maxHARQ_Tx:%u", 
				req->ue_index,req->maxHARQ_Tx);

			free(msg);
			break;
		}
		case RRC_MAC_BCCH_PARA_CFG_REQ:
		{
			rrc_mac_bcch_para_config_req *req = (rrc_mac_bcch_para_config_req *)msg->data;
			
			rrc_mac_bcch_req(req);
			free(msg);
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

	if (g_timing_sync == false)
		return;

	frame = g_context.frame;
	sub_frame = g_context.subframe;

	frame = (frame + (sub_frame + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
	sub_frame = (sub_frame + TIMING_ADVANCE) % MAX_SUBSFN;

	LOG_INFO(MAC, "[TEST] mac_pre_handler frame:%u, sub_frame:%u", frame, sub_frame);

	handle_rrc_msg();

	//pre_schedule(frame, sub_frame, g_context.mac);

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
			LOG_INFO(MAC, "[TEST] mac_pre_handler frame:%u, sub_frame:%u", g_context.frame, g_context.subframe);
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

