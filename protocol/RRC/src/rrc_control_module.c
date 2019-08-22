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
#include <LogicChannelConfig.h>
#include <RLC-Config.h>
#include <SN-FieldLength.h>
#include <T-Reordering.h>

#include <interface_rrc_rlc.h> 
#include <log.h> 
/**************************function******************************/

 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月20日
 * @param: mode_type :        [param description ]
 * @param: *message :         [param description ]
 */
int  rrc_Receive_Signal_Msg(uint16_t mode_type, void *message)
{
	//! process signal msg,include BCCH_MSG,CCCH_MSG 
	uint32_t  msg_type; 
    mac_rrc_bcch_sib1_rpt  *sib1_rpt; 
    SystemInformationBlockType1_t      sib1_decode; 
    CCCH_Message_t   ccch_message; 
    mac_rrc_ccch_rpt  *ccch_message_rpt; 
    RRCConnectionRequest_t    rrc_connect_request; 
    uint8_t            encode_buffer[256]; 
    uint32_t           encode_buffer_size = 0; 
    int                encode_decode_ret = 0; 
    rb_info            temp_rb;
    rb_info            rb_array[4] ; 
    rrc_rlc_srb_addmod_req   srb_add; 
    rrc_rlc_drb_addmod_req   drb_add; 

  
    switch (msg_type)
	{
	    //!this version don't have MAC_RRC_BCCH_SIB1_RPT message report
		case MAC_RRC_BCCH_SIB1_RPT: //!SIB1 info 
		{
			AssertFatal(mode_type ==  D2D_MODE_TYPE_DESTINATION, 
			           RRC, 
			           "only DESTINATION can receive MAC_RRC_BCCH_SIB1_RPT message\n"); 
			           
            sib1_rpt = (mac_rrc_bcch_sib1_rpt *)message; 
            #if 0
            if (RRC_MESSAGE_EN_DECODE_OK != DecodeD2dSib1(&sib1_decode, 
                                                          (uint8_t *)(sib1_rpt->data_ptr),
                                                          sib1_rpt->data_size));
            #endif 
            rrc_Wrap_Message_Process(DecodeD2dSib1(&sib1_decode, 
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
 			rrc_Wrap_Message_Process (DecodeD2dCcch(&ccch_message, 
 			                                               (uint8_t *)(ccch_message_rpt->data_ptr),
 			                                               ccch_message_rpt->data_size));
          
            if (CCCH_MessageType_PR_rrcConnectionrequest == ccch_message.message.present)
            {
		
				AssertFatal(mode_type ==  D2D_MODE_TYPE_SOURCE, 
			           RRC, 
			           "only SOURCE can receive rrcConnectionrequest  message\n"); 

				rrc_connect_request = ccch_message.message.choice.rrcConnectionrequest; 
				if (RRCConnectionRequest__establishmentCause_normal == rrc_connect_request.establishmentCause)
				{
				   //!generate connectsetup message, config MAC/RLC,PHY 
				   rrc_Wrap_Message_Process(EncodeD2dCcch(encode_buffer,256,
				                                      &encode_buffer_size,
				                                      CCCH_MessageType_PR_rrcConnectionsetup)); 

			  
					//!config RLC srb1 and drb here, not like LTE's 
					//!TODO: whether generate SRB1 or not ? 
					//!generate SRB1
					temp_rb = rrc_Rlc_Rbinfo_Generate(RB_TYPE_SRB1,1,
					                       LogicChannelConfig__channel_type_ccch,0,
					                       RLC_MODE_UM_DL_AND_UL ,SN_FieldLength_size10,
					                       T_Reordering_ms200); 


                    srb_add =  rrc_Rlc_Srb_Config(RB_TYPE_SRB1,1,&temp_rb); 
               

                    //!TODO send message to RLC ,


                    //！generate DRB 
                    
					temp_rb = rrc_Rlc_Rbinfo_Generate(RB_TYPE_DRB,3,
											 LogicChannelConfig__channel_type_dtch,1,
											 RLC_MODE_UM_DL_AND_UL,SN_FieldLength_size10,
											  T_Reordering_ms200); 
                    
                    drb_add = rrc_Rlc_Drb_Config(RB_TYPE_DRB,1,&temp_rb);

                    //!TODO send message to RLC 


                    //!config MAC  DTCH
                   // rrc_Mac_ConnectSetup_Config( )
						 

						                   
			

				  
					
				}


            }
            else if (CCCH_MessageType_PR_rrcConnectionsetup == ccch_message.message.present)
            {
            
                AssertFatal(mode_type ==  D2D_MODE_TYPE_DESTINATION, 
			           RRC, 
			           "only SOURCE can receive rrcConnectionsetup  message\n"); 
				

				//! config mac/rlc/phy acccording to message 

				//! generate  rrcconnectinocomplete message to send 

				//! update rrc status for destination 

				
            }
            else if (CCCH_MessageType_PR_rrcConectioncomplete == ccch_message.message.present)
            {
				AssertFatal(mode_type ==  D2D_MODE_TYPE_SOURCE, 
			           RRC, 
			           "only SOURCE can receive rrcConectioncomplete  message\n");

				//!update RRC status for source 


            }
            else 
            {
				AssertFatal(0,RRC,"CCCH message decode IE error\n");
            }
 		
 			 break; 
        }	   	
        default :break; 	     	
	}
	
}


