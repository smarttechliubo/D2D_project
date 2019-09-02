/**********************************************************
* @file pre_schedule.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/08/05
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "mac.h"
#include "typedef.h"
#include "common_channel.h"
#include "mac_vars.h"
#include "interface_mac_rlc.h"

void handle_buffer_status(const rlc_mac_buf_status_rpt *buf)
{
	
}

void pre_schedule(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	sch.frame = frame;
	sch.subframe = subframe;

	//handle_buffer_status();
	if (mac->mode == 0)//source
	{
		schedule_common(frame, subframe, mac);
	}
	//schedule_ra(frame, subframe, mac);
}

