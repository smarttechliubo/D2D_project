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
#include "mac_cce.h"
#include "interface_mac_rlc.h"
#include "interface_mac_phy.h"

#include "msg_handler.h"

#include "messageDefine.h"//MAC_TEST

void init_mac_tx(const uint16_t cellId)
{
	g_sch.cellId = cellId;
	g_sch.frame = INVALID_U32;
	g_sch.subframe = INVALID_U32;
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

	ue = &g_sch_mac->ue[ueIndex];

	if (ue->active == false ||
		ue->out_of_sync)
	{
		LOG_WARN(MAC, "ue ueIndex:%u, rnti:%u is inactive?active:%u(1:active, 0:inactive)", ueIndex, rnti, ue->active);
		return;
	}

	ue_buffer = &ue->buffer;
	ue_buffer->chan_num = 0;
	ue_buffer->buffer_total = 0;

	//ue_buffer->chan_num = logic_chan_num;

	for (uint32_t i = 0; i < logic_chan_num; i++)
	{
		if (buffer.buffer_byte_size[i] + buffer.rlc_header_byte_size[i] > 0)
		{
			ue_buffer->lc_tbs_req[ue_buffer->chan_num] = 0;
			ue_buffer->chan_id[ue_buffer->chan_num] = buffer.logicchannel_id[i];
			ue_buffer->buffer_size[ue_buffer->chan_num] = buffer.buffer_byte_size[i] + buffer.rlc_header_byte_size[i];
			ue_buffer->buffer_total += ue_buffer->buffer_size[i] + (ue_buffer->buffer_size[i] < 128 ? 2 : 3);

			ue_buffer->chan_num++;

			LOG_ERROR(MAC, "update_buffer_status, lcNum:%u, total:%u, lcId:%u, lcSize:%u",
				logic_chan_num,ue_buffer->buffer_total,ue_buffer->chan_id[i],ue_buffer->buffer_size[i]);
		}
	}

}

void handle_buffer_status(const rlc_mac_buf_status_rpt *rpt)
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
			add_temp_ue(RA_RNTI);
			break;
		}

		if (buffer.valid_flag)
		{
			update_buffer_status(buffer);
		}
	}
}

void handle_buffer_status_ind(msgDef* msg)
{
	//MAC_TEST
	msgId msgid = get_msgId(msg);

	switch (msgid)
	{
		case RLC_MAC_BUF_STATUS_RPT:
		{
			rlc_mac_buf_status_rpt *rpt = (rlc_mac_buf_status_rpt *)message_ptr(msg);

			LOG_INFO(MAC, "RLC_MAC_BUF_STATUS_RPT");
			handle_buffer_status(rpt);
			break;
		}
		default:
		{
			LOG_ERROR(MAC, "handle_buffer_status_ind, Unknown RLC msg! msgId:%u", get_msgId(msg));
			break;
		}
	}
}

uint32_t calculate_available_rbs()
{
	uint16_t sch_num = 0;
	sch_ind *sch;
	uint32_t available_rbs = get_available_rbs(g_sch_mac->bandwith);

	for (uint32_t i = 0; i < sch_num; i++)
	{
		sch = &g_sch.tx_info.sch[i];
		available_rbs = available_rbs - sch->rb_num;
	}
	return available_rbs;
}

uint32_t get_logic_channel_header_size(uint32_t buffer_size)
{
	uint32_t header_size = 0;

	if (buffer_size < 128)
	{
		header_size = 2;
	}

	if (buffer_size >= 128)
	{
		header_size = 3;
	}

	return header_size;
}

void add_to_scheduling_list0(uint16_t ueIndex)
{
	if (ueIndex >= MAX_UE || g_sch_mac->num0 >= MAX_UE)
	{
		LOG_ERROR(MAC, "add ue list fail, ueIndex:%u, sch_ue_num:%u", ueIndex, g_sch_mac->num0);
		return;
	}
	g_sch_mac->scheduling_list0[g_sch_mac->num0++] = ueIndex;
}

bool update_scheduled_ue(const rnti_t rnti, const uint32_t data_size, uint8_t* dataptr, const bool cancel)
{
	uint16_t num = 0;
	sch_ind* sch = NULL;
	dci_ind* dci =NULL;

	if (dataptr == NULL)
	{
		LOG_ERROR(MAC, "update_scheduled_ue, invalid dataptr");
		return false;
	}

	for (uint32_t i = 0; i < MAX_TX_UE && num < 2; ++i)
	{
		sch = &g_sch.tx_info.sch[i];
		dci = &g_sch.tx_info.dci[i];

		if (rnti == sch->rnti)
		{
			if (cancel)
			{
				sch->cancel = true;
			}
			else
			{
				if (data_size > sch->pdu_len)
				{
					LOG_WARN(MAC, "rlc pdu length wrong, rnti:%u, dataSize:%u, pdu_len:%u", 
						rnti, data_size, sch->pdu_len);
				}

				sch->data = dataptr;
			}

			num++;
		}

		if (rnti == dci->rnti)
		{
			if (cancel)
			{
				dci->cancel = true;
			}

			num++;
		}
	}

	if (num < 2)
	{
		LOG_ERROR(MAC, "ue rnti:%u, has no dci or sch", rnti);
	}

	return true;
}

void handle_rlc_data_result(const rlc_mac_data_ind* ind)
{
	bool cancel = false;
	bool ret = false;
	uint32_t ue_num = ind->ue_num;
	uint16_t ueIndex = INVALID_U16;
	//mac_info_s *mac = g_sch_mac;
	//ueInfo* ue = &mac->ue[0];
	rnti_t rnti = INVALID_U16;
	uint32_t data_size = 0;
	uint8_t* dataptr = NULL;

	for (uint32_t i = 0; i < ue_num; i++)
	{
		if (ind->sdu_pdu_info[i].rnti == RA_RNTI)
		{
			ueIndex = 0;
		}
		else
		{
			ueIndex = find_ue(ind->sdu_pdu_info[i].rnti);
		}

		if (ueIndex == INVALID_U16)
		{
			LOG_ERROR(MAC, "handle_rlc_data_result, did not find scheduled ue rnti:%u", 
				ind->sdu_pdu_info[i].rnti);
			continue;
		}

		//ue = &mac->ue[ueIndex];
		if (!ind->sdu_pdu_info[i].valid_flag)
		{
			LOG_INFO(MAC, "rlc data result, ue rnti:%u canceled", 
				ind->sdu_pdu_info[i].rnti);
			cancel = true;
		}

		rnti = ind->sdu_pdu_info[i].rnti;
		data_size = ind->sdu_pdu_info[i].tb_byte_size;
		dataptr = (uint8_t*)ind->sdu_pdu_info[i].data_buffer_adder_ptr;
		

		ret = update_scheduled_ue(rnti, data_size, dataptr, cancel);

		if (ret)
		{

		}
	}

}

void handle_rlc_data_ind()
{
	msgDef* msg;
	//uint32_t msg_len = 0;
	msgId msg_id = 0;
	uint32_t timeout = 1;

	msg = message_receive_timeout(TASK_D2D_MAC_SCH, timeout);

	if (msg == NULL)
	{
		return;
	}

	msg_id = get_msgId(msg);

	switch (msg_id)
	{
		case RLC_MAC_DATA_IND:
		{
			rlc_mac_data_ind *ind = (rlc_mac_data_ind *)message_ptr(msg);

			LOG_INFO(MAC, "RLC_MAC_DATA_IND ");
			handle_rlc_data_result(ind);
			break;
		}
		default:
		{
			LOG_ERROR(MAC, "handle_rlc_data_ind, Unknown RLC msg! msgId:%u", get_msgId(msg));
			break;
		}
	}

	message_free(msg);
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
	mac_info_s *mac = g_sch_mac;
	sort_ue_params *params = _p;
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

	return 0;
}

void sort_ues(uint16_t list[MAX_UE], uint16_t size)
{
	sort_ue_params params = {ESORTING_HIGH};

	qsort_r(list, size, sizeof(uint16_t), ue_compare, &params);
}

int32_t logic_channel_compare(const void *_a, const void *_b, void *_p)
{
	sort_lc_params* params = _p;
	uint16_t index1 = *(const uint16_t *)_a;
	uint16_t index2 = *(const uint16_t *)_b;

	lc_config lc1 = params->config[index1];
	lc_config lc2 = params->config[index2];
	uint32_t buffer1 = params->buffer->buffer_size[index1];
	uint32_t buffer2 = params->buffer->buffer_size[index2];

	if (lc1.priority > lc2.priority)
	{
		return -1;
	}
	else if (lc1.priority < lc2.priority)
	{
		return 1;
	}

	if (buffer1 == 0 && buffer2 == 0)
	{
		return 0;
	}

	if (buffer1 > 0 && buffer2 == 0)
	{
		return -1;
	}

	if (buffer1 == 0 && buffer2 > 0)
	{
		return 1;
	}


	return 0;
}

void sort_logic_channel(uint16_t list[MAX_LOGICCHAN_NUM], uint16_t size, sort_lc_params* params)
{
	qsort_r(list, size, sizeof(uint16_t), logic_channel_compare, params);
}

void pre_assign_rbs(const frame_t frame, const sub_frame_t subframe)
{
	bool scheduled = false;
	uint16_t mcs = 0;
	//uint16_t cqi = 0;
	mac_info_s *mac = g_sch_mac;
	uint8_t harqId = get_harqId(subframe);
	ueInfo* ue = NULL;
	uint32_t rbg_size = get_rbg_size(mac->bandwith);
	uint32_t rb_max = get_rb_num(mac->bandwith);
	uint32_t rbs_req = 0;
	uint32_t tbs = 0;

	for (uint32_t i = 0; i < MAX_UE; i++)
	{
		if (mac->ue[i].active == false ||
			mac->ue[i].out_of_sync == true)
		{
			continue;
		}

		ue = &mac->ue[i];
		ue->sch_info.pre_rbs_alloc = 0;

		rbs_req = 0;
		//cqi = ue->sch_info.cqi;
		//mcs = cqi_to_mcs(cqi);
		mcs = ue->sch_info.mcs;
		rbs_req = (mcs == 0) ? (uint32_t)mac->min_rbs_per_ue : rbg_size;

		tbs = get_tbs(mcs, rbg_size);

		if (ue->buffer.buffer_total > 0)
		{
			LOG_ERROR(MAC, "pre_assign_rbs buffer.buffer_total:%u, rbg_size:%u, tbs:%u, mcs:%u",
			ue->buffer.buffer_total, rbg_size, tbs, mcs);

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

			ue->sch_info.pre_tbs = tbs;
			ue->sch_info.pre_rbs_alloc = rbs_req;

			scheduled = true;
		}
		else if (ue->sch_info.crc[harqId] > ECRC_NULL)
		{
			rbs_req = mac->min_rbs_per_ue;
			tbs = get_tbs(mcs, rbs_req);

			ue->sch_info.pre_tbs = tbs;
			ue->sch_info.pre_rbs_alloc = rbs_req;

			scheduled = true;
		}
		else if (ue->harq[harqId].reTx)
		{
			scheduled = true;
		}
			

		if (scheduled)
		{
			scheduled = false;
			LOG_ERROR(MAC, "pre_assign_rbs, ue rnti:%u, pre_tbs:%u, buffer_total:%u, crc:%u, reTx:%u", 
				ue->rnti, ue->sch_info.pre_tbs,ue->buffer.buffer_total, ue->sch_info.crc[harqId],ue->harq[harqId].reTx);
			add_to_scheduling_list0(i);
		}
	}
}

void pre_schedule_reset()
{
	g_sch_mac->num0 = 0;
	g_sch_mac->num1 = 0;
	g_sch_mac->num2 = 0;

}

void scheduler_pre_handler(const frame_t frame, const sub_frame_t subframe)
{
	pre_schedule_reset();

	pre_assign_rbs(frame, subframe);

}

void scheduler_resouce_calc_greedy(const frame_t frame, const sub_frame_t subframe)
{
	uint32_t totalUe = 0;
	uint32_t newTxUe = 0;
	uint32_t reTxUe = 0;
	uint32_t rbs_reTx = 0;
	uint32_t rbs_newTx = 0;
	uint16_t num0 = 0;
	uint8_t harqId = get_harqId(subframe);

	mac_info_s *mac = g_sch_mac;
	ueInfo *ue = &mac->ue[0];
	uint32_t available_rbs = calculate_available_rbs();

	num0 = mac->num0;
	uint16_t ueIndex = INVALID_U16;

	for (uint32_t i = 0; i < num0; i++)
	{
		ueIndex = mac->scheduling_list0[i];

		if (ueIndex >= MAX_UE ||
			mac->ue[ueIndex].active == false ||
			mac->ue[ueIndex].out_of_sync == true) 
		{
			LOG_ERROR(MAC, "ue is not available, ueIndex:%u, active:%u, outOfSync:%u", 
				ueIndex, mac->ue[ueIndex].active, mac->ue[ueIndex].out_of_sync);
			continue;
		}

		ue = &mac->ue[ueIndex];

		if (ue->harq[harqId].reTx)
		{
			reTxUe++;
			totalUe++;
			rbs_reTx += ue->harq[harqId].rbs_alloc;
			available_rbs = available_rbs - ue->harq[harqId].rbs_alloc;
		}
		else //if (ue->buffer.buffer_total > 0)
		{
			newTxUe++;
			totalUe++;
			ue->sch_info.pre_rbs_alloc = MIN(ue->sch_info.pre_rbs_alloc, available_rbs);
			ue->sch_info.pre_tbs = get_tbs(ue->sch_info.mcs, ue->sch_info.pre_rbs_alloc);
			available_rbs = available_rbs - ue->sch_info.pre_rbs_alloc;
			rbs_newTx += ue->sch_info.pre_rbs_alloc;
		}
	}
}

void scheduler_resouce_calc_fair(const frame_t frame, const sub_frame_t subframe)
{
	uint16_t ueIndex = INVALID_U16;
	uint32_t totalUe = 0;
	uint32_t newTxUe = 0;
	uint16_t newTxUe_list[MAX_UE];
	uint32_t reTxUe = 0;
	uint32_t rbs_reTx = 0;
	uint32_t rbs_newTx = 0;
	uint16_t num0 = 0;
	uint8_t harqId = get_harqId(subframe);

	mac_info_s *mac = g_sch_mac;
	ueInfo *ue = &mac->ue[0];
	uint32_t available_rbs = calculate_available_rbs();
	uint32_t average_rbs_per_ue = 0;
	uint32_t rbg_size = get_rbg_size(mac->bandwith);

	num0 = mac->num0;

	for (uint32_t i = 0; i < num0; i++)
	{
		ueIndex = mac->scheduling_list0[i];

		if (ueIndex >= MAX_UE ||
			mac->ue[ueIndex].active == false ||
			mac->ue[ueIndex].out_of_sync == true) 
		{
			LOG_ERROR(MAC, "ue is not available, ueIndex:%u, active:%u, outOfSync:%u", 
				ueIndex, mac->ue[ueIndex].active, mac->ue[ueIndex].out_of_sync);
			continue;
		}

		ue = &mac->ue[ueIndex];

		if (ue->harq[harqId].reTx)
		{
			reTxUe++;
			totalUe++;
			rbs_reTx += ue->harq[harqId].rbs_alloc;
		}
		else if (ue->buffer.buffer_total > 0)
		{
			totalUe++;
			newTxUe_list[newTxUe++] = ueIndex;
			rbs_newTx += ue->sch_info.pre_rbs_alloc;
		}
	}

	available_rbs = available_rbs - rbs_reTx;

	if (newTxUe == 0)
	{
		average_rbs_per_ue = 0;
	}
	else if (rbg_size*newTxUe <= available_rbs)
	{
		average_rbs_per_ue = available_rbs/newTxUe;
	}
	else
	{
		average_rbs_per_ue = rbg_size;
	}

	for (uint32_t i = 0; i < newTxUe; i++)
	{
		ue = &mac->ue[newTxUe_list[i]];
		ue->sch_info.pre_rbs_alloc = MIN(average_rbs_per_ue, ue->sch_info.pre_rbs_alloc);
		available_rbs = available_rbs - ue->sch_info.pre_rbs_alloc;
	}
}

uint32_t scheduler_rballoc(mac_info_s *mac, const uint16_t ueIndex, const uint32_t first_rb, const uint32_t total_rb_num)
{
    //uint32_t rb_start = first_rb;
	uint32_t rb_num = 0;
	uint32_t pre_rbs_alloc = 0;
	ueInfo* ue = &mac->ue[ueIndex];
	uint8_t harqId = get_harqId(mac->subframe);

	if (ue->harq[harqId].reTx)
	{
		pre_rbs_alloc = ue->harq[harqId].rbs_alloc;
	}
	else
	{
		pre_rbs_alloc = ue->sch_info.pre_rbs_alloc;
	}

	for(uint32_t i = first_rb; i < total_rb_num && pre_rbs_alloc > 0; i++)
	{
		if (mac->rb_available[i] == 1)
		{
			rb_num++;
			pre_rbs_alloc--;
			mac->rb_available[i] = 0;
		}
		else
		{
			break;
		}
	}

	if (ue->harq[harqId].reTx)
	{
		if (rb_num != ue->harq[harqId].rbs_alloc)
		{
			LOG_ERROR(MAC, "rballoc, no enough rbs for ue:%u reTx", ue->ueIndex);
		}
		ue->harq[harqId].rbs_alloc = rb_num;//TODO: reTx should be transmmited if no enough rbs for reTx?
		ue->harq[harqId].rb_start = first_rb;
	}
	else
	{
		ue->sch_info.pre_tbs = get_tbs(ue->sch_info.mcs, rb_num);

		ue->sch_info.rb_start = first_rb;
		ue->sch_info.pre_rbs_alloc = rb_num;

		ue->harq[harqId].rbs_alloc = rb_num;
		ue->harq[harqId].rb_start = first_rb;
		ue->harq[harqId].tbs = ue->sch_info.pre_tbs;
	}

	return rb_num;
}

void scheduler_resource_locate()
{
	uint16_t ueIndex = INVALID_U16;
	uint16_t num0 = 0;
	//uint16_t *scheduling_list0;
	mac_info_s *mac = g_sch_mac;
	//ueInfo *ue = &mac->ue[0];
	uint32_t first_rb = get_first_rb(mac->bandwith, mac);
	uint32_t total_rbs = get_rb_num(mac->bandwith);
	uint32_t rb_alloc_count = 0;
	uint16_t num1 = 0;

	num0 = mac->num0;

	for (uint32_t i = 0; i < num0; i++)
	{
		ueIndex = mac->scheduling_list0[i];

		if (ueIndex >= MAX_UE ||
			mac->ue[ueIndex].active == false ||
			mac->ue[ueIndex].out_of_sync == true)
		{
			LOG_ERROR(MAC, "ue is not available, ueIndex:%u, active:%u, outOfSync:%u", 
				ueIndex, mac->ue[ueIndex].active, mac->ue[ueIndex].out_of_sync);
			continue;
		}

		//ue = &mac->ue[ueIndex];
		first_rb = first_rb + rb_alloc_count;

		if (first_rb >= total_rbs)
		{
			break;
		}

		rb_alloc_count = scheduler_rballoc(mac, ueIndex, first_rb, total_rbs);

		mac->scheduling_list1[num1++] = ueIndex;
	}

	mac->num1 = num1;
}

void schedule_ue(const frame_t frame, const sub_frame_t subframe)
{
	uint16_t num0 = 0;
	uint16_t *scheduling_list0;
	mac_info_s *mac = g_sch_mac;

	scheduler_pre_handler(frame, subframe);

	num0 = mac->num0;
	scheduling_list0 = &mac->scheduling_list0[0];

	if(num0 > 0)
	{
		sort_ues(scheduling_list0, num0);
	}

	if (mac->alloc_pattern == EPATTERN_GREEDY)
	{
		scheduler_resouce_calc_greedy(frame, subframe);
	}
	else
	{
		scheduler_resouce_calc_fair(frame, subframe);
	}

	scheduler_resource_locate();
}

void handle_ue_macce(ueInfo* ue)
{
// so far, no mac ce
}

void handle_ue_logic_channel(ueInfo* ue, uint16_t lc_index[MAX_LOGICCHAN_NUM], uint16_t lc_num, const sub_frame_t subframe)
{
	uint8_t harqId = get_harqId(subframe);
	harq_info* harq = &ue->harq[harqId];
	txBuffer* buffer = &ue->buffer;
	schedule_info* sch_info = &ue->sch_info;
	int32_t tbs = sch_info->pre_tbs;
	uint16_t index = 0;

	uint32_t tbs_size = 4;
	uint32_t tbs_req = 0;
	uint32_t header_size = 0;
	uint32_t lc_size = 0;

	if (harq->reTx)
	{
		return;
	}

	buffer->lc_num = 0;

	for (uint16_t i = 0; i < lc_num && tbs > 0; i++)
	{
		index = lc_index[i];

		if (buffer->buffer_size[index] > 0)
		{
			header_size = get_logic_channel_header_size(buffer->buffer_size[index]);

			tbs_req = 0;
			tbs_req = tbs_size;
			buffer->buffer_size[index] += header_size;
			lc_size = buffer->buffer_size[index];

			while (tbs_req < lc_size)
			{
				tbs_req += tbs_size;
			}

			if (tbs_req > lc_size)
			{
				while (tbs_req > lc_size)
					tbs_req--;
			}

			//pre_rbs_alloc = pre_rbs_alloc - rbs_req;

			if (tbs - tbs_req < 0)
			{
				while ((tbs - tbs_req) >= 0)
					tbs_req--;
			}

			tbs = tbs - tbs_req;
			buffer->lc_tbs_req[index] = tbs_req;

			LOG_INFO(MAC, "handle_ue_logic_channel, lc_num:%u, pre_tbs:%u, lc_size:%u, tbs_req:%u",
				lc_num,sch_info->pre_tbs,lc_size,tbs_req);

			buffer->lc_priority_index[buffer->lc_num++] = index;
		}
	}

	if (tbs > 0)
	{
	//	LOG_WARN(MAC, "there are still tbs unassgin tbs:%d", tbs);
	}
}

void handle_ue_result(ueInfo* ue, const sub_frame_t subframe)
{
	uint16_t lc_num = ue->buffer.chan_num;
	lc_config lc_config[MAX_LOGICCHAN_NUM];

	txBuffer* buffer = &ue->buffer;
	//schedule_info* sch_info = &ue->sch_info;

	sort_lc_params params = {lc_config, buffer};

	uint16_t lc_index[lc_num];

	for (uint16_t i = 0; i < lc_num; i++)
	{
		uint32_t j = 0;

		for (j = 0; j < ue->lc_num; j++)
		{
			if (buffer->chan_id[i] == ue->lc_config[j].lc_id)
			{
				lc_config[i] = ue->lc_config[j];
				break;
			}
		}

		if (j == ue->lc_num)
		{
			LOG_ERROR(MAC, "No logical channel find, lc_num:%u, j:%u",ue->lc_num,j);
		}

		lc_index[i] = i;
	}

	if (ue->ce_num > 0)
	{
		handle_ue_macce(ue);
	}

	sort_logic_channel(lc_index, lc_num, &params);

	handle_ue_logic_channel(ue, lc_index, lc_num, subframe);
}

bool fill_dci_result(
	const uint16_t  ueIndex,
	const rnti_t    rnti,
	dci_ind* dci, 
	const uint16_t dci_num, 
	const uint32_t rb_num, 
	const uint32_t rb_start, 
	const uint8_t mcs,
	const uint8_t data_ind)
{
	int32_t cce_offset = -1;
	uint16_t aggregation_level = 2;//get_aggregation_level(bandwith, EFORMAT0, 2);

	cce_offset = allocate_CCE(aggregation_level);

	if (cce_offset >= 0)
	{
		dci[dci_num].cancel = false;
		dci[dci_num].rnti = rnti;
		dci[dci_num].ueIndex = ueIndex;

		dci[dci_num].cce_rb_num = aggregation_level;
		dci[dci_num].cce_rb = cce_offset;
		dci[dci_num].rb_num = rb_num;
		dci[dci_num].rb_start = rb_start;
		dci[dci_num].mcs = mcs;
		dci[dci_num].data_ind = data_ind;
		dci[dci_num].ndi = 0;
		dci[dci_num].rv = 0;
	}
	else
	{	
		LOG_WARN(MAC, "No CCE Resoure for ue ueIndex:%u, rnti:%u", ueIndex, rnti);
		return false;
	}

	return true;
}

void fill_sch_result(ueInfo* ue, 
	sch_ind* sch, 
	uint16_t sch_num,
	const uint32_t rb_num,
	const uint32_t rb_start,
	const uint8_t mcs, 
	const uint8_t rv,
	const uint8_t data_ind,
	const uint8_t harqId,
	const uint8_t ack,
	const uint16_t pdu_len)
{
	sch[sch_num].cancel = false;
	sch[sch_num].ueIndex = ue->ueIndex;
	sch[sch_num].rnti = ue->rnti;
	sch[sch_num].rb_start = rb_start;
	sch[sch_num].rb_num = rb_num;
	sch[sch_num].mcs = mcs;
	sch[sch_num].data_ind = data_ind; //TODO:
	sch[sch_num].modulation = 1;//TODO: not define yet
	sch[sch_num].rv = rv;
	sch[sch_num].harqId = harqId;
	sch[sch_num].ack = ack;

	sch[sch_num].pdu_len = pdu_len;
}

uint8_t get_data_ind(ueInfo* ue, const uint8_t harqId)
{
	// 1:ACK/NACK; 2:DATA;  3:DATA + ACK/NACK
	if ((ue->buffer.buffer_total > 0 || ue->harq[harqId].reTx) && ue->sch_info.crc[harqId] > ECRC_NULL)
	{
		return 3;
	}
	else if (ue->buffer.buffer_total > 0 || ue->harq[harqId].reTx)
	{
		return 2;
	}
	else if (ue->sch_info.crc[harqId] > ECRC_NULL)
	{
		return 1;
	}

	return 3;
}

bool fill_schedule_result(ueInfo* ue, const sub_frame_t subframe)
{
	bool ret = false;
	tx_req_info* tx = &g_sch.tx_info;
	uint16_t dci_num = tx->dci_num;
	uint16_t sch_num = tx->sch_num;
	dci_ind* dci = &tx->dci[0];
	sch_ind* sch = &tx->sch[0];

	uint8_t harqId = get_harqId(subframe);
    uint32_t rb_start = 0;
	uint32_t rb_num  = 0;
	uint8_t mcs = 0;
	uint8_t rv = 0;
	uint8_t data_ind = get_data_ind(ue, harqId);// 1:ACK/NACK; 2:DATA;  3:DATA + ACK/NACK
	uint16_t pdu_len = 0;
	uint8_t ack = 3; 

	if (ue->sch_info.crc[harqId] == ECRC_ACK)
	{
		ack = 1;
	}
	else if (ue->sch_info.crc[harqId] == ECRC_NACK)
	{
		ack = 0;
	}

	if (ue->harq[harqId].reTx)
	{
		rb_start = ue->harq[harqId].rb_start;
		rb_num = ue->harq[harqId].rb_num;
		mcs = ue->harq[harqId].mcs;
		pdu_len = ue->harq[harqId].tbs;
		rv = get_rv(ue->harq[harqId].reTx_num);
	}
	else
	{
		rb_start = ue->sch_info.rb_start;
		rb_num = ue->sch_info.rb_num;
		mcs = ue->sch_info.mcs;
		pdu_len = ue->sch_info.pre_tbs;
		rv = 0;
	}

	ret = fill_dci_result(ue->ueIndex, ue->rnti, dci, dci_num, rb_num, rb_start, mcs, data_ind);

	if (ret)
	{
		fill_sch_result(ue, sch, sch_num, rb_num, rb_start, mcs, rv, data_ind, harqId, ack, pdu_len);

		dci_num++;
		sch_num++;
	}
	else
	{
		return false;
	}

	tx->dci_num = dci_num;
	tx->sch_num = sch_num;

	LOG_INFO(MAC, "fill_schedule_result, ret:%u, mode:%u, rnti:%u, dci_num:%u, sch_num:%u", 
		ret, g_sch_mac->mode,ue->rnti, dci_num, sch_num);

	return true;
}

void handle_schedule_result(const frame_t frame, const sub_frame_t subframe)
{
	uint16_t ueIndex = INVALID_U16;
	uint16_t num1 = 0;
	mac_info_s* mac = g_sch_mac;
	ueInfo* ue = NULL;
	//uint32_t rb_num = get_rb_num(mac->bandwith);
	//uint32_t rb_alloc_count = 0;
	uint16_t num2 = mac->num2;
	bool ret = false;

	num1 = mac->num1;

	for (uint32_t i = 0; i < num1; i++)
	{
		ueIndex = mac->scheduling_list1[i];

		if (ueIndex >= MAX_UE ||
			mac->ue[ueIndex].active == false ||
			mac->ue[ueIndex].out_of_sync == true) 
		{
			LOG_ERROR(MAC, "ue is not available, ueIndex:%u, active:%u, outOfSync:%u", 
				ueIndex, mac->ue[ueIndex].active, mac->ue[ueIndex].out_of_sync);
			continue;
		}

		ue = &mac->ue[ueIndex];

		handle_ue_result(ue, subframe);

		ret = fill_schedule_result(ue, subframe);

		if (ret)
		{
			mac->scheduling_list2[num2++] = ueIndex;
		}
	}

	mac->num2 = num2;
}

void handle_schedule_commom(const frame_t frame, const sub_frame_t subframe)
{
	bool ret = false;
	tx_req_info* tx = &g_sch.tx_info;
	uint16_t dci_num = tx->dci_num;
	uint16_t sch_num = tx->sch_num;
	dci_ind* dci = &tx->dci[0];
	sch_ind* sch = &tx->sch[0];

	uint32_t rb_start = 0;
	uint32_t rb_num  = 0;
	uint8_t mcs = 0;
	uint8_t data_ind = 3;// 1:ACK/NACK; 2:DATA;  3:DATA + ACK/NACK

	for (uint32_t i = 0; i < sch_num; i++)
	{
		sch = &tx->sch[i];

		rb_num = sch->rb_num;
		rb_start = sch->rb_start;
		mcs = sch->mcs;
		data_ind = sch->data_ind;
		LOG_INFO(MAC, "common result, ueIndex:%u, rnti:%u", sch->ueIndex, sch->rnti);
		ret = fill_dci_result(sch->ueIndex, sch->rnti, dci, dci_num, rb_num, rb_start, mcs, data_ind);

		if (ret)
		{
			tx->dci_num++;
		}
	}
}

void mac_rlc_data_request(const frame_t frame, const sub_frame_t subframe)
{
	mac_info_s* mac = g_sch_mac;
	ueInfo* ue = &mac->ue[0];
	uint16_t ue_num = mac->num2;
	uint16_t ra_num = 0;
	uint16_t ueIndex = mac->scheduling_list2[0];
	uint8_t harqId = get_harqId(subframe);
	uint16_t lc_index = 0;

	msgDef* msg = NULL;
	mac_rlc_data_req *req;
	msgSize msg_size = sizeof(mac_rlc_data_req);

	rlc_data_req* data = NULL;

	msg = new_message(MAC_RLC_DATA_REQ, TASK_D2D_MAC_SCH, TASK_D2D_RLC_TX, msg_size);

	if (msg == NULL)
	{
		LOG_ERROR(MAC, "mac_rlc_data_request, new mac message fail!");
		return;
	}

	req = (mac_rlc_data_req*)message_ptr(msg);
	req->sfn = frame;
	req->sub_sfn = subframe;
	req->ue_num = 0;

	ra_num = mac_ra_data_request(&req->rlc_data_request[0], req->ue_num);

	req->ue_num = ra_num;

	for (uint32_t i = 0; i < ue_num; i++)
	{
		ueIndex = mac->scheduling_list2[i];
		ue = &mac->ue[ueIndex];

		if (ue->harq[harqId].reTx || ue->buffer.buffer_total <= 0)
		{
			continue;
		}

		data = &req->rlc_data_request[req->ue_num++];
		data->rnti = ue->rnti;
		data->tb_size = 0;
		data->logic_chan_num = ue->buffer.lc_num;

		for (uint32_t j = 0; j < ue->buffer.lc_num; j++)
		{
			lc_index = ue->buffer.lc_priority_index[j];
			data->logicchannel_id[j] = ue->buffer.chan_id[lc_index];
			data->mac_pdu_byte_size[j] = ue->buffer.lc_tbs_req[lc_index];
			data->tb_size += ue->sch_info.pre_tbs;//ue->buffer.lc_tbs_req[lc_index];
		}
	}

	if (req->ue_num > 0)
	{
		if (message_send(TASK_D2D_RLC_TX, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_RLC_DATA_REQ, MAC->RLC send msg!, num:%u, lcNum:%u, rnti:%u, tbs:%u,lcNum:%u,lcId:%u,lcSize:%u",
				req->ue_num, req->rlc_data_request[0].logic_chan_num, req->rlc_data_request[0].rnti, 
				req->rlc_data_request[0].tb_size,
				req->rlc_data_request[0].logic_chan_num,
				req->rlc_data_request[0].logicchannel_id[0],
				req->rlc_data_request[0].mac_pdu_byte_size[0]);
		}
	}
	else
	{
		message_free(msg);
	}
}

void send_pdcch_req(const frame_t frame, const sub_frame_t subframe)
{
	tx_req_info* tx = &g_sch.tx_info;
	tx_req_info* common = &g_sch.common;
	uint16_t common_dci_num = common->dci_num;
	uint16_t dci_num = tx->dci_num;
	dci_ind* common_dci = &common->dci[0];
	dci_ind* dci = &tx->dci[0];
	uint32_t num_ue = 0;

	if (dci_num + common_dci_num <= 0)
	{
		return;
	}

	msgDef* msg = NULL;
	PHY_PdcchSendReq* req;
	msgSize msg_size = sizeof(PHY_PdcchSendReq);

	msg = new_message(MAC_PHY_PDCCH_SEND, TASK_D2D_MAC_SCH, TASK_D2D_PHY_TX, msg_size);

	if (msg != NULL)
	{
		req = (PHY_PdcchSendReq*)message_ptr(msg);
		req->frame = frame;
		req->subframe = subframe;

		for (uint32_t i = 0; i < common_dci_num; i++)
		{
			req->dci[i].type = (dci_type_e)common_dci[i].type;
			req->dci[i].rnti = common_dci[i].rnti;
			req->dci[i].dci_rb_start = common_dci[i].cce_rb_num;
			req->dci[i].dci_rb_num = common_dci[i].cce_rb;
			req->dci[i].data_size = MAX_DCI_LEN;
			req->dci[i].data[0] = (common_dci[i].rb_start & 0X7F) << 1;
			req->dci[i].data[0] |= (common_dci[i].rb_num & 0X40) >> 6;
			req->dci[i].data[1] = (common_dci[i].rb_num & 0X3F) << 2;
			req->dci[i].data[1] |= (common_dci[i].mcs & 0X1F) >> 3;
			req->dci[i].data[2] = (common_dci[i].mcs & 0X07) << 5;
			req->dci[i].data[2] |= (common_dci[i].data_ind & 0X03) << 3;
			req->dci[i].data[2] |= (common_dci[i].ndi & 0X01) << 2;
		}

		for (uint32_t i = 0; i < dci_num; i++)
		{
			if (dci[i].cancel == true)
			{
				continue;
			}

			req->dci[i+common_dci_num].type = (dci_type_e)dci[i].type;
			req->dci[i+common_dci_num].rnti = dci[i].rnti;
			req->dci[i+common_dci_num].dci_rb_start = dci[i].cce_rb_num;
			req->dci[i+common_dci_num].dci_rb_num = dci[i].cce_rb;
			req->dci[i+common_dci_num].data_size = MAX_DCI_LEN;
			req->dci[i+common_dci_num].data[0] = (dci[i].rb_start & 0X7F) << 1;
			req->dci[i+common_dci_num].data[0] |= (dci[i].rb_num & 0X40) >> 6;
			req->dci[i+common_dci_num].data[1] = (dci[i].rb_num & 0X3F) << 2;
			req->dci[i+common_dci_num].data[1] |= (dci[i].mcs & 0X1F) >> 3;
			req->dci[i+common_dci_num].data[2] = (dci[i].mcs & 0X07) << 5;
			req->dci[i+common_dci_num].data[2] |= (dci[i].data_ind & 0X03) << 3;
			req->dci[i+common_dci_num].data[2] |= (dci[i].ndi & 0X01) << 2;
			num_ue++;
		}

		req->num_dci = num_ue + common_dci_num;

		if (message_send(TASK_D2D_PHY_TX, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_PHY_PDCCH_SEND send.frame:%u, subframe:%u, num_dci:%u,dci:%x,%x,%x", 
				req->frame, req->subframe, req->num_dci, req->dci[0].data[0],req->dci[0].data[1],req->dci[0].data[2]);
		}
	}
	else
	{
		LOG_ERROR(MAC, "PDCCH send, new mac message fail!");
	}
}

void send_pusch_req(const frame_t frame, const sub_frame_t subframe)
{
	tx_req_info* tx = &g_sch.tx_info;
	tx_req_info* common = &g_sch.common;
	uint16_t common_sch_num = common->sch_num;
	uint16_t sch_num = tx->sch_num;
	sch_ind* common_sch = &common->sch[0];
	sch_ind* sch = &tx->sch[0];

	if (sch_num + common_sch_num <= 0)
	{
		return;
	}

	msgDef* msg = NULL;
	PHY_PuschSendReq* req;
	msgSize msg_size = sizeof(PHY_PuschSendReq);

	msg = new_message(MAC_PHY_PUSCH_SEND, TASK_D2D_MAC_SCH, TASK_D2D_PHY_TX, msg_size);

	if (msg != NULL)
	{
		req = (PHY_PuschSendReq*)message_ptr(msg);
		req->num = sch_num + common_sch_num;
		req->frame = frame;
		req->subframe = subframe;

		for (uint32_t i = 0; i < common_sch_num; i++)
		{
			req->pusch[i].rnti = common_sch[i].rnti;
			req->pusch[i].rb_start = common_sch[i].rb_start;
			req->pusch[i].rb_num = common_sch[i].rb_num;
			req->pusch[i].mcs = common_sch[i].mcs;
			req->pusch[i].data_ind = common_sch[i].data_ind;
			req->pusch[i].modulation = common_sch[i].modulation;
			req->pusch[i].rv = common_sch[i].rv;
			req->pusch[i].harqId = common_sch[i].harqId;
			req->pusch[i].ack = common_sch[i].ack;
			req->pusch[i].pdu_len = common_sch[i].pdu_len;
			req->pusch[i].data = common_sch[i].data;
		}

		for (uint32_t i = 0; i < sch_num; i++)
		{			
			if (sch[i].cancel == true)
			{
				continue;
			}

			req->pusch[i+common_sch_num].rnti = sch[i].rnti;
			req->pusch[i+common_sch_num].rb_start = sch[i].rb_start;
			req->pusch[i+common_sch_num].rb_num = sch[i].rb_num;
			req->pusch[i+common_sch_num].mcs = sch[i].mcs;
			req->pusch[i+common_sch_num].data_ind = sch[i].data_ind;
			req->pusch[i+common_sch_num].modulation = sch[i].modulation;
			req->pusch[i+common_sch_num].rv = sch[i].rv;
			req->pusch[i+common_sch_num].harqId = sch[i].harqId;
			req->pusch[i+common_sch_num].ack = sch[i].ack;
			req->pusch[i+common_sch_num].pdu_len = sch[i].pdu_len;
			req->pusch[i+common_sch_num].data = sch[i].data;
		}

		if (message_send(TASK_D2D_PHY_TX, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_PHY_PUSCH_SEND send. schnum:%u, common:%u, frame:%u, subframe:%u, num_pusch:%u", 
				sch_num, common_sch_num, req->frame, req->subframe, req->num);
			LOG_INFO(MAC, "LGC: MAC_PHY_PUSCH_SEND send. rnti:%u, mcs:%u, data_ind:%u, rv:%u, harqId:%u, ack:%u, pduLen:%u, data:%x,%x,%x,%x,%x,%x,%x,%x,%x", 
				req->pusch[0].rnti, req->pusch[0].mcs,req->pusch[0].data_ind,req->pusch[0].rv,
				req->pusch[0].harqId,req->pusch[0].ack,req->pusch[0].pdu_len, 
				req->pusch[0].data[0],
				req->pusch[0].data[1],
				req->pusch[0].data[2],
				req->pusch[0].data[3],
				req->pusch[0].data[4],
				req->pusch[0].data[5],
				req->pusch[0].data[6],
				req->pusch[0].data[7],
				req->pusch[0].data[8],
				req->pusch[0].data[9]);
		}
	}
	else
	{
		LOG_ERROR(MAC, "PXSCH send, new mac message fail!");
	}
}

void mac_phy_data_send(const frame_t frame, const sub_frame_t subframe)
{
	send_pdcch_req(frame, subframe);
	send_pusch_req(frame, subframe);
}

void update_ue_result(const frame_t frame, const sub_frame_t subframe)
{
	uint16_t sch_ue_num = g_sch_mac->num0;
	uint16_t ueIndex = INVALID_U16;
	ueInfo* ue = NULL;
	uint8_t harqId = get_harqId(subframe);

	for(uint32_t i = 0; i < sch_ue_num; i++)
	{
		ueIndex = g_sch_mac->scheduling_list0[i];
		ue = &g_sch_mac->ue[ueIndex];

		if (ue->temp == true)
		{
			remove_ue(ueIndex, EMAC_DEST);
		}

		ue->buffer.lc_num = 0;
		ue->buffer.buffer_total = 0;

		for(uint32_t j = 0; j < ue->buffer.chan_num; j++)
		{
			ue->buffer.buffer_size[j] = 0;
		}

		ue->buffer.chan_num = 0;
		ue->sch_info.crc[harqId] = ECRC_NULL;
	}
}

void scheduler_reset(const frame_t frame, const sub_frame_t subframe)
{
	tx_req_info* tx = &g_sch.tx_info;
	tx_req_info* common = &g_sch.common;

	common->dci_num = 0;
	tx->dci_num = 0;
	common->sch_num = 0;
	tx->sch_num = 0;

	memset(&g_sch_mac->rb_available[0], 1, MAX_RBS);
}

void mac_scheduler()
{
	frame_t frame = g_sch.frame;
	sub_frame_t subframe = g_sch.subframe;

	schedule_ra(frame, subframe);

	schedule_ue(frame, subframe);

	handle_schedule_commom(frame, subframe);

	handle_schedule_result(frame, subframe);

	mac_rlc_data_request(frame, subframe);

	//sleep(100us);
	handle_rlc_data_ind();

	mac_phy_data_send(frame, subframe);

	update_ue_result(frame, subframe);

	scheduler_reset(frame, subframe);
}

