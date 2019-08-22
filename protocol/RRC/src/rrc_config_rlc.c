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
 
 
 
 
/**************************function******************************/
void rrc_Rlc_InitialConfig( )
{
	rrc_rlc_initial_req rlc_req; 

	rlc_req.initial_flag = 1; 

	//!TODO
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
	if ( rlc_mode > 3 ) /**1:tm_dl,2:tm_ul, 3:tm ul&dl, 3:um_dl,4:um_ul, 5:um_dl&ul **/
	{
		temp_rb.rlc_mode_cfg.rlc_um.sn_field = um_sm_field; 
		temp_rb.rlc_mode_cfg.rlc_um.t_recordering = um_t_recording; 
	}
	else 
	{
		//!TODO ALARM ,just support um 
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


		if ((rb_type != 1) || (srb_count > MAX_SRB_COUNT))
		{
			//! TODO ALARM 
		}
        	
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
	rrc_rlc_bcch_para_cfg  bcch_req; 

	bcch_req.srb_cfg_req = *srb_info; 

	//!TODO
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

	


 		if ((rb_type != 2) ||  (drb_count > MAX_DRB_COUNT))
		{
			//!TO ALARAM 
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
void rrc_Rlc_ConnectSetup_Config(rrc_rlc_drb_addmod_req *drb_info)
{
	rrc_rlc_connect_setup_cfg  connect_setup_req; 
	
	connect_setup_req.drb_cfg_req = *drb_info; 

	//!TODO send msssage
}

