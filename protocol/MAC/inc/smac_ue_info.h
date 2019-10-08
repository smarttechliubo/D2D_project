/**********************************************************
* @file smac_ue_info.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_UE_INFO_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_UE_INFO_H_

typedef struct
{
	rnti_t rnti;
	uint16_t maxHARQ_Tx;
	uint16_t max_out_sync;
}ueInfo;//ue

typedef struct
{
	bool active;
	uint16_t ueId;
	uint16_t ueIndex;
	ueInfo *ue;
}ueContext;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_UE_INFO_H_ */
