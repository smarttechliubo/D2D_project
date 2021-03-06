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
#include "mac_ra.h"
#include "log.h"
#include "interface_mac_rlc.h"
#include "d2d_message_type.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_handler.h"


void select_cs0(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	
}

void pre_ue_select(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	select_cs0(frame, subframe, mac);
}

void pre_schedule(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	g_sch.frame = frame;
	g_sch.subframe = subframe;
	g_sch.tx_info.dci_num = 0;
	g_sch.tx_info.sch_num = 0;

	//handle_buffer_status_req(frame, subframe);

	if (mac->mode == 0)//source
	{
		//schedule_common(frame, subframe, mac);
	}

	pre_ue_select(frame, subframe, mac);
	//schedule_ra(frame, subframe, mac);
}

