/**********************************************************
* @file smac_schedule_result.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_SCHEDULE_RESULT_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_SCHEDULE_RESULT_H_

#include "mac_defs.h"

typedef struct
{
	uint8_t rb_start;
	uint8_t rb_num;
	uint8_t modulation;
	uint8_t rv;

	uint8_t data_ind;// 1:ack/nack, 2:data, 3:ack/nack + data
	uint8_t ack;
	uint8_t pdu_len;
	uint8_t *data;
}sch_ind;

typedef struct
{
	uint8_t cce_rb;
	uint8_t cce_rb_num;

    uint8_t rb_start;
	uint8_t rb_num;
	uint8_t mcs;
	uint8_t data_ind;
	uint8_t ndi;
	uint8_t rv;
}dci_ind;

typedef struct
{
	uint16_t ueIndex;
	rnti_t rnti;
	dci_ind dci;
	sch_ind sch;
}tx_req_info;

typedef struct
{
	uint32_t num_tx;
	tx_req_info tx_info[MAX_TX_NUM];
}mac_tx_req;

typedef struct
{
	frame_t frame;
	sub_frame_t subframe;
	uint16_t cellId;
	uint16_t padding;
	mac_tx_req tx_req;
}schedule_result_s;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_SCHEDULE_RESULT_H_ */
