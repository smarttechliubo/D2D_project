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
	uint32_t size;
	uint8_t data[8];
}mac_ce;

typedef struct
{
	bool reTx;
	uint8_t harqId;
	uint8_t reTx_num;
	uint16_t tbs;

    uint32_t rb_start;
	uint32_t rb_num;
	uint32_t rbs_alloc;//pre alloc

	uint16_t cqi;
	uint16_t mcs;
}harq_info;

typedef struct
{
	crc_e    crc[MAX_HARQ_NUM];
	uint16_t cqi;
	uint16_t mcs;
	uint16_t coefficient;
	uint16_t padding;

	uint32_t rb_start;
	uint32_t rb_num;

	uint32_t pre_tbs;
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
	uint8_t  padding[3];
	uint32_t buffer_total;
	uint8_t  chan_id[MAX_LOGICCHAN_NUM];
	uint32_t buffer_size[MAX_LOGICCHAN_NUM];

	uint32_t lc_num;//sorted logical channel
	uint16_t lc_priority_index[MAX_LOGICCHAN_NUM];
	uint32_t lc_tbs_req[MAX_LOGICCHAN_NUM];
}txBuffer;

typedef struct
{
	bool      active;
	bool      temp;
	uint16_t  ueId;
	uint16_t  ueIndex;
	rnti_t    rnti;
	uint16_t  maxHARQ_Tx;
	bool      out_of_sync;
	uint16_t  max_out_sync;
	uint16_t  out_sync_count;
	uint16_t  padding;

	uint16_t  lc_num;
	lc_config lc_config[MAX_LOGICCHAN_NUM];
	uint32_t  ce_num;
	mac_ce    macCE[2];

	//uint16_t cqi[4];

	txBuffer      buffer;
	schedule_info sch_info;
	harq_info     harq[MAX_HARQ_NUM];
}ueInfo;//ue

typedef struct
{
	sorting_policy_e policy;
}sort_ue_params;

typedef struct
{
	lc_config* config;
	txBuffer* buffer;
}sort_lc_params;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_SMAC_UE_INFO_H_ */
