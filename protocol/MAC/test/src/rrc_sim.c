/**********************************************************
* @file rrc_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/03
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "rrc_sim.h"
#include "mytask.h"
#include "msg_queue.h"
#include "messageDefine.h"
#include "interface_rrc_mac.h"
#include "d2d_message_type.h"
#include "log.h"

rrc_info g_rrc;
uint32_t g_runtime = 0;


void msgHandler()
{
	
}

void rrcStatusHandler()
{
	msgDef msg;

	if (g_rrc.status == RRC_NONE)
	{
		rrc_mac_initial_req *req;
		msgSize msg_size = sizeof(rrc_mac_initial_req);

		msg.data = (uint8_t*)msg_malloc(msg_size);

		if (msg.data != NULL)
		{
			msg.header.msgId = RRC_MAC_INITIAL_REQ;
			msg.header.source = RRC_TASK;
			msg.header.destination = MAC_TASK;
			msg.header.msgSize = msg_size;

			req = (rrc_mac_initial_req*)msg.data;
			req->cellId = 0;
			req->bandwith = 1;
			req->pdcch_config.rb_num = 2;
			req->pdcch_config.rb_start_index = 2;
			req->subframe_config = 0;
			req->mode = 0;

			if (msgSend(RRC_MAC_QUEUE, (char *)&msg, sizeof(msgDef)))
			{
				g_rrc.status = RRC_INITAIL;
				LOG_ERROR(MAC, "[TEST]: rrc_mac_initial_req send");
			}

			//msg_free(msg);
		}
		else
		{
			LOG_ERROR(MAC, "[TEST]: new rrc message fail!");
		}
	}

	if (g_rrc.status == RRC_INITAIL_CFM)
	{
		rrc_mac_bcch_para_config_req *req;
		msgSize msg_size = sizeof(rrc_mac_bcch_para_config_req);

		msg.data = (uint8_t*)msg_malloc(msg_size);

		if (msg.data != NULL)
		{
			uint8_t *sib_pdu = (uint8_t *)malloc(8);
			msg.header.msgId = RRC_MAC_BCCH_PARA_CFG_REQ;
			msg.header.source = RRC_TASK;
			msg.header.destination = MAC_TASK;
			msg.header.msgSize = msg_size;

			req = (rrc_mac_bcch_para_config_req*)msg.data;
			req->flag = 3;
			req->mib.systemFrameNumber = 0;
			req->mib.pdcch_config.rb_num = 2;
			req->mib.pdcch_config.rb_start_index = 2;
			req->sib.size = 8;
			req->sib.sib_pdu = sib_pdu;

			memset(req->sib.sib_pdu, 0xFE, req->sib.size);
			
			if (msgSend(RRC_MAC_QUEUE, (char *)&msg, sizeof(msgDef)))
			{
				g_rrc.status = RRC_BCCH_SEND;
			}

			//msg_free(msg);
		}
		else
		{
			LOG_ERROR(MAC, "[TEST]: new rrc message fail!");
		}
	}
}

void rrcMsgHandler()
{
	msgDef msg;
	uint32_t msg_len = 0;

	while (1)
	{ 
		msg_len = msgRecv(RRC_QUEUE, (char *)&msg, MQ_MSGSIZE);

		if (msg_len == 0)
		{
			return;
		}

		switch (msg.header.msgId)
		{
			case MAC_RRC_INITIAL_CFM:
			{
				mac_rrc_initial_cfm *cfm = (mac_rrc_initial_cfm *)msg.data; //TODO: add msg body handler function

				LOG_INFO(MAC, "[TEST] mac_rrc_initial_cfm, status:%u, error_:%u",cfm->status, cfm->error_code);

				g_rrc.status = RRC_INITAIL_CFM;
				msg_free(cfm);
				break;
			}
			case MAC_RRC_RELEASE_CFM:
			{
				mac_rrc_release_cfm *cfm = (mac_rrc_release_cfm *)msg.data; //TODO: add msg body handler function

				LOG_INFO(MAC, "[TEST] mac_rrc_release_cfm, status:%u",cfm->status);
				msg_free(cfm);
				break;
			}
			case MAC_RRC_CONNECT_SETUP_CFG_CFM:
			{
				mac_rrc_connection_cfm *cfm = (mac_rrc_connection_cfm *)msg.data; //TODO: add msg body handler function

				LOG_INFO(MAC, "[TEST] mac_rrc_connection_cfm, status:%u,ue_index:%u,rnti:%u,error:%u",
					cfm->status,cfm->ue_index,cfm->rnti,cfm->error_code);
				msg_free(cfm);
				break;
			}
			case MAC_RRC_BCCH_PARA_CFG_CFM:
			{
				mac_rrc_bcch_para_config_cfm *cfm = (mac_rrc_bcch_para_config_cfm *)msg.data; //TODO: add msg body handler function
				
				LOG_INFO(MAC, "[TEST] mac_rrc_bcch_para_config_cfm, status:%u,flag:%u,error:%u", cfm->status,cfm->flag,cfm->error_code);
				msg_free(cfm);
				break;
			}
			case MAC_RRC_OUTSYNC_RPT:
			{
				mac_rrc_outsync_rpt *cfm = (mac_rrc_outsync_rpt *)msg.data; //TODO: add msg body handler function
				
				LOG_INFO(MAC, "[TEST] mac_rrc_outsync_rpt, rnti:%u, outsync_flag:%u", cfm->rnti,cfm->outsync_flag);
				msg_free(cfm);
				break;
			}
			default:
				break;
		}
	}
}

void *rrc_thread()
{
	uint32_t timer_fd = 0;
	uint32_t period_us = 4000;

	if (!make_timer(period_us, &timer_fd, true))
	{
		LOG_ERROR(MAC, "[TEST]: run_thread make periodic timer error");
		return NULL;
	}

	while(1)
	{
		rrcStatusHandler();
		rrcMsgHandler();
		if (g_runtime < 20)
		{
			g_runtime++;
			wait_period(timer_fd);
		}
		else 
		{
			break;
		}
	}
	return 0;
}


void init_rrc_sim()
{
	g_runtime = 0;

	g_rrc.cellId = 0;
	g_rrc.mode = 0;//source
	g_rrc.status = RRC_NONE;
	
}



