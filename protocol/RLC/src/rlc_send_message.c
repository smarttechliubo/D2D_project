/******************************************************************
 * @file  rlc_send_message.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月5日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月5日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc_type.h>
#include <interface_rrc_rlc.h>
#include <interface_mac_rlc.h>



void rlc_Rrc_Configure_Cfm(uint32_t        message_id)
{
#if 0
    MessageDef  *message; 
	rlc_rrc_initial_cfm *rlc_cfm = calloc(1,sizeof(rlc_rrc_initial_cfm)); 

	rlc_cfm->error_code = 0; 
	rlc_cfm->status = 1; //！pass

	//!TODO
	message = itti_alloc_new_message(TASK_D2D_RLC message_id,
	                       ( char *)rlc_cfm, sizeof(rrc_rlc_initial_req));

	itti_send_msg_to_task(TASK_D2D_RRC,0, message);
#endif

}


 
 
 
/**************************function******************************/
