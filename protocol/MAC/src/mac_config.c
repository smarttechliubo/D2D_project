/**********************************************************
* @file mac_config.c
* @brief  mac config function
*
* @author	guicheng.liu
* @date 	2019/07/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "typedef.h"
#include "log.h"
#include "interface_rrc_mac.h"
#include "d2d_message_type.h"
#include "smac_context.h"
#include "mac_ra.h"
#include "mac_tx.h"
#include "mac_ue.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

extern context_s g_context;

// MAC_TEST
bool mac_config_cfm(bool success)
{
	msgDef msg;
	mac_rrc_initial_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_initial_cfm);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RRC_INITIAL_CFM;
		msg.header.source = MAC_TASK;
		msg.header.destination = RRC_TASK;
		msg.header.msgSize = msg_size;

		cfm = (mac_rrc_initial_cfm*)msg.data;
		cfm->status = 1;
		cfm->error_code = success;

		if (msgSend(RRC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
			return true;
		}

		//msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "[TEST]: new mac message fail!");
		return false;
	}
	return false;
}

void mac_config(const rrc_mac_initial_req *req)
{
	bool success = false;
	mac_info_s *mac = g_context.mac;

	if(mac != NULL && mac->status == STATUS_NONE)
	{
		mac->mode = (mode_e)req->mode;
		mac->cellId = req->cellId;
		mac->bandwith = req->bandwith;
		mac->subframe_config = req->subframe_config;
		mac->rb_num = req->pdcch_config.rb_num;
		mac->rb_start_index = req->pdcch_config.rb_start_index;
		
		mac->max_rbs_per_ue = MAX_RBS;

		success = true;

		for (uint32_t i = mac->rb_start_index; i < (mac->rb_start_index + mac->rb_num); i++)
		{
			mac->rb_available[i] = 0;
		}

		if (mac_config_cfm(success))
		{
			mac->status = STATUS_ACTIVE;
			init_mac_tx(mac->cellId);
		}
	}
	else
	{
		LOG_ERROR(MAC, "MAC config error");
	}


	init_ra(mac->cellId);
}

void rrc_mac_bcch_cfm(bool success)
{
	msgDef msg;
	mac_rrc_bcch_para_config_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_bcch_para_config_cfm);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RRC_BCCH_PARA_CFG_CFM;
		msg.header.source = MAC_TASK;
		msg.header.destination = RRC_TASK;
		msg.header.msgSize = msg_size;

		cfm = (mac_rrc_bcch_para_config_cfm*)msg.data;
		cfm->flag = 3;
		cfm->status = 1;
		cfm->error_code = success;

		if (msgSend(RRC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
		}

		//msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "[TEST]: new mac message fail!");
	}
}

void rrc_mac_bcch_req(const rrc_mac_bcch_para_config_req *req)
{
	bool success =false;
	mac_info_s *mac = g_context.mac;

	if (req->flag & 0x1)
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
		success = true;
	}

	if (req->flag & 0x2)
	{
		mac->common_channel.sib_size = req->sib.size;
		memcpy(mac->common_channel.sib_pdu, req->sib.sib_pdu, req->sib.size);
		success = true;
	}

	if ((req->flag & 0x3) == 0)
	{
		LOG_ERROR(MAC, "BCCH got invalid BCCH info, flag:%d", req->flag);
	}

	rrc_mac_bcch_cfm(success);

}

void handle_rrc_msg()
{
//MAC_TEST
	msgDef msg;
	uint32_t msg_len = 0;

	while (1)
	{
		msg_len = msgRecv(RRC_MAC_QUEUE, (char *)&msg, MQ_MSGSIZE);

		if (msg_len == 0)
		{
			return;
		}

		switch (msg.header.msgId)
		{
			case RRC_MAC_INITIAL_REQ:
			{
				rrc_mac_initial_req *req = (rrc_mac_initial_req *)msg.data; //TODO: add msg body handler function

				LOG_INFO(MAC, "RRC_MAC_INITIAL_REQ, msgId:%u,source:%u, dest:%u, cellId:%u, mode:%u, bw:%u", msg.header.msgId,msg.header.source,msg.header.destination,
					req->cellId,req->mode,req->bandwith);

				mac_config(req);
				msg_free(req);
				break;
			}
			case RRC_MAC_RELEASE_REQ:
			{
				rrc_mac_release_req *req = (rrc_mac_release_req *)msg.data; //TODO: add msg body handler function

				LOG_INFO(MAC, "RRC_MAC_RELEASE_REQ, cellId:%u, ue_index:%u, releaseCause:%u", 
					req->cellId,req->ue_index,req->releaseCause);

				mac_user_release(req);
				msg_free(req);
				break;
			}
			case RRC_MAC_CONNECT_SETUP_CFG_REQ:
			{
				rrc_mac_connnection_setup *req = (rrc_mac_connnection_setup *)msg.data;

				LOG_INFO(MAC, "RRC_MAC_CONNECT_SETUP_CFG_REQ, ue_index:%u, maxHARQ_Tx:%u", 
					req->ue_index,req->maxHARQ_Tx);

				mac_user_setup(req);
				msg_free(req);
				break;
			}
			case RRC_MAC_BCCH_PARA_CFG_REQ:
			{
				rrc_mac_bcch_para_config_req *req = (rrc_mac_bcch_para_config_req *)msg.data;
				
				LOG_INFO(MAC, "RRC_MAC_BCCH_PARA_CFG_REQ, flag:%u", req->flag);

				rrc_mac_bcch_req(req);
				msg_free(req);
				break;
			}
			case RRC_MAC_BCCH_SIB1_REQ:
				break;
			default:
				break;
		}
	}
}

