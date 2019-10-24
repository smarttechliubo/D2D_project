/**********************************************************
* @file common_channel.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/08/05
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "log.h"
#include "smac_info.h"
#include "smac_schedule_result.h"
#include "mac_vars.h"
#include "mac_cce.h"
#include "interface_mac_phy.h"
#include "d2d_message_type.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

bool send_pbch_msg(const frame_t frame, const sub_frame_t subframe, const common_channel_s *common_channel)
{
	msgDef msg;
	PHY_PBCHSendReq *cfm;
	msgSize msg_size = sizeof(PHY_PBCHSendReq);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_PHY_PBCH_TX_REQ;
		msg.header.source = MAC_TASK;
		msg.header.destination = PHY_TASK;
		msg.header.msgSize = msg_size;

		cfm = (PHY_PBCHSendReq*)msg.data;
		cfm->frame = frame;
		cfm->subframe = subframe;

		if (msgSend(PHY_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
			return true;
		}
	}
	else
	{
		LOG_ERROR(MAC, "PBCH, new mac message fail!");
		return false;
	}

	return false;
}

// TODO: common msg should be constructed here? or construct it at the end of scheduling?
void handle_common_result(const frame_t frame, const sub_frame_t subframe, tx_req_info *tx_info)
{
	g_sch.common = *tx_info;
}

void schedule_mib(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	common_channel_s *common_channel = &mac->common_channel;
	uint8_t sfn = (uint8_t)((frame>>2)&0xFF);
	uint8_t rb_num = (uint8_t)(common_channel->bch_info.rb_num);
	uint8_t pdcch_index = (uint8_t)(common_channel->bch_info.rb_start_index);

	uint8_t pdcch = (rb_num<<4) | (pdcch_index<<1);
	
	//sfn: 8bit, pdcch: 7bit, padding: 9bit
	common_channel->mib_pdu[0] = sfn;
	common_channel->mib_pdu[1] = pdcch&0xFE;
	common_channel->mib_pdu[2] = 0;

	if (!send_pbch_msg(frame, subframe, common_channel))
	{
		LOG_ERROR(MAC, "send pbch fail! frame:%u, subframe:%u", frame, subframe);
	}
}

void schedule_sib(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac, tx_req_info *tx_info)
{
	common_channel_s *common_channel = &mac->common_channel;
	uint16_t bandwith = mac->bandwith;
	uint16_t dci_num = tx_info->dci_num;
	uint16_t sch_num = tx_info->sch_num;

	uint16_t sib_len = common_channel->sib_size;
	uint32_t rb_max = get_rb_num(bandwith);
	uint16_t rb_start_index = get_rb_start(bandwith);
	uint32_t rbg_size = get_rbg_size(bandwith);
	uint32_t rbs_req = 0;
	uint16_t aggregation_level = get_aggregation_level(bandwith, EFORMAT0, 2);
	int32_t cce_offset = -1;

	uint8_t mcs = 2; // for sib
	uint32_t tbs = get_tbs(mcs, rbg_size);
	
	while (tbs < sib_len)
	{
		rbs_req += rbg_size;
	
		if (rbs_req > mac->max_rbs_per_ue || rbs_req > rb_max) 
		{
			rbs_req = MIN(mac->max_rbs_per_ue,rb_max);
			tbs = get_tbs(mcs, rbs_req);
			break;
		}
		tbs = get_tbs(mcs, rbs_req);
	}

	if (tbs >= sib_len)
	{
		tx_info->sch[sch_num].rnti = SI_RNTI;
		tx_info->sch[sch_num].ueIndex = INVALID_U16;

		tx_info->sch[sch_num].rb_start = rb_start_index;
		tx_info->sch[sch_num].rb_num = rbs_req;
		tx_info->sch[sch_num].mcs = mcs;
		tx_info->sch[sch_num].data_ind = 2;
		tx_info->sch[sch_num].modulation = 2;//QPSK
		tx_info->sch[sch_num].rv = 0;
		tx_info->sch[sch_num].harqId = INVALID_U8;
		tx_info->sch[sch_num].ack = INVALID_U8;
		tx_info->sch[sch_num].pdu_len = sib_len;
		tx_info->sch[sch_num].data = common_channel->sib_pdu;
		tx_info->sch_num++;
	}
	else
	{
		LOG_WARN(MAC, "No RB Resoure for SIB SFN:%u", frame*4+subframe);
		return;
	}

	if (tbs == sib_len)
	{
		tx_info->sch[sch_num].padding_len = 0;
	}
	else
	{
		tx_info->sch[sch_num].padding_len = tbs - sib_len;
	}

	cce_offset = allocate_CCE(aggregation_level);

	if (cce_offset >= 0)
	{
		tx_info->dci[dci_num].rnti = SI_RNTI;
		tx_info->dci[dci_num].ueIndex = INVALID_U16;

		tx_info->dci[dci_num].cce_rb_num = aggregation_level;
		tx_info->dci[dci_num].cce_rb = cce_offset;
		tx_info->dci[dci_num].rb_num = rbs_req;
		tx_info->dci[dci_num].rb_start = rb_start_index;
		tx_info->dci[dci_num].mcs = mcs;
		tx_info->dci[dci_num].data_ind = 2;
		tx_info->dci[dci_num].ndi = 0;
		tx_info->dci[dci_num].rv = 0;
		tx_info->dci_num++;
	}
	else
	{	
		for (uint32_t i = rb_start_index; i < rbs_req; i++)
		{
			mac->rb_available[i] = 1;
		}
		tx_info->sch_num--;

		LOG_WARN(MAC, "No CCE Resoure for SIB SFN:%u", frame*4+subframe);
	}
}

void schedule_common(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	tx_req_info tx_info;

	if ((mac->common_channel.mib_size > 0) && ((subframe == 0) && (frame % 4) == 0))
	{
		schedule_mib(frame, subframe, mac);
	}

	if ((mac->common_channel.sib_size > 0) && ((subframe == 1) && (frame % 2) == 0))
	{
		schedule_sib(frame, subframe, mac, &tx_info);
		handle_common_result(frame, subframe, &tx_info);
	}
}

