/**********************************************************
* @file mac_ue.h
* 
* @brief  ue info haeder
* @author   guicheng.liu
* @date     2019/08/29
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_UE_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_UE_H_

#include "mac_defs.h"
#include "interface_rrc_mac.h"

void mac_user_setup(const rrc_mac_connnection_setup *req);
void mac_user_release(const rrc_mac_release_req *req);


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_UE_H_ */
