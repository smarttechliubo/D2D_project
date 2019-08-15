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
#include <rrc_global_def.h>
#include <interface_rrc_phy.h>

 
 
 
/**************************function******************************/
 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: init_var :         [global initial varibal ]
 */
void  rrc_Phy_InitialConfig(rrc_init_var init_var)
{
	rrc_phy_initial_req   phy_init_req; 
	
    phy_init_req.source_type =  init_var.source_type; 
	phy_init_req.bandwith = (uint16_t )init_var.band_width; 
	phy_init_req.cellId = init_var.cell_id; 
	
    phy_init_req.subframe_config = (uint16_t)init_var.subframe_config; 
	phy_init_req.dl_freq = init_var.dl_freq; 
	phy_init_req.ul_freq = init_var.ul_freq; 

	 //!TODO
    phy_init_req.pusch_config_basic.hop_mode = 0; 
	phy_init_req.pusch_config_basic.pusch_hop_offset = 0; 

	phy_init_req.ul_ref_signal_pusch.grp_hop_enabled = 0; 
	phy_init_req.ul_ref_signal_pusch.grp_assign_pusch = 0; 
	phy_init_req.ul_ref_signal_pusch.seq_hop_enabled = 0; 
	phy_init_req.ul_ref_signal_pusch.cyc_shift = 0; 

	//!TODO send message to PHY
	
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
	rrc_phy_cs_req  cs_req; 

	cs_req.dl_freq = dl_freq;
	cs_req.ul_freq = ul_freq; 

    //!TODO send message 
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
	
	rrc_phy_bcch_para_cfg_req    bcch_req; 
	bcch_req.pusch_config_basic = push_basic_config; 
	bcch_req.ul_ref_signal_pusch =	ul_refsig_config; 

    //!TODO send message 

}

 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: beta_off_ack_ind : [param description ]
 */
void rrc_Phy_ConnectSetup_Config(uint16_t              beta_off_ack_ind)
{
	rrc_phy_connection_setup_req   connect_setup_req; 
	connect_setup_req.pusch_dedi_config.beta_off_ack_ind = beta_off_ack_ind; 
	
     //!TODO send message 
}


