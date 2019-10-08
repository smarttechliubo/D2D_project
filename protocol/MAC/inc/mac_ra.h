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
#include "smac_info.h"
#include "emac_mode.h"
#include "interface_mac_rlc.h"

typedef enum {
    RA_IDLE = 0,
	RA_ADDED,
    RA_MSG1_SEND,
	RA_MSG1_RECEIVED,
	RA_MSG2_SEND,
	RA_MSG2_RECEIVED,
    RA_MSG3_SEND,
	RA_MSG3_RECEIVED,
    RA_MSG4_SEND,
	RA_MSG4_RECEIVED,
   // RA_WAITMSG4ACK,
   // RA_MSGCRNTI,
   // RA_MSGCRNTI_ACK
} ra_state;

typedef struct node
{
	uint16_t   raId;     
	uint16_t ra_rnti;
	uint16_t ra_timer;
	ra_state state;
	uint32_t dataSize;
	struct node *next;
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

void init_ra(const uint16_t cellId);
bool add_ra(const uint16_t cellId, mode_e mode);
void schedule_ra(const frame_t frame, const sub_frame_t subframe);
bool update_ra_state(const rnti_t rnti);
void update_ra_buffer(rlc_buffer_rpt buffer);

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_RA_H_ */
