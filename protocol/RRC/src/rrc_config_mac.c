/******************************************************************
 * @file  rrc_config_mac.c
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
#include <interface_rrc_mac.h>
 
 
 
 
/**************************function******************************/
 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: init_var :         [initial rrc varibal ]
 */
void rrc_Mac_InitialConfig(uint16_t mode_type,rrc_init_var init_var)
{
	rrc_mac_initial_req   mac_init_req;

   
	mac_init_req.cellId = init_var.cell_id; 
	mac_init_req.bandwith = (uint16_t)init_var.band_width;
	mac_init_req.subframe_config = (uint16_t)init_var.subframe_config; 
	mac_init_req.mode = (uint16_t)init_var.source_type; 

	mac_init_req.pdcch_config.rb_start_index = init_var.mib_info.pdcch_rb_start; 
	mac_init_req.pdcch_config.rb_num = init_var.mib_info.pdcch_rb_num; 

    
     //!TODO send message 
}



 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: mib_or_sib1 :      [1:mib, 2:sib ]
 * @param: *bcch_info :       [pointer of message info ]
 */
void rrc_Mac_BcchPara_Config(uint16_t mib_or_sib1,void *bcch_info)
{

    rrc_mac_bcch_para_config_req bcch_para_config_req ;
	bcch_mib_info_s bcch_mib_info; 
	bcch_si_info_s  bcch_sib_info; 

	
	bcch_para_config_req.flag = mib_or_sib1;
	if (1 == bcch_para_config_req.flag) //!mib 
	{
		bcch_mib_info = *((bcch_mib_info_s * )bcch_info);
		bcch_para_config_req.mib = bcch_mib_info; 
	}
	else if(2 == bcch_para_config_req.flag)
	{ 
	    bcch_sib_info = *((bcch_si_info_s *)bcch_info);
	    bcch_para_config_req.sib = bcch_sib_info; 
	}
	else 
	{	
		//!alarm

	}


	//!TODO send message 
	
	
}

 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: ue_index :         [param description ]
 * @param: max_harq_tx :      [param description ]
 * @param: max_out_sync :     [param description ]
 * @param: logicch_num :      [valid logic channel number ]
 * @param: *LogicChannelConfigPtr :[pointer to the logic channel info ]
 */
void rrc_Mac_ConnectSetup_Config(uint16_t ue_index, uint16_t max_harq_tx,uint16_t max_out_sync,
                                            uint16_t logicch_num, logical_channel_config_s *LogicChannelConfigPtr )
{
    uint16_t lch_index = 0; 
	rrc_mac_connnection_setup   mac_connect_req; 

	mac_connect_req.ue_index = ue_index; 
	mac_connect_req.maxHARQ_Tx = max_harq_tx; 
	
	mac_connect_req.max_out_sync = max_out_sync; 

    
	for (lch_index = 0;lch_index < logicch_num;lch_index++)
	{
        memcpy((void *)&mac_connect_req.logical_channel_config[lch_index],
               (void *)LogicChannelConfigPtr++,
               sizeof(logical_channel_config_s)); 
	}

    //!TODO send message 
}

 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: cell_id :          [param description ]
 * @param: ue_idx :           [param description ]
 * @param: release_cause :    [param description ]
 */
void rrc_Mac_Release_Config(uint16_t cell_id,uint16_t ue_idx, uint16_t release_cause)
{	
	rrc_mac_release_req  mac_release_req; 

	mac_release_req.cellId = cell_id; 
	mac_release_req.ue_index  = ue_idx; 
	mac_release_req.releaseCause = release_cause; 

	//!TODO send message 

}

