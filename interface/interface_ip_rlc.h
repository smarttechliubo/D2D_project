/******************************************************************
 * @file  interface_ip_rlc.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年10月28日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年10月28日        bo.liu       create file

*****************************************************************/

#ifndef     INTERFACE_IP_RLC_H
#define     INTERFACE_IP_RLC_H
 
#include <interface_rrc_rlc.h>
#include <intertask_interface.h>

typedef struct ip_rlc_data_ind_s
{
   rb_type_e    rb_type;  /** 0:srb0; 1:srb1; 3:drb */
	rb_id_t     rb_id; 
	rnti_t      rnti;
	uint32_t    data_sn; 
	uint32_t    data_size; /**unit: byte*/
	uint32_t    *data_addr_ptr; /**data address*/
}ip_rlc_data_ind;


typedef struct rlc_ip_data_rpt_s
{
    frame_t              frame; 
    sub_frame_t          sub_frame; 
    rb_type_e            rb_type;  /** 0:srb0; 1:srb1; 3:drb */
	rb_id_t             rb_id; 
	rnti_t              rnti;
	uint32_t            data_sn; 
	uint32_t            data_size; /**unit: byte*/
	uint32_t            *data_addr_ptr; /**data address*/
}rlc_ip_data_rpt;
 
 
#endif
