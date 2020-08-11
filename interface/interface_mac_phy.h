/**********************************************************
* @file interface_mac_phy.h
* @brief  define msg structure for MAC and RRC
*
* @author	guicheng.liu
* @date 	2019/07/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#ifndef _SMARTLOGICTECH_INTERFACE_MAC_PHY_H
#define _SMARTLOGICTECH_INTERFACE_MAC_PHY_H

#include "typedef.h"
#include "d2d_system_global.h"

typedef enum{
	DCI_TYPE_SI,
	DCI_TYPE_UE
}dci_type_e;

#pragma pack(4)

typedef struct
{
	frame_t sfn;
	rnti_t  rnti;
	uint8_t dci_rb_start;//dci rb start
	uint8_t dci_rb_num;//dci rb num
	uint8_t ack_num;
	uint8_t ack_bits;//1:crc=OK, 0:crc=NG
	uint8_t dataFlag;//0:no data, 1:data, 2:PSS+DCI
	uint8_t data_size;
	uint8_t data[MAX_DCI_LEN];
}dci_info;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
	uint16_t num_dci;
	dci_info dci[MAX_DCI_NUM];
}PHY_PdcchSendReq;

typedef struct
{
	rnti_t rnti;

	uint32_t rb_start;
	uint32_t rb_num;
	uint8_t mcs;
	uint8_t data_ind;// 1:ack/nack, 2:data, 3:ack/nack + data

	uint8_t modulation;
	uint8_t rv;
	uint8_t harqId;
	uint8_t ack;//no use

	uint16_t buffer_id;//0:buffer 0,  1:buffer 1, 2:buffer 2
	uint16_t pdu_len;
	uint8_t *data;
}pusch_info;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
	uint32_t num;
	pusch_info pusch[MAX_TX_UE];
}PHY_PuschSendReq;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
	uint8_t mib[3];
}PHY_PBCHSendReq;

typedef struct
{
	rnti_t rnti;
	uint16_t crc;// 0:NACK, 1: ACK, 3:Not define

	uint16_t buffer_id;//0:buffer 0,  1:buffer 1, 2:buffer 2, 3:invalid
	uint16_t dataSize;//0:invalid
	uint8_t *dataptr;
}pusch_result;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
	uint16_t cellId;
	uint16_t num_ue;
	pusch_result result[MAX_RX_UE];
}PHY_PuschReceivedInd;//PHY_MAC_DECOD_DATA_RPT

typedef struct
{
	frame_t frame;
	sub_frame_t subframe;
	uint16_t cellId;
	uint16_t flag;
}PHY_PBCHReceivedInd;

typedef struct
{
	rnti_t rnti;
	uint8_t ack_num;
	uint8_t ack_bits;//0:NACK, 1:ACK
}ack_ind;

typedef struct
{
	frame_t frame;
	sub_frame_t subframe;
	uint32_t num;
	ack_ind ack[MAX_RX_UE];
}PHY_ACKInd;

typedef struct
{
	rnti_t rnti;
	uint16_t cqi;
}cqi_info;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
	uint16_t cellId;
	uint16_t num;
	cqi_info cqiInfo[MAX_RX_UE];
}PHY_CQIInd;

typedef struct
{
	rnti_t rnti;
	uint16_t status; //0:outOfSync, 1: inSync
}link_status;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
	uint32_t num;
	link_status status[MAX_RX_UE];
}PHY_LinkStatusReportInd;
#pragma pack()

#endif //_SMARTLOGICTECH_INTERFACE_MAC_PHY_H
