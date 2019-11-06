/**********************************************************
* @file rlc_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "rlc_sim.h"
#include "mytask.h"
#include "messageDefine.h"
#include "d2d_message_type.h"
#include "log.h"
#include "msg_handler.h"
#include "interface_mac_rlc.h"

rlc_info g_rlc;
//static uint32_t g_runtime = 0;

void init_rlc_sim()
{
	//g_runtime = 0;
	message_int(RLC_TASK, EBLOCK);
}

void handle_mac_rlc_buf_status_req(const mac_rlc_buf_status_req *req)
{
	msgDef msg;
	rlc_mac_buf_status_rpt *rpt;
	msgSize msg_size = sizeof(rlc_mac_buf_status_rpt);

	if (new_message(&msg, RLC_MAC_BUF_STATUS_RPT, RLC_TASK, MAC_MAIN_TASK, msg_size))
	{
		rpt = (rlc_mac_buf_status_rpt*)msg.data;
		rpt->sfn = req->sfn;
		rpt->sub_sfn = req->sub_sfn;

		//for (uint32_t i = 0; i <)
		{
			
		}
	}
}

void rlcMsgHandler()
{
	msgDef msg;
	uint32_t msg_len = 0;
	msgId msg_id = 0;

	while (1)
	{ 
		msg_len = message_receive(RLC_TASK, (char *)&msg, msg_len);

		if (msg_len == 0)
		{
			continue;
		}

		msg_id = get_msgId(&msg);

		switch (msg_id)
		{
			case MAC_RLC_BUF_STATUS_REQ:
			{
				mac_rlc_buf_status_req *req = (mac_rlc_buf_status_req *)msg.data;

				handle_mac_rlc_buf_status_req(req);
				message_free(req);
				break;
			}
			case MAC_RLC_DATA_REQ:
			{
				break;
			}
			case MAC_RLC_DATA_RPT:
			{
				break;
			}
			default:
			{
				LOG_ERROR(MAC, "UNknown RLC msg, msgId:%u", msg_id);
				break;
			}
		}
	}
}

void *rlc_thread()
{

	rlcMsgHandler();

	return 0;
}


