/**********************************************************
* @file phy_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/11/05
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "messageDefine.h"
#include "d2d_message_type.h"
#include "log.h"
#include "msg_handler.h"
#include "interface_mac_phy.h"
#include "mytask.h"

void init_phy_sim()
{	
	message_int(PHY_TASK, ENONBLOCK);
	message_int(INTERFACE_TASK, ENONBLOCK);
}

void phyMsgHandler()
{
	msgDef msg;
	uint32_t msg_len = 0;
	msgId msg_id = 0;

	while (1)
	{ 
		msg_len = message_receive(PHY_TASK, (char *)&msg, msg_len);

		if (msg_len == 0)
		{
			continue;
		}

		msg_id = get_msgId(&msg);

		switch (msg_id)
		{
			case MAC_PHY_PBCH_TX_REQ:
			{
				break;
			}
			case MAC_PHY_PDCCH_SEND:
			{
				break;
			}
			case MAC_PHY_PUSCH_SEND:
			{
				break;
			}
			default:
				break;
		}
	}
}

void handle_phy_rx()
{
	
}

void *phy_thread()
{
	while(thread_wait(MAC_TASK, PERIODIC_4MS))
	{
		handle_phy_rx();
		phyMsgHandler();
	}

	return 0;
}

