/**********************************************************
* @file mac_rx.c
* @brief  mac rx function
*
* @author	guicheng.liu
* @date 	2019/07/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "typedef.h"
#include "interface_mac_phy.h"
#include "d2d_message_type.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

void init_mac_rx()
{

}

void handle_phy_msg()
{
	msgDef msg;
	uint32_t msg_len = 0;

	while(1)
	{
		msg_len = msgRecv(PHY_MAC_QUEUE, (char *)&msg, MQ_MSGSIZE);

		if (msg_len == 0)
		{
			return;
		}

		switch (msg.header.msgId)
		{
			case PHY_MAC_PBCH_PDU_RPT:
			{
				
				break;
			}
			case PHY_MAC_DECOD_DATA_RPT:
			{
				break;
			}
			case PHY_MAC_ACK_RPT:
			{
				break;
			}
			case PHY_MAC_CQI_IND:
			{
				break;
			}
			case PHY_MAC_LINK_STATUS_IND:
			{
				break;
			}
			default:
				break;
		}
	}
}


