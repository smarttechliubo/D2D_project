/**********************************************************
* @file rrc_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/03
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "rrc_sim.h"
#include "mytask.h"
#include "msg_queue.h"
#include "messageDefine.h"
#include "interface_rrc_mac.h"
#include "d2d_message_type.h"
#include "log.h"

rrc_info g_rrc;
uint32_t g_runtime = 0;


void msgHandler()
{
	
}

void rrcStatusHandler()
{
	rrc_mac_initial_req *req;
	LOG_INFO(MAC, "[TEST]: rrcStatusHandler! 0");

	if (g_rrc.status == RRC_NONE)
	{
		msgSize msg_size = sizeof(rrc_mac_initial_req);
		msgDef* msg = (msgDef*)new_msg(msg_size);
		if (msg != NULL)
		{
			msg->header.msgId = RRC_MAC_INITIAL_REQ;
			msg->header.source = RRC_TASK;
			msg->header.destination = MAC_TASK;
			msg->header.msgSize = msg_size;

			req = (rrc_mac_initial_req*)msg->data;
			req->cellId = 0;
			req->bandwith = 1;
			req->pdcch_config.rb_num = 2;
			req->pdcch_config.rb_start_index = 2;
			req->subframe_config = 0;
			req->mode = 0;

			msgSend(MAC_QUEUE, (char *)msg, msg_size + sizeof(msgDef));
		}
		else
		{
			LOG_ERROR(MAC, "[TEST]: new rrc message fail!");
		}
	}
}


void *rrc_thread()
{
	uint32_t timer_fd = 0;
	uint32_t period_us = 4000;

	if (!make_timer(period_us, &timer_fd, true))
	{
		LOG_ERROR(MAC, "[TEST]: run_thread make periodic timer error");
		return NULL;
	}

	while(1)
	{
		rrcStatusHandler();
		msgHandler();
		if (g_runtime < 100)
		{
			g_runtime++;
			wait_period(timer_fd);
		}
		else 
		{
			break;
		}
	}
	return 0;
}


void init_rrc_sim()
{
	g_runtime = 0;

	g_rrc.cellId = 0;
	g_rrc.mode = 0;//source
	g_rrc.status = RRC_NONE;
	
	//msgq_init(RRC_QUEUE);
}



