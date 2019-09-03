/**********************************************************
* @file mac_cce.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/03
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "mac_cce.h"
#include "mac_vars.h"

int32_t get_cce_offset(const uint16_t rb_num, const uint16_t rb_start_index, const uint16_t aggregation_level)
{
	bool find = false;
	const uint32_t cce_bits = g_context.mac->cce_bits;

	for (uint32_t i = 0; i < rb_num; i++)
	{
		find = true;
		for (uint32_t l = 0; l < aggregation_level; l++)
		{
			if((cce_bits &(1<<(rb_start_index + i + l))) !=0)
			{
				find = false;
				i = i + l;
				break;
			}
		}
		if(find == true)
		{
			return (rb_start_index + i);
		}
	}
	return -1;
}

int32_t allocate_CCE(const uint16_t      aggregation_level)
{
	mac_info_s *mac = g_context.mac;
	uint16_t rb_num = mac->rb_num;
	uint16_t rb_start_index = mac->rb_start_index;
	uint16_t candidates = rb_num/aggregation_level;
	
	int32_t cce_offset = get_cce_offset(rb_num, aggregation_level, rb_start_index);
	if (cce_offset >= 0)
	{
		for (uint32_t l = 0; l < aggregation_level; l++)
		{
			mac->cce_bits |= 1<<(cce_offset+l);
		}
	}
	
	return cce_offset;
}

