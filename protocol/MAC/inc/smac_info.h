/**********************************************************
* @file struct_mac_info.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_INFO_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_INFO_H_

#include "typedef.h"
#include "emac_mode.h"
#include "emac_status.h"
#include "smac_common_channel.h"
#include "smac_ue_info.h"

typedef struct
{
    frame_t     frame;
    sub_frame_t subframe;
	mode_e      mode;
	mac_status_e status;
	uint16_t    cellId; // cell ID for PHY scrambling
	uint16_t    bandwith;
	uint16_t    max_rbs_per_ue;
	uint16_t    subframe_config;// frame type config
	uint32_t    cce_bits;//1: in use, 0: available
	uint16_t    rb_num;//pdcch rb num
	uint16_t    rb_start_index; // pdcch rb start
	uint8_t     rb_available[MAX_RBS];
	common_channel_s common_channel;
	ue_info_s ue;
}mac_info_s;//cell

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_INFO_H_ */
