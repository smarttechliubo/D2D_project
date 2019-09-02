/**********************************************************
* @file ra.h
* 
* @brief  mac ra header
* @author   guicheng.liu
* @date     2019/08/29
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_RA_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_RA_H_

#include "typedef.h"

typedef enum {
    RA_IDLE = 0,
    RA_MSG1,
	RA_MSG2,
    RA_MSG3,
    RA_MSG4
   // RA_WAITMSG4ACK,
   // RA_MSGCRNTI,
   // RA_MSGCRNTI_ACK
} ra_state;

typedef struct
{
	uint16_t ueIndex;
	rnti_t   rnti;     
	uint16_t ra_rnti;
	uint16_t padding;
	ra_state state;
	struct ra_list *next;
}ra_list;

typedef struct
{
	uint16_t cellId;
	uint16_t max_ra_tx;
	uint16_t ra_num;
	uint16_t padding;
	ra_list  *ra_list;
	ra_list  *tail;
}ra_t;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_RA_H_ */
