/**********************************************************
* @file emac_status.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_EMAC_STATUS_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_EMAC_STATUS_H_

typedef enum
{
	STATUS_NONE,
	STATUS_INIT,
    STATUS_SYNC,
    STATUS_OUTSYNC,
    STATUS_CONNECTION
}mac_status_e;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_EMAC_STATUS_H_ */
