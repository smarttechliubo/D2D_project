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
#include "messageDefine.h"
#include "interface_rrc_mac.h"
#include "d2d_message_type.h"
#include "log.h"
#include "msg_handler.h"

rrc_info g_rrc;
static uint32_t g_runtime = 0;

void init_rrc_sim()
{
	g_runtime = 0;

	g_rrc.cellId = 0;
	g_rrc.mode = 0;//source
	g_rrc.status = RRC_NONE;
	
	message_int(RRC_TASK, ENONBLOCK);
}

void rrcStatusHandler()
{
	msgDef msg;

	if (g_rrc.status == RRC_NONE)
	{
		rrc_mac_initial_req *req;
		msgSize msg_size = sizeof(rrc_mac_initial_req);

		if (new_message(&msg, RRC_MAC_INITIAL_REQ, RRC_TASK, MAC_PRE_TASK, msg_size))
		{
			req = (rrc_mac_initial_req*)msg.data;
			req->cellId = 0;
			req->bandwith = 1;
			req->pdcch_config.rb_num = 2;
			req->pdcch_config.rb_start_index = 2;
			req->subframe_config = 0;
			req->mode = 0;

			if (message_send(MAC_PRE_TASK, (char *)&msg, sizeof(msgDef)))
			{
				g_rrc.status = RRC_INITAIL;
				LOG_INFO(RRC, "rrc_mac_initial_req send");
			}

			//msg_free(msg);
		}
		else
		{
			LOG_ERROR(RRC, "[TEST]: new rrc message fail!");
		}
	}

	if (g_rrc.status == RRC_INITAIL_CFM)
	{
		rrc_mac_bcch_para_config_req *req;
		msgSize msg_size = sizeof(rrc_mac_bcch_para_config_req);

		if (new_message(&msg, RRC_MAC_BCCH_PARA_CFG_REQ, RRC_TASK, MAC_PRE_TASK, msg_size))
		{
			uint8_t *sib_pdu = (uint8_t *)malloc(8);

			req = (rrc_mac_bcch_para_config_req*)msg.data;
			req->flag = 3;
			req->mib.systemFrameNumber = 0;
			req->mib.pdcch_config.rb_num = 2;
			req->mib.pdcch_config.rb_start_index = 2;
			req->sib.size = 8;
			req->sib.sib_pdu = sib_pdu;

			memset(req->sib.sib_pdu, 0xFE, req->sib.size);
			
			if (message_send(MAC_PRE_TASK, (char *)&msg, sizeof(msgDef)))
			{
				g_rrc.status = RRC_BCCH_SEND;
			}

		}
		else
		{
			LOG_ERROR(RRC, "[TEST]: new rrc message fail!");
		}
	}
}

void rrcMsgHandler()
{
	msgDef msg;
	uint32_t msg_len = 0;
	msgId msg_id = 0;

	while (1)
	{ 
		msg_len = message_receive(RRC_TASK, (char *)&msg, msg_len);

		if (msg_len == 0)
		{
			return;
		}

		msg_id = get_msgId(&msg);

		switch (msg_id)
		{
			case MAC_RRC_INITIAL_CFM:
			{
				mac_rrc_initial_cfm *cfm = (mac_rrc_initial_cfm *)msg.data;

				LOG_INFO(RRC, "[TEST] mac_rrc_initial_cfm, status:%u, error_:%u",cfm->status, cfm->error_code);

				g_rrc.status = RRC_INITAIL_CFM;
				message_free(cfm);
				break;
			}
			case MAC_RRC_RELEASE_CFM:
			{
				mac_rrc_release_cfm *cfm = (mac_rrc_release_cfm *)msg.data;

				LOG_INFO(RRC, "[TEST] mac_rrc_release_cfm, status:%u",cfm->status);
				message_free(cfm);
				break;
			}
			case MAC_RRC_BCCH_PARA_CFG_CFM:
			{
				mac_rrc_bcch_para_config_cfm *cfm = (mac_rrc_bcch_para_config_cfm *)msg.data;

				LOG_INFO(RRC, "[TEST] mac_rrc_bcch_para_config_cfm, status:%u,flag:%u,error:%u", 
					cfm->status,cfm->flag,cfm->error_code);

				g_rrc.status = RRC_BCCH_CFM;
				message_free(cfm);
				break;
			}
			case MAC_RRC_BCCH_MIB_RPT:
			{
				mac_rrc_bcch_mib_rpt *rpt = (mac_rrc_bcch_mib_rpt *)msg.data;
				
				LOG_INFO(RRC, "[TEST] mac_rrc_bcch_mib_rpt, SFN:%u,subsfn:%u", 
					rpt->SFN, rpt->subsfn);
				message_free(rpt);
				break;
			}
			case MAC_RRC_BCCH_SIB1_RPT:
			{
				mac_rrc_bcch_sib1_rpt *rpt = (mac_rrc_bcch_sib1_rpt *)msg.data;
				
				LOG_INFO(RRC, "[TEST] mac_rrc_bcch_para_config_cfm, SFN:%u, subsfn:%u, data_size:%u", 
					rpt->sfn, rpt->subsfn, rpt->data_size);
				message_free(rpt);
				break;
			}
			case MAC_RRC_OUTSYNC_RPT:
			{
				mac_rrc_outsync_rpt *cfm = (mac_rrc_outsync_rpt *)msg.data;
				
				LOG_INFO(RRC, "[TEST] mac_rrc_outsync_rpt, rnti:%u, outsync_flag:%u", cfm->rnti,cfm->outsync_flag);
				message_free(cfm);
				break;
			}
			case MAC_RRC_CCCH_RPT:
			{
				mac_rrc_ccch_rpt *cfm = (mac_rrc_ccch_rpt *)msg.data;

				LOG_INFO(RRC, "[TEST] mac_rrc_ccch_rpt");
				message_free(cfm);

				break;
			}
			case MAC_RRC_CONNECT_SETUP_CFG_CFM:
			{
				mac_rrc_connection_cfm *cfm = (mac_rrc_connection_cfm *)msg.data;

				LOG_INFO(RRC, "[TEST] mac_rrc_connection_cfm, status:%u,ue_index:%u,rnti:%u,error:%u",
					cfm->status,cfm->ue_index,cfm->rnti,cfm->error_code);
				message_free(cfm);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void *rrc_thread()
{
	uint32_t timer_fd = 0;
	uint32_t period_us = 4000;

	if (!make_timer(period_us, &timer_fd, true))
	{
		LOG_ERROR(RRC, "[TEST]: rrc_thread make periodic timer error");
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

