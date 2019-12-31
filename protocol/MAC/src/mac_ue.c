/**********************************************************
* @file mac_ue.c
* 
* @brief  	define ue info
* @author   guicheng.liu
* @date     2019/08/29
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "mac_defs.h"
#include "mac_vars.h"
#include "interface_rrc_mac.h"
#include "d2d_message_type.h"
#include "log.h"
#include "ue_index.h"
#include "mac_osp_interface.h"
#include "mac_ra.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_handler.h"

bool get_ue_status(const uint16_t ueIndex)
{
	if (ueIndex >= MAX_UE)
		return false;

	return g_context.mac->ue[ueIndex].out_of_sync;
}

void set_ue_status(const uint16_t ueIndex, const uint16_t status)
{
	if (ueIndex >= MAX_UE)
		return;
	g_context.mac->ue[ueIndex].out_of_sync = status;
}


void remove_temp_ue(const rnti_t rnti)
{
	ueInfo* ue;

	for(uint32_t i = 0; i < MAX_UE; i++)
	{
		ue = &g_sch_mac->ue[i];

		if(ue->active == true &&
			ue->rnti == rnti &&
			ue->temp == true)
		{
			ue->active = false;
			ue->temp = false;
			break;
		}
	}
}

rnti_t new_rnti(const uint16_t cellId, const uint16_t ueIndex)
{
	return (cellId + 1) * 1000 + ueIndex;
}

ueInfo* createPtr()
{
	ueInfo* node = (ueInfo*)mem_alloc(sizeof(ueInfo));

	if (node == NULL)
	{
		LOG_ERROR(MAC, "memory alloc for new ue fail!");
		return NULL;
	}

	return node;
}

bool remove_ue(const uint16_t  ueIndex, const uint32_t mode)
{
	ueInfo* ue = ((mode == 0) ? &g_context.mac->ue[ueIndex] : &g_context.macd->ue[ueIndex]);

	release_index(ue->ueIndex, mode);

	ue->active = false;
	ue->ueIndex = INVALID_U16;
	ue->temp = false;

	return true;
}

ueInfo* new_ue(const uint16_t cellId, const uint16_t ueIndex)
{	
	uint16_t rnti;
	ueInfo* ue = createPtr();

	if (ue != NULL)
	{
		rnti = new_rnti(cellId, ueIndex);

		if (ueIndex != INVALID_U16 && ueIndex < MAX_UE)
		{
			ue->rnti = rnti;
		}
		else
		{
			mem_free((char*)ue);
		}
	}
	return ue;
}

bool is_temp_ue(uint16_t ueIndex)
{
	if (ueIndex >= MAX_UE)
	{
		LOG_ERROR(MAC, "is_temp_ue");
		return false;
	}

	return g_sch_mac->ue[ueIndex].temp;
}

uint16_t find_ue(const rnti_t rnti)
{
	ueInfo* ue;
	uint16_t ueIndex = INVALID_U16;

	for(uint32_t i = 0; i < MAX_UE; i++)
	{
		ue = &g_sch_mac->ue[i];

		if(ue->active == true &&
			ue->rnti == rnti)
		{
			return ue->ueIndex;
		}
	}

	if (ueIndex == INVALID_U16)
	{
		LOG_WARN(MAC, "ue rnti:%u does not exist!", rnti);
	}

	return ueIndex;
}

uint16_t find_ue_by_ueId(const uint16_t ue_index)
{
	ueInfo ue;
	uint16_t ueIndex = INVALID_U16;

	for(uint32_t i = 0; i < MAX_UE; i++)
	{
		ue = g_context.mac->ue[i];

		if(ue.active == true &&
			ue.ueId == ue_index)
		{
			return ue.ueIndex;
		}
	}

	if (ueIndex == INVALID_U16)
	{
		LOG_ERROR(MAC, "ue ueId:%u does not exist!", ue_index);
	}

	return ueIndex;
}

ueInfo* add_new_ue(rnti_t rnti, const uint16_t ueId, const uint16_t mode)
{
	mac_info_s *mac = (mode == 0) ? g_context.mac : g_context.macd;
	ueInfo *ue = NULL;
	uint16_t cellId = mac->cellId;
	uint16_t ueIndex = (mode == 0) ? find_ue(rnti) : find_ue(RA_RNTI);

	if (ueIndex == INVALID_U16)
	{
		ueIndex = new_index(mode);
	}

	if(mac->count_ue >= MAX_UE || ueIndex >= MAX_UE)
	{
		LOG_ERROR(MAC, "mac user setup fail! ueIndex:%u",ueIndex );
	}
	else
	{
		ue = &mac->ue[ueIndex];

		if (ue->active == false || ue->temp == true)
		{
			ue->active = true;
			ue->temp   = false;
			ue->ueId = ueId;
			ue->rnti = ((mode == 0) ? new_rnti(cellId, ueIndex) : rnti);
			ue->ueIndex = ueIndex;

			mac->count_ue++;

			LOG_INFO(MAC, "add_new_ue, mac mode:%u, ue rnti:%u, ueId:%u",mode, ue->rnti, ueId);
		}
		else
		{
			LOG_ERROR(MAC, "add_new_ue ue already exist, ueIndex:%u",ueIndex);
			release_index(ueIndex, mode);

			return NULL;
		}
	}

	return ue;
}

void add_temp_ue(const rnti_t rnti)
{
	ueInfo *ue = NULL;
	uint16_t mode = 1;

	ue = add_new_ue(rnti, INVALID_U16, mode);

	if (ue != NULL)
	{
		ue->temp = true;
	}
}

void mac_user_setup_cfm(const rrc_mac_connnection_setup *req, const bool result, const rnti_t rnti)
{
	msgDef* msg = NULL;
	mac_rrc_connection_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_connection_cfm);

	msg = new_message(MAC_RRC_CONNECT_SETUP_CFG_CFM, TASK_D2D_MAC, TASK_D2D_RRC, msg_size);

	if (msg != NULL)
	{
		cfm = (mac_rrc_connection_cfm*)message_ptr(msg);
		cfm->ue_index = req->ue_index;
		cfm->rnti = rnti;
		cfm->status = result;
		cfm->error_code = INVALID_U16;

		if (message_send(TASK_D2D_RRC, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_RRC_CONNECT_SETUP_CFG_CFM send");
		}

		//msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "user setup, new mac message fail!");
	}
}

void mac_user_setup(const rrc_mac_connnection_setup *req)
{
	ueInfo *ue = NULL;
	rnti_t rnti = INVALID_U16;
	bool result = false;

	ue = add_new_ue(req->rnti, req->ue_index, req->mode);

	if (ue != NULL)
	{
		ue->maxHARQ_Tx = req->maxHARQ_Tx;
		ue->max_out_sync = req->max_out_sync;
		ue->lc_num = 1;// CCCH is default channel
		
		ue->lc_config[0].lc_id = 0;
		ue->lc_config[0].priority = 16;
		
		for(uint32_t i = 0; i < req->logical_channel_num; i++)
		{
			ue->lc_config[ue->lc_num+i].lc_id = req->logical_channel_config[i].logical_channel_id;
			ue->lc_config[ue->lc_num+i].priority = req->logical_channel_config[i].priority;
		}
		
		ue->lc_num += req->logical_channel_num;

		ue->sch_info.mcs = 2;//TODO: default MCS

		rnti = ue->rnti;
		result = true;
	}
	else
	{
		result = false;
	}

	mac_user_setup_cfm(req, result, rnti);
}

void mac_release_cfm(const rrc_mac_release_req *req, bool success)
{
	msgDef* msg = NULL;
	mac_rrc_release_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_release_cfm);

	msg = new_message(MAC_RRC_RELEASE_CFM, TASK_D2D_MAC, TASK_D2D_RRC, msg_size);

	if (msg != NULL)
	{
		cfm = (mac_rrc_release_cfm*)message_ptr(msg);
		cfm->status = 1;
		cfm->error_code = success;

		if (message_send(TASK_D2D_RRC, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_RRC_RELEASE_CFM send");
		}
	}
	else
	{
		LOG_ERROR(MAC, "release user, new mac message fail!");
	}
}

void mac_user_release(const rrc_mac_release_req *req)//TODO: mac reset ue release
{
	//uint16_t ueIndex;
	uint32_t mode = 0;
	bool ret;
	bool find = false;

	ueInfo* ue = NULL;

	for(uint32_t i = 0; i < MAX_UE; i++)
	{
		ue = &g_context.mac->ue[i];

		if(ue->active == true &&
			ue->ueId == req->ue_index)
		{
			find = true;
			mode = 0;
			break;
		}

		ue = &g_context.macd->ue[i];

		if(ue->active == true &&
			ue->ueId == req->ue_index)
		{
			find = true;
			mode = 1;
			break;;
		}
	}

	if (find == false)
	{
		LOG_ERROR(MAC, "ue ueId:%u does not exist!", req->ue_index);
		ret = false;
	}
	else
	{
		//ueIndex = find_ue_by_ueId(req->ue_index);
		ret = remove_ue(ue->ueIndex, mode);
	}

	mac_release_cfm(req, ret);
}

void mac_rrc_status_report(const rnti_t rnti, bool status)
{
	msgDef* msg = NULL;
	mac_rrc_outsync_rpt *rpt;
	msgSize msg_size = sizeof(mac_rrc_outsync_rpt);

	msg = new_message(MAC_RRC_OUTSYNC_RPT, TASK_D2D_MAC_SCH, TASK_D2D_RRC, msg_size);

	if (msg != NULL)
	{
		rpt = (mac_rrc_outsync_rpt*)message_ptr(msg);
		rpt->rnti = rnti;
		rpt->outsync_flag = status;

		if (message_send(TASK_D2D_RRC, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_RRC_OUTSYNC_RPT send");
		}
	}
	else
	{
		LOG_ERROR(MAC, "status report, new mac message fail!");
	}
}

//for future using, get csi info
void get_csi_paras(const uint16_t ueIndex, uint16_t* cqi_periodic)
{
	//ueInfo* ue = &g_context.mac->ue[ueIndex];
}
/*
bool update_temp_ue_crc_result(const sub_frame_t subframe, const rnti_t rnti, const uint16_t crc)
{
	if (rnti != RA_RNTI)
	{
		LOG_ERROR(MAC, "temp_ue, rnti:%u", rnti);
		return false;
	}

	if (crc == 0)
	{
		add_temp_ue(rnti);
	}
	else if (crc == 1)
	{
		remove_temp_ue(rnti);
	}

	return true;
}
*/

bool update_crc_result(const sub_frame_t subframe, const rnti_t rnti, const uint16_t crc)
{
	ueInfo* ue = NULL;
	crc_e c = ECRC_NULL;
	uint8_t harqId = get_harqId(subframe);
	uint16_t ueIndex = INVALID_U16;

	if ((ueIndex = find_ue(rnti)) < MAX_UE)
	{
		ue = &g_sch_mac->ue[ueIndex];
	}
	else if ((ueIndex = find_ue(RA_RNTI)) < MAX_UE)
	{
		// rrc user setup(msg2) received at destination
		if (crc == 1)
		{
			remove_ra(RA_RNTI);
		}

		ue = &g_sch_mac->ue[ueIndex];

		ue->rnti = rnti;
	}
	else
	{
		LOG_ERROR(MAC, "update crc fail, no such ue");
		return false;
	}

	if (crc == 0)
	{
		c = ECRC_NACK;
	}
	else if (crc == 1)
	{
		c = ECRC_ACK;
	}

	ue->sch_info.crc[harqId] = c;

	LOG_INFO(MAC, "update_crc_result, rnti:%u, crc:%u, harqId:%u", rnti, crc, harqId);

	return true;
}

bool update_ue_cqi(const rnti_t rnti, const uint16_t cqi)
{
	uint16_t ueIndex = find_ue(rnti);
	ueInfo* ue = NULL;
	uint16_t cqi_periodic = 0;

	if (ueIndex == INVALID_U16)
	{
		LOG_ERROR(MAC, "update cqi fail, no such ue rnti:%u", rnti);
		return false;
	}

	get_csi_paras(ueIndex, &cqi_periodic);

	ue = &g_sch_mac->ue[ueIndex];

	ue->sch_info.cqi = cqi;
	ue->sch_info.mcs = cqi_to_mcs(cqi);

	ue->sch_info.mcs = 15;// TODO: PHY should do mcs selecting

	return true;
}

void update_harq_info(const sub_frame_t subframe, const rnti_t rnti, const uint16_t ack)
{
	uint16_t ueIndex = find_ue(rnti);
	ueInfo* ue = NULL;
	uint8_t harqId = get_harqId((subframe + MAX_SUBSFN - 2)%4);

	if (ueIndex == INVALID_U16)
	{
		LOG_ERROR(MAC, "update harq fail, no such ue rnti:%u", rnti);
		return;
	}

	ue = &g_sch_mac->ue[ueIndex];

	if (ack == 0)//nack
	{
		if (ue->harq[harqId].reTx_num >= ue->maxHARQ_Tx)
		{
			//release harq
			ue->harq[harqId].reTx = false;
			ue->harq[harqId].reTx_num = 0;
		}
		else
		{
			ue->harq[harqId].reTx = true;
			ue->harq[harqId].reTx_num++;
		}
	}

	if (ack == 1)
	{
		//release harq
		ue->harq[harqId].reTx = false;
		ue->harq[harqId].reTx_num = 0;
	}
}

void update_ue_status(const rnti_t rnti, const uint16_t status)
{
	uint16_t ueIndex = find_ue(rnti);
	ueInfo* ue = NULL;

	if (ueIndex == INVALID_U16)
	{
		LOG_ERROR(MAC, "update status fail, no such ue rnti:%u", rnti);
		return;
	}

	ue = &g_sch_mac->ue[ueIndex];

	if (status == 0) //outOfSync
	{
		ue->out_sync_count++;

		if (!ue->out_of_sync)
		{
			ue->out_of_sync = true;
		}

		if (ue->out_sync_count >= ue->max_out_sync)
		{
			mac_rrc_status_report(ue->rnti, true);
		}
	}
	else if (status == 1)// inSync
	{
		if (ue->out_of_sync == true)
		{
			ue->out_sync_count = 0;
			ue->out_of_sync = false;
			mac_rrc_status_report(ue->rnti, false);
		}
	}
}

