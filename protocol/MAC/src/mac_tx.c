/**********************************************************
* @file mac_tx.c
* @brief  mac tx function
*
* @author	guicheng.liu
* @date 	2019/07/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#define _GNU_SOURCE
#include <stdlib.h>

#include "d2d_message_type.h"
#include "mac_defs.h"
#include "mac.h"
#include "mac_ue.h"
#include "mac_vars.h"
#include "mac_ra.h"
#include "log.h"
#include "interface_mac_rlc.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

void init_mac_tx(const uint16_t cellId)
{
	g_sch.cellId = cellId;
	g_sch.frame = INVALID_U32;
	g_sch.subframe = INVALID_U32;
}

void add_to_scheduling_list0(uint16_t ueIndex)
{
	if (ueIndex >= MAX_UE || g_context.mac->num0 >= MAX_UE)
	{
		LOG_ERROR(MAC, "add ue list fail, ueIndex:%u, sch_ue_num:%u", ueIndex, g_context.mac->num0);
		return;
	}
	g_context.mac->scheduling_list0[g_context.mac->num0++] = ueIndex;
}

void update_buffer_status(rlc_buffer_rpt buffer)
{
	rnti_t rnti = buffer.rnti;
	uint8_t logic_chan_num = buffer.logic_chan_num;
	//uint8_t logicchannel_id;
	//uint32_t buffer_byte_size;
	ueInfo* ue;
	txBuffer *ue_buffer;
	uint16_t ueIndex = INVALID_U16;

	if ((ueIndex = find_ue(rnti)) == INVALID_U16 || ueIndex >= MAX_UE)
	{
		LOG_ERROR(MAC, "ue rnti:%u does not exist!", rnti);
		return;
	}

	ue = &g_context.mac->ue[ueIndex];

	if (ue->active == false ||
		ue->out_of_sync)
	{
		LOG_WARN(MAC, "ue ueIndex:%u, rnti:%u is inactive?active:%u(1:active, 0:inactive)", ueIndex, rnti, ue->active);
		return;
	}

	ue_buffer = &ue->buffer;
	ue_buffer->chan_num = 0;
	ue_buffer->buffer_total = 0;

	ue_buffer->chan_num = logic_chan_num;

	for (uint32_t i = 0; i < logic_chan_num; i++)
	{
		ue_buffer->chan_id[i] = buffer.logicchannel_id[i];
		ue_buffer->buffer_size[i] = buffer.buffer_byte_size[i];
		ue_buffer->buffer_total += buffer.buffer_byte_size[i];
	}

	//if(ue_buffer->buffer_total > 0)
	//{
	//	add_to_scheduling_list(ueIndex);
	//}
}

void handle_buffer_status(const frame_t frame, const sub_frame_t subframe, const rlc_mac_buf_status_rpt *rpt)
{
	uint32_t ue_num = rpt->valid_ue_num; 
	rlc_buffer_rpt buffer;
	//uint16_t cellId = g_sch.cellId;
	//uint8_t logic_chan_num = 0;

	for (uint32_t i = 0; i < ue_num; i++)
	{
		buffer = rpt->rlc_buffer_rpt[i];
		//logic_chan_num = buffer.logic_chan_num;

		if (buffer.rnti == RA_RNTI && buffer.logicchannel_id[i] == CCCH_)// TODO: no data should be transmit before ue get connection setup.
		{
			update_ra_buffer(buffer);
			break;
		}

		if (buffer.valid_flag)
		{
			update_buffer_status(buffer);
		}
	}
}

void handle_buffer_status_ind(const frame_t frame, const sub_frame_t subframe)
{
	//MAC_TEST
	msgDef msg;
	uint32_t msg_len = 0;
	//while (1)
	{
		msg_len = msgRecv(RLC_MAC_QUEUE, (char *)&msg, MQ_MSGSIZE);

		if (msg_len == 0)
		{
			return;
		}

		switch (msg.header.msgId)
		{
			case RLC_MAC_BUF_STATUS_RPT:
			{
				rlc_mac_buf_status_rpt *rpt = (rlc_mac_buf_status_rpt *)msg.data;

				handle_buffer_status(frame, subframe, rpt);
				msg_free(rpt);
				break;
			}
			default:
			{
				LOG_ERROR(MAC, "Unknown RLC msg! msgId:%u", msg.header.msgId);
				break;
			}
		}
	}
}

int32_t sort_ue_high(const ueInfo *ue1, const ueInfo *ue2, const uint8_t harqId)
{
	
	if (ue1->harq[harqId].reTx_num > ue2->harq[harqId].reTx_num)
	{
		return -1;
	}
	else if (ue1->harq[harqId].reTx_num < ue2->harq[harqId].reTx_num)
	{
		return 1;
	}
	else if (ue1->harq[harqId].cqi > ue2->harq[harqId].cqi)
	{
		return -1;
	}
	else if (ue1->harq[harqId].cqi > ue2->harq[harqId].cqi)
	{
		return 1;
	}
	return 0;
}

int32_t sort_ue_low(ueInfo *ue1, ueInfo *ue2)
{

	return 0;
}

int32_t ue_compare(const void *_a, const void *_b, void *_p)
{
	int32_t ret = 0;
	mac_info_s *mac = g_context.mac;
	sort_params *params = _p;
	uint16_t ueIndex1 = *(const uint16_t *)_a;
	uint16_t ueIndex2 = *(const uint16_t *)_b;
	ueInfo ue1 = mac->ue[0];
	ueInfo ue2 = mac->ue[1];
	uint8_t harqId = get_harqId(mac->subframe);

	if (ueIndex1 >= MAX_UE || ueIndex2 >= MAX_UE)
	{
		LOG_ERROR(MAC, "ue compare, invalid ueIndex%u, %u", ueIndex1, ueIndex2);
		return 0;
	}

	ue1 = mac->ue[ueIndex1];
	ue2 = mac->ue[ueIndex2];

	switch (params->policy)
	{
		case ESORTING_HIGH:
		{
			ret = sort_ue_high(&ue1, &ue2, harqId);
			return ret;
		}
		case ESORTING_LOW:
		{
			ret = sort_ue_low(&ue1, &ue2);
			return ret;
		}
		default:
		{
			LOG_ERROR(MAC, "ue compare unsupport sort policy:%u", params->policy);
			break;
		}
	}
	
/*
	switch (params->pri)
	{
		case EPRI_RETX:
		{
			break;
		}
		case EPRI_CQI:
		{
			break;
		}
		default:
			break;
	}
*/
	return 0;
}

void sort_ues(uint16_t list[MAX_UE], uint16_t size)
{
	sort_params params = {ESORTING_HIGH};

	qsort_r(list, size, sizeof(uint16_t), ue_compare, &params);

}

void pre_assign_rbs()
{
	//uint16_t ueIndex = INVALID_U16;
	uint16_t mcs = 0;
	uint16_t cqi = 0;
	mac_info_s *mac = g_context.mac;
	uint8_t harqId = get_harqId(mac->subframe);
	ueInfo* ue = NULL;
	uint32_t rbg_size = get_rbg_size(mac->bandwith);
	uint32_t rb_max = get_rb_num(mac->bandwith);
	uint32_t rbs_req = 0;
	uint32_t tbs = get_tbs(mcs, rbg_size);

	for (uint32_t i = 0; i < MAX_UE; i++)
	{
		//ueIndex = mac->scheduling_list0[i];
		if (mac->ue[i].active == false ||
			mac->ue[i].out_of_sync == true)
		{
			LOG_WARN(MAC, "pre assign rbs fail for ueIndex:%u", i);
			continue;
		}

		ue = &mac->ue[i];
		ue->sch_info.pre_rbs_alloc = 0;

		if (ue->buffer.buffer_total > 0)
		{
			rbs_req = 0;
			cqi = ue->sch_info.cqi;
			mcs = cqi_to_mcs(cqi);
			mcs = ue->sch_info.mcs;
			rbs_req = (mcs == 0) ? (uint32_t)mac->min_rbs_per_ue : rbg_size;

			while (tbs < ue->buffer.buffer_total)
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

			ue->sch_info.pre_rbs_alloc = rbs_req;
		}

		if (ue->buffer.buffer_total > 0 || ue->harq[harqId].reTx)
		{
			add_to_scheduling_list0(i);
		}
	}
}

void pre_schedule_reset()
{
	g_context.mac->num0 = 0;
}

void scheduler_pre_handler()
{
	pre_schedule_reset();

	pre_assign_rbs();

}

void schedule_ue(const frame_t frame, const sub_frame_t subframe)
{
	uint16_t num0 = 0;
	uint16_t *scheduling_list0;
	mac_info_s *mac = g_context.mac;

	scheduler_pre_handler();
	num0 = mac->num0;
	scheduling_list0 = &mac->scheduling_list0[0];

	sort_ues(scheduling_list0, num0);
}

void mac_scheduler()
{
	frame_t frame = g_sch.frame;
	sub_frame_t subframe = g_sch.subframe;

	handle_buffer_status_ind(frame, subframe);

	schedule_ra(frame, subframe);

	schedule_ue(frame, subframe);
}
#if 0
void resource_alloc()
{
	mac_info_s *mac = g_context.mac;
	common_channel_s *common_channel = &mac->common_channel;
	mac_tx_req *tx_req = &g_sch.tx_req;
	uint32_t sib_len = common_channel->sib_size;
	uint32_t rb_max = get_rb_num(mac->bandwith);
	uint32_t rb_start_index = get_rb_start(mac->bandwith);
	uint32_t rbg_size = get_rbg_size(mac->bandwith);
	uint32_t rbs_req = 0;
	uint16_t aggregation_level = 2;
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
	} // end of while

	tx_req->tx_info[tx_req->num_tx].sch.rb_start = rb_start_index;
	tx_req->tx_info[tx_req->num_tx].sch.rb_num = rbs_req;
	tx_req->tx_info[tx_req->num_tx].sch.modulation = 2;//QPSK
	tx_req->tx_info[tx_req->num_tx].sch.rv = 0;
	tx_req->tx_info[tx_req->num_tx].sch.data_ind = 2;
	tx_req->tx_info[tx_req->num_tx].sch.ack = INVALID_U8;
	tx_req->tx_info[tx_req->num_tx].sch.pdu_len = sib_len;
	tx_req->tx_info[tx_req->num_tx].sch.data = common_channel->sib_pdu;

	cce_offset = allocate_CCE(aggregation_level);

	if (cce_offset >= 0)
	{
		tx_req->tx_info[tx_req->num_tx].rnti = SI_RNTI;
		tx_req->tx_info[tx_req->num_tx].ueIndex = INVALID_U16;

		tx_req->tx_info[tx_req->num_tx].dci.cce_rb_num = aggregation_level;
		tx_req->tx_info[tx_req->num_tx].dci.cce_rb = cce_offset;
		tx_req->tx_info[tx_req->num_tx].dci.rb_num = rbs_req;
		tx_req->tx_info[tx_req->num_tx].dci.rb_start = rb_start_index;
		tx_req->tx_info[tx_req->num_tx].dci.mcs = mcs;
		tx_req->tx_info[tx_req->num_tx].dci.data_ind = 2;
		tx_req->tx_info[tx_req->num_tx].dci.ndi = 0;
		tx_req->tx_info[tx_req->num_tx].dci.rv = 0;
		tx_req->num_tx++;
	}
	else
	{
		LOG_WARN(MAC, "No CCE Resoure for SIB SFN:%u", frame*4+subframe);
	}

}
#endif
