/**********************************************************
* @file rrc_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/11/07
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "rrc_sim.h"
#include "mytask.h"
#include "messageDefine.h"
#include "interface_rrc_mac.h"
#include "interface_rrc_rlc.h"
#include "d2d_message_type.h"
#include "log.h"
#include "msg_handler.h"

rrc_info g_rrc_dst;
uint16_t g_ueId = 0;


void init_rrc_dst_sim()
{
	g_rrc_dst.cellId = 0;
	g_rrc_dst.mode = 1;//source
	g_rrc_dst.status = ERRC_NONE;
	g_rrc_dst.num_ue = 0;

	for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		g_rrc_dst.ue[i].status = ERRC_UE_INVALID;
	}
}

rrc_ue_info* find_dst_user(const uint16_t ueId)
{
	rrc_ue_info* ue = &g_rrc_dst.ue[0];

	for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rrc_dst.ue[i];

		if (ue->ueId == ueId)
		{
			return ue;
		}
	}

	return NULL;
}

void remove_dst_user(rrc_ue_info* ue)
{
	ue->ueId = INVALID_U16;
	ue->rnti = INVALID_U16;
	ue->status = ERRC_UE_INVALID;
	ue->setup_timer = 0;
}

bool dst_add_new_user(const uint16_t ueId, const rnti_t rnti)
{
	rrc_ue_info* ue = NULL;

	for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rrc_dst.ue[i];

		if (ue->status == ERRC_UE_INVALID)
		{
			ue->ueId = ueId;
			ue->rnti = rnti;
			ue->mode = g_rrc_dst.mode;
			ue->setup_timer = 0;
			ue->status = ERRC_UE_SETUP;

			return true;
		}
	}

	return false;
}

void dst_user_setup_complete(const uint16_t ueId, const rnti_t rnti, const uint16_t flag, const uint16_t cause)
{
	msgDef* msg = NULL;
	rrc_rlc_data_ind *ind;
	msgSize msg_size = sizeof(rrc_rlc_data_ind);
	uint16_t data_size = sizeof(ccch_info);
	ccch_info* ccch = (ccch_info *)malloc(data_size);

	msg = new_message(RRC_RLC_DATA_IND, TASK_D2D_RRC, TASK_D2D_RLC, msg_size);

	if (msg != NULL)
	{
		ind = (rrc_rlc_data_ind*)message_ptr(msg);
		ind->rb_type = RB_TYPE_SRB0;
		ind->data_size = data_size;
		ind->data_addr_ptr = (uint32_t*)ccch;
		ccch->flag = flag;
		ccch->cause = cause;
		ccch->ueId = ueId;
		ccch->rnti = rnti;

		if (message_send(TASK_D2D_RLC, msg, sizeof(msgDef)))
		{
			LOG_INFO(RRC, "LGC: rrc_rlc_data_ind send");
		}
	}
	else
	{
		LOG_ERROR(RRC, "[TEST]: src_user_setup new rrc message fail!");
	}

}

void dst_user_setup(const uint16_t       ueId, const rnti_t rnti, const uint16_t cause)
{
	msgDef* msg = NULL;
	rrc_mac_connnection_setup *setup;
	msgSize msg_size = sizeof(rrc_mac_connnection_setup);

	uint16_t flag = 2; // 0:setup req, 1:setup, 2:setup complete, 

	rrc_ue_info* ue = find_dst_user(ueId);

	if (ue == NULL)
	{
		dst_user_setup_complete(ueId, rnti, flag, 0);
		LOG_WARN(RRC, "dst ue:%u does not exist", ueId);
		return;
	}

    if (cause == 0)
    {
		remove_dst_user(ue);
	}

	ue->setup_timer = 0;
	ue->rnti = rnti;

	msg = new_message(RRC_MAC_CONNECT_SETUP_CFG_REQ, TASK_D2D_RRC, TASK_D2D_MAC, msg_size);

	if (msg != NULL)
	{
		setup = (rrc_mac_connnection_setup*)message_ptr(msg);
		setup->ue_index = ue->ueId;
		setup->maxHARQ_Tx = 4;
		setup->max_out_sync = 4;
		setup->logical_channel_num = 1;
		setup->logical_channel_config[0].chan_type = DTCH;
		setup->logical_channel_config[0].priority = 15;
		setup->logical_channel_config[0].logical_channel_id = 1;

		if (message_send(TASK_D2D_MAC, msg, sizeof(msgDef)))
		{
			LOG_INFO(RRC, "LGC: RRC_MAC_CONNECT_SETUP_CFG_REQ, send msg!");
		}
	}
	
}

void dst_user_setup_req(const uint16_t ueId, const uint16_t flag)
{
	msgDef* msg = NULL;
	rrc_rlc_data_ind *ind;
	msgSize msg_size = sizeof(rrc_rlc_data_ind);
	uint16_t data_size = sizeof(ccch_info);
	ccch_info* ccch = (ccch_info *)malloc(data_size);

	if (!dst_add_new_user(ueId, INVALID_U16))
	{
		LOG_ERROR(MAC, "src user setup fail");
		return;
	}

	{
		msg = new_message(RRC_RLC_DATA_IND, TASK_D2D_RRC, TASK_D2D_RLC, msg_size);

		if (msg != NULL)
		{
			ind = (rrc_rlc_data_ind*)message_ptr(msg);
			ind->rb_type = RB_TYPE_SRB0;
			ind->data_size = data_size;
			ind->data_addr_ptr = (uint32_t*)ccch;
			ccch->flag = flag;
			ccch->ueId = ueId;
			ccch->rnti = INVALID_U16;

			if (message_send(TASK_D2D_RLC, msg, sizeof(msgDef)))
			{
				LOG_INFO(RRC, "LGC: rrc_rlc_data_ind send");
			}
		}
		else
		{
			LOG_ERROR(RRC, "[TEST]: src_user_setup new rrc message fail!");
		}
	}
}

void dst_user_setup_cfm(const mac_rrc_connection_cfm *cfm)
{
	uint16_t ueId = cfm->ue_index;
	rnti_t rnti = cfm->rnti;
	uint16_t  status = cfm->status;
	rrc_ue_info* ue = NULL;
	uint16_t flag = 2; // 0:setup req, 1:setup, 2:setup complete, 
	uint16_t cause = 0;

	ue  = find_dst_user(ueId);

	if (ue == NULL)
	{
		LOG_WARN(RRC, "src ue:%u does not exist", ueId);
		cause = 0;
	}

	else if (status == 0)
	{
		remove_dst_user(ue);

		LOG_WARN(RRC, "user setup fail, ueId:%u", ueId);
		cause = 0;
	}
	else
	{
		cause = 1;
		ue->setup_timer = 0;
	}

	dst_user_setup_complete(ueId, rnti, flag, cause);

}

void handle_ccch_rpt_dst(mac_rrc_ccch_rpt *rpt)
{
	frame_t frame = rpt->sfn;
	sub_frame_t subframe = rpt->subsfn;
	ccch_info* ccch = (ccch_info*)rpt->data_ptr;
	uint32_t flag = ccch->flag; // 0:setup req, 1:setup, 2:setup complete
	uint16_t cause = ccch->cause;
	uint16_t ueId = ccch->ueId;
	rnti_t rnti = ccch->rnti;

	LOG_INFO(RRC, "ccch msg received. frame:%u, subframe:%u, flag:%u", frame, subframe, flag);

	if (flag == 0)// 0:setup req
	{
		LOG_ERROR(RRC, "src_ueser, unexpect state");
	}
	else if (flag == 1)// 1:setup
	{
		dst_user_setup(ueId, rnti, cause);
	}
	else if (flag == 2)// 2:setup complete
	{
	}
	else
	{
		LOG_ERROR(RRC, "unknown ccch msg. flag:%u", flag);
	}
}

void rrcDstcStatusHandler()
{
	msgDef* msg = NULL;

	if (g_rrc_dst.status == ERRC_BCCH_CFM)
	{
		rrc_mac_initial_req *req;
		msgSize msg_size = sizeof(rrc_mac_initial_req);

		msg = new_message(RRC_MAC_INITIAL_REQ, TASK_D2D_RRC, TASK_D2D_MAC, msg_size);

		if (msg != NULL)
		{
			req = (rrc_mac_initial_req*)message_ptr(msg);
			req->cellId = 0;
			req->bandwith = 1;
			req->pdcch_config.rb_num = 2;
			req->pdcch_config.rb_start_index = 2;
			req->subframe_config = 0;
			req->mode = 0;

			if (message_send(TASK_D2D_MAC, msg, sizeof(msgDef)))
			{
				g_rrc_dst.status = ERRC_INITAIL;
				LOG_INFO(RRC, "LGC: rrc_mac_initial_req send");
			}

			//msg_free(msg);
		}
		else
		{
			LOG_ERROR(RRC, "[TEST]: new rrc message fail!");
		}
	}
/*
	if (g_rrc_dst.status == ERRC_INITAIL_CFM)
	{
		rrc_mac_bcch_para_config_req *req;
		msgSize msg_size = sizeof(rrc_mac_bcch_para_config_req);

		msg = new_message(RRC_MAC_BCCH_PARA_CFG_REQ, TASK_D2D_RRC, TASK_D2D_MAC, msg_size);

		if (msg != NULL)
		{
			uint8_t *sib_pdu = (uint8_t *)malloc(8);

			req = (rrc_mac_bcch_para_config_req*)message_ptr(msg);
			req->flag = 3;
			req->mib.systemFrameNumber = 0;
			req->mib.pdcch_config.rb_num = 2;
			req->mib.pdcch_config.rb_start_index = 2;
			req->sib.size = 8;
			req->sib.sib_pdu = sib_pdu;

			memset(req->sib.sib_pdu, 0xFE, req->sib.size);
			
			if (message_send(TASK_D2D_MAC, msg, sizeof(msgDef)))
			{
				LOG_INFO(RRC, "rrc_mac_bcch_para_config_req send");
			}
		}
		else
		{
			LOG_ERROR(RRC, "[TEST]: new rrc message fail!");
		}
	}
*/
}


void rrcDstUserStatusHandler()
{
	uint16_t ueId = 0;
	uint16_t num_ue = 0;
	rrc_ue_info* ue = NULL;
	uint16_t flag = 2; // 0:setup req, 1:setup, 2:setup complete, 
	uint16_t cause = 0;

	if (g_rrc_dst.status == ERRC_BCCH_CFM)
	{
		g_rrc_dst.status = ERRC_NONE;
		ueId = g_ueId++;

		dst_user_setup_req(ueId, 0);
	}

	num_ue = g_rrc_dst.num_ue;
	
	if (num_ue <= 0)
		return;

	for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rrc_dst.ue[i];

		if (ue->status != ERRC_UE_SETUP)
			continue;

		ue->setup_timer++;

		if (ue->setup_timer >= 10)
		{
			cause = 0;
			dst_user_setup_complete(ueId, ue->rnti, flag, cause);
			remove_dst_user(ue);
		}
	}
}

void rrcDstMsgHandler(msgDef* msg, const msgId msg_id)
{
	switch (msg_id)
	{
		case MAC_RRC_INITIAL_CFM:
		{
			mac_rrc_initial_cfm *cfm = (mac_rrc_initial_cfm *)message_ptr(msg);

			LOG_INFO(RRC, "[TEST] mac_rrc_initial_cfm, status:%u, error_:%u",cfm->status, cfm->error_code);

			g_rrc_dst.status = ERRC_INITAIL_CFM;
			break;
		}
		/*
		case MAC_RRC_BCCH_PARA_CFG_CFM:
		{
			mac_rrc_bcch_para_config_cfm *cfm = (mac_rrc_bcch_para_config_cfm *)message_ptr(msg);

			LOG_INFO(RRC, "[TEST] mac_rrc_bcch_para_config_cfm, status:%u,flag:%u,error:%u", 
				cfm->status,cfm->flag,cfm->error_code);

			g_rrc_dst.status = ERRC_BCCH_CFM;
			message_free(cfm);
			break;
		}
		*/
		case MAC_RRC_BCCH_MIB_RPT:
		{
			mac_rrc_bcch_mib_rpt *rpt = (mac_rrc_bcch_mib_rpt *)message_ptr(msg);

			LOG_INFO(RRC, "[TEST] mac_rrc_bcch_mib_rpt, SFN:%u,subsfn:%u", 
				rpt->SFN, rpt->subsfn);

			if (g_rrc_dst.status == ERRC_NONE)
			{
				g_rrc_dst.status = ERRC_BCCH_CFM;
			}

			break;
		}
		case MAC_RRC_BCCH_SIB1_RPT:
		{
			mac_rrc_bcch_sib1_rpt *rpt = (mac_rrc_bcch_sib1_rpt *)message_ptr(msg);

			LOG_INFO(RRC, "[TEST] mac_rrc_bcch_para_config_cfm, SFN:%u, subsfn:%u, data_size:%u", 
				rpt->sfn, rpt->subsfn, rpt->data_size);
			//g_rrc_dst.status = ERRC_BCCH_CFM;
			break;
		}
		case MAC_RRC_OUTSYNC_RPT:
		{
			mac_rrc_outsync_rpt *cfm = (mac_rrc_outsync_rpt *)message_ptr(msg);

			LOG_INFO(RRC, "[TEST] mac_rrc_outsync_rpt, rnti:%u, outsync_flag:%u", 
				cfm->rnti,cfm->outsync_flag);
			break;
		}
		case MAC_RRC_CCCH_RPT:
		{
			mac_rrc_ccch_rpt *rpt = (mac_rrc_ccch_rpt *)message_ptr(msg);

			LOG_INFO(RRC, "[TEST] mac_rrc_ccch_rpt");
			handle_ccch_rpt_dst(rpt);

			break;
		}
		case MAC_RRC_CONNECT_SETUP_CFG_CFM:
		{
			mac_rrc_connection_cfm *cfm = (mac_rrc_connection_cfm *)message_ptr(msg);

			LOG_INFO(RRC, "[TEST] mac_rrc_connection_cfm, status:%u,ue_index:%u,rnti:%u,error:%u",
				cfm->status,cfm->ue_index,cfm->rnti,cfm->error_code);

			dst_user_setup_cfm(cfm);

			break;
		}
		case MAC_RRC_RELEASE_CFM:
		{
			mac_rrc_release_cfm *cfm = (mac_rrc_release_cfm *)message_ptr(msg);

			LOG_INFO(RRC, "[TEST] mac_rrc_release_cfm, status:%u",cfm->status);
			break;
		}
		default:
		{
			break;
		}
	}
}


