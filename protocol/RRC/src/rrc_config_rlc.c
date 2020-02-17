/******************************************************************
 * @file  rrc_config_rlc.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月15日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月15日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rrc_global_def.h>
#include <interface_rrc_rlc.h> 
#include <intertask_interface.h> 
#include <d2d_message_type.h> 
#include <log.h>
 
 
/**************************function******************************/

 /*!   
 * @brief:  initial RLC 
 * @author:  bo.liu
 * @Date:  2019年8月29日
 */
void rrc_Rlc_InitialConfig(uint32_t        mode )
{
    MessageDef  *message; 
	rrc_rlc_initial_req *rlc_req = (rrc_rlc_initial_req *)OSP_Alloc_Mem(sizeof(rrc_rlc_initial_req)); 

	rlc_req->initial_flag = 1; 
	rlc_req->mode = mode;

	//!TODO
	message = itti_alloc_new_message(TASK_D2D_RRC, RRC_RLC_INITIAL_REQ,
	                       ( char *)rlc_req, sizeof(rrc_rlc_initial_req));

	itti_send_msg_to_task(TASK_D2D_RLC,0, message);
}


 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: rb_type :          [param description ]
 * @param: rb_id :            [param description ]
 * @param: logicch_type :     [param description ]
 * @param: logicch_id :       [param description ]
 * @param: rlc_mode :         [param description ]
 * @param: um_sm_field :      [param description ]
 * @param: um_t_recording :   [param description ]
 */
rb_info rrc_Rlc_Rbinfo_Generate(rb_type_e rb_type, uint8_t rb_id, 
                                        uint8_t logicch_type,uint8_t logicch_id,
                                        rlc_mode_e rlc_mode,
                                        uint16_t um_sm_field,
                                        uint16_t um_t_recording)
{
	rb_info temp_rb; 

	temp_rb.rb_id = rb_id; 
	temp_rb.rlc_mode = rlc_mode; 
	temp_rb.logicchannel_type = logicch_type; 
	temp_rb.logicchannel_id = logicch_id; 
	AssertFatal(rlc_mode != RLC_MODE_AM, RRC,"RRC can't support AM mode for this version\n"); 
	if ( rlc_mode >= RLC_MODE_UM ) /**1:tm_dl,2:tm_ul, 3:tm ul&dl, 3:um_dl,4:um_ul, 5:um_dl&ul **/
	{
		temp_rb.rlc_mode_cfg.ul_um_cfg.sn_field = um_sm_field; 
		temp_rb.rlc_mode_cfg.dl_um_cfg.sn_field = um_sm_field; 
		temp_rb.rlc_mode_cfg.dl_um_cfg.t_recordering = um_t_recording; 
	}
	else //!TM 
	{
		
	}

    
	return temp_rb;

}

 /*!   
 * @brief:  config srb1
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: rb_type :          [1:srb ]
 * @param: srb_count :        [srb count  ]
 * @param: *rb_info_ptr :     [pointer to srb info array]
 */
rrc_rlc_srb_addmod_req  rrc_Rlc_Srb_Config(rb_type_e rb_type,uint16_t srb_count,rb_info *rb_info_ptr)
{
		rrc_rlc_srb_addmod_req  srb_info; 
		uint16_t srb_index = 0; 

		AssertFatal(srb_count <= MAX_SRB_COUNT, RRC, "RRC %d ,  SRB config error,count:%d exceed the max limit\n",
						rrc_GetModeType(), srb_count); 
		
        	
        srb_info.srb_count = srb_count;
        srb_info.requset_id = 0; //!TODO,maybe useless 
		srb_info.rb_type = rb_type; 
		for (srb_index = 0; srb_index < srb_count; srb_index ++)
		{
			memcpy((void *)&srb_info.srb_list[srb_index],
			        (void *)rb_info_ptr++, 
			        sizeof(rb_info));
		}

		return  srb_info;
	
}


 /*!   
 * @brief:  send to rlc srb1 config message 
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: *srb_info :        [pointer to srb_info array  ]
 */
void rrc_Rlc_BcchPara_Config(rrc_rlc_srb_addmod_req *srb_info)
{
	rrc_rlc_bcch_para_cfg  *bcch_req; 
	MessageDef  *message; 
    uint32_t rb_index = 0; 

	bcch_req = (rrc_rlc_bcch_para_cfg  *) OSP_Alloc_Mem(sizeof(rrc_rlc_bcch_para_cfg)); 

	bcch_req->srb_cfg_req = *srb_info;

	LOG_DEBUG(RRC, "RRC config RLC SRB info: srb_type = %d,srb_count = %d\n",srb_info->rb_type, srb_info->srb_count); 

	for (rb_index = 0; rb_index < srb_info->srb_count; rb_index++)
	{
		LOG_DEBUG(RRC, "RRC MODE:%d, srb index =%d, rb_id = %d, rb_rlc_mode = %d\n", rrc_GetModeType(),
			rb_index, srb_info->srb_list[rb_index].rb_id, srb_info->srb_list[rb_index].rlc_mode); 
		if (srb_info->srb_list[rb_index].rlc_mode >= RLC_MODE_UM)
		{
			LOG_DEBUG(RRC, "srb index =%d, UM mode para: sn_field = %d, t_recordering = %d\n",
			rb_index, srb_info->srb_list[rb_index].rlc_mode_cfg.dl_um_cfg.sn_field,
			srb_info->srb_list[rb_index].rlc_mode_cfg.dl_um_cfg.t_recordering); 
		}
	}

	
	//!TODO send msssage
	message = itti_alloc_new_message(TASK_D2D_RRC, RRC_RLC_BCCH_PARA_CFG_REQ,
	                       ( char *)bcch_req, sizeof(rrc_rlc_bcch_para_cfg));

	itti_send_msg_to_task(TASK_D2D_RLC,  0, message);
}






 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: rb_type :          [param description ]
 * @param: drb_conut :        [param description ]
 * @param: *rb_info_ptr :     [param description ]
 */
rrc_rlc_drb_addmod_req  rrc_Rlc_Drb_Config(rb_type_e rb_type,uint16_t drb_count,rb_info *rb_info_ptr)
{
		rrc_rlc_drb_addmod_req  drb_info; 
		uint16_t rb_index = 0; 

	


 		if ((rb_type != RB_TYPE_DRB) ||  (drb_count > MAX_DRB_COUNT))
		{
			LOG_ERROR(RRC,"rrc_Rlc_Drb_Config rb_type error, or drb_count error!\n");
		}


        drb_info.requset_id = 0; //!TODO,maybe useless 
		drb_info.rb_type = rb_type; 
		drb_info.drb_count = drb_count; 
		for (rb_index = 0; rb_index < drb_count; rb_index ++)
		{
			memcpy((void *)&drb_info.drb_list[rb_index],
			        (void *)rb_info_ptr++, 
			        sizeof(rb_info));
		}

		return  drb_info;
		

}





 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: *drb_info :        [param description ]
 */
void rrc_Rlc_ConnectSetup_Config(uint32_t ue_rnti, uint32_t ue_index ,
										  rrc_rlc_srb_addmod_req * srb_info,
										  rrc_rlc_drb_addmod_req *drb_info)
{

    MessageDef  *message; 
	rrc_rlc_connect_setup_cfg  *connect_setup_req = (rrc_rlc_connect_setup_cfg  *)OSP_Alloc_Mem(sizeof(rrc_rlc_connect_setup_cfg)); 

	
	connect_setup_req->ue_index = ue_index; 
	connect_setup_req->ue_rnit = ue_rnti; 
	memcpy((void *)&connect_setup_req->drb_cfg_req,(void *)drb_info,sizeof(rrc_rlc_drb_addmod_req)); 
	memcpy((void *)&connect_setup_req->srb_cfg_req,(void *)srb_info,sizeof(rrc_rlc_srb_addmod_req));

	//!TODO send msssage
	message = itti_alloc_new_message(TASK_D2D_RRC, RRC_RLC_CONNECT_SETUP_CFG_REQ,
	                       ( char *)connect_setup_req, sizeof(rrc_rlc_connect_setup_cfg));

	itti_send_msg_to_task(TASK_D2D_RLC,  0, message);
}


 /*!   
 * @brief:  send buffer status request message to rlc 
 * @author:  bo.liu
 * @Date:  2019年8月28日
 * @param: data_size :        [size of the send message ]
 */
void rrc_Rlc_DataBuf_Sta_Req(rb_type_e         rb_type,uint32_t rb_id,rnti_t rnti, uint32_t data_size)
{
	MessageDef  *message; 

	rrc_rlc_buffer_status_req    *data_status_req = (rrc_rlc_buffer_status_req    *)OSP_Alloc_Mem(sizeof(rrc_rlc_buffer_status_req)); 

	data_status_req->request_id = 0; 
	data_status_req->rb_type = rb_type; 
	data_status_req->rb_id = rb_id;
	data_status_req->rnti  = rnti; 
	data_status_req->send_data_size = data_size; 

	//! there should be insert the requset to RRC_RLC_FIFO,  and delete the node when receive the buffer status report
     
    LOG_DEBUG(RRC, "RRC send RLC buffer status inquiry:send data's size = %d\n", data_size);
    message = itti_alloc_new_message(TASK_D2D_RRC, RRC_RLC_BUF_STATUS_REQ,
	                       ( char *)data_status_req, sizeof(rrc_rlc_buffer_status_req));

	itti_send_msg_to_task(TASK_D2D_RLC,  0, message);

}


 /*!   
 * @brief:  send data to RLC 
 * @author:  bo.liu
 * @Date:  2019年8月29日
 * @param: rb_type :          [param description ]
 * @param: *data_buffer :     [param description ]
 * @param: data_size :        [param description ]
 */
void rrc_Rlc_Data_Send(rb_type_e rb_type, rb_id_t rb_id, rnti_t rnti, uint32_t *data_buffer, uint32_t data_size)
{
	MessageDef  *message;  

	rrc_rlc_data_ind  *data_ind_ptr = (rrc_rlc_data_ind  *)OSP_Alloc_Mem(sizeof(rrc_rlc_data_ind)); 

 	data_ind_ptr->rb_type = rb_type; 
 	data_ind_ptr->rb_id = rb_id; 
 	data_ind_ptr->rnti = rnti; 
 	data_ind_ptr->data_addr_ptr = data_buffer; 
 	data_ind_ptr->data_size = data_size; 

 	
    message = itti_alloc_new_message(TASK_D2D_RRC, RRC_RLC_DATA_IND,
	                       ( char *)data_ind_ptr, sizeof(rrc_rlc_data_ind));

	itti_send_msg_to_task(TASK_D2D_RLC,  0, message);
}



 /*!   
 * @brief:  rrc_Rlc_Release_Req 
 * @author:  bo.liu
 * @Date:  2019年8月29日
 * @param: cell_id :          [cell id  ]
 * @param: ue_index :         [ue_index  ]
 */
void rrc_Rlc_Release_Req(uint32_t cell_id, uint32_t ue_index)
{
	MessageDef  *message;  

	rrc_rlc_release_req   *data_ind_ptr = (rrc_rlc_release_req   *)OSP_Alloc_Mem(sizeof(rrc_rlc_release_req)); 

    data_ind_ptr->cell_id = cell_id; 
    data_ind_ptr->ue_index  = ue_index; 

    message = itti_alloc_new_message(TASK_D2D_RRC, RRC_RLC_RELEASE_REQ,
	                       ( char *)data_ind_ptr, sizeof(rrc_rlc_release_req));
	                       
	itti_send_msg_to_task(TASK_D2D_RLC,  0, message);


}
