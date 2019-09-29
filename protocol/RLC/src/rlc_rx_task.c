/******************************************************************
 * @file  rlc_rx_task.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月29日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月29日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc.h>
#include <d2d_message_type.h>
#include <interface_mac_rlc.h>
#include <log.h>
#include <intertask_interface.h>
#include <rlc_type.h>



void rlc_mac_data_process(mac_rlc_data_info *ue_data_info,frame_t frame, sub_frame_t subsfn)
{
	uint32_t ue_index = 0; 

	
	rnti_t  ue_rnti;
	logical_chan_id_t  logic_ch; 
	uint32_t logic_index; 
	uint32_t mac_tb_size; 
	uint32_t *mac_data_addr; 
	char	 *buffer_pP; 
	tb_size_t			tb_sizeP; 


    
	
	AssertFatal(1 == ue_data_info->valid_flag , RLC, "MAC_RLC_DATA_RPT message has error flag in data_ind!\n"); 
    ue_rnti = ue_data_info->rnti; 

	for (logic_index = 0; logic_index < ue_data_info->logic_chan_num;logic_index++)
	{
		logic_ch = ue_data_info->logicchannel_id[logic_index]; 
		buffer_pP = (char *)ue_data_info->mac_pdu_buffer_ptr[logic_index]; 
		tb_sizeP = ue_data_info->mac_pdu_size[logic_index]; 
		
		mac_rlc_data_ind(g_rlc_protocol_ctxt.module_id,
						ue_rnti,
								g_rlc_protocol_ctxt.eNB_index,
								frame,
								g_rlc_protocol_ctxt.enb_flag,
								logic_ch,
								buffer_pP,
								tb_sizeP,
								1);
	}
    
}

void rlc_rx_process(void *message, MessagesIds      msg_type)
{


	mac_rlc_data_rpt   *mac_report_data_ptr = NULL; 
	mac_rlc_data_info  *ue_mac_data_ptr = NULL; 
	
	frame_t    receive_frame; 
	sub_frame_t   receive_subsfn; 

	uint32_t ue_index; 
	


	switch(msg_type)
	{
		case MAC_RLC_DATA_RPT:
		{
			mac_report_data_ptr = (mac_rlc_data_rpt *)message; 
			receive_frame = mac_report_data_ptr->sfn; 
			receive_subsfn = mac_report_data_ptr->sub_sfn; 
            ue_mac_data_ptr = (mac_rlc_data_info *)mac_report_data_ptr->data_ind; 

            for (ue_index = 0; ue_index < mac_report_data_ptr->ue_num; ue_index++)
            {
				rlc_mac_data_process((mac_rlc_data_info *)(ue_mac_data_ptr + ue_index),
									 receive_frame,
									 receive_subsfn); 
            }
			break; 
	    }
	    default:break; 

	}

}





void *rlc_rx_task( )
{

	MessageDef *recv_msg;

	while(1)
	{
		//!TODO change RLC TASK ID 
        if (0 == itti_receive_msg(TASK_D2D_RLC_RX, &recv_msg))
        {
         	
			rlc_rx_process(recv_msg->message_ptr, recv_msg->ittiMsgHeader.messageId);
			itti_free_message(recv_msg);
        }


	}
}
  
 
 
/**************************function******************************/