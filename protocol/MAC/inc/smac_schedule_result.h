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
	bool  scheduled;
	uint16_t ueIndex;
	rnti_t rnti;

	uint32_t rb_start;
	uint32_t rb_num;
	uint8_t mcs;
	uint8_t data_ind;// 1:ack/nack, 2:data, 3:ack/nack + data

	uint8_t modulation;
	uint8_t rv;
	uint8_t harqId;
	uint8_t ack;

	uint16_t padding_len;
	uint16_t pdu_len;
	uint8_t *data;
}sch_ind;

typedef struct
{
	bool scheduled;
	uint32_t type;//0:SI, 1:UE
	uint16_t ueIndex;
	rnti_t rnti;

	uint8_t cce_rb;//dci rb start
	uint8_t cce_rb_num;//dci rb num
	uint16_t padding;
	//dci
    uint32_t rb_start;
	uint32_t rb_num;
	uint8_t mcs;
	uint8_t data_ind;// 1:ACK/NACK; 2:DATA;  3:DATA + ACK/NACK
	uint8_t ndi;
	uint8_t rv;
}dci_ind;

typedef struct
{
	uint16_t dci_num;
	uint16_t sch_num;
	dci_ind dci[MAX_TX_UE];
	sch_ind sch[MAX_TX_UE];
}tx_req_info;

typedef struct
{
	frame_t frame;
	sub_frame_t subframe;
	uint16_t cellId;
	uint16_t padding;
	tx_req_info tx_info;
	tx_req_info common;
}schedule_result_s;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_SCHEDULE_RESULT_H_ */
