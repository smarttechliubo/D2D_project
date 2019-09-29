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
 #include <dictionary.h>
 
 
/**************************function******************************/
 /*!   
 * @brief:  rrc task initial function
 * @author:  bo.liu
 * @Date:  2019年8月15日
 */
void rrc_module_Initial()
{
	
	memset((void *)&g_rrc_init_para,0,sizeof(g_rrc_init_para));
	
#ifdef RRC_SOURCE	
	rrc_SetModeType(D2D_MODE_TYPE_SOURCE);    //! source
#else 
    rrc_SetModeType(D2D_MODE_TYPE_DESTINATION);    //!destination
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

	g_rrc_ue_info_dict = dict_init(); 



}


 /*!   
 * @brief:  rrc Status Change to next status 
 * @author:  bo.liu
 * @Date:  2019年8月15日
 * @param: rrc_status :       [next valid rrc status ]
 */
void rrc_SetStatus(rrc_status_e          rrc_next_status)
{
	g_rrc_status = rrc_next_status; 
	LOG_INFO(RRC, "MODE: %d, RRC STATUS = %d\n", rrc_GetModeType(),g_rrc_status);
}

 /*!   
 * @brief:  rrc Get CurrentStatus 
 * @author:  bo.liu
 * @Date:  2019年8月15日
 */
rrc_status_e  rrc_GetCurrentStatus( )
{
	return g_rrc_status; 
}


int rrc_SetModeType(int mode_type)
{
	g_rrc_init_para.source_type = mode_type; 
}

int rrc_GetModeType( )
{
	return g_rrc_init_para.source_type;
}

 