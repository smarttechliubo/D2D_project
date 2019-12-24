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
#include "d2d_system_global.h"
#include "interface_rrc_rlc.h"

typedef struct
{
	uint16_t valid;
	uint16_t ueId;
	rnti_t   rnti;
	uint16_t mode;
	bool      hasCCCH;
	uint16_t  ccch_data_size;
	uint8_t  ccch_data[128]; 

	uint32_t lc_num;//logical channel num
	uint32_t data_size[MAX_LOGICCHAN_NUM];
	char *data[MAX_LOGICCHAN_NUM];
	uint32_t *data_ptr;
}rlc_ue_info;

typedef struct
{
    frame_t     frame;
    sub_frame_t subframe;
	uint16_t    mode;
	uint16_t    cellId; // cell ID for PHY scrambling
	uint16_t    bandwith;
	uint16_t    num_ue;
	rlc_ue_info ue[D2D_MAX_USER_NUM];
}rlc_info;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RLC_SIM_H_ */

