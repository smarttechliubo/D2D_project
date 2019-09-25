/**********************************************************
* @file ra.c
* 
* @brief  define mac ue ra procedure
* @author   guicheng.liu
* @date     2019/08/29
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "mac_ra.h"
#include "log.h"
#include "mac_defs.h"
#include "ue_index.h"
#include "mac_vars.h"

ra_t g_ra;

void init_ra(const uint16_t cellId)
{
	g_ra.cellId = cellId;
	g_ra.max_ra_tx = 4;
	g_ra.ra_num = 0;
	g_ra.ra_list = NULL;
	g_ra.tail = NULL;

	init_index();
}

ra_list* createNode()
{
	ra_list*node = (ra_list*)malloc(sizeof(ra_list));
	if (node == NULL)
	{
		LOG_ERROR(MAC, "memory alloc for new ra ue fail!");
		return NULL;
	}
	node->next = NULL;
	return node;
}

rnti_t new_rnti(const uint16_t cellId, const uint16_t ueIndex)
{
	return (cellId + 1) * 1000 + ueIndex;
}

ra_list* new_ra(const uint16_t cellId)
{
	ra_list *node = createNode();
	uint16_t ueIndex = INVALID_U16;

	if (node != NULL)
	{
		node->ra_rnti = RA_RNTI;//TODO: set a default value for D2D
		ueIndex = new_index();
		if (ueIndex == INVALID_U16)
		{
			LOG_ERROR(MAC, "no invalid ue index");
			return NULL;
		}
		node->ueIndex = ueIndex;
		node->rnti = new_rnti(cellId, ueIndex);
		node->state = RA_IDLE;
		node->dataSize = 0;
		node->next = NULL;
	}
	return node;
}

void ra_push_back(ra_list* ra)
{
	if (g_ra.ra_list == NULL)
	{
		g_ra.ra_list = ra;
		g_ra.ra_list->next = NULL;
		g_ra.tail = g_ra.ra_list;
	}
	else
	{
		g_ra.tail->next = ra;
		g_ra.tail = ra;
	}
}

void remove_ra(const rnti_t    rnti, const bool release)
{
	ra_list * node = NULL;
	ra_list * pre = NULL;
	ra_list * cur = g_ra.ra_list;

	if (cur == NULL)
	{
		return;
	}
	else
	{
		while(cur)
		{
			if (cur->rnti == rnti)
			{
				node = cur;

				if (release)
					release_index(cur->ueIndex);

				if (cur->next == NULL)
					g_ra.tail = pre;
				if (pre != NULL)
					pre->next = cur->next;
				else
					cur = cur->next;
				
				free(node);
				break;
			}
			pre = cur;
			cur = cur->next;
		}
	}
#if 0
	else if (ra->rnti == rnti)
	{
		if (release)
			release_index(ra->ueIndex);
		node = ra;
		ra = ra->next;
		g_ra.tail = ra->next;
		free(node);
	}
	else 
	{
		while (ra)
		{
			if (ra->next == NULL)
			{
				LOG_ERROR(MAC, "no such ue rnti:%u", rnti);
				break;
			}
			if (ra->next->rnti == rnti)
			{
				if (release)
					release_index(ra->next->ueIndex);
				node = ra->next->next;
				free(ra->next);
				ra->next = node;
				if (node == NULL)
					g_ra.tail = ra;
				break;
			}
			ra = ra->next;
		}
	}
#endif
}

bool add_ra(const uint16_t cellId, mode_e mode)
{
	ra_list* ra = NULL;
	
	if (g_ra.ra_num >= MAX_RA_NUM)
	{
		LOG_ERROR(MAC, "Too much ra ue! add new fail ra_num:%u, ra:%u", g_ra.ra_num, ra != NULL);
		return false;
	}
	
	ra = new_ra(cellId);

	if (ra != NULL)
	{
		ra_push_back(ra);
		g_ra.ra_num++;
	}
	else
	{
		LOG_ERROR(MAC, "add new ra fail! ra_num:%u, ra:%u", g_ra.ra_num, ra != NULL);
		return false;
	}

	ra->state = (mode == MAC_SRC) ? RA_MSG1_RECEIVED : RA_ADDED;
	ra->ra_timer = 0;

	return true;
}

ra_list* find_ra(rnti_t rnti)
{
	ra_list * ra = g_ra.ra_list;

	while(ra != NULL)
	{
		if (ra->ra_rnti == rnti)
			break;

		ra = ra->next;
	}
	return ra;
}

void update_ra_buffer(rlc_buffer_rpt buffer)
{
	ra_list* ra = NULL;
	rnti_t rnti = buffer.rnti;
	uint8_t logic_chan_num = buffer.logic_chan_num;

	if ((ra = find_ra(rnti)) == NULL)
	{
		LOG_ERROR(MAC, "ra does not exist!");
		return;
	}

	for(uint32_t i = 0; i < logic_chan_num; i++)// TODO: for ra ue, chan_num == 1
	{
		ra->dataSize += buffer.buffer_byte_size[i];
	}
}

void ra_msg1(const frame_t frame, const sub_frame_t subframe, ra_list *ra)
{
	mac_info_s *mac = g_context.mac;
	uint16_t bandwith = mac->bandwith;
	mac_tx_req *tx_req = &g_sch.tx_req;
	uint32_t msg1_len = ra->dataSize;
	uint32_t rb_max = get_rb_num(bandwith);
	//uint32_t rb_start_index = get_rb_start(bandwith);
	uint32_t rbg_size = get_rbg_size(bandwith);
	uint32_t rbs_req = 0;
	uint8_t mcs = 2;// TODO: for msg1, mcs=?
	uint32_t tbs = get_tbs(mcs, rbg_size);
	uint32_t first_rb = get_first_rb(bandwith);
	uint8_t harqId = get_harqId(subframe);

	if (first_rb > MAX_RBS)
	{
		LOG_ERROR(MAC, "No availabe resource for msg1!");
		return;
	}
		
	while (tbs < msg1_len)
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
	
	tx_req->tx_info[tx_req->num_tx].sch.rb_start = first_rb;
	tx_req->tx_info[tx_req->num_tx].sch.rb_num = rbs_req;
	tx_req->tx_info[tx_req->num_tx].sch.mcs = mcs;
	tx_req->tx_info[tx_req->num_tx].sch.data_ind = 2;
	tx_req->tx_info[tx_req->num_tx].sch.modulation = 2;//QPSK
	tx_req->tx_info[tx_req->num_tx].sch.rv = 0;
	tx_req->tx_info[tx_req->num_tx].sch.harqId = harqId;
	tx_req->tx_info[tx_req->num_tx].sch.ack = INVALID_U8;
	tx_req->tx_info[tx_req->num_tx].sch.pdu_len = msg1_len;
	tx_req->tx_info[tx_req->num_tx].sch.data = NULL;

	for (uint32_t i = first_rb; i < rbs_req; i++)
	{
		mac->rb_available[i] = 0;
	}
}

void schedule_ra(const frame_t frame, const sub_frame_t subframe)
{
	ra_list *ra = g_ra.ra_list;

	while (ra != NULL)
	{
		switch (ra->state)
		{
			case RA_ADDED:
			{
				if (ra->ra_timer >= MAX_RA_TIMER)
				{
					remove_ra(ra->rnti, true);
					LOG_ERROR(MAC, "Ra timer expired! RA fail");
					break;
				}

				ra->ra_timer++;

				if (ra->dataSize != 0)
				{
					ra_msg1(frame, subframe, ra);
				}
				break;
			}
			case RA_MSG1_SEND:
			{
				break;
			}
			case RA_MSG1_RECEIVED:
			{
				if (ra->ra_timer >= MAX_RA_TIMER)
				{
					remove_ra(ra->rnti, true);
					LOG_ERROR(MAC, "Ra timer expired! RA fail");
					break;
				}

				ra->ra_timer++;
				
				break;
			}
			case RA_MSG2_SEND:
			case RA_MSG2_RECEIVED:
			case RA_MSG3_SEND:
			case RA_MSG3_RECEIVED:
			case RA_MSG4_SEND:
			case RA_MSG4_RECEIVED:
				break;
			default:
				break;
		}

		ra = ra->next;
	}
}


