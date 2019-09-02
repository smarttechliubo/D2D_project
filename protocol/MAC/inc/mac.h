/**********************************************************
* @file mac.h
* @brief  define structure for MAC
*
* @author	guicheng.liu
* @date 	2019/07/27
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#ifndef _SMARTLOGICTECH_MAC_H
#define _SMARTLOGICTECH_MAC_H


#include "mac_defs.h"
#include "interface_rrc_mac.h"
#include "interface_mac_phy.h"


#define TIMING_SYNC_PERIOD 20

typedef enum 
{
	STATUS_NONE,
	STATUS_INIT,
    STATUS_SYNC,
    STATUS_OUTSYNC,
    STATUS_CONNECTION
} mac_status_e;

typedef enum
{
	SOPURCE,
	DESTINATION
}mode_e;

typedef struct
{

}ue_info_s;//ue

typedef struct{
	//uint16_t cellId;
	//uint16_t bandWidth;
	bool mib_flag;
	bool sib_flag;
	bcch_mib_info_s mib;
	bcch_si_info_s si;
	//ra_info_s ra; //connection setup info
	pdu_t mib_pdu[MIB_PDU_SIZE];
	pdu_t sib_pdu[SIB_PDU_SIZE];
}common_channel_s;

typedef struct
{
#if 0
    void *arg;
    pthread_t thread;
    pthread_attr_t attr;
#endif
}task_info_s;

typedef struct
{
    frame_t     frame;
    sub_frame_t subframe;
	mode_e      mode;
	mac_status_e status;
	uint16_t    cellId; // cell ID for PHY scrambling
	uint16_t    bandwith;
	uint16_t    max_rbs_per_ue;
	uint16_t    subframe_config;// frame type config
	uint32_t    cce_bits;//1: in use, 0: available
	pdcch_config_s pdcch_config;
	common_channel_s common_channel; 
	ue_info_s ue;
}mac_info_s;//cell

typedef struct
{
    frame_t frame;
    sub_frame_t subframe;
#ifdef TASK_MAC
	task_info_s task;
#endif
	mac_info_s *mac;
}context_s;

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

void init_mac();

void mac_reset();
void *run_thread();


#endif //_SMARTLOGICTECH_MAC_H
