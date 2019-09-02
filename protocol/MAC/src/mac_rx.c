/**********************************************************
* @file mac_rx.c
* @brief  mac rx function
*
* @author	guicheng.liu
* @date 	2019/07/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "interface_mac_phy.h"
#include "d2d_message_type.h"

int init_mac_rx()
{

}

void handle_phy_msg()
{
	message_type_t msg_type = PHY_MAC_PBCH_PDU_RPT; // TODO: add msg header handler function
	PHY_PBCHReceivedInd *req; //TODO: add msg body handler function

	switch (msg_type)
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


