/**********************************************************
* @file mac_tx.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/08/27
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_TX_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_TX_H_

#include "messageDefine.h"

void init_mac_tx(uint16_t cellId);
void mac_scheduler();
void handle_buffer_status_ind(msgDef* msg);


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_TX_H_ */
