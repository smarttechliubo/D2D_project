/******************************************************************
 * @file  rlc_rrc.c
 * @brief:    [rlc receive rrc message and process the coresponding function] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc_type.h>
#include <interface_rrc_rlc.h>
#include <interface_mac_rlc.h>
#include <intertask_interface.h>
#include <log.h>
#include <d2d_message_type.h>


void rlc_Rrc_Configure_Cfm(uint32_t        message_id)
{

    MessageDef  *message; 
	rlc_rrc_initial_cfm *rlc_cfm = (rlc_rrc_initial_cfm *)OSP_Alloc_Mem(sizeof(rlc_rrc_initial_cfm)); 

	rlc_cfm->error_code = 0; 
	rlc_cfm->status = 1; //！pass


	message = itti_alloc_new_message(TASK_D2D_RLC, message_id,
	                       ( char *)rlc_cfm, sizeof(rlc_rrc_initial_cfm ));

	itti_send_msg_to_task(TASK_D2D_RRC,0, message);

}



void rlc_Rrc_BufStatus_Rpt(uint32_t send_data_size, rb_id_t rb_id, rb_type_e rb_type, rnti_t rnti, 
									uint32_t  rlc_buffer_empty_size, uint32_t rlc_buffer_valid)
{

	MessageDef  *message; 
	rlc_rrc_buffer_rpt  *buf_rpt_ptr = (rlc_rrc_buffer_rpt  *)OSP_Alloc_Mem(sizeof(rlc_rrc_buffer_rpt)); 

    buf_rpt_ptr->rb_id = rb_id; 
    buf_rpt_ptr->rb_type = rb_type; 
    buf_rpt_ptr->rnti = rnti; 
	buf_rpt_ptr->send_data_size = send_data_size; 
    buf_rpt_ptr->rlc_buffer_data_size = rlc_buffer_empty_size; 
    buf_rpt_ptr->rlc_buffer_valid = rlc_buffer_valid; 
    

	LOG_INFO(RLC_TX, "RLC_RRC_BUF_STATUS_RPT info: rb_type:%d,rb_id:%d, \
						send_data_size:%d,rlc_valid_flag:%d,rlc_empty_size:%d\n",
						rb_type,rb_id, send_data_size,rlc_buffer_valid,rlc_buffer_empty_size);
	message = itti_alloc_new_message(TASK_D2D_RLC, RLC_RRC_BUF_STATUS_RPT,
	                       ( char *)buf_rpt_ptr, sizeof(rlc_rrc_buffer_rpt ));

	itti_send_msg_to_task(TASK_D2D_RRC,0, message);

}

 
/**************************function******************************/
