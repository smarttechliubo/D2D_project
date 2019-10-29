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

typedef struct
{
	dci_type_e type;
	rnti_t rnti;
	uint8_t data_size;
	uint8_t data[MAX_DCI_LEN];
}dci_info;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
	uint16_t num_dci;
	dci_info dci[MAX_DCI_NUM];
}PHY_PdcchSendReq;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
}PHY_PuschSendReq;

typedef struct{
	frame_t frame;
	sub_frame_t subframe;
}PHY_PBCHSendReq;

typedef struct
{
	rnti_t rnti;
	uint16_t crc;// 0:NACK, 1: ACK
	uint16_t dataSize;
	uint8_t *dataptr;
	uint8_t padding;
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
	uint16_t ack;//0:NACK, 1:ACK
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

#endif //_SMARTLOGICTECH_INTERFACE_MAC_PHY_H
