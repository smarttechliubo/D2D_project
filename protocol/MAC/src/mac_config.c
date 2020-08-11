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
#include "msg_handler.h"

extern context_s g_context;

bool mac_paras_check(const rrc_mac_initial_req *req)
{
	if (req->cellId > 503 || req->bandwith > 4 || req->subframe_config > 1 || req->mode > 1)
	{
		LOG_ERROR(MAC, "mac init paras check fail.cellId:%u, bandwith:%u, subframeconfig:%u, mode:%u", 
			req->cellId, req->bandwith, req->subframe_config, req->mode);

		return false;
	}

	if (req->pdcch_config.rb_num > 2 || req->pdcch_config.rb_num <= 0 || req->pdcch_config.rb_start_index > 3)
	{
		LOG_ERROR(MAC, "mac init paras pdcch check fail. rb_num:%u, rb_start_index:%u", 
			req->pdcch_config.rb_num, req->pdcch_config.rb_start_index);

		return false;
	}

	return true;
}

bool mac_config_cfm(bool success)
{
	msgDef* msg = NULL;
	mac_rrc_initial_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_initial_cfm);

	msg = new_message(MAC_RRC_INITIAL_CFM, TASK_D2D_MAC, TASK_D2D_RRC, msg_size);

	if (msg != NULL)
	{

		cfm = (mac_rrc_initial_cfm*)message_ptr(msg);
		cfm->status = 1;
		cfm->error_code = success;

		if (message_send(TASK_D2D_RRC, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_RRC_INITIAL_CFM send");
			return true;
		}

	}
	else
	{
		LOG_ERROR(MAC, "new mac message fail!");
		return false;
	}
	return false;
}

void mac_config(const rrc_mac_initial_req *req)
{
	bool success = mac_paras_check(req);
	mac_info_s *mac = g_context.mac;

	if(mac != NULL && mac->status == ESTATUS_NONE)
	{
		mac->mode = (mode_e)req->mode;
		mac->cellId = req->cellId;
		mac->bandwith = req->bandwith;
		mac->subframe_config = req->subframe_config;
		mac->rb_num = req->pdcch_config.rb_num;
		mac->rb_start_index = req->pdcch_config.rb_start_index;
		
		mac->max_rbs_per_ue = MAX_RBS;
		//mac->max_rbs_per_ue = 1;

		success = true;

		for (uint32_t i = mac->rb_start_index; i < (mac->rb_start_index + mac->rb_num); i++)
		{
			mac->rb_available[i] = 0;
		}

		if (mac_config_cfm(success))
		{
			mac->status = ESTATUS_ACTIVE;
			init_mac_tx(mac->cellId);
		}
	}
	else
	{	
		//mac_config_cfm(success);
		LOG_ERROR(MAC, "MAC config error, mode:%u, mac:%u, status:%u", req->mode, mac != NULL, mac->status);
	}


	init_ra(mac->cellId);
}

void rrc_mac_bcch_cfm(bool success)
{
	msgDef* msg = NULL;
	mac_rrc_bcch_para_config_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_bcch_para_config_cfm);

	msg = new_message(MAC_RRC_BCCH_PARA_CFG_CFM, TASK_D2D_MAC, TASK_D2D_RRC, msg_size);

	if (msg != NULL)
	{
		cfm = (mac_rrc_bcch_para_config_cfm*)message_ptr(msg);
		cfm->flag = 3;
		cfm->status = 1;
		cfm->error_code = success;

		if (message_send(TASK_D2D_RRC, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_RRC_BCCH_PARA_CFG_CFM send");
		}
	}
	else
	{
		LOG_ERROR(MAC, "bcch, new mac message fail!");
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

void handle_rrc_msg(msgDef *msg)
{
	msgId msg_id = 0;

	msg_id = get_msgId(msg);

	switch (msg_id)
	{
		case RRC_MAC_INITIAL_REQ:
		{
			rrc_mac_initial_req *req = (rrc_mac_initial_req *)message_ptr(msg);

			LOG_INFO(MAC, "RRC_MAC_INITIAL_REQ, cellId:%u, mode:%u, bw:%u", 
				req->cellId,req->mode,req->bandwith);

			mac_config(req);
			break;
		}
		case RRC_MAC_RELEASE_REQ:
		{
			rrc_mac_release_req *req = (rrc_mac_release_req *)message_ptr(msg);

			LOG_INFO(MAC, "RRC_MAC_RELEASE_REQ, cellId:%u, ue_index:%u, releaseCause:%u", 
				req->cellId,req->ue_index,req->releaseCause);

			mac_user_release(req);
			break;
		}
		case RRC_MAC_CONNECT_SETUP_CFG_REQ:
		{
			rrc_mac_connnection_setup *req = (rrc_mac_connnection_setup *)message_ptr(msg);

			LOG_INFO(MAC, "RRC_MAC_CONNECT_SETUP_CFG_REQ, mode:%u, ue_index:%u, maxHARQ_Tx:%u", 
				req->mode, req->ue_index,req->maxHARQ_Tx);

			mac_user_setup(req);
			break;
		}
		case RRC_MAC_BCCH_PARA_CFG_REQ:
		{
			rrc_mac_bcch_para_config_req *req = (rrc_mac_bcch_para_config_req *)message_ptr(msg);
			
			LOG_INFO(MAC, "RRC_MAC_BCCH_PARA_CFG_REQ, flag:%u", req->flag);

			rrc_mac_bcch_req(req);
			break;
		}
		case RRC_MAC_BCCH_SIB1_REQ:
		{
			//rrc_mac_bcch_s *req = (rrc_mac_bcch_para_config_req *)message_ptr(msg);
			
			LOG_INFO(MAC, "RRC_MAC_BCCH_SIB1_REQ");

			//rrc_mac_sib_update(req);
			break;
		}
		default:
		{
			LOG_WARN(MAC, "unknown rrc msg:msgId:%u",msg_id);
			break;
		}
	}

}

