/**********************************************************
* @file ue_index.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/08/31
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "ue_index.h"
#include "mac_defs.h"

static uint16_t ringBuffer[MAX_UE] = {0,};
static uint16_t free_start = 0;
static uint16_t free_end = MAX_UE;
static uint16_t ue_num;

void init_index()
{
	ue_num = 0;

	for (uint16_t i = 0; i < MAX_UE; i++)
	{
		ringBuffer[i] = i;
	}
}

uint16_t new_index()
{
	uint16_t index = INVALID_U16;

	if (ue_num >= MAX_UE)
	{
		return INVALID_U16;
	}

	index = ringBuffer[free_start];
	
	free_start++;
	if (free_start == MAX_UE)
	{
		free_start = 0;
	}

	ue_num++;
	if (abs(free_start-free_end) != ue_num)
	{
		return INVALID_U16;
	}
	return index;
}

uint16_t release_index(const uint16_t index)
{
	if (index >= MAX_UE && ue_num > 0)
	{
		return INVALID_U16;
	}

	ringBuffer[free_end] = index;
	free_end++;
	if (free_end == MAX_UE)
	{
		free_end = 0;
	}
	ue_num--;

	return free_end;
}
