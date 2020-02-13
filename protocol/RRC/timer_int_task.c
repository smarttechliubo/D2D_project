/******************************************************************
 * @file  timer_int_task.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年12月10日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年12月10日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <d2d_system_global.h>
#include <intertask_interface.h>
#include <interface_rrc_mac.h>
#include <interface_rrc_phy.h>
#include <interface_rrc_rlc.h>
#include <d2d_message_type.h>
#include <log.h>

#include <CCCH-Message.h> 
#include <unistd.h>
#include <rrc_global_def.h>
#include <unistd.h>


extern uint32_t   g_d2d_subsfn ; 
extern uint32_t   g_d2d_sfn ; 
extern void mac_Rlc_Bufstat_Req(uint16_t frame, uint16_t subsfn);


int  g_ut_timer_cnt = 0; 

int g_ut_timerfd = 0;  

extern unsigned int g_udp_send_cnt[2];

extern unsigned int g_udp_send_cnt[2]; 
extern char msg_buffer[2048]; 

uint32_t   g_fpga_test_num = 1500;
extern uint32_t g_mac_ready ; // 0:not ready,  1:ready

int timer_int_init( )
{

    void* pTimer;
	S32 ret;
    uint32_t  i = 0; 

    for (i =0; i < 2048; i++)
    {
		msg_buffer[i] = i%255;	

    }
	
	
	pTimer = OSP_timerCreateSim(TASK_D2D_DUMMY_INT,1,400,0);
	ret = OSP_timerStart(pTimer);

	return OSP_OK;

}





void timer_int_task(MessageDef *recv_msg)
{
	
	Osp_Msg_Head *pMsg = &(recv_msg->ittiMsgHeader);

	if(IS_TIMER_MSG(pMsg))
	{
		 
		    LOG_DEBUG(DUMMY, "-----------------------------[sfn-subsfn] = {%d--%d}-------------\n",g_d2d_sfn,g_d2d_subsfn);
		    
			g_d2d_subsfn++;
			g_d2d_subsfn = g_d2d_subsfn % 10; 
			if ((g_d2d_subsfn % 10) == 0)
			{
				g_d2d_sfn++;
				g_d2d_sfn = g_d2d_sfn % 1024; 
			}
			#if 0 
			//if ((g_ut_timer_cnt % 4) < 2)//!模拟DDUU 
			if (((g_udp_send_cnt[0] % 4) == 0) ||((g_udp_send_cnt[0] % 4) == 1))
			{
				mac_Rlc_Bufstat_Req(g_d2d_sfn,g_d2d_subsfn);
			}
			#endif 
			if (((g_ut_timer_cnt% 4) == 0) ||((g_ut_timer_cnt % 4) == 1))
			{
	            if (RRC_STATUS_CONNECTE_COMPLETE == rrc_GetCurrentStatus() && (1 == g_mac_ready))
	            {
					Ip_Rlc_Data_Send(RB_TYPE_DRB,
										3,
										0XFF00,
										g_udp_send_cnt[0],
										msg_buffer,
										g_fpga_test_num); 
					
						
	 				LOG_ERROR(DUMMY, "sfn:subsfn[%d,%d]: Send Data %d byte to RLC tx \n ",g_d2d_sfn,g_d2d_subsfn,g_fpga_test_num);
					g_udp_send_cnt[0]++;
				}
			}
			g_ut_timer_cnt ++; 
    }

    itti_free_message(recv_msg);
}

#ifdef RLC_UT_DEBUG  

uint32_t   Get_Subsfn( )
{
	return g_d2d_subsfn;

}

uint32_t   Get_Frame( )
{
	return g_d2d_sfn;

}
#endif 
 
/**************************function******************************/
