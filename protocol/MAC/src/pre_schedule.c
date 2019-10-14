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
#include "msg_queue.h"

void handle_buffer_status_req(const frame_t frame, const sub_frame_t subframe)
{
	msgDef msg;

	mac_rlc_buf_status_req *req;
	msgSize msg_size = sizeof(mac_rlc_buf_status_req);

	msg.data = (uint8_t*)msg_malloc(msg_size);
	
	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RLC_BUF_STATUS_REQ;
		msg.header.source = MAC_TASK;
		msg.header.destination = RLC_TASK;
		msg.header.msgSize = msg_size;
	
		req = (mac_rlc_buf_status_req*)msg.data;
		req->sfn = frame;
		req->sub_sfn = subframe;
	
		if (msgSend(RLC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
			//LOG_ERROR(MAC, "mac_rlc_buf_status_req send");
		}
	
		//msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "handle_buffer_status_req new rlc message fail!");
	}
}

void select_cs0(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	
}

void handle_ue_status(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	select_cs0(frame, subframe, mac);
}

void pre_schedule(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	g_sch.frame = frame;
	g_sch.subframe = subframe;

	handle_buffer_status_req(frame, subframe);

	if (mac->mode == 0)//source
	{
		schedule_common(frame, subframe, mac);
	}

	handle_ue_status(frame, subframe, mac);
	//schedule_ra(frame, subframe, mac);
}

