/******************************************************************
 * @file  ip_msg_task.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年11月29日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年11月29日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <d2d_message_type.h>
#include <interface_ip_rlc.h>
#include <log.h>
#include <sys/timerfd.h>
#include <time.h>
#include <rlc.h>
#include <test_time.h> 


void ip_msg_process(void *message, MessagesIds      msg_type)
{

	rlc_ip_data_rpt    *rlc_report_data_ptr = NULL; 
	
	
	frame_t    receive_frame; 
	sub_frame_t   receive_subsfn; 
    uint32_t      sn ; 
    tb_size_t     tbsize; 
    uint8_t       *sdu_buffer_addr; 
	uint8_t       *sdu_real_data_addr ; 
	
  
    LOG_ERROR(RLC_TX, "IP_MSG TASK receive message = %d\n",msg_type);

    switch(msg_type)
    {
		case RLC_IP_DATA_RPT:
		{
			rlc_report_data_ptr = (rlc_ip_data_rpt *)message; 
			receive_frame =  rlc_report_data_ptr->frame; 
			receive_subsfn = rlc_report_data_ptr->sub_frame; 
 			sn            =  rlc_report_data_ptr->data_sn; 
 			tbsize         = rlc_report_data_ptr->data_size -sizeof(mem_block_t); 
 			sdu_buffer_addr  = rlc_report_data_ptr->data_addr_ptr; 
 			sdu_real_data_addr = (uint8_t *)(((mem_block_t *)(rlc_report_data_ptr->data_addr_ptr))->data); 
 			
			LOG_DEBUG(IP_MSG, "frame-subsfn[%d,%d] receive rlc sdu, sn = %d, total_tb_size = %d,data_size = %d, buffer_addr = %x,\
data_addr = %x\n", receive_frame,receive_subsfn,sn, rlc_report_data_ptr->data_size,tbsize,sdu_buffer_addr,sdu_real_data_addr); 

            /*!write data to socket **/
 
             /*!free buffer */
			free_mem_block(sdu_buffer_addr,__func__);

			
			break; 
		}

    }
    
}
 
 
 
 
/**************************function******************************/
void *ip_msg_task(MessageDef *recv_msg)
{
	
	ip_msg_process(MSG_HEAD_TO_COMM(recv_msg), recv_msg->ittiMsgHeader.MsgType);

	itti_free_message(recv_msg);

}
