/**********************************************************
* @file rrc_sim.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/07
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RRC_SIM_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RRC_SIM_H_

#include "typedef.h"

typedef enum
{
	RRC_NONE,
	RRC_INITAIL,
	RRC_SETUP
}rrc_status;

typedef enum
{
	RRC_IDLE,
	RRC_CONNECT
}rrc_ue_status;

typedef struct
{
	uint16_t ueId;
	rrc_ue_status status;
}rrc_ue_info;

typedef struct
{
    frame_t     frame;
    sub_frame_t subframe;
	rrc_status status;
	uint16_t    mode;
	uint16_t    cellId; // cell ID for PHY scrambling
	uint16_t    bandwith;

	rrc_ue_info ue;
}rrc_info;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RRC_SIM_H_ */
