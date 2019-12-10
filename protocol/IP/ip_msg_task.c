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


#define	SA	      (struct sockaddr *)

char *local_ip = "192.168.57.136";
char *rlc_sdu_uplayer_ip = "192.168.57.1";

int   g_iprlc_send_sdu_fd = 0; 
uint32_t  g_rlcip_data_cnt = 0; 

int  ip_rlc_sdu_udp_initial(void )
{
   int sockfd_1;

	struct sockaddr_in sver_addr, cli_addr;






	sockfd_1 = socket(AF_INET,SOCK_DGRAM,0);
	bzero(&sver_addr,sizeof(sver_addr));

	sver_addr.sin_family = AF_INET;
	inet_pton(AF_INET, local_ip,&(sver_addr.sin_addr.s_addr));
	sver_addr.sin_port = htons(1234);

	bind(sockfd_1,SA&sver_addr,sizeof(sver_addr));

    g_iprlc_send_sdu_fd = sockfd_1; 
	return 0; 

}
void  ip_rlc_sdu_udp_send(int sockfd_1,uint8_t *buffer,uint32_t recv_length,uint32_t sn)
{

    struct sockaddr_in PC_addr_src, PC_addr_dst;
   	int  PC_addr_dst_length;
   
	PC_addr_dst.sin_family = AF_INET;
	inet_pton(AF_INET, rlc_sdu_uplayer_ip,&(PC_addr_dst.sin_addr.s_addr));
	PC_addr_dst.sin_port = htons(8888);
	
    errno = 0; 
    
    PC_addr_dst_length = sendto(sockfd_1,buffer,recv_length,0,SA&PC_addr_dst,sizeof(PC_addr_dst));
    if (-1 == PC_addr_dst_length)
    {
		AssertFatal(0, IP_MSG, "%s send data to uplayer failed,errno = %d \n",__func__,errno); 
    }
    else 
    {
    	
		LOG_ERROR(IP_MSG,"IP_MSG send data counter = %d,  SN = %d, %d bytes to uplayer! \n",g_rlcip_data_cnt,sn, PC_addr_dst_length );
		g_rlcip_data_cnt++;
    }

}

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
            ip_rlc_sdu_udp_send(g_iprlc_send_sdu_fd,sdu_real_data_addr,tbsize,sn);
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
