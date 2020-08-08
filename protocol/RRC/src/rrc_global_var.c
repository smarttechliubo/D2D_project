/******************************************************************
 * @file  rrc_global_var.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月13日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月13日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
 #include <typedef.h>
 #include <rrc_global_def.h>
 
 
 
 
/**************************global var*****************************/
rrc_init_var  g_rrc_init_para; 
rrc_init_var  g_dst_rrc_init_para; 
rrc_status_e  g_rrc_status; 

ue_info_dict  *g_rrc_ue_info_dict; 
uint8_t   g_rrc_messge_encode[ENCODE_MAX_SIZE] = {0}; 
uint16_t  g_rrc_mac_report_rnti; 
uint16_t  g_rrc_release_cnt = 0; 
uint16_t  g_rrc_release_ue_idx = 0; 
uint16_t  g_rrc_release_ue_rnti = 0; 
uint16_t  g_rrc_reestablish_max_cnt = 1;
uint16_t  g_rrc_reestablish_cnt = 0;