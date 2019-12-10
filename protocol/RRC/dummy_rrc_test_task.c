/******************************************************************
 * @file  rrc_dummy_test.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月28日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月28日        bo.liu       create file

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


extern int rrc_GetModeType( ); 
extern rrc_status_e  rrc_GetCurrentStatus( );


#define   RLC_MODULE_TEST
/**************************function******************************/
void dummy_rrc_confirm_message(uint16_t msg_id)
{
    MessageDef * message; 
	phy_rrc_initial_cfm *initial_cfm = calloc(1,sizeof(phy_rrc_initial_cfm)); 
    mac_rrc_connection_cfm *mac_connect_setup_ptr = calloc(1,sizeof(mac_rrc_connection_cfm)); 
    rlc_rrc_connect_setup_cfg_cfm *rlc_connect_setup_ptr = calloc(1,sizeof(rlc_rrc_connect_setup_cfg_cfm));
    mac_rrc_bcch_para_config_cfm  *mac_bcch_para_cfm_ptr = calloc(1,sizeof(mac_rrc_bcch_para_config_cfm)) ; 
    
    static int mac_bcch_para_cfg_flag = 1; 
    LOG_DEBUG(DUMMY,"send confirm message type:%d \n" ,msg_id); 
	
	switch (msg_id)
	{

	    
		case PHY_RRC_INITIAL_CFM: 
		{
			initial_cfm->status = 1; 
			initial_cfm->error_code =0; 
			message = itti_alloc_new_message(TASK_D2D_PHY, PHY_RRC_INITIAL_CFM, 
			                      (char *) initial_cfm, sizeof(phy_rrc_initial_cfm)); 
			itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);
			break; 
		}
 		case MAC_RRC_INITIAL_CFM: 
 		{
 			initial_cfm->status = 1; 
			initial_cfm->error_code =0; 
			message = itti_alloc_new_message(TASK_D2D_MAC, MAC_RRC_INITIAL_CFM, 
			                      (char *) initial_cfm, sizeof(phy_rrc_initial_cfm)); 
			itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);
			break; 
 		}
        case RLC_RRC_INITIAL_CFM:
 		{

 			initial_cfm->status = 1; 
			initial_cfm->error_code =0; 
			message = itti_alloc_new_message(TASK_D2D_RLC, RLC_RRC_INITIAL_CFM, 
			                      (char *) initial_cfm, sizeof(phy_rrc_initial_cfm)); 
			itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);
			break; 
 		}
 		case PHY_RRC_BCCH_PARA_CFG_CFM: 
 		{
			initial_cfm->status = 1; 
			initial_cfm->error_code =0; 
			message = itti_alloc_new_message(TASK_D2D_PHY, PHY_RRC_BCCH_PARA_CFG_CFM, 
			                      (char *) initial_cfm, sizeof(phy_rrc_initial_cfm)); 
			itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);
			break; 


 		}
 		case  RLC_RRC_BCCH_PARA_CFG_CFM: 
 		{
			initial_cfm->status = 1; 
			initial_cfm->error_code =0; 
			message = itti_alloc_new_message(TASK_D2D_RLC, RLC_RRC_BCCH_PARA_CFG_CFM, 
			                      (char *) initial_cfm, sizeof(phy_rrc_initial_cfm)); 
			itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);
			break; 


 		}
 		case MAC_RRC_CONNECT_SETUP_CFG_CFM:
 		{
			mac_connect_setup_ptr->status = 1; 
			mac_connect_setup_ptr->error_code = 0; 
			mac_connect_setup_ptr->rnti = 0x65; //!rnti from MAC 
			mac_connect_setup_ptr->ue_index = 0; 

			message = itti_alloc_new_message(TASK_D2D_MAC, MAC_RRC_CONNECT_SETUP_CFG_CFM, 
			                      (char *) mac_connect_setup_ptr, sizeof(mac_rrc_connection_cfm)); 
			itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);

			break; 
 		}
 		case RLC_RRC_CONNECT_SETUP_CFG_CFM:
 		{

			rlc_connect_setup_ptr->status = 1; 
			rlc_connect_setup_ptr->error_code = 0; 


			message = itti_alloc_new_message(TASK_D2D_RLC, RLC_RRC_CONNECT_SETUP_CFG_CFM, 
			                      (char *) mac_connect_setup_ptr, sizeof(mac_rrc_connection_cfm)); 
			itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);

			break; 
 		}
 		case MAC_RRC_BCCH_PARA_CFG_CFM:
 		{
 			mac_bcch_para_cfm_ptr->status = 1;
			mac_bcch_para_cfm_ptr->error_code = 0; 
			mac_bcch_para_cfm_ptr->flag = mac_bcch_para_cfg_flag; 
			mac_bcch_para_cfg_flag++;


			message = itti_alloc_new_message(TASK_D2D_MAC, MAC_RRC_BCCH_PARA_CFG_CFM, 
			                      (char *) mac_bcch_para_cfm_ptr, sizeof(mac_rrc_bcch_para_config_cfm));
			itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);
			break;

 		}
 			
 		default:break;

		

	}





}





void dummy_rrc_rpt_message(uint16_t msg_id,CCCH_MessageType_PR ccch_message)
{
	MessageDef * message; 
	mac_rrc_bcch_mib_rpt    *mib_rpt_ptr; 

	mac_rrc_ccch_rpt *ccch_rpt_ptr ; 

	uint8_t  *buf = MALLOC(256); 
	uint32_t encode_size = 0; 
	switch (msg_id)
	{
		case MAC_RRC_BCCH_MIB_RPT:
		{
			AssertFatal((D2D_MODE_TYPE_DESTINATION  == rrc_GetModeType()),  \
			         RRC, "only RRC Destination can receive MIB RPT\n"); 

	        mib_rpt_ptr = (mac_rrc_bcch_mib_rpt *)calloc(1,sizeof(mac_rrc_bcch_mib_rpt)); 
	        mib_rpt_ptr->SFN = 0x256; 
	        mib_rpt_ptr->subsfn = 0x1;
	        mib_rpt_ptr->mib_receive_flag = 1; 
            
	        message = itti_alloc_new_message(TASK_D2D_MAC, MAC_RRC_BCCH_MIB_RPT, 
				                      (char *) mib_rpt_ptr, sizeof(mac_rrc_bcch_mib_rpt)); 
		    itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);
		    break; 
			
		}
		case MAC_RRC_CCCH_RPT: 
		{
		    ccch_rpt_ptr = MALLOC(sizeof(mac_rrc_ccch_rpt)); 
		
			if (D2D_MODE_TYPE_SOURCE  == rrc_GetModeType() )  //!source should receive rrc connect request message
            {
				EncodeD2dCcch(buf, 256, &encode_size,ccch_message); 
				ccch_rpt_ptr->sfn = 0x5; 
				ccch_rpt_ptr->subsfn = 0x4; 
				ccch_rpt_ptr->data_ptr = buf; 
				ccch_rpt_ptr->data_size = encode_size;
               
				message = itti_alloc_new_message(TASK_D2D_MAC, MAC_RRC_CCCH_RPT, 
				                      (char *) ccch_rpt_ptr, sizeof(mac_rrc_ccch_rpt)); 
		        itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);

            }
            else  //!destinatioin receive connect setup message 
            {
				
				EncodeD2dCcch(buf, 256, &encode_size,CCCH_MessageType_PR_rrcConnectionsetup); 
				ccch_rpt_ptr->sfn = 0x5; 
				ccch_rpt_ptr->subsfn = 0x4; 
				ccch_rpt_ptr->data_ptr = buf; 
				ccch_rpt_ptr->data_size = encode_size;

				message = itti_alloc_new_message(TASK_D2D_MAC, MAC_RRC_CCCH_RPT, 
				                      (char *) ccch_rpt_ptr, sizeof(mac_rrc_ccch_rpt)); 
		        itti_send_msg_to_task(TASK_D2D_RRC, 0,  message);


            }
            break; 



		}
		default: break; 
	}
	
}

int dummy_init( )
{

    void* pTimer;
	S32 ret;
	
	pTimer = OSP_timerCreateSim(TASK_D2D_DUMMY,0,1,0);
	ret = OSP_timerStart(pTimer);

	return OSP_OK;


}

uint gdummy_timer_cnt = 0;


void dummy_test_task(MessageDef *recv_msg)
{
	uint32_t rrc_mode = rrc_GetModeType(); 
	Osp_Msg_Head *pMsg = &(recv_msg->ittiMsgHeader);

	if(IS_TIMER_MSG(pMsg))
	{
		gdummy_timer_cnt++; 
    
		
		LOG_DEBUG(DUMMY,"gdummy_timer_cnt = %d \n", gdummy_timer_cnt);
		if (D2D_MODE_TYPE_SOURCE == rrc_mode)
		{
	       
			while ( RRC_STATUS_INITIAL !=  rrc_GetCurrentStatus())  \
			{
				LOG_DEBUG(RRC, "SOURCE wait for initial stasus\n");
			}

			//！simulate phy/mac/rlc confirm message 
			
			dummy_rrc_confirm_message(PHY_RRC_INITIAL_CFM); 

			dummy_rrc_confirm_message(MAC_RRC_INITIAL_CFM); 
			
#ifndef RLC_MODULE_TEST
			dummy_rrc_confirm_message(RLC_RRC_INITIAL_CFM); 
#endif 
			while (RRC_STATUS_INITIAL_CFM != rrc_GetCurrentStatus()) { }
	        dummy_rrc_confirm_message(MAC_RRC_BCCH_PARA_CFG_CFM); //!mib schedule cfm 
	 
	        dummy_rrc_confirm_message(MAC_RRC_BCCH_PARA_CFG_CFM); //!sib1 schedule cfm 

			//! send rrc connect request message to source 
			dummy_rrc_rpt_message(MAC_RRC_CCCH_RPT,CCCH_MessageType_PR_rrcConnectionrequest); 


			dummy_rrc_confirm_message(MAC_RRC_CONNECT_SETUP_CFG_CFM);
			
#ifndef RLC_MODULE_TEST

			dummy_rrc_confirm_message(RLC_RRC_CONNECT_SETUP_CFG_CFM);
#endif 
			while (RRC_STATUS_CONNECTED !=  rrc_GetCurrentStatus()) 
			{	
				
				LOG_DEBUG(RRC, "SOURCE wait for connect complete message  \n");
			}
			//!send rrc conncet complete message to source 
			dummy_rrc_rpt_message(MAC_RRC_CCCH_RPT,CCCH_MessageType_PR_rrcConectioncomplete); 

	     
		}
		else 
		{
			while ( RRC_STATUS_CELL_SEARCH !=  rrc_GetCurrentStatus())  \
			{
				LOG_DEBUG(RRC, "DESTINATION wait for initial stasus\n");
			}


			dummy_rrc_rpt_message(MAC_RRC_BCCH_MIB_RPT,0);

			while (RRC_STATUS_IDLE != rrc_GetCurrentStatus())
			{
				LOG_DEBUG(RRC, "DESTINATION wait for PHY_RRC_BCCH_PARA_CFG_CFM report"); 
			}

	        dummy_rrc_confirm_message(PHY_RRC_BCCH_PARA_CFG_CFM);


	        dummy_rrc_confirm_message(RLC_RRC_BCCH_PARA_CFG_CFM); //srb 0 established
	        while(RRC_STATUS_CONNECT_REQUEST != rrc_GetCurrentStatus())
	        {
				LOG_DEBUG(RRC, "DESTINATION wait for CONNECT SETUP message \n"); 
	        }
	        //!send rrc conncet setup message to destination  
	        dummy_rrc_rpt_message(MAC_RRC_CCCH_RPT,CCCH_MessageType_PR_rrcConnectionsetup); 
	        
		}
	}

	itti_free_message(recv_msg);
}
