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
#include "msg_queue.h"

void init_mac_tx(const uint16_t cellId)
{
	g_sch.cellId = cellId;
	g_sch.frame = INVALID_U32;
	g_sch.subframe = INVALID_U32;
}

uint32_t calculate_available_rbs()
{
	uint16_t sch_num = 0;
	sch_ind *sch;
	uint32_t available_rbs = get_available_rbs(g_context.mac->bandwith);

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
	if (ueIndex >= MAX_UE || g_context.mac->num0 >= MAX_UE)
	{
		LOG_ERROR(MAC, "add ue list fail, ueIndex:%u, sch_ue_num:%u", ueIndex, g_context.mac->num0);
		return;
	}
	g_context.mac->scheduling_list0[g_context.mac->num0++] = ueIndex;
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
				sch->scheduled = false;
			}
			else
			{
				if (data_size != sch->pdu_len)
				{
					LOG_WARN(MAC, "rlc pdu length wrong, rnti:%u, dataSize:%u, pdu_len:%u", 
						rnti, data_size, sch->pdu_len);
				}

				sch->data = dataptr;
				sch->scheduled = true;
			}
			num++;
		}

		if (rnti == dci->rnti)
		{
			if (cancel)
			{
				dci->scheduled = false;
			}
			else
			{
				dci->scheduled = true;
			}
			num++;
		}
	}

	return true;
}

void handle_rlc_data_result(const rlc_mac_data_ind* ind)
{
	bool cancel = false;
	bool ret = false;
	uint32_t ue_num = ind->ue_num;
	uint16_t ueIndex = INVALID_U16;
	//mac_info_s *mac = g_context.mac;
	//ueInfo* ue = &mac->ue[0];
	rnti_t rnti = INVALID_U16;
	uint32_t data_size = 0;
	uint8_t* dataptr = NULL;

	for (uint32_t i = 0; i < ue_num; i++)
	{
		ueIndex = find_ue(ind->sdu_pdu_info[i].rnti);

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
	msgDef msg;
	uint32_t msg_len = 0;
	msgId msgid = 0;

	while (1)
	{
		msg_len = message_receive(MAC_MAIN_TASK, (char *)&msg, 0);

		if (msg_len == 0)
		{
			return;
		}

		msgid = get_msgId(&msg);

		switch (msgid)
		{
			case RLC_MAC_DATA_IND:
			{
				rlc_mac_data_ind *ind = (rlc_mac_data_ind *)msg.data;

				handle_rlc_data_result(ind);
				message_free(ind);
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

	if (buffer1 == 0 && buffer2 == 0)
	{
		return 0;
	}

	if(buffer1 > 0 && buffer2 == 0)
	{
		return -1;
	}

	if(buffer1 == 0 && buffer2 > 0)
	{
		return 1;
	}

	if (lc1.priority > lc2.priority)
	{
		return -1;
	}
	else if (lc1.priority < lc2.priority)
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
	//uint16_t ueIndex = INVALID_U16;
	uint16_t mcs = 0;
	uint16_t cqi = 0;
	mac_info_s *mac = g_context.mac;
	uint8_t harqId = get_harqId(subframe);
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
	g_context.mac->num1 = 0;
	g_context.mac->num2 = 0;

	g_sch.tx_info.dci_num = 0;
	g_sch.tx_info.sch_num = 0;
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

	mac_info_s *mac = g_context.mac;
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
		else if (ue->buffer.buffer_total > 0)
		{
			newTxUe++;
			totalUe++;
			ue->sch_info.pre_rbs_alloc = MIN(ue->sch_info.pre_rbs_alloc, available_rbs);
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

	mac_info_s *mac = g_context.mac;
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
		ue->sch_info.rb_start = first_rb;
		ue->sch_info.pre_rbs_alloc = rb_num;
		ue->harq[harqId].rbs_alloc = rb_num;
		ue->harq[harqId].rb_start = first_rb;
	}

	return rb_num;
}

void scheduler_resource_locate()
{
	uint16_t ueIndex = INVALID_U16;
	uint16_t num0 = 0;
	//uint16_t *scheduling_list0;
	mac_info_s *mac = g_context.mac;
	//ueInfo *ue = &mac->ue[0];
	uint32_t first_rb = get_first_rb(mac->bandwith);
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
	mac_info_s *mac = g_context.mac;

	scheduler_pre_handler(frame, subframe);

	num0 = mac->num0;
	scheduling_list0 = &mac->scheduling_list0[0];

	if(num0 > 0)
	{
		sort_ues(scheduling_list0, num0);
	}

	if (mac->alloc_pattern)
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
	int32_t pre_rbs_alloc = sch_info->pre_rbs_alloc;
	uint16_t index = 0;

	uint32_t rbg_size = 4;
	uint32_t rbs_req = 0;
	uint32_t header_size = 0;
	uint32_t buffer_total = 0;

	if (harq->reTx)
	{
		return;
	}

	buffer->lc_num = 0;

	for (uint16_t i = 0; i < lc_num && pre_rbs_alloc > 0; i++)
	{
		index = lc_index[i];

		if (buffer->buffer_size[index] > 0)
		{
			header_size = get_logic_channel_header_size(buffer->buffer_size[index]);

			rbs_req = 0;
			rbs_req = rbg_size;
			buffer_total = buffer->buffer_size[index] + header_size;

			while (rbs_req < buffer_total)
			{
				rbs_req += rbg_size;
			}

			if (rbs_req > buffer_total)
			{
				while (rbs_req <= buffer_total)
					rbs_req--;
			}

			//pre_rbs_alloc = pre_rbs_alloc - rbs_req;

			if (pre_rbs_alloc - rbs_req < 0)
			{
				while ((pre_rbs_alloc - rbs_req) >= 0)
					rbs_req--;
			}

			pre_rbs_alloc = pre_rbs_alloc - rbs_req;
			buffer->lc_rbs_alloc[index] = rbs_req;

			buffer->lc_priority_index[buffer->lc_num++] = index;
		}
	}

	if (pre_rbs_alloc > 0)
	{
		LOG_WARN(MAC, "there are still rbs unassgin pre_rbs_alloc:%d", pre_rbs_alloc);
	}
}

void handle_ue_result(ueInfo* ue, const sub_frame_t subframe)
{
	uint16_t lc_num = ue->lc_num;
	lc_config* lc_config = &ue->lc_config[0];

	txBuffer* buffer = &ue->buffer;
	//schedule_info* sch_info = &ue->sch_info;

	sort_lc_params params = {lc_config, buffer};

	uint16_t lc_index[lc_num];

	for (uint16_t i = 0; i < lc_num; i++)
	{
		lc_index[i] = i;
	}

	if (ue->ce_num > 0)
	{
		handle_ue_macce(ue);
	}

	sort_logic_channel(lc_index, lc_num, &params);

	handle_ue_logic_channel(ue, lc_index, lc_num, subframe);
}

bool fill_dci_result(ueInfo* ue, dci_ind* dci, uint16_t dci_num, const uint32_t rb_num, const uint32_t rb_start, const uint8_t mcs)
{
	int32_t cce_offset = -1;
	uint16_t aggregation_level = 2;//get_aggregation_level(bandwith, EFORMAT0, 2);

	cce_offset = allocate_CCE(aggregation_level);

	if (cce_offset >= 0)
	{
		dci[dci_num].scheduled = false;
		dci[dci_num].rnti = ue->rnti;
		dci[dci_num].ueIndex = ue->ueIndex;

		dci[dci_num].cce_rb_num = aggregation_level;
		dci[dci_num].cce_rb = cce_offset;
		dci[dci_num].rb_num = rb_num;
		dci[dci_num].rb_start = rb_start;
		dci[dci_num].mcs = mcs;
		dci[dci_num].data_ind = 2;
		dci[dci_num].ndi = 0;
		dci[dci_num].rv = 0;
		dci_num++;
	}
	else
	{	
		LOG_WARN(MAC, "No CCE Resoure for ue ueIndex:%u", ue->ueIndex);
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
	const uint8_t rv)
{
	sch[sch_num].scheduled = false;
	sch[sch_num].ueIndex = ue->ueIndex;
	sch[sch_num].rnti = ue->rnti;
	sch[sch_num].rb_start = rb_start;
	sch[sch_num].rb_num = rb_num;
	sch[sch_num].mcs = mcs;
	sch[sch_num].data_ind = 2; //TODO:
	sch[sch_num].modulation = 1;//TODO: not define yet
	sch[sch_num].rv = rv;
	//sch[sch_num].harqId = 
	//sch[sch_num].ack = 

	sch[sch_num].pdu_len = ue->buffer.buffer_total;
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

	if (ue->harq[harqId].reTx)
	{
		rb_start = ue->harq[harqId].rb_start;
		rb_num = ue->harq[harqId].rb_num;
		mcs = ue->harq[harqId].mcs;
		rv = get_rv(ue->harq[harqId].reTx_num);
	}
	else
	{
		rb_start = ue->sch_info.rb_start;
		rb_num = ue->sch_info.rb_num;
		mcs = ue->sch_info.mcs;
		rv = 0;
	}

	ret = fill_dci_result(ue, dci, dci_num, rb_num, rb_start, mcs);

	if (ret)
	{
		fill_sch_result(ue, sch, sch_num, rb_num, rb_start, mcs, rv);
	}
	else
	{
		return false;
	}

	tx->dci_num = dci_num;
	tx->sch_num = sch_num;

	return true;
}

void handle_schedule_result(const frame_t frame, const sub_frame_t subframe)
{
	uint16_t ueIndex = INVALID_U16;
	uint16_t num1 = 0;
	mac_info_s* mac = g_context.mac;
	ueInfo* ue = NULL;
	//uint32_t rb_num = get_rb_num(mac->bandwith);
	//uint32_t rb_alloc_count = 0;
	uint16_t num2 = 0;
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
}

void mac_rlc_data_request(const frame_t frame, const sub_frame_t subframe)
{
	mac_info_s* mac = g_context.mac;
	ueInfo* ue = &mac->ue[0];
	uint16_t ue_num = mac->num2;
	uint16_t ueIndex = mac->scheduling_list2[0];
	uint8_t harqId = get_harqId(subframe);
	uint16_t lc_index = 0;

	msgDef msg;
	mac_rlc_data_req *req;
	msgSize msg_size = sizeof(mac_rlc_data_req);

	rlc_data_req* data = NULL;

	if (!new_message(&msg, MAC_RLC_DATA_REQ, MAC_MAIN_TASK, RLC_TASK, msg_size))
	{
		LOG_ERROR(MAC, "mac_rlc_data_request, new mac message fail!");
		return;
	}

	req = (mac_rlc_data_req*)msg.data;
	req->sfn = frame;
	req->sub_sfn = subframe;
	req->ue_num = 0;

	for (uint32_t i = 0; i < ue_num; i++)
	{
		ueIndex = mac->scheduling_list2[i];
		ue = &mac->ue[ueIndex];

		if (ue->harq[harqId].reTx)
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
			data->mac_pdu_byte_size[j] = ue->buffer.buffer_size[lc_index];
			data->tb_size += ue->buffer.lc_rbs_alloc[lc_index];
		}
	}

	if (!message_send(RLC_TASK, (char *)&msg, sizeof(msgDef)))
	{
		LOG_ERROR(MAC, "MAC_RLC_DATA_REQ, send msg fail!");
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

	msgDef msg;
	PHY_PdcchSendReq* req;
	msgSize msg_size = sizeof(PHY_PdcchSendReq);

	if (new_message(&msg, MAC_PHY_PDCCH_SEND, MAC_MAIN_TASK, PHY_TASK, msg_size))
	{
		req = (PHY_PdcchSendReq*)msg.data;
		req->num_dci = dci_num + common_dci_num;
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
			req->dci[i].data[0] |= (common_dci[i].rb_num & 0X7F) >> 6;
			req->dci[i].data[1] = (common_dci[i].rb_num & 0X7F) << 2;
			req->dci[i].data[1] |= (common_dci[i].mcs & 0X1F) >> 3;
			req->dci[i].data[2] = (common_dci[i].mcs & 0X1F) >> 5;
			req->dci[i].data[2] |= (common_dci[i].data_ind & 0X03) << 3;
			req->dci[i].data[2] |= (common_dci[i].ndi & 0X01) << 2;
		}

		for (uint32_t i = 0; i < dci_num; i++)
		{
			if (dci[i].scheduled == false)
			{
				continue;
			}

			req->dci[i+common_dci_num].type = (dci_type_e)dci[i].type;
			req->dci[i+common_dci_num].rnti = dci[i].rnti;
			req->dci[i+common_dci_num].dci_rb_start = dci[i].cce_rb_num;
			req->dci[i+common_dci_num].dci_rb_num = dci[i].cce_rb;
			req->dci[i+common_dci_num].data_size = MAX_DCI_LEN;
			req->dci[i+common_dci_num].data[0] = (dci[i].rb_start & 0X7F) << 1;
			req->dci[i+common_dci_num].data[0] |= (dci[i].rb_num & 0X7F) >> 6;
			req->dci[i+common_dci_num].data[1] = (dci[i].rb_num & 0X7F) << 2;
			req->dci[i+common_dci_num].data[1] |= (dci[i].mcs & 0X1F) >> 3;
			req->dci[i+common_dci_num].data[2] = (dci[i].mcs & 0X1F) >> 5;
			req->dci[i+common_dci_num].data[2] |= (dci[i].data_ind & 0X03) << 3;
			req->dci[i+common_dci_num].data[2] |= (dci[i].ndi & 0X01) << 2;
		}

		if (!message_send(PHY_TASK, (char *)&msg, sizeof(msgDef)))
		{
			LOG_ERROR(MAC, "MAC_PHY_PDCCH_SEND fail");
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

	msgDef msg;
	PHY_PuschSendReq* req;
	msgSize msg_size = sizeof(PHY_PuschSendReq);

	if (new_message(&msg, MAC_PHY_PUSCH_SEND, MAC_MAIN_TASK, PHY_TASK, msg_size))
	{
		req = (PHY_PuschSendReq*)msg.data;
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
			if (sch[i].scheduled == false)
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

		if (!message_send(PHY_TASK, (char *)&msg, sizeof(msgDef)))
		{
			LOG_ERROR(MAC, "MAC_PHY_PUSCH_SEND fail");
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

void mac_scheduler()
{
	frame_t frame = g_sch.frame;
	sub_frame_t subframe = g_sch.subframe;

	schedule_ra(frame, subframe);

	schedule_ue(frame, subframe);

	handle_schedule_result(frame, subframe);

	mac_rlc_data_request(frame, subframe);

	//sleep(100us);
	handle_rlc_data_ind();

	mac_phy_data_send(frame, subframe);
}

