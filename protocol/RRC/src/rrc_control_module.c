/******************************************************************
 * @file  rrc_control_module.c
 * @brief:    [rrc control module file ] 
 * @author: bo.liu
 * @Date 2019年8月16日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月16日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rrc_global_def.h> 
#include <d2d_system_global.h>
#include <d2d_message_type.h>

 
 
 
 
/**************************function******************************/

int  rrc_Receive_Signal_Msg(uint16_t mode_type, void *message)
{
	//! process signal msg,include BCCH_MSG,CCCH_MSG 
	uint32_t  msg_type; 
    mac_rrc_bcch_sib1_rpt  *sib1_rpt; 
    SystemInformationBlockType1_t      sib1_decode; 
    CCCH_Message_t   ccch_message; 
    mac_rrc_ccch_rpt  *ccch_message_rpt; 
    RRCConnectionRequest_t    rrc_connect_request; 
	switch (msg_type)
	{
	    //!this version don't have MAC_RRC_BCCH_SIB1_RPT message report
		case MAC_RRC_BCCH_SIB1_RPT: //!SIB1 info 
		{
			if (mode_type != D2D_MODE_TYPE_DESTINATION)
			{
			    //!TODO ALARM 
				return -1; 
			}
            sib1_rpt = (mac_rrc_bcch_sib1_rpt *)message; 
            if (RRC_MESSAGE_EN_DECODE_OK != DecodeD2dSib1(&sib1_decode, 
                                                          (uint8_t *)(sib1_rpt->data_ptr),
                                                          sib1_rpt->data_size));
            {
				//!TODO ALARM
				return -1; 
            }
            //! config destination MAC & PHY
            
			
			break; 
		}	
	    case MAC_RRC_CCCH_RPT: 
        {
 			ccch_message_rpt = (mac_rrc_ccch_rpt *)(message); 
 			if (RRC_MESSAGE_EN_DECODE_OK != DecodeD2dCcch(&ccch_message, 
 			                                               (uint8_t *)(ccch_message_rpt->data_ptr),
 			                                               ccch_message_rpt->data_size));
            {
				//!TODO ALARM
				return -1; 
            }
            if (CCCH_MessageType_PR_rrcConnectionrequest == ccch_message.message.present)
            {
				if (mode_type != D2D_MODE_TYPE_SOURCE)
				{
					return -2; 
				}

				rrc_connect_request = ccch_message.message.choice.rrcConnectionrequest; 
				if (RRCConnectionRequest__establishmentCause_normal == rrc_connect_request.establishmentCause)
				{
				   //!generate connectsetup message, config MAC/RLC,PHY 
					
				}


            }
            else if (CCCH_MessageType_PR_rrcConnectionsetup == ccch_message.message.present)
            {
				if (mode_type != D2D_MODE_TYPE_DESTINATION)
				{
					return -2; 
				}
            }
            else if (CCCH_MessageType_PR_rrcConectioncomplete == ccch_message.message.present)
            {
				
				if (mode_type != D2D_MODE_TYPE_SOURCE)
				{
					return -2; 
				}


            }
            else 
            {
				//!TODO ALARM
				return -1; 
            }

            
 
 		
 			 break; 
        }	   	
        default :break;
 	     	




	}








}


