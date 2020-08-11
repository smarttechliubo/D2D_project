/**********************************************************
* @file struct_mac_common_channel.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_COMMON_CHANNEL_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_COMMON_CHANNEL_H_

#include "mac_defs.h"

typedef struct
{
	uint16_t rb_num;// value: 1..2, default: 2
	uint16_t rb_start_index; // value: 2..3, default: 2

	uint16_t sib_size;
	uint8_t *sib_pdu;
	uint8_t padding;
}mac_bch_info_s;

typedef struct{
	mac_bch_info_s bch_info;
	uint8_t mib_size;
	uint8_t sib_size;
	uint8_t padding[3];
	uint8_t mib_pdu[MIB_PDU_SIZE];
	uint8_t sib_pdu[SIB_PDU_SIZE];
}common_channel_s;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_COMMON_CHANNEL_H_ */
