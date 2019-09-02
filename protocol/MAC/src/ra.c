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

#include "ra.h"
#include "log.h"
#include "mac_defs.h"
#include "ue_index.h"

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
		node->ra_rnti = RA_RNTI;//set a default value for D2D
		ueIndex = new_index();
		if (ueIndex == INVALID_U16)
		{
			LOG_ERROR(MAC, "no invalid ue index");
			return NULL;
		}
		node->ueIndex = ueIndex;
		node->rnti = new_rnti(cellId, ueIndex);
		node->state = RA_IDLE;
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

void remove_ra(const rnti_t    rnti, const bool free)
{
	ra_list *ra = g_ra.ra_list;
	ra_list * node = NULL;

	if (ra == NULL)
	{
		return;
	}
	else if (ra->rnti == rnti)
	{
		if (free)
			free_index(ra->ueIndex);
		node = ra;
		ra = ra->next;
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
				if (free)
					free_index(ra->next->ueIndex);
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
}

bool add_ra(const uint16_t cellId)
{
	ra_list* ra = new_ra(cellId);
	if (g_ra.ra_num < MAX_RA_NUM && ra != NULL)
	{
		ra_push_back(ra);
		g_ra.ra_num++;
	}
	else
	{
		LOG_ERROR(MAC, "add new ra fail! ra_num:%u, ra:%u", g_ra.ra_num, ra != NULL);
		return false;
	}
	return true;
}


//void schedule_ra(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
//{
	
//}


