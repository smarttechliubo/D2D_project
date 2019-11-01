/**********************************************************
* @file rlc_sim.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RLC_SIM_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RLC_SIM_H_

#include "typedef.h"

typedef struct
{
    frame_t     frame;
    sub_frame_t subframe;
	uint16_t    mode;
	uint16_t    cellId; // cell ID for PHY scrambling
	uint16_t    bandwith;

}rlc_info;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RLC_SIM_H_ */

