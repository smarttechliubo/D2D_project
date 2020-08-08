/**********************************************************
* @file mac_cce.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/03
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_CCE_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_CCE_H_

#include "mac_defs.h"

int32_t get_cce_offset(const uint16_t rb_num, const uint16_t rb_start_index, const uint16_t aggregation_level);
int32_t allocate_CCE(const uint16_t      aggregation_level);
uint16_t get_aggregation_level(const uint16_t bandwith, const uint8_t dci_fmt, const uint16_t cqi);

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_CCE_H_ */
