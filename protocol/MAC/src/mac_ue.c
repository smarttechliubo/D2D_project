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

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

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

rnti_t new_rnti(const uint16_t cellId, const uint16_t ueIndex)
{
	return (cellId + 1) * 1000 + ueIndex;
}

ueInfo* createPtr()
{
	ueInfo* node = (ueInfo*)malloc(sizeof(ueInfo));
	if (node == NULL)
	{
		LOG_ERROR(MAC, "memory alloc for new ue fail!");
		return NULL;
	}

	return node;
}
bool remove_ue(uint16_t ueIndex)
{
	ueInfo* ue;

	if (ueIndex >= MAX_UE)
	{
		LOG_ERROR(MAC, "remove ue fail, Invalid ueIndex:%u", ueIndex);
		return false;
	}

	ue = &g_context.mac->ue[ueIndex];
	release_index(ue->ueIndex);
	ue->active = false;
	ue->ueIndex = INVALID_U16;
	free(ue);

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
			free(ue);
		}
	}
	return ue;
}

uint16_t find_ue(const rnti_t rnti)
{
	ueInfo ue;
	uint16_t ueIndex = INVALID_U16;

	for(uint32_t i = 0; i < MAX_UE; i++)
	{
		ue = g_context.mac->ue[i];

		if(ue.active == true &&
			ue.rnti == rnti)
		{
			return ue.ueIndex;
		}
	}

	if (ueIndex == INVALID_U16)
	{
		LOG_ERROR(MAC, "ue rnti:%u does not exist!", rnti);
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

void mac_user_setup_cfm(const rrc_mac_connnection_setup *req, const bool result, const rnti_t rnti)
{
	msgDef msg;
	mac_rrc_connection_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_connection_cfm);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RRC_INITIAL_CFM;
		msg.header.source = MAC_TASK;
		msg.header.destination = RRC_TASK;
		msg.header.msgSize = msg_size;

		cfm = (mac_rrc_connection_cfm*)msg.data;
		cfm->ue_index = req->ue_index;
		cfm->rnti = rnti;
		cfm->status = result;
		cfm->error_code = INVALID_U16;

		if (msgSend(RRC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
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
	mac_info_s *mac = g_context.mac;
	ueInfo *ue = NULL;
	uint16_t ueIndex = INVALID_U16;
	rnti_t rnti = INVALID_U16;
	uint16_t cellId = mac->cellId;
	bool result = false;

	ueIndex = new_index();

	if(g_context.mac->count_ue >= MAX_UE || ueIndex >= MAX_UE)
	{
		LOG_ERROR(MAC, "mac user setup fail!");
	}
	else
	{
		ue = new_ue(cellId, ueIndex);

		if (ue != NULL)
		{
			ue = &mac->ue[ueIndex];
			ue->active = true;
			ue->ueId = req->ue_index;
			ue->ueIndex = ueIndex;

			ue->maxHARQ_Tx = req->maxHARQ_Tx;
			ue->max_out_sync = req->max_out_sync;
			ue->lc_num = req->logical_channel_num;

			for(uint32_t i = 0; i < req->logical_channel_num; i++)
			{
				ue->lc_config[i].lc_id = req->logical_channel_config[i].logical_channel_id;
				ue->lc_config[i].priority = req->logical_channel_config[i].priority;
			}

			rnti = ue->rnti;
			result = true;
			g_context.mac->count_ue++;

			//ue_push_back(ue);
		}
		else
		{
			release_index(ueIndex);
		}
	}

	mac_user_setup_cfm(req, result, rnti);
}

void mac_release_cfm(const rrc_mac_release_req *req, bool success)
{
	msgDef msg;
	mac_rrc_release_cfm *cfm;
	msgSize msg_size = sizeof(mac_rrc_release_cfm);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RRC_INITIAL_CFM;
		msg.header.source = MAC_TASK;
		msg.header.destination = RRC_TASK;
		msg.header.msgSize = msg_size;

		cfm = (mac_rrc_release_cfm*)msg.data;
		cfm->status = 1;
		cfm->error_code = success;

		if (msgSend(RRC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
		}
	}
	else
	{
		LOG_ERROR(MAC, "release user, new mac message fail!");
	}
}

void mac_user_release(const rrc_mac_release_req *req)//TODO: mac reset ue release
{
	uint16_t ueIndex;
	bool ret;

	ueIndex = find_ue_by_ueId(req->ue_index);
	ret = remove_ue(ueIndex);

	mac_release_cfm(req, ret);
}

void mac_rrc_status_report(const rnti_t rnti, bool status)
{
	msgDef msg;
	mac_rrc_outsync_rpt *rpt;
	msgSize msg_size = sizeof(mac_rrc_outsync_rpt);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RRC_INITIAL_CFM;
		msg.header.source = MAC_TASK;
		msg.header.destination = RRC_TASK;
		msg.header.msgSize = msg_size;

		rpt = (mac_rrc_outsync_rpt*)msg.data;
		rpt->rnti = rnti;
		rpt->outsync_flag = status;

		if (msgSend(RRC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{

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

bool update_crc_result(rnti_t rnti, uint16_t crc)
{
	uint16_t ueIndex = find_ue(rnti);
	ueInfo* ue = NULL;
	crc_result_e c = ECRC_NULL;

	if (ueIndex == INVALID_U16)
	{
		LOG_ERROR(MAC, "update cqi fail, no such ue rnti:%u", rnti);
		return false;
	}

	c = (crc == 0) ? ECRC_NACK : ECRC_ACK;

	ue = &g_context.mac->ue[ueIndex];
	ue->sch_info.crc = c;

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

	ue = &g_context.mac->ue[ueIndex];

	ue->sch_info.cqi = cqi;
	ue->sch_info.mcs = cqi_to_mcs(cqi);

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

	ue = &g_context.mac->ue[ueIndex];

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

	ue = &g_context.mac->ue[ueIndex];

	if (status == 0) //outOfSync
	{
		ue->out_sync_count++;

		if (!get_ue_status(ueIndex))
		{
			set_ue_status(ueIndex, true);
		}

		if (ue->out_sync_count >= ue->max_out_sync)
		{
			mac_rrc_status_report(ue->rnti, true);
		}
	}
	else if (status == 1)// inSync
	{
		if (get_ue_status(ueIndex))
		{
			ue->out_sync_count = 0;
			set_ue_status(ueIndex, false);
			mac_rrc_status_report(ue->rnti, false);
		}
	}
}

