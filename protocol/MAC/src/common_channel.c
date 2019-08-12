/**********************************************************
* @file common_channel.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/08/05
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "mac.h"
#include "typedef.h"

void schedule_mib(const frame_t frame, mac_info_s *mac)
{
	common_channel_s *common_channel = &mac->common_channel;

	if (common_channel->mib_flag)
	{
		uint8_t sfn = (uint8_t)((frame>>2)&0xFF);
		uint8_t pdcch_index = (uint8_t)(common_channel->mib.pdcch_config.rb_start_index);

		//sfn: 8bit, pdcch: 7bit, padding: 9bit
		common_channel->mib_pdu[0] = sfn;
		common_channel->mib_pdu[1] = (pdcch_index<<1)&0xFE;
		common_channel->mib_pdu[2] = 0;
	}
}

void schedule_sib(const frame_t frame, const sub_frame_t sub_frame, mac_info_s *mac)
{

}

void schedule_common(const frame_t frame, const sub_frame_t sub_frame, mac_info_s *mac)
{
	if ((sub_frame == 0) && (frame & 3) == 0)
		schedule_mib(frame, mac);
	if ((sub_frame == 0) && (frame & 3) == 0)
		schedule_sib(frame, sub_frame, mac);
}

