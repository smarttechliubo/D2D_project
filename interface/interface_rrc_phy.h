/**********************************************************
* @file interface_rrc_phy.h
* 
* @brief  
* @author   xiangyang.zhang
* @date     2019/08/08
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_INTERFACE_INTERFACE_RRC_PHY_H_
#define _SMARTLOGICTECH_INTERFACE_INTERFACE_RRC_PHY_H_

typedef struct{
	uint16_t  hop_mode;//0£ºINTER_SUBFRAME, 1:INTRA_AND_INTER_SUBFRAME
	uint16_t  pusch_hop_offset;//value=0..98
}pusch_config_basic_s;

typedef struct{
	uint16_t  grp_hop_enabled;//0:GP_HOPPING_DISABLED, 1:GP_HOPPING_ENABLED
	uint16_t  grp_assign_pusch;//value=0..29
	uint16_t  seq_hop_enabled;//0:SQ_HOPPING_DISABLED, 1:SQ_HOPPING_ENABLED
	uint16_t  cyc_shift;//value=0..7
}ul_ref_signal_pusch_s;

typedef struct{
	uint16_t cellId;//value=0..503
	uint16_t bandwith;//0: 1.5m,1: 3m, 2: 6m, 3: 12m, 4: 20m
	uint16_t subframe_config;//0:DDUUDDUU  1: NOT define
	uint16_t dl_freq; //DL freq
	uint16_t ul_freq; //UL freq
	pusch_config_basic_s  pusch_config_basic;
	ul_ref_signal_pusch_s ul_ref_signal_pusch;
}rrc_phy_initial_req; //RRC_PHY_INITIAL_REQ

typedef struct{
	pusch_config_basic_s  pusch_config_basic;
	ul_ref_signal_pusch_s ul_ref_signal_pusch;
}rrc_phy_bcch_para_cfg_req; //RRC_PHY_BCCH_PARA_CFG_REQ


typedef struct{
	uint16_t  beta_off_ack_ind;//value=0..15
	uint16_t  reseved;
}pusch_dedi_config_s;

typedef struct{
	pusch_dedi_config_s pusch_dedi_config;
}rrc_phy_connection_setup; //RRC_PHY_CONNECTION_SETUP_CFG_REQ

typedef struct{
	uint16_t dl_freq; //DL freq
	uint16_t ul_freq; //UL freq
	uint16_t padding;
}rrc_phy_cs_req; //RRC_PHY_CS_REQ

#endif /* _SMARTLOGICTECH_INTERFACE_INTERFACE_RRC_PHY_H_ */
