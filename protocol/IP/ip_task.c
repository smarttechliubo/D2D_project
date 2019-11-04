/******************************************************************
 * @file  ip_task.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年10月28日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年10月28日        bo.liu       create file

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
#define  MAX_BUFFER_LENGTH     2048
//char *vm_addr1 = "192.168.1.168";
//char *vm_addr2= "192.168.84.129";

char *vm_addr1 = "192.168.0.127";
char *vm_addr2= "192.168.84.1";

char *PC_src_ip = "192.168.1.135";
char *PC_dst_ip = "192.168.84.1";
char msg_buffer[MAX_BUFFER_LENGTH] = {0};
char msg_buffer_2[MAX_BUFFER_LENGTH] = {0};
char pc_addr_ip[32] = {0};



void Ip_Rlc_Data_Send(rb_type_e rb_type, rb_id_t rb_id, rnti_t rnti, uint32_t data_number,uint32_t *data_buffer, uint32_t data_size)
{
	MessageDef  *message;  

	ip_rlc_data_ind  *data_ind_ptr = calloc(1,sizeof(ip_rlc_data_ind)); 

 	data_ind_ptr->rb_type = rb_type; 
 	data_ind_ptr->rb_id = rb_id; 
 	data_ind_ptr->rnti = rnti; 
 	data_ind_ptr->data_addr_ptr = data_buffer; 
 	data_ind_ptr->data_size = data_size; 

 	data_ind_ptr->data_sn = data_number; 
    message = itti_alloc_new_message(TASK_D2D_IP, IP_RLC_DATA_IND,
	                       ( char *)data_ind_ptr, sizeof(ip_rlc_data_ind));

	itti_send_msg_to_task(TASK_D2D_RLC_TX,  0, message);
}

void ip_task( )
{

	int sockfd_1;
	int sockfd_2;
	struct sockaddr_in sver_addr, cli_addr;
	struct sockaddr_in PC_addr_src, PC_addr_dst;
	int  recv_length;
	int recv_length_2;
        int  PC_addr_src_length;
	int  PC_addr_dst_length;
	fd_set    rset;
    fd_set    wset;
	int       max_fd;
	int       max_timer_fd; 
#ifdef RLC_UT_DEBUG
		//!start 2 timer for MAC_RLC  message 
	struct itimerspec ts1;
	struct itimerspec ts2; 
	struct timespec start , now;  
	int  timerfd_1,timerfd_2; 
	struct timeval curtime[2]; 
	struct timeval oldtime[2];
	struct timespec timer2_request;
	unsigned long  current_cycle = 0;
	unsigned long  old_cycle = 0;
	unsigned long  timer_expire_count[2] ={0}; 

	ts1.it_value.tv_sec = 0; 
	ts1.it_value.tv_nsec = 4000000; //!4ms

	ts1.it_interval = ts1.it_value; 

	ts2 = ts1;  //!ts2 is peroidical timer of 4ms too. 
	
#endif  

 	unsigned int  recv_cnt[2] = {0};
    unsigned int  send_cnt[2] = {0};
    

	FD_ZERO(&rset);
	FD_ZERO(&wset);

	sockfd_1 = socket(AF_INET,SOCK_DGRAM,0);
	sockfd_2 = socket(AF_INET,SOCK_DGRAM,0);

	bzero(&sver_addr,sizeof(sver_addr));
	bzero(&cli_addr,sizeof(cli_addr));
	bzero(&PC_addr_src,sizeof(PC_addr_src));
	bzero(&PC_addr_dst,sizeof(PC_addr_dst));

	sver_addr.sin_family = AF_INET;
	inet_pton(AF_INET, vm_addr1,&(sver_addr.sin_addr.s_addr));
	sver_addr.sin_port = htons(1234);

	cli_addr.sin_family = AF_INET;
	inet_pton(AF_INET, vm_addr2,&(cli_addr.sin_addr.s_addr));
	cli_addr.sin_port = htons(1234);

	PC_addr_src.sin_family = AF_INET;
	inet_pton(AF_INET, PC_src_ip,&(PC_addr_src.sin_addr.s_addr));
	PC_addr_src.sin_port = htons(9999);


	PC_addr_dst.sin_family = AF_INET;
	inet_pton(AF_INET, PC_dst_ip,&(PC_addr_dst.sin_addr.s_addr));
	PC_addr_dst.sin_port = htons(8888);

	bind(sockfd_1,SA&sver_addr,sizeof(sver_addr));
	bind(sockfd_2,SA&cli_addr,sizeof(cli_addr));

#ifdef  RLC_UT_DEBUG 
	timerfd_1 = timerfd_create(CLOCK_MONOTONIC,0);
	

	timerfd_settime(timerfd_1, 0, &ts1, NULL); 
#if 1
	timerfd_2 = timerfd_create(CLOCK_MONOTONIC,0);
    //!sleep 1ms ,and then start timer2 
	if (clock_gettime(CLOCK_MONOTONIC, &timer2_request) == -1)
		perror("clock_gettime");
    timer2_request.tv_nsec += 1000000; //! add 1ms 
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timer2_request, NULL);  //!sleep 1ms ,and then start timer2

    timerfd_settime(timerfd_2, 0, &ts2, NULL); 
#endif 

#endif 

    //sver_addr 用来接收主机的IP1 传输的数据，然后发送给vm_addr2,
       // LOG_DEBUG(IP," ip = 0x%x, port = 0x%x\n",sver_addr.sin_addr.s_addr, sver_addr.sin_port);

	for (;;)
	{
		FD_SET(sockfd_1,&rset);
	   //!FD_SET(sockfd_2,&rset);
#ifdef RLC_UT_DEBUG 	   
	   FD_SET(timerfd_1,&rset); 
	   FD_SET(timerfd_2,&rset); 
#endif 
		max_fd = (sockfd_1>=sockfd_2) ? sockfd_1:sockfd_2;
#ifdef RLC_UT_DEBUG 
		max_fd = (max_fd >= timerfd_1)?max_fd:timerfd_1; 
		max_fd = (max_fd >= timerfd_2)?max_fd:timerfd_2; 
#endif 

	    max_fd = max_fd +1;
		select(max_fd, &rset,NULL,NULL,NULL);
		if (FD_ISSET(timerfd_1,&rset))
		{
			read(timerfd_1,&timer_expire_count[0],sizeof(timer_expire_count[0]));
			curtime[0] = GetTimeUs();
			LOG_ERROR(IP,"******timerfd_1,expire count:%lld, time:%lld(s), %lld(us),elapse time:%lld(us) \n",timer_expire_count[0],curtime[0].tv_sec, curtime[0].tv_usec,
					 ((curtime[0].tv_sec*100000 + curtime[0].tv_usec) - (oldtime[0].tv_sec*100000 + oldtime[0].tv_usec)) );
			oldtime[0] = curtime[0];
			//current_cycle = GetCpuCycle();
			//LOG_INFO(IP, "timerfd_1,expire count:%lld, cycle:%lld, elapse cycle = %lld \n",timer_expire_count,current_cycle, current_cycle - old_cycle);
			//old_cycle = current_cycle;
		}

		if (FD_ISSET(timerfd_2,&rset))
		{
			read(timerfd_2,&timer_expire_count[1],sizeof(timer_expire_count[1]));
			curtime[1] = GetTimeUs();
			LOG_ERROR(IP,"------timerfd_2,expire count:%lld, time:%lld(s), %lld(us),elapse time:%lld(us) \n",timer_expire_count[1],curtime[1].tv_sec, curtime[1].tv_usec,
					 ((curtime[1].tv_sec*100000 + curtime[1].tv_usec) - (oldtime[1].tv_sec*100000 + oldtime[1].tv_usec)) );
			oldtime[1] = curtime[1];
			//current_cycle = GetCpuCycle();
			//LOG_INFO(IP, "timerfd_1,expire count:%lld, cycle:%lld, elapse cycle = %lld \n",timer_expire_count,current_cycle, current_cycle - old_cycle);
			//old_cycle = current_cycle;
		}
		
		if (FD_ISSET(sockfd_1,&rset))
		{

			errno = 0;
			recv_length = recvfrom(sockfd_1,msg_buffer,MAX_BUFFER_LENGTH,0,NULL,NULL); //!获取PC 发送的地址
			inet_ntop(AF_INET,&PC_addr_src.sin_addr.s_addr, pc_addr_ip,sizeof(pc_addr_ip));
			LOG_DEBUG(IP,"message buffer = %s\n",msg_buffer);

			if (0 == errno)
	 		{
		       // LOG_DEBUG(IP,"receive data from ip:%s, port: %d, length:%d ! \n",pc_addr_ip,ntohs(PC_addr_src.sin_port),recv_length);
				LOG_INFO(IP,"channel:0 -- receive data no.: %d \n",recv_cnt[0]++);
				LOG_ERROR(IP,"channel:0 --data_length = %d, send data no.: %d \n",recv_length,send_cnt[0]++);
#ifdef RLC_UT_DEBUG
				//! 组包消息，向RLC 发送消息
				Ip_Rlc_Data_Send(RB_TYPE_DRB,
								3,
								0X65,
								send_cnt[0],
								msg_buffer,
								recv_length); 
#else 
				//sendto(sockfd_2,msg_buffer,recv_length,0,SA&PC_addr_dst,sizeof(PC_addr_dst));
#endif 
				
	 			

				if (0 != errno)
				{
					LOG_ERROR(IP,"channel 0--send errno = %d\n", errno);
				}
			}
			else
			{
				LOG_ERROR(IP,"channel 0: receive errno = %d\n", errno);
			}
		}


		if (FD_ISSET(sockfd_2,&rset))
		{

		       // LOG_DEBUG(IP,"msg buffer = %s\n",msg_buffer);
			//LOG_DEBUG(IP,"fd = %d,start receive data in 0x%x,length = %d \n",sockfd_1,msg_buffer,sizeof(msg_buffer));
			errno = 0;
			LOG_DEBUG(IP,"start receive ip from %s\n",PC_dst_ip);
			recv_length_2 = recvfrom(sockfd_2,msg_buffer_2,MAX_BUFFER_LENGTH,0,NULL,NULL); //!获取PC 发送的地址
			inet_ntop(AF_INET,&PC_addr_dst.sin_addr.s_addr, pc_addr_ip,sizeof(pc_addr_ip));
 			if (0 == errno)
			{
				LOG_DEBUG(IP,"receive data from ip:%s, port: %d, length:%d ! \n",pc_addr_ip,ntohs(PC_addr_dst.sin_port),recv_length_2);
				LOG_DEBUG(IP,"channel:1 -- receive data no.: %d \n",recv_cnt[1]++);
				sendto(sockfd_1,msg_buffer_2,recv_length_2,0,SA&PC_addr_src,sizeof(PC_addr_src));
				LOG_DEBUG(IP,"channel:1 -- send data no.: %d \n",send_cnt[1]++);
				if (0 != errno)
				{
					LOG_ERROR(IP,"channel 1: send errno = %d\n",errno);
				}
			}
			else
			{
				LOG_ERROR(IP,"channel 1: receive errno = %d\n",errno);
			}
		}
	}
} 
 
 
/**************************function******************************/
