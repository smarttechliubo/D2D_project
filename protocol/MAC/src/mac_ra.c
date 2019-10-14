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
uint16_t g_raId = 0;

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
	ra_list* node = (ra_list*)malloc(sizeof(ra_list));
	if (node == NULL)
	{
		LOG_ERROR(MAC, "memory alloc for new ra ue fail!");
		return NULL;
	}
	node->next = NULL;
	return node;
}

rnti_t new_raId()
{
	for (uint16_t i = 0; i < 16; i++) // i <= MAX_RA_NUM
	{
		if ((g_raId & (0x01 << i)) == 0)
		{
			g_raId |= (0x01 << i);
			return i;
		}
	}
	return INVALID_U16;
}

ra_list* new_ra(const uint16_t cellId)
{
	ra_list *node = createNode();

	if (node != NULL)
	{
		node->ra_rnti = RA_RNTI;//TODO: set a default value for D2D
		node->raId = new_raId();
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

void remove_ra(const uint16_t    raId)
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
			if (cur->raId == raId)
			{
				node = cur;

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

	ra->state = (mode == EMAC_SRC) ? RA_MSG1_RECEIVED : RA_ADDED;
	ra->ra_timer = 0;

	return true;
}

ra_list* find_ra(const rnti_t rnti)
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

bool update_ra_state(const rnti_t rnti)
{
	ra_list* ra = NULL;
	ra_state state = RA_IDLE;

	if ((ra = find_ra(rnti)) == NULL)
	{
		LOG_ERROR(MAC, "ra does not exist!");
		return false;
	}

	state = ra->state;

	if (state == RA_ADDED)
	{
		ra->state = RA_MSG1_SEND;
	}
	
	else if (state == RA_MSG1_SEND)
	{
		ra->state = RA_MSG2_RECEIVED;
	}
	else if (state == RA_MSG2_RECEIVED)
	{
		ra->state = RA_MSG3_SEND;
		//remove_ra(rnti, false);
	}
	else if (state == RA_MSG1_RECEIVED)
	{
		ra->state = RA_MSG2_SEND;
	}
	else if(state == RA_MSG2_SEND)
	{
		ra->state = RA_MSG3_RECEIVED;
		//remove_ra(rnti, false);
		//add_ue(rnti, ra->ueIndex);
	}
	else
	{
		LOG_ERROR(MAC, "ra state incorrect, State:%u:", state);
		return false;
	}

	return true;	
}

void update_ra_buffer(rlc_buffer_rpt buffer)
{
	ra_list* ra = NULL;
	rnti_t rnti = buffer.rnti;
	uint8_t logic_chan_num = buffer.logic_chan_num;

	if (buffer.rnti == RA_RNTI)// dest
	{
		if (!add_ra(g_sch.cellId, EMAC_DEST))
		{
			LOG_ERROR(MAC, "add new ra ue fail! cellId:%u", g_sch.cellId);
		}	
	}

	ra = find_ra(rnti);

	if (ra != NULL && (ra->state == RA_ADDED ||  ra->state == RA_MSG1_RECEIVED ||  ra->state == RA_MSG2_RECEIVED))
	{	
		for(uint32_t i = 0; i < logic_chan_num; i++)// TODO: for ra ue, chan_num == 1
		{
			ra->dataSize += buffer.buffer_byte_size[i];
		}
	}
	else
	{
		LOG_ERROR(MAC, "ra does not exist!");
	}
}

void ra_msg(const frame_t frame, const sub_frame_t subframe, ra_list *ra)
{
	mac_info_s *mac = g_context.mac;
	uint16_t bandwith = mac->bandwith;
	tx_req_info *tx_info = &g_sch.tx_info;
	uint16_t sch_num = tx_info->sch_num;
	uint32_t msg_len = ra->dataSize;
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

	while (tbs < msg_len)
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

	tx_info->sch[sch_num].ueIndex = INVALID_U16;
	tx_info->sch[sch_num].rnti = ra->ra_rnti;

	tx_info->sch[sch_num].rb_start = first_rb;
	tx_info->sch[sch_num].rb_num = rbs_req;
	tx_info->sch[sch_num].mcs = mcs;
	tx_info->sch[sch_num].data_ind = 2;
	tx_info->sch[sch_num].modulation = 2;//QPSK
	tx_info->sch[sch_num].rv = 0;
	tx_info->sch[sch_num].harqId = harqId;
	tx_info->sch[sch_num].ack = INVALID_U8;
	tx_info->sch[sch_num].pdu_len = msg_len;
	tx_info->sch[sch_num].data = NULL;
	tx_info->sch_num++;

	for (uint32_t i = first_rb; i < rbs_req; i++)
	{
		mac->rb_available[i] = 0;
	}

	ra->dataSize = 0;

	LOG_INFO(MAC, "RA msg resource alloc, raId:%u, state:%u",ra->raId, ra->state);
}

bool check_ra_timer(ra_list *ra)
{
	if (ra->ra_timer >= MAX_RA_TIMER)
	{
		LOG_ERROR(MAC, "Ra timer expired! RA fail, raId:&=%u, state:%u", ra->raId, ra->state);
		remove_ra(ra->raId);
		return false;
	}

	ra->ra_timer++;

	return true;
}

void schedule_ra(const frame_t frame, const sub_frame_t subframe)
{
	ra_list *ra = g_ra.ra_list;

	while (ra != NULL)
	{
		if (ra->state > RA_IDLE)
		{
			if (!check_ra_timer(ra))
			{
			
			}

			if (ra->dataSize > 0)
			{
				ra_msg(frame, subframe, ra);
			}
		}
		ra = ra->next;
	}
}


