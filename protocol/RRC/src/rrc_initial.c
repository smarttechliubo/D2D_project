/******************************************************************
 * @file  rrc_initial.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月13日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月13日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
 #include <typedef.h>
 #include <d2d_system_global.h>
 #include <rrc_global_def.h>
 
 
 
/**************************function******************************/
void rrc_initial()
{
	
#ifdef RRC_SOURCE	
	g_rrc_init_para.source_type = RRC_TYPE_SOURCE;    //! source
#else 
    g_rrc_init_para.source_type = RRC_TYPE_DESTINATION;    //!destination
#endif 

    g_rrc_init_para.cell_id = 0; 

	g_rrc_init_para.subframe_config = SUBFRAME_CONFIG_DDUUDDUU; 
	g_rrc_init_para.band_width = BANDWIDTH_3M; 

	   //!TODO 
    g_rrc_init_para.band_info = 0; 
	g_rrc_init_para.dl_freq = 0; 
	g_rrc_init_para.ul_freq = 0; 

	//! mib 
	g_rrc_init_para.mib_info.mib_sfn = 0; //!don't care this param
	g_rrc_init_para.mib_info.pdcch_rb_start = 2; 
	g_rrc_init_para.mib_info.pdcch_rb_num = 2; 

}

 
