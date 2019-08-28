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
#include <intertask_interface.h>
#include <dictionary.h>
/**************************function******************************/
 
 /*!   
 * @brief:   extract rb infomation from connect setup message,include SRB and DRB 
 * @author:  bo.liu
 * @Date:  2019年8月23日
 * @param: connect_setup :    [connect setup message]
 * @param: *srb_add :         [pointer to srb info  ]
 * @param: *drb_add :         [pointer to drb info  ]
 * @param: *logic_ch_config : [logic channel info pointer]
 * @param: *logic_ch_num :    [logic channel num  ]
 * @param: *max_harq_tx :     [mac config parameter:max_tx_harq  ]
 * @param: *max_sync :        [mac config parameter:max_out_sync ]
 */
 void  rrc_rbinfo_decode_connect_setup(RRCConnectionSetup_t connect_setup, 
                                                 rrc_rlc_srb_addmod_req *srb_add,
                                                 rrc_rlc_drb_addmod_req *drb_add,
                                                 logical_channel_config_s *logic_ch_config,
                                                 uint32_t  *logic_ch_num,
                                                 uint32_t *max_harq_tx,
 											     uint32_t *max_sync                                             
                                                 )
                                                 
 {
 	uint32_t rb_cnt  = 0; 

 	SRB_ToAddModList_t *srb_list_ptr; 
 	SRB_ToAddMod_t     *srb_addmod_ptr; 

    DRB_ToAddModList_t *drb_list_ptr; 
 	DRB_ToAddMod_t     *drb_addmod_ptr;   

 	

	memset((void *)srb_add,0,sizeof(rrc_rlc_srb_addmod_req)); 
	memset((void *)drb_add,0,sizeof(rrc_rlc_drb_addmod_req)); 
	*logic_ch_num = 0; 
	if (connect_setup.srb_ToAddModList)
	{
	    srb_list_ptr = connect_setup.srb_ToAddModList; 
	    srb_add->rb_type = RB_TYPE_SRB1;
	 	for (rb_cnt = 0; rb_cnt < srb_list_ptr->list.count; rb_cnt++)
	 	{
			
			srb_addmod_ptr = srb_list_ptr->list.array[rb_cnt]; 
			
			srb_add->srb_list[rb_cnt].rb_id = srb_addmod_ptr->srb_Identity; 
            srb_add->srb_list[rb_cnt].logicchannel_id = srb_addmod_ptr->logicChannelConfig.logicch_id;
            srb_add->srb_list[rb_cnt].logicchannel_type = srb_addmod_ptr->logicChannelConfig.channel_type; 
			//!record SRB rlc parameter
			if (RLC_Config_PR_um_bi_direction == srb_addmod_ptr->rlc_config.present)  {
				srb_add->srb_list[rb_cnt].rlc_mode = RLC_MODE_UM_DL_AND_UL; 
				srb_add->srb_list[rb_cnt].rlc_mode_cfg.ul_um_cfg.sn_field =  \
					srb_addmod_ptr->rlc_config.choice.um_bi_direction.ul_um_rlc.sn_FieldLength; //! 1:SN = 10

				srb_add->srb_list[rb_cnt].rlc_mode_cfg.dl_um_cfg.sn_field =  \ 	
					srb_addmod_ptr->rlc_config.choice.um_bi_direction.dl_um_rlc.sn_FieldLength; //! 1:SN = 10

				srb_add->srb_list[rb_cnt].rlc_mode_cfg.dl_um_cfg.t_recordering = \ 
					srb_addmod_ptr->rlc_config.choice.um_bi_direction.dl_um_rlc.t_Reordering; 
				
			}
			else {
				LOG_ERROR(RRC, "DESTINATION RECEIVE CONNECT SETUP message, SRB RLC mode config ERROR\n");
			}

			LOG_DEBUG(RRC, "DESTINATION CONNECT SETUP : SRB:index = %d,rb_id = %d, logicid = %d,logictype = \
							%d,rlc_mode = %d, rlc_ul_sn = %d, rlc_dl_sn = %d, rlc_dl_trecorder = %d\n", \
							srb_add->srb_count,
			                srb_add->srb_list[rb_cnt].rb_id, 
			                srb_add->srb_list[rb_cnt].logicchannel_id,
			                srb_add->srb_list[rb_cnt].logicchannel_type,
			                srb_add->srb_list[rb_cnt].rlc_mode, 
			                srb_add->srb_list[rb_cnt].rlc_mode_cfg.ul_um_cfg.sn_field, 
			                srb_add->srb_list[rb_cnt].rlc_mode_cfg.dl_um_cfg.sn_field, 
			                srb_add->srb_list[rb_cnt].rlc_mode_cfg.dl_um_cfg.t_recordering
			                ); 
			srb_add->srb_count++; 


             //!mac logic channel 
			logic_ch_config[*logic_ch_num].chan_type = srb_addmod_ptr->logicChannelConfig.channel_type; 
			logic_ch_config[*logic_ch_num].logical_channel_id =  srb_addmod_ptr->logicChannelConfig.logicch_id; 
			logic_ch_config[*logic_ch_num].priority = srb_addmod_ptr->logicChannelConfig.priority; 
			
			*logic_ch_num++; 


	 	}
	}

	if (connect_setup.drb_ToAddModList)
	{
		drb_list_ptr = connect_setup.drb_ToAddModList; 

		drb_add->rb_type = RB_TYPE_DRB; 
		for (rb_cnt = 0; rb_cnt < drb_list_ptr->list.count;rb_cnt++)
		{
			drb_addmod_ptr = drb_list_ptr->list.array[rb_cnt]; 
			drb_add->drb_list[rb_cnt].rb_id = drb_addmod_ptr->drb_Identity; 
			drb_add->drb_list[rb_cnt].logicchannel_id = drb_addmod_ptr->logicChannelConfig.logicch_id; 
			drb_add->drb_list[rb_cnt].logicchannel_type = drb_addmod_ptr->logicChannelConfig.channel_type; 


            if (RLC_Config_PR_um_bi_direction == drb_addmod_ptr->rlc_config.present) {

				drb_add->drb_list[rb_cnt].rlc_mode = RLC_MODE_UM_DL_AND_UL; 
				drb_add->drb_list[rb_cnt].rlc_mode_cfg.dl_um_cfg.sn_field = \
					drb_addmod_ptr->rlc_config.choice.um_bi_direction.dl_um_rlc.sn_FieldLength;
					
		        drb_add->drb_list[rb_cnt].rlc_mode_cfg.dl_um_cfg.t_recordering = \
					drb_addmod_ptr->rlc_config.choice.um_bi_direction.dl_um_rlc.t_Reordering;

                drb_add->drb_list[rb_cnt].rlc_mode_cfg.ul_um_cfg.sn_field = \
					drb_addmod_ptr->rlc_config.choice.um_bi_direction.ul_um_rlc.sn_FieldLength;

            }
            else {
				LOG_ERROR(RRC, "DESTINATION RECEIVE CONNECT SETUP message, DRB RLC mode config ERROR\n");
            }


            LOG_DEBUG(RRC, "DESTINATION CONNECT SETUP : DRB:index = %d,rb_id = %d, logicid = %d,logictype = \
							%d,rlc_mode = %d, rlc_ul_sn = %d, rlc_dl_sn = %d, rlc_dl_trecorder = %d\n", \
							drb_add->drb_count,
			                drb_add->drb_list[rb_cnt].rb_id, 
			                drb_add->drb_list[rb_cnt].logicchannel_id,
			                drb_add->drb_list[rb_cnt].logicchannel_type,
			                drb_add->drb_list[rb_cnt].rlc_mode, 
			                drb_add->drb_list[rb_cnt].rlc_mode_cfg.ul_um_cfg.sn_field, 
			                drb_add->drb_list[rb_cnt].rlc_mode_cfg.dl_um_cfg.sn_field, 
			                drb_add->drb_list[rb_cnt].rlc_mode_cfg.dl_um_cfg.t_recordering
			                ); 


			drb_add->drb_count++; 


			 //!mac logic channel 
			logic_ch_config[*logic_ch_num].chan_type = drb_addmod_ptr->logicChannelConfig.channel_type; 
			logic_ch_config[*logic_ch_num].logical_channel_id =  drb_addmod_ptr->logicChannelConfig.logicch_id; 
			logic_ch_config[*logic_ch_num].priority = drb_addmod_ptr->logicChannelConfig.priority; 
			
			*logic_ch_num++;

		}
	}

    *max_harq_tx  = connect_setup.mac_config.maxharq_tx;
	*max_sync     = connect_setup.mac_config.max_out_sync;

 }




 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月20日
 * @param: mode_type :        [source or desitination  ]
 * @param: *message :         [message content pointer ]
  * @param: msg_type :        [message id ]
 */
int  rrc_Receive_Signal_Msg(uint16_t mode_type, void *message, MessagesIds  msg_type)
{
	//! process signal msg,include BCCH_MSG,CCCH_MSG 
    mac_rrc_bcch_sib1_rpt  *sib1_rpt; 
    SystemInformationBlockType1_t      sib1_decode; 
    CCCH_Message_t   ccch_message; 
    mac_rrc_ccch_rpt  *ccch_message_rpt; 
    RRCConnectionRequest_t    rrc_connect_request; 
    uint8_t            encode_buffer = g_rrc_messge_encode; 

    rb_info            temp_rb;
    rb_info            rb_array[4] ; 
    rrc_rlc_srb_addmod_req   srb_add; 
    rrc_rlc_drb_addmod_req   drb_add; 
    pusch_config_basic_s     pusch_config; 
    ul_ref_signal_pusch_s    ref_signal_pusch_config; 
    uint32_t                 ue_request_index; 

    RadioResourceConfigDedicate_t   *RadioResourceConfigDedicate_ptr; 
    rlc_rrc_initial_cfm *initial_cfm; 
    bcch_mib_info_s bcch_mib_info;  
    mac_rrc_bcch_para_config_cfm  *mac_bcch_cfm_ptr; 
    bcch_si_info_s  bcch_sib_info; 
    rlc_rrc_buffer_rpt  *rlc_buffer_rpt_ptr ; 
    mac_rrc_connection_cfm *mac_rrc_connection_cfm_ptr; 

    logical_channel_config_s     logic_channel_config[MAX_LOGICCHAN_NUM] = {0}; 
    uint32_t                     logic_ch_num = 0; 
    uint32_t                     max_harq_tx = 0; 
    uint32_t                     max_out_sync = 0; 

    uint32_t           encode_buffer_size = 0; 
                                                
    switch (msg_type)
	{

		case PHY_RRC_INITIAL_CFM: 
		{
			initial_cfm = (rlc_rrc_initial_cfm *)message;  //!
			AssertFatal((1 == initial_cfm->status), RRC , 
			           "initial CFM message:status error! message_id = %d,error_no = %d",msg_type, 
			           initial_cfm->error_code); 
			if (1 == initial_cfm->status)
			{
				g_rrc_init_para.phy_initial_cfm = 1; 
			}
			else 
			{
			    g_rrc_init_para.phy_initial_cfm = 0; 
				LOG_ERROR(RRC, "RRC initial confirm message status error, msg_id = %d\n", msg_type);
			}

			if (g_rrc_init_para.phy_initial_cfm & g_rrc_init_para.mac_initial_cfm & g_rrc_init_para.rlc_initial_cfm)
			{
				rrc_SetStatus(RRC_STATUS_INITIAL_CFM);
			}
			break; 
		}
		case MAC_RRC_INITIAL_CFM: 
		{
			initial_cfm = (rlc_rrc_initial_cfm *)message; 
			AssertFatal((1 == initial_cfm->status), RRC , 
			           "initial CFM message:status error! message_id = %d,error_no = %d",msg_type, 
			           initial_cfm->error_code); 
			if (1 == initial_cfm->status)
			{
				g_rrc_init_para.mac_initial_cfm = 1; 
			}
			else 
			{
			    g_rrc_init_para.mac_initial_cfm = 0; 
				LOG_ERROR(RRC, "RRC initial confirm message status error, msg_id = %d\n", msg_type);
			}

			if (g_rrc_init_para.phy_initial_cfm & g_rrc_init_para.mac_initial_cfm & g_rrc_init_para.rlc_initial_cfm)
			{
				rrc_SetStatus(RRC_STATUS_INITIAL_CFM);
			}

			if (D2D_MODE_TYPE_SOURCE  == rrc_GetModeType() && (RRC_STATUS_INITIAL_CFM == rrc_GetCurrentStatus()))
			{
				bcch_mib_info.systemFrameNumber = 0xff; 
				bcch_mib_info.pdcch_config.rb_start_index = g_rrc_init_para.mib_info.pdcch_rb_start; 
				bcch_mib_info.pdcch_config.rb_num = g_rrc_init_para.mib_info.pdcch_rb_num; 
				//!notify MAC to schedule MIB 
				rrc_Mac_BcchPara_Config(1, &bcch_mib_info);
			}
			break; 
		}
		case RLC_RRC_INITIAL_CFM: 
		{
			initial_cfm = (rlc_rrc_initial_cfm *)message; 
			AssertFatal((1 == initial_cfm->status), RRC , 
			           "initial CFM message:status error! message_id = %d,error_no = %d",msg_type, 
			           initial_cfm->error_code); 
			if (1 == initial_cfm->status)
			{
				g_rrc_init_para.rlc_initial_cfm = 1; 
			}
			else 
			{
			    g_rrc_init_para.rlc_initial_cfm = 0; 
				LOG_ERROR(RRC, "RRC initial confirm message status error, msg_id = %d\n", msg_type);
			}

			if (g_rrc_init_para.phy_initial_cfm & g_rrc_init_para.mac_initial_cfm & g_rrc_init_para.rlc_initial_cfm)
			{
				rrc_SetStatus(RRC_STATUS_INITIAL_CFM);
			}
			break; 
		}
		case MAC_RRC_BCCH_PARA_CFG_CFM: //!receive MIB para_cfm,then send sib1 para cfg message 
		{
			mac_bcch_cfm_ptr = (mac_rrc_bcch_para_config_cfm * )message; 
			AssertFatal((mac_bcch_cfm_ptr->flag > 0), RRC, "MAC_RRC_BCCH_PARA_CFM message flag error\n"); 
			AssertFatal((1 == initial_cfm->status), RRC , 
			           "initial CFM message:status error! message_id = %d,error_no = %d",msg_type, 
			           mac_bcch_cfm_ptr->error_code); 
			if (1 == mac_bcch_cfm_ptr)
			{
			    //!encode SIB1 
                rrc_Wrap_Message_Process(EncodeD2dSib1(encode_buffer,ENCODE_MAX_SIZE, 
				                                      &encode_buffer_size)); 
		    	bcch_sib_info.sib_pdu = encode_buffer; 
		    	bcch_sib_info.size = encode_buffer_size; 
				//!notify MAC to schedule SIB1
				rrc_Mac_BcchPara_Config(2, &bcch_mib_info); 
			}
		}
		case  MAC_RRC_CONNECT_SETUP_CFG_CFM:
		{
			mac_rrc_connection_cfm_ptr = (mac_rrc_connection_cfm *)message; 
			dict_SetValue(g_rrc_ue_info_dict,
						 mac_rrc_connection_cfm_ptr->rnti,
						 mac_rrc_connection_cfm_ptr->ue_index); 

			break; 
		}
		case MAC_RRC_BCCH_MIB_RPT: //!mib report 
		{
			AssertFatal(mode_type ==  D2D_MODE_TYPE_DESTINATION, 
			           RRC, 
			           "only DESTINATION can receive MAC_RRC_BCCH_MIB_RPT message\n"); 
		
			
		    //according to design spec, the desitination should send SIB1 config messge when receiving mib rpt
		    //! rrc_phy_bcch_para_cfg

            pusch_config.hop_mode = 0; 
            pusch_config.pusch_hop_offset = 0; 

            
            ref_signal_pusch_config.grp_hop_enabled = 0;
            ref_signal_pusch_config.seq_hop_enabled = 0;
            ref_signal_pusch_config.grp_assign_pusch = 0;
			ref_signal_pusch_config.cyc_shift = 0;

			rrc_Phy_BcchPara_Config(pusch_config , ref_signal_pusch_config);


             //!there is no cfg para to mac and RLC 
		    
			rrc_SetStatus(RRC_STATUS_IDLE); 
			break; 
		}
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
                                                                                                         
            {
				//!TODO ALARM
				return -1; 
            }
            #endif 
            rrc_Wrap_Message_Process(DecodeD2dSib1(&sib1_decode, 
                                                          (uint8_t *)(sib1_rpt->data_ptr),
                                                          sib1_rpt->data_size));
           
            //! config destination MAC & PHY SIB1 info 
            pusch_config.hop_mode = sib1_decode.radioResourceConfigCommon.psush_Hop_Config.hop_mode; 
            pusch_config.pusch_hop_offset = sib1_decode.radioResourceConfigCommon.psush_Hop_Config.hop_offset; 

            
            ref_signal_pusch_config.grp_hop_enabled = 
            sib1_decode.radioResourceConfigCommon.ul_ref_signal_pusch.group_hopping_enable; 
            
            ref_signal_pusch_config.seq_hop_enabled = 
            sib1_decode.radioResourceConfigCommon.ul_ref_signal_pusch.seq_hopping_enable;
            ref_signal_pusch_config.grp_assign_pusch = 
            sib1_decode.radioResourceConfigCommon.ul_ref_signal_pusch.group_assign_pusch;
            
			ref_signal_pusch_config.cyc_shift = sib1_decode.radioResourceConfigCommon.ul_ref_signal_pusch.cycle_shift;

			rrc_Phy_BcchPara_Config(pusch_config , ref_signal_pusch_config);
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

			     rrc_SetStatus(RRC_STATUS_CONNECT_SETUP);       

				rrc_connect_request = ccch_message.message.choice.rrcConnectionrequest; 
				if (RRCConnectionRequest__establishmentCause_normal == rrc_connect_request.establishmentCause)
				{
				

			  
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
                    rrc_Rlc_ConnectSetup_Config(&drb_add);

                    //!Get new ue_index 
                    ue_request_index = dict_GetNewUeIndex(g_rrc_ue_info_dict);

                    logic_channel_config[0].logical_channel_id = 1; 
                    logic_channel_config[1].chan_type = LogicChannelConfig__channel_type_dtch; 
                    logic_channel_config[1].priority = 2;  
                    //!config MAC  DTCH
                    rrc_Mac_ConnectSetup_Config(ue_request_index,
                                                 4,
                                                 4,
                                                 1,
                                                 logic_channel_config
                                                );

                    //!config PHY 
                    rrc_Phy_ConnectSetup_Config(0);

                     //!generate connectsetup message, config MAC/RLC,PHY 
				    rrc_Wrap_Message_Process(EncodeD2dCcch(encode_buffer,ENCODE_MAX_SIZE,
				                                      &encode_buffer_size,
				                                      CCCH_MessageType_PR_rrcConnectionsetup)); 


				    //!TODO send buffer_status require message to RLC 
				    rrc_Rlc_DataBuf_Sta_Req(encode_buffer_size); 
				    
						 
                    rrc_SetStatus(RRC_STATUS_CONNECT_SETUP);
					
				}


            }
            else if (CCCH_MessageType_PR_rrcConnectionsetup == ccch_message.message.present)
            {

              
                AssertFatal(mode_type ==  D2D_MODE_TYPE_DESTINATION, 
			           RRC, 
			           "only Destination can receive rrcConnectionsetup  message\n"); 
				

                rrc_SetStatus(RRC_STATUS_CONNECT_SETUP);    
				 //! config RLC
                rrc_rbinfo_decode_connect_setup(ccch_message.message.choice.rrcConnectionsetup,
                                                &srb_add,
                                                &drb_add,
                                                logic_channel_config, 
                                                &logic_ch_num, 
                                                &max_harq_tx, 
                                                &max_out_sync
                                            ); 

                
				
                //!TODO rlc doesn't need config SRB ?
                rrc_Rlc_ConnectSetup_Config(&drb_add);

                rrc_Mac_ConnectSetup_Config( 0, //!fix to 0 at destination mode 
                                             max_harq_tx,  
                                             max_out_sync,  
                                             logic_ch_num,  
                                             logic_channel_config);
                

                RadioResourceConfigDedicate_ptr = \
                	&ccch_message.message.choice.rrcConnectionsetup.radioResourceConfigCommon; 
                
                rrc_Phy_ConnectSetup_Config(RadioResourceConfigDedicate_ptr->pusch_dedi_config.beta_off_ack_ind); 

                


				

				//! generate  rrcconnectinocomplete message to send 
				 rrc_Wrap_Message_Process(EncodeD2dCcch(encode_buffer,256,
				                                      &encode_buffer_size,
				                                      CCCH_MessageType_PR_rrcConectioncomplete)); 

				 //!TODO send encode buffer to RLC and send to air interface 
						                                      

				//! update rrc status for destination 
                rrc_SetStatus(RRC_STATUS_CONNECTED);
				
            }
            else if (CCCH_MessageType_PR_rrcConectioncomplete == ccch_message.message.present)
            {
				AssertFatal(D2D_MODE_TYPE_SOURCE == mode_type, 
				       RRC, 
			           "only SOURCE can receive rrcConectioncomplete  message\n");

				//!update RRC status for source 
                rrc_SetStatus(RRC_STATUS_CONNECTED);

            }
            else 
            {
				AssertFatal(0,RRC,"CCCH message decode IE error\n");
            }
 		
 			 break; 
        }	   	
        case RLC_RRC_BUF_STATUS_RPT: 
        {
			//!according to RLC's BUFFER SIZE ,decide to send message to rlc or not 
			rlc_buffer_rpt_ptr = (rlc_rrc_buffer_rpt *)message; 
			AssertFatal((1 == rlc_buffer_rpt_ptr->rlc_buffer_valid),RRC, "RLC BUFFER is full,can't send message !\n"); 
			

			//!TODO ,send DATA_IND message to RLC 
			rrc_Rlc_Data_Send(RB_TYPE_SRB0, g_rrc_messge_encode,rlc_buffer_rpt_ptr->send_data_size); 

			break ;
        }
        
        default :break; 	     	
	}
	
}





void  rrc_Sche_Task( )
{
	rrc_Initial(); 


	if (D2D_MODE_TYPE_SOURCE == rrc_GetModeType())
	{
		//!cell setup, initial PHY, MAC, RLC.
		rrc_Phy_InitialConfig(g_rrc_init_para); 
		rrc_Mac_InitialConfig(rrc_GetModeType(), g_rrc_init_para);
 		rrc_Rlc_InitialConfig(); 
	}
	if ( D2D_MODE_TYPE_DESTINATION == rrc_GetModeType())
	{
		//！config PHY to cell search 
		rrc_Phy_CellSearch(g_rrc_init_para.ul_freq,g_rrc_init_para.dl_freq); 
		//!TODO  start timer 
	}















}
