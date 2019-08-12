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

#define TIMING_SYNC_PERIOD 20

typedef enum 
{
	RRC_IDL,
    RRC_SYNC,
    RRC_CONNECTION
} rrc_status_e;

typedef enum
{
	SOPURCE,
	DESTINATION
}mode_e;

typedef struct
{

}ue_info_s;//ue

typedef struct{
	//uint16_t cellId; //keep cell ID for PHY scrambling
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
	uint16_t    cellId; // cell ID for PHY scrambling
	uint16_t    bandwith;
	uint16_t    subframe_config;// frame type config
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

void init_mac();

void mac_reset();
void *run_thread();


#endif //_SMARTLOGICTECH_MAC_H
