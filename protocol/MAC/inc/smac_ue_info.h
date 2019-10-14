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
	sorting_policy_e policy;
}sort_params;

typedef struct
{
	bool reTx;
	uint8_t harqId;
	uint8_t reTx_num;

	uint16_t cqi;
	uint16_t mcs;
}harq_info;

typedef struct
{
	uint16_t cqi;
	uint16_t mcs;
	uint16_t coefficient;

	uint32_t pre_rbs_alloc;
}schedule_info;

typedef struct
{
	uint16_t priority;// value:0..16
	uint16_t lc_id;// value:0..16
}lc_config;

typedef struct
{
	uint8_t  chan_num;
	uint16_t buffer_total;
	uint8_t  chan_id[MAX_LOGICCHAN_NUM];
	uint32_t buffer_size[MAX_LOGICCHAN_NUM];
}txBuffer;

typedef struct
{
	bool active;
	uint16_t ueId;
	uint16_t ueIndex;
	rnti_t rnti;
	uint16_t padding;
	bool out_of_sync;
	uint16_t maxHARQ_Tx;
	uint16_t max_out_sync;
	uint16_t lc_num;
	lc_config lc_config[MAX_LOGICCHAN_NUM];

	txBuffer buffer;
	harq_info harq[MAX_HARQ_NUM];
	schedule_info sch_info;
}ueInfo;//ue

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_UE_INFO_H_ */
