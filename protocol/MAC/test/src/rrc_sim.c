/**********************************************************
* @file rrc_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/03
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "rrc_sim.h"
#include "mytask.h"
#include "messageDefine.h"
#include "interface_rrc_mac.h"
#include "d2d_message_type.h"
#include "log.h"
#include "msg_handler.h"

//rrc_info g_rrc;
static uint32_t g_runtime = 0;
uint16_t g_rrc_mode = 0;//0:source, 1:destination


void init_rrc_sim(const uint16_t mode)
{
	g_runtime = 0;
	g_rrc_mode = mode;

	if (mode == 0)
		init_rrc_src_sim();
	else
		init_rrc_dst_sim();

	message_int(RRC_TASK, ENONBLOCK);
}

uint16_t get_rrc_mode()
{
	return g_rrc_mode;
}
void rrcMsgHandler()
{
	msgDef msg;
	uint32_t msg_len = 0;
	msgId msg_id = 0;

	while (1)
	{
		msg_len = message_receive(RRC_TASK, (char *)&msg, msg_len);

		if (msg_len == 0)
		{
			return;
		}

		msg_id = get_msgId(&msg);

		switch (msg_id)
		{
			case MAC_RRC_BCCH_MIB_RPT:
			case MAC_RRC_BCCH_SIB1_RPT:
			{
				rrcDstMsgHandler(&msg, msg_id);
				break;
			}
			case MAC_RRC_BCCH_PARA_CFG_CFM:
			{
				rrcSrsMsgHandler(&msg, msg_id);
				break;
			}
			case MAC_RRC_INITIAL_CFM:
			case MAC_RRC_RELEASE_CFM:
			case MAC_RRC_OUTSYNC_RPT:
			case MAC_RRC_CCCH_RPT:
			case MAC_RRC_CONNECT_SETUP_CFG_CFM:
			{
				if (g_rrc_mode == 0)
				{
					rrcSrsMsgHandler(&msg, msg_id);
				}
				else
				{
					rrcDstMsgHandler(&msg, msg_id);
				}

				break;
			}
			default:
			{
				LOG_ERROR(RRC, "unknown rrc msg received, msgId:%u",msg_id);
				break;
			}
		}
	}
}

void *rrc_thread()
{
	uint32_t timer_fd = 0;
	uint32_t period_us = 4000;

	if (!make_timer(period_us, &timer_fd, true))
	{
		LOG_ERROR(RRC, "[TEST]: rrc_thread make periodic timer error");
		return NULL;
	}

	while(1)
	{
		rrcMsgHandler();

		if (g_rrc_mode == 0)
		{
			rrcSrcStatusHandler();
			rrcSrcUserStatusHandler();
		}
		else
		{
			rrcDstcStatusHandler();
			rrcDstUserStatusHandler();
		}

		if (g_runtime < 20)
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

