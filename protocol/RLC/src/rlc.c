/******************************************************************
 * @file  rlc.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <d2d_system_global.h>
#include <hashtable.h> 
#include <interface_rrc_rlc.h>
#include <interface_mac_rlc.h>
#include <rlc.h>

rlc_union_t 	*rlc_Get_HashNode(const protocol_ctxt_t* const ctxt_pP,
									   boolean_t  srb_flagP,
									   rb_id_t rb_idP)

{
	hashtable_rc_t		   h_lcid_rc;
	hashtable_rc_t		   h_rc;
	rlc_union_t 		   *rlc_union_p = NULL;
	hash_key_t			   key		   = HASHTABLE_NOT_A_KEY_VALUE;
	hash_key_t			   key_lcid    = HASHTABLE_NOT_A_KEY_VALUE;
	uint32_t               sdu_buffer_size = 0;
	uint32_t               buffer_occupancy = 0;
	

    

	key = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);

	h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);

	if (HASH_TABLE_OK == h_rc)
	{
		LOG_DEBUG(RLC,PROTOCOL_CTXT_FMT"[%s,rb_id:%d] Get hashnode OK!\n", PROTOCOL_CTXT_ARGS(ctxt_pP),
						   srb_flagP?"SRB":"DRB",
						   rb_idP);
		return rlc_union_p; 
	}
	else 
	{
		LOG_ERROR(RLC, PROTOCOL_CTXT_FMT"[%s,rb_id:%d] Get hashnode ERROR!\n",
						   PROTOCOL_CTXT_ARGS(ctxt_pP),
						   srb_flagP?"SRB":"DRB",
						   rb_idP);
		AssertFatal(HASH_TABLE_OK == h_rc, RLC,"Get hashnode ERROR\n"); 
		return NULL; 
	}

}



void rlc_util_print_hex_octets(char *module_name, unsigned char *dataP, const signed long sizeP)
//-----------------------------------------------------------------------------
{
#if 0
  unsigned long octet_index = 0;

  if (dataP == NULL) {
    return;
  }

  LOG_DEBUG(module_name, "+-----+-------------------------------------------------+\n");
  LOG_DEBUG(module_name, "|     |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |\n");
  LOG_DEBUG(module_name, "+-----+-------------------------------------------------+\n");

  for (octet_index = 0; octet_index < sizeP; octet_index++) {

    //!<每行打16个字节
    if ((octet_index % 16) == 0) {
      if (octet_index != 0) {
        LOG_DEBUG(componentP, " |\n");
      }

      LOG_DEBUG(componentP, " %04lu |", octet_index);
    }

    /*
     * Print every single octet in hexadecimal form
     */
    LOG_DEBUG(componentP, " %02x", dataP[octet_index]);
    /*
     * Align newline and pipes according to the octets in groups of 2
     */
  }

  /*
   * Append enough spaces and put final pipe
   */
  unsigned char index;

  for (index = octet_index; index < 16; ++index) {
    LOG_DEBUG(componentP, "   ");
  }

  LOG_DEBUG(componentP, " |\n");
 #endif 
 
}



void rb_free_rlc_union (void *rlcu_pP)
{

  //-----------------------------------------------------------------------------
  rlc_union_t * rlcu_p;

  if (rlcu_pP) {
    rlcu_p = (rlc_union_t *)(rlcu_pP);
    LOG_DEBUG(RLC,"%s %p \n",__func__,rlcu_pP);

    switch (rlcu_p->mode) {
    #ifdef AM_ENABLE 
	    case RLC_MODE_AM:
	      rlc_am_cleanup(&rlcu_p->rlc.am);
	      break;
    #endif 
	    case RLC_MODE_UM:
	      rlc_um_cleanup(&rlcu_p->rlc.um);  //！释放UM 使用的接收和发送memory链表
	      break;

	    case RLC_MODE_TM:
	      rlc_tm_cleanup(&rlcu_p->rlc.tm); //! 释放TM 使用的接收和发送memory链表
	      break;

	    default:
	      LOG_WARN(RLC,
	            "%s %p unknown RLC type\n",
	            __func__,
	            rlcu_pP);
	      break;
    }
  }

}


 

int  rlc_module_initial( )
{

  
	rlc_coll_p = hashtable_create ((MAX_SRB_COUNT + MAX_DRB_COUNT + 2) *D2D_MAX_USER_NUM , NULL, rb_free_rlc_union);
   
	AssertFatal(rlc_coll_p != NULL, RLC, "RLC initial HASH TABLE failed\n"); 

    //!memory pool initial 
 
  //  pool_buffer_init(); 

    //!initial  g_rlc_protocol_ctxt as destination 
    g_rlc_protocol_ctxt.enb_flag = 0; 
    g_rlc_protocol_ctxt.eNB_index = 0; 
    g_rlc_protocol_ctxt.module_id = 0; 
    g_rlc_protocol_ctxt.rnti = 0xffff;  //!initial value ; 
    g_rlc_protocol_ctxt.configured = FALSE; 
 
   
    memset((void *)g_rlc_buffer_status,0,sizeof(g_rlc_buffer_status));

    return 0;


}
 

int   rlc_Get_Buffer_Status(rlc_buffer_rpt *buffer_status) 
{
	uint32_t  ue_index = 0; 
	uint16_t  srb_index = 0; 
	uint32_t  drb_index = 0; 
	logical_chan_id_t  channel_id = 0; 
	uint32_t   data_size = 0; 
	uint32_t  ue_num = 0; 
	uint32_t  rnti_index = 0; 
    rnti_t    temp_rnti = 0; 
    uint16_t  ue_have_data_send = 0; 
    uint32_t  logic_ch_index = 0; 

    rlc_buffer_rpt   rlc_buffer_stat; 
#ifdef RLC_UT_DEBUG 
    g_rlc_no_data_transfer = 0; 
#endif
	memset((void *)buffer_status,0,(D2D_MAX_USER_NUM +1) *sizeof(rlc_buffer_rpt)); 

   // pthread_mutex_lock(&g_rlc_buffer_mutex); 
	for(ue_index = 0; ue_index < (D2D_MAX_USER_NUM +1); ue_index++)
	{
		if (1 == g_rlc_buffer_status[ue_index].valid_flag)
		{
			buffer_status[ue_num].valid_flag = 1; 
			buffer_status[ue_num].logic_chan_num = g_rlc_buffer_status[ue_index].latest_logic_ch_num; 

            //!data_size include the rlc header size , then report to MAC Layer 
			for (logic_ch_index = 0; logic_ch_index < buffer_status[ue_num].logic_chan_num; logic_ch_index++)
			{
#ifdef RLC_UT_DEBUG 
				if (0 == g_rlc_buffer_status[ue_index].data_size[logic_ch_index])
				{
					g_rlc_no_data_transfer = 1;
					
				}
				else 
				{
					g_rlc_no_data_transfer = 0; 
					g_rlc_ut_tx_buffer_size[ue_index][logic_ch_index] = g_rlc_buffer_status[ue_index].data_size[logic_ch_index] + \
																		 g_rlc_buffer_status[ue_index].rlc_header_size[logic_ch_index];
					 //!模拟MAC 加上mac subheader size 
					if (g_rlc_ut_tx_buffer_size[ue_index][logic_ch_index] >= 128)
					{
						g_rlc_ut_tx_buffer_size[ue_index][logic_ch_index] += 3; 
					}
					else 
					{
						g_rlc_ut_tx_buffer_size[ue_index][logic_ch_index] += 2; 
					}
					LOG_DEBUG(RLC_TX, "RLC GET TX buffer status: ue_index:%d, logic_index:%d, data_size:%d, header_size =%d \n", 
							ue_index,logic_ch_index,g_rlc_buffer_status[ue_index].data_size[logic_ch_index],
							g_rlc_buffer_status[ue_index].rlc_header_size[logic_ch_index]);
				}

#endif 
			}
			
			memcpy((void *)buffer_status[ue_num].logicchannel_id,
					(void *)g_rlc_buffer_status[ue_index].logicchannel_id,
					buffer_status[ue_num].logic_chan_num * sizeof(uint8_t)); 
					
			memcpy((void *)buffer_status[ue_num].buffer_byte_size,
					(void *)g_rlc_buffer_status[ue_index].data_size,
					buffer_status[ue_num].logic_chan_num * sizeof(uint32_t)); 
					
			memcpy((void *)buffer_status[ue_num].rlc_header_byte_size,
					(void *)g_rlc_buffer_status[ue_index].rlc_header_size,
					buffer_status[ue_num].logic_chan_num * sizeof(uint32_t)); 
			

			ue_num++; 

		}
		
	}
	
    //pthread_mutex_unlock(&g_rlc_buffer_mutex); 

    return ue_num; 


}


void  rlc_ue_data_status_update(rnti_t rnti, 
										rlc_mode_e rlc_mode, 
										uint32_t fill_sdu_num,
										uint32_t remaind_sdu_num,
										logical_chan_id_t logical_chan_id, 
									    uint16_t send_data_size)
{
	uint32_t    ue_index; 
    uint32_t    rlc_header_size; 
	
	
	ue_index = dict_GetValue(g_rrc_ue_info_dict,rnti);
	AssertFatal((ue_index < (D2D_MAX_USER_NUM + 1)), RLC, "ue num exceed max limit!!\n"); 

	AssertFatal(g_rlc_buffer_status[ue_index].valid_flag == 1, RLC, "ue_index, %due_rnti must be valid for status update! \n",ue_index); 

	LOG_ERROR(RLC_TX, "ue_index].data_size[logical_chan_id]  = %d\n",g_rlc_buffer_status[ue_index].data_size[logical_chan_id]);
	g_rlc_buffer_status[ue_index].data_size[logical_chan_id] -= send_data_size; 

    
	if (RLC_MODE_UM == rlc_mode)
	{
		rlc_header_size  = fill_sdu_num;

         //!计算发送的header size 
		 if (fill_sdu_num <= 1) 
		 {
			rlc_header_size = 0;  
		  } else 
		  { 
			rlc_header_size = (((fill_sdu_num - 1) * 3) / 2) + ((fill_sdu_num - 1) % 2);
			//!rlc header size 的更新按照剩余的sdu 个数实时计算，加上固定header 2byte 
		
          }
	}
	else 
	{
		rlc_header_size = 0; 
	}
	
      //!计算剩余的SDU 的header size 
     if (remaind_sdu_num <= 1) 
     {
		g_rlc_buffer_status[ue_index].rlc_header_size [logical_chan_id] = 2;
     }
     else
     {
    	g_rlc_buffer_status[ue_index].rlc_header_size [logical_chan_id] = 2 + (((remaind_sdu_num - 1) * 3) / 2) + ((remaind_sdu_num - 1) % 2);
     }
     
    LOG_ERROR(RLC_TX, "%s, rnti:%d, lc:%d, update buffer status: datasize substarct %d,send_sdu_num:%d, rlc_header_size substract %d,remaind_sdu_num:%d, \
    remained_data_size:%d,remined_header_size:%d \n", 
          __func__,
			rnti, 
			logical_chan_id, 
			send_data_size, 
			fill_sdu_num, 
			rlc_header_size, 
			remaind_sdu_num,
			g_rlc_buffer_status[ue_index].data_size[logical_chan_id],
			g_rlc_buffer_status[ue_index].rlc_header_size[logical_chan_id] ); 


}


void   rlc_Set_Buffer_Status(rnti_t rnti,
									   rlc_mode_e rlc_mode,
									   uint32_t input_sdu_num,
									   logical_chan_id_t logical_chan_id, 
									   uint16_t data_size)
{
	uint32_t   ue_index ; 
	uint32_t   lch_index = 0; 
	uint32_t   rlc_header_size = 0; 
	
	//pthread_mutex_lock(&g_rlc_buffer_mutex); 

	 //!calculate rlc header size for all of the SDU for UM
	if (RLC_MODE_UM == rlc_mode)
	{
		 if (input_sdu_num <= 1) 
		 {
			rlc_header_size = 2;  //!fixed header size
		  } else 
		  { 
			rlc_header_size = 2 + (((input_sdu_num - 1) * 3) / 2) + ((input_sdu_num - 1) % 2);
          }

          LOG_WARN(RLC_TX, "rnti:%d,input_sdu_num:%d , rlc_header_size:%d,data_size:%d \n",rnti,input_sdu_num,rlc_header_size,data_size );

	}
	else 
	{
		rlc_header_size = 0; 
	}

    
        
	ue_index = dict_GetValue(g_rrc_ue_info_dict,rnti);
	AssertFatal((ue_index < (D2D_MAX_USER_NUM + 1)), RLC_TX, "ue num exceed max limit!!\n"); 
	
	if (0 == g_rlc_buffer_status[ue_index].valid_flag)
	{


		//!add new ue to array
		memset((void *)&g_rlc_buffer_status[ue_index],0,sizeof(rlc_buffer_status));

		g_rlc_buffer_status[ue_index].rnti = rnti; 
		lch_index = 0; 
		g_rlc_buffer_status[ue_index].logicchannel_id[lch_index] = logical_chan_id; 
		g_rlc_buffer_status[ue_index].data_size[lch_index] = data_size; 
		
		g_rlc_buffer_status[ue_index].rlc_header_size[lch_index] = rlc_header_size; //!update rlc header size   
		g_rlc_buffer_status[ue_index].latest_logic_ch_num++; 
		g_rlc_buffer_status[ue_index].valid_flag = 1; 
	}
	else 
	{


		//! modify UE info to array
		for(lch_index = 0; lch_index < g_rlc_buffer_status[ue_index].latest_logic_ch_num;lch_index++)
		{
			if (logical_chan_id == g_rlc_buffer_status[ue_index].logicchannel_id[lch_index])
			{
				break; 
			}
		}
		g_rlc_buffer_status[ue_index].logicchannel_id[lch_index] = logical_chan_id; 
		g_rlc_buffer_status[ue_index].data_size[lch_index] += data_size; 
		g_rlc_buffer_status[ue_index].latest_logic_ch_num = lch_index+1; 
		g_rlc_buffer_status[ue_index].rlc_header_size[lch_index] = rlc_header_size; //!update rlc header size   


	}

   

	//pthread_mutex_unlock(&g_rlc_buffer_mutex); 

}


/**************************function******************************/
