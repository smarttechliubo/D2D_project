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
#include "interface_mac_rlc.h"
#include "common_channel.h"

void handle_buffer_status(const rlc_mac_buf_status_rpt *buf)
{
	
}

void pre_schedule(const frame_t frame, const sub_frame_t sub_frame, mac_info_s *mac)
{
	//handle_buffer_status();
	schedule_common(frame, sub_frame, mac);
}

