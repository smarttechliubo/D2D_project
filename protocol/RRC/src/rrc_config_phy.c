/******************************************************************
 * @file  rrc_config_phy.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月15日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月15日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <d2d_message_type.h>
#include <rrc_global_def.h>
#include <interface_rrc_phy.h>
#include <intertask_interface.h>
 
 
 
/**************************function******************************/
 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: init_var :         [global initial varibal ]
 */
void  rrc_Phy_InitialConfig(rrc_init_var init_var)
{

    MessageDef  *message; 
	rrc_phy_initial_req   *phy_init_req = calloc(1,sizeof(rrc_phy_initial_req)); 
	
    phy_init_req->source_type =  init_var.source_type; 
	phy_init_req->bandwith = (uint16_t )init_var.band_width; 
	phy_init_req->cellId = init_var.cell_id; 
	
    phy_init_req->subframe_config = (uint16_t)init_var.subframe_config; 
	phy_init_req->dl_freq = init_var.dl_freq; 
	phy_init_req->ul_freq = init_var.ul_freq; 

	 //!TODO
    phy_init_req->pusch_config_basic.hop_mode = 0; 
	phy_init_req->pusch_config_basic.pusch_hop_offset = 0; 

	phy_init_req->ul_ref_signal_pusch.grp_hop_enabled = 0; 
	phy_init_req->ul_ref_signal_pusch.grp_assign_pusch = 0; 
	phy_init_req->ul_ref_signal_pusch.seq_hop_enabled = 0; 
	phy_init_req->ul_ref_signal_pusch.cyc_shift = 0; 

	//!TODO send message to PHY
	message = itti_alloc_new_message(TASK_D2D_RRC, RRC_PHY_INITIAL_REQ,
	                       ( char *)phy_init_req, sizeof(rrc_phy_initial_req));

	itti_send_msg_to_task(TASK_D2D_PHY,  0, message);
	
}


 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: ul_freq :          [param description ]
 * @param: dl_freq :          [param description ]
 */
void rrc_Phy_CellSearch(uint16_t ul_freq, uint16_t dl_freq)
{
     MessageDef  *message; 
	rrc_phy_cs_req  *cs_req = calloc(1,sizeof(rrc_phy_cs_req)); 

	cs_req->dl_freq = dl_freq;
	cs_req->ul_freq = ul_freq; 

    //!TODO send message 
   message = itti_alloc_new_message(TASK_D2D_RRC, RRC_PHY_CS_REQ,
	                       ( char *)cs_req, sizeof(rrc_phy_cs_req));

	itti_send_msg_to_task(TASK_D2D_PHY,  0, message);
	
}


 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: push_basic_config :[param description ]
 * @param: ul_refsig_config : [param description ]
 */
void rrc_Phy_BcchPara_Config(pusch_config_basic_s push_basic_config,
                                      ul_ref_signal_pusch_s ul_refsig_config)
{
	
    MessageDef  *message; 
	rrc_phy_bcch_para_cfg_req    *bcch_req = calloc(1,sizeof(rrc_phy_bcch_para_cfg_req)); 
	bcch_req->pusch_config_basic = push_basic_config; 
	bcch_req->ul_ref_signal_pusch =	ul_refsig_config; 

    //!TODO send message 
    message = itti_alloc_new_message(TASK_D2D_RRC, RRC_PHY_BCCH_PARA_CFG_REQ,
	                       ( char *)bcch_req, sizeof(rrc_phy_bcch_para_cfg_req));

	itti_send_msg_to_task(TASK_D2D_PHY,  0, message);

}

 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: beta_off_ack_ind : [param description ]
 */
void rrc_Phy_ConnectSetup_Config(uint16_t              beta_off_ack_ind)
{

    MessageDef  *message; 
	rrc_phy_connection_setup_req   *connect_setup_req = calloc(1,sizeof(rrc_phy_connection_setup_req)); 

	connect_setup_req->pusch_dedi_config.beta_off_ack_ind = beta_off_ack_ind; 
	
     //!TODO send message 
    message = itti_alloc_new_message(TASK_D2D_RRC, RRC_PHY_CONNECT_SETUP_CFG_REQ,
	                       ( char *)connect_setup_req, sizeof(rrc_phy_connection_setup_req));
	                       
	itti_send_msg_to_task(TASK_D2D_PHY,  0, message);
}


