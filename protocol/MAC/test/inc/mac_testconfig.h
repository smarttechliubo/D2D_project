/**********************************************************
* @file mac.h
* @brief  define structure for MAC
*
* @author	guicheng.liu
* @date 	2019/07/27
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#ifndef _SMARTLOGICTECH_TESTCONFIG_H
#define _SMARTLOGICTECH_TESTCONFIG_H

#include "interface_rrc_mac.h"

typedef struct
{
	rrc_mac_initial_req init;

	rrc_mac_bcch_para_config_req bcch;
}mac_testConfig;

typedef struct
{
	uint32_t ue_num;

	rrc_mac_connnection_setup ue[2];
}mac_testUeConfig;

typedef struct
{
	bool DL_TX;
	bool UL_TX;
	bool RA_OK;
	bool TX_OK;
	bool IN_SYNC;
	bool OUT_SYNC;
	bool pad1;
	bool pad2;
	bool pad3;
	bool pad4;
	bool pad5;
}mac_testPolicy;


extern uint32_t init_rrc_sim();
extern uint32_t init_rlc_sim();
extern uint32_t init_phy_tx_sim();
extern uint32_t init_phy_rx_sim();
extern int32_t init_mac_period();
extern int32_t init_mac_scheduler();

extern void rrc_sim_thread(msgDef* msg);
extern void rlc_sim_thread(msgDef* msg);
extern void phy_tx_sim_thread(msgDef* msg);
extern void phy_rx_sim_thread(msgDef *msg);
extern void run_period(msgDef* msg);
extern void run_scheduler(msgDef* msg);

#endif //_SMARTLOGICTECH_TESTCONFIG_H
