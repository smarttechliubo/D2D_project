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
void remove_ue(uint16_t ueIndex)
{
	ueContext* ueContext;

	if (ueIndex >= MAX_UE)
	{
		LOG_ERROR(MAC, "remove ue fail, Invalid ueIndex:%u", ueIndex);
		return;
	}

	ueContext = &g_context.mac->ue[ueIndex];
	ueContext->active = false;
	ueContext->ueIndex = INVALID_U16;
	free(ueContext->ue);
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
	ueContext ueContext;
	uint16_t ueIndex = INVALID_U16;

	for(uint32_t i = 0; i < MAX_UE; i++)
	{
		ueContext = g_context.mac->ue[i];

		if(ueContext.ue != NULL &&
			ueContext.active == true &&
			ueContext.ue->rnti == rnti)
		{
			return ueContext.ueIndex;
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
	ueContext ueContext;
	uint16_t ueIndex = INVALID_U16;

	for(uint32_t i = 0; i < MAX_UE; i++)
	{
		ueContext = g_context.mac->ue[i];

		if(ueContext.ue != NULL &&
			ueContext.active == true &&
			ueContext.ueId == ue_index)
		{
			return ueContext.ueIndex;
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
	ueContext *ueContext = NULL;
	ueInfo *ue = NULL;
	uint16_t ueIndex = INVALID_U16;
	rnti_t rnti = INVALID_U16;
	uint16_t cellId = mac->cellId;
	bool result = false;

	ueIndex = new_index();

	if(g_context.mac->num_ue >= MAX_UE || ueIndex >= MAX_UE)
	{
		LOG_ERROR(MAC, "mac user setup fail!");
	}
	else
	{
		ue = new_ue(cellId, ueIndex);

		if (ue != NULL)
		{
			ueContext = &mac->ue[ueIndex];
			ueContext->active = true;
			ueContext->ueId = req->ue_index;
			ueContext->ueIndex = ueIndex;
			ueContext->ue = ue;

			ue->maxHARQ_Tx = req->maxHARQ_Tx;
			ue->max_out_sync = req->max_out_sync;

			rnti = ue->rnti;
			result = true;
			g_context.mac->num_ue++;

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
	
	ueIndex = find_ue_by_ueId(req->ue_index);
	remove_ue(ueIndex);
	mac_release_cfm(req, true);
}

