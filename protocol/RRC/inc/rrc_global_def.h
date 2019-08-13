/******************************************************************
 * @file  rrc_global_def.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月13日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月13日        bo.liu       create file

*****************************************************************/

#ifndef      RRC_GLOBAL_DEF_H
#define      RRC_GLOBAL_DEF_H

#define  RRC_TYPE_SOURCE     1
#define  RRC_TYPE_DESTINATION  0

typedef enum
{
	BANDWIDTH_1P5M,
	BANDWIDTH_3M, 
	BANDWIDTH_6M,
	BANDWIDTH_12M,
	BANDWIDTH_20M
}bandwidth_e;

typedef enum
{
	SUBFRAME_CONFIG_DDUUDDUU, 
    NOT_DEFINED
}subframeconfig_e; 

typedef struct rrc_mib_info_s
{

	uint16_t  mib_sfn; 

	uint8_t   pdcch_rb_start; 
	uint8_t   pdcch_rb_num;

} rrc_mib_info; 


 
typedef struct rrc_init_var_s
{
	uint16_t  source_type;       //! 1: source; 0:destination
    uint16_t  cell_id;  
	
	bandwidth_e       band_width;     //!0: 1.5m,1: 3m, 2: 6m, 3: 12m, 4: 20m
	subframeconfig_e  subframe_config; //!0:DDUUDDUU  1: NOT define

	uint16_t  band_info; 
	uint16_t  padding; 
	
	uint16_t  dl_freq; 
	uint16_t  ul_freq; 

	rrc_mib_info  mib_info; 
 
}rrc_init_var; 













/**************************************extern var *******************************/
extern  rrc_init_var  g_rrc_init_para;


/*************************************extern function****************************/
 
 
#endif
