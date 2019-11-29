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
#include "mac_osp_interface.h"

//rrc_info g_rrc;
extern uint16_t g_testing_mode;//0:source, 1:destination

uint32_t init_rrc_sim()
{
	void* pTimer;
	int32_t ret;
	
	pTimer = _timerCreate(TASK_D2D_RRC, 1, 4,0);
	ret = _timerStart(pTimer);

	LOG_INFO(MAC,"init_rrc_sim pTimer is %p, ret:%u\r\n", pTimer, ret);

	if (g_testing_mode == 0)
		init_rrc_src_sim();
	else
		init_rrc_dst_sim();

	return 0;
}

uint16_t get_rrc_mode()
{
	return g_testing_mode;
}

void rrcMsgHandler(msgDef* msg)
{
	//msgDef* msg = NULL;
	msgId msg_id = 0;

	//while (1)
	{
		msg_id = get_msgId(msg);
		LOG_ERROR(RRC, "rrcMsgHandler msg_id:%u", msg_id);

		switch (msg_id)
		{
			case MAC_RRC_BCCH_MIB_RPT:
			case MAC_RRC_BCCH_SIB1_RPT:
			{
				rrcDstMsgHandler(msg, msg_id);
				break;
			}
			case MAC_RRC_BCCH_PARA_CFG_CFM:
			{
				rrcSrsMsgHandler(msg, msg_id);
				break;
			}
			case MAC_RRC_INITIAL_CFM:
			case MAC_RRC_RELEASE_CFM:
			case MAC_RRC_OUTSYNC_RPT:
			case MAC_RRC_CCCH_RPT:
			case MAC_RRC_CONNECT_SETUP_CFG_CFM:
			{
				if (g_testing_mode == 0)
				{
					rrcSrsMsgHandler(msg, msg_id);
				}
				else
				{
					rrcDstMsgHandler(msg, msg_id);
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

void rrc_sim_thread(msgDef* msg)
{	

	if (!is_timer(msg))
	{
		rrcMsgHandler(msg);
	}
	else if (g_testing_mode == 0)
	{
		rrcSrcStatusHandler();
		rrcSrcUserStatusHandler();
	}
	else if (g_testing_mode == 1)
	{
		rrcDstcStatusHandler();
		rrcDstUserStatusHandler();
	}

	message_free(msg);
}

/*
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
		rrcMsgHandler(msg);

		if (g_testing_mode == 0)
		{
			rrcSrcStatusHandler();
			rrcSrcUserStatusHandler();
		}
		else
		{
			rrcDstcStatusHandler();
			rrcDstUserStatusHandler();
		}

		wait_period(timer_fd);
	}

	return 0;
}
*/
