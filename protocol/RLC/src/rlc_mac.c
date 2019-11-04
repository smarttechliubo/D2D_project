/******************************************************************
 * @file  rlc_mac.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月18日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月18日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc.h>
#include <d2d_message_type.h>
#include <interface_mac_rlc.h>
#include <intertask_interface.h>
#include <mac_rlc_primitives.h>


 
 
 
 
/**************************function******************************/
void rlc_Mac_BufferSta_Rpt(uint16_t   sfn, uint16_t  subsfn, uint32_t valid_ue_num,rlc_buffer_rpt *buffer_status)
{

    MessageDef  *message; 
	rlc_mac_buf_status_rpt  *rlc_rpt = calloc(1,sizeof(rlc_mac_buf_status_rpt)); 

	rlc_rpt->sfn = sfn;
	rlc_rpt->sub_sfn = subsfn;
	rlc_rpt->valid_ue_num = valid_ue_num;
	
	memcpy((void *)rlc_rpt->rlc_buffer_rpt,(void *)buffer_status,valid_ue_num * sizeof(rlc_buffer_rpt));


	message = itti_alloc_new_message(TASK_D2D_RLC, RLC_MAC_BUF_STATUS_RPT,
	                       ( char *)rlc_rpt, sizeof(rlc_mac_buf_status_rpt ));
    
	itti_send_msg_to_task(TASK_D2D_MAC_RLC,0, message);

}




tbs_size_t mac_rlc_serialize_tb (char* buffer_pP, 
										list_t transport_blocksP)
{
  //-----------------------------------------------------------------------------
  mem_block_t *tb_p;
  tbs_size_t   tbs_size;
  tbs_size_t   tb_size;

  tbs_size = 0;
  //! 循环tb 个数
  while (transport_blocksP.nb_elements > 0) {
    tb_p = list_remove_head (&transport_blocksP); //！从head 开始依次的处理，每次处理后，更新head 

    if (tb_p != NULL) {
      tb_size = ((struct mac_tb_req *) (tb_p->data))->tb_size; 
      //！每次将一个节点所指向的地址的数据向buffer中copy,并增加tbs_size ,直到所有的节点处理完
      //! 这里注意，这里搬移的数据是包含了RLC header的数据
      memcpy(&buffer_pP[tbs_size], &((struct mac_tb_req *) (tb_p->data))->data_ptr[0], tb_size);
      tbs_size = tbs_size + tb_size;
      free_mem_block(tb_p, __func__);
    }
  }

  return tbs_size;
}

struct mac_data_ind mac_rlc_deserialize_tb (char	   *buffer_pP,
													const tb_size_t tb_sizeP,
													uint32_t	num_tbP)
{
	//-----------------------------------------------------------------------------
	struct mac_data_ind  data_ind;
	mem_block_t*		   tb_p;
	uint32_t			   nb_tb_read;
	tbs_size_t		   tbs_size;

	nb_tb_read = 0;
	tbs_size	 = 0;
	list_init(&data_ind.data, NULL); //!初始化链表


    //!按照逻辑信道的TB 块进行搬移，如果逻辑信道有2个TB块，则会有2个节点添加到list中去。
	while (num_tbP > 0) {
		//！get 一块大为mem_block_t + sizeof (mac_rlc_max_rx_header_size_t) + tb_sizeP 大小的memory 
		tb_p = get_free_mem_block(sizeof (mac_rlc_max_rx_header_size_t) + tb_sizeP, __func__);

		if (tb_p != NULL) {
		((struct mac_tb_ind *) (tb_p->data))->first_bit = 0;
		((struct mac_tb_ind *) (tb_p->data))->data_ptr = (uint8_t*)&tb_p->data[sizeof (mac_rlc_max_rx_header_size_t)];
		((struct mac_tb_ind *) (tb_p->data))->size = tb_sizeP;
		((struct mac_tb_ind *) (tb_p->data))->error_indication = 0; //! CRC is not used
		

		memcpy(((struct mac_tb_ind *) (tb_p->data))->data_ptr, &buffer_pP[tbs_size], tb_sizeP);

#ifdef DEBUG_MAC_INTERFACE
		LOG_DEBUG(RLC, "[MAC-RLC] DUMP RX PDU(%d bytes):\n", tb_sizeP);
		rlc_util_print_hex_octets(RLC, ((struct mac_tb_ind *) (tb_p->data))->data_ptr, tb_sizeP);
#endif

		nb_tb_read = nb_tb_read + 1;
		tbs_size	 = tbs_size   + tb_sizeP;
		list_add_tail_eurecom(tb_p, &data_ind.data); //！将mac给过来的数据添加到data_ind.dat链表后面
		}

		num_tbP = num_tbP - 1;
	}

	data_ind.no_tb		= nb_tb_read;
	data_ind.tb_size	= tb_sizeP << 3; //!unit:bit 

	return data_ind;
}



void rlc_mac_ue_data_process(frame_t frameP, 
									sub_frame_t  subframeP,
								    rlc_data_req *rlc_data_ptr, 
								    protocol_ctxt_t *protocol_ctxt_ptr,
								    mac_pdu_size_para  *ue_pdu_size_para_ptr,
								    uint8_t *ue_pdu_buffer,
								    uint8_t *ue_mac_subheader_ptr
								    )
{
	rnti_t   ue_rnti; 
	logical_chan_id_t  logic_index; 
	logical_chan_id_t  logic_num; 
	uint8_t            logic_ch_id; 

	protocol_ctxt_t     temp_ctxt;
	tb_size_t           tb_size_for_lc; 

	tb_size_t           pdu_total_size; 
	tb_size_t           insert_tb_size; 

    mac_datapdu_subheader         subheader[MAX_LOGICCHAN_NUM]; 
    logic_channel_pdu_component   lc_pdu_component[MAX_LOGICCHAN_NUM]; 

    uint8_t      mac_subheader_without_l = 0; 
    uint8_t      mac_subheader_2byte[2] = {0,0}; 
    uint8_t      mac_subheader_3byte[3] = {0,0,0}; 
    uint8_t      *subheader_ptr = ue_mac_subheader_ptr; 
    uint8_t      e = 0; 
    uint8_t      f = 0; 
    uint8_t      pading_header_i = 0; 
    uint32_t     mac_subheader_length = 0; 
    uint32_t     mac_sdu_tb_size = 0; 

    uint8_t      *mac_pdu_buffer_ptr = ue_pdu_buffer; 
    
	ue_rnti = rlc_data_ptr->rnti; 
	logic_num = rlc_data_ptr->logic_chan_num; 	
	pdu_total_size = rlc_data_ptr->tb_size; 
	AssertFatal(pdu_total_size <= MAX_DLSCH_PAYLOAD_BYTES , RLC, "mac request pdu size exceed the maximum bytes of RLC buffer !\n"); 

	

	//！initial ctxt
    PROTOCOL_CTXT_SET_BY_MODULE_ID(&temp_ctxt, 
                                    protocol_ctxt_ptr->module_id, 
    								protocol_ctxt_ptr->enb_flag, 
    								ue_rnti,
    								frameP, subframeP, 
    								protocol_ctxt_ptr->eNB_index); 


	

	
	insert_tb_size = 0; 
	memset((void *)subheader,0,sizeof(subheader)); 
	memset((void *)lc_pdu_component,0,sizeof(lc_pdu_component)); 
	memset((void *)ue_pdu_size_para_ptr,0,sizeof(mac_pdu_size_para)); 
	memset((void *)ue_mac_subheader_ptr,0,(MAX_LOGICCHAN_NUM + 1) * 3 * sizeof(uint8_t)); 

	ue_pdu_size_para_ptr->total_pdu_size = pdu_total_size; 

    for (logic_index = 0; logic_index < logic_num; logic_index++)
    {
		lc_pdu_component[logic_index].is_last_sub_header_flag = (logic_index == (logic_num-1))?1:0; 	
		lc_pdu_component[logic_index].mac_reqeust_tb_size =  rlc_data_ptr->mac_pdu_byte_size[logic_index];
		lc_pdu_component[logic_index].final_mac_sdu_size = lc_pdu_component[logic_index].mac_reqeust_tb_size; //!initial mac sdu size 
		lc_pdu_component[logic_index].remain_mac_pdu_size = lc_pdu_component[logic_index].mac_reqeust_tb_size; //!initial 
		lc_pdu_component[logic_index].logic_ch_index = logic_index; 
		lc_pdu_component[logic_index].valid_flag = 1; 
    }

	list_init (&g_rlc_mac_data_req.data, NULL);  
	for (logic_index = 0; logic_index < logic_num; logic_index++)
	{
		logic_ch_id = rlc_data_ptr->logicchannel_id[logic_index];
		tb_size_for_lc = rlc_data_ptr->mac_pdu_byte_size[logic_index]; 

        //!ue remian space 
        ue_pdu_size_para_ptr->remain_pdu_size = ue_pdu_size_para_ptr->total_pdu_size - ue_pdu_size_para_ptr->total_mac_subheader_size - \
        										ue_pdu_size_para_ptr->total_mac_ce_size - ue_pdu_size_para_ptr->total_mac_sdu_size; 
        										
        										
        //!calculate rlc data length ,mac header length, mac subheader type, mac_padding_header ,pading size for logic channel 
		rlc_mac_data_send(temp_ctxt,
						 logic_ch_id,
						 tb_size_for_lc,
						 &lc_pdu_component[logic_index],
						 ue_pdu_size_para_ptr,
						 &g_rlc_mac_data_req);

        //!update the occupied size 
		ue_pdu_size_para_ptr->total_mac_subheader_size += lc_pdu_component[logic_index].mac_subheader_length; 
		ue_pdu_size_para_ptr->total_mac_sdu_size += lc_pdu_component[logic_index].final_mac_sdu_size; 

	
	
	}

     /**!
     When single-byte or two-byte padding is required, one or two MAC PDU subheaders corresponding to padding are placed at the beginning of 
     the MAC PDU before any other MAC PDU subheader.
    */
    if((ue_pdu_size_para_ptr->remain_pdu_size > 0) && (ue_pdu_size_para_ptr->remain_pdu_size <= 2))
    {
    	
		mac_subheader_without_l = (1 << 5 | 0x1f); //!logic id = b'11111, padding ID 	

		for (pading_header_i = 0; pading_header_i <ue_pdu_size_para_ptr->remain_pdu_size;pading_header_i++)
		{
        	memcpy((void *)ue_mac_subheader_ptr,(void *)mac_subheader_without_l,1); 
			ue_mac_subheader_ptr += 1; 
			mac_subheader_length ++; 
        }

        ue_pdu_size_para_ptr->remain_pdu_size = 0; //!no padding in the end of PDU when insert padding subheader 

    }

	for (logic_index = 0; logic_index < logic_num; logic_index++)
	{
		e = !(lc_pdu_component[logic_index].is_last_sub_header_flag); 
		f = (lc_pdu_component[logic_index].final_mac_sdu_size >=128)?1:0; 
		switch(lc_pdu_component[logic_index].mac_subheader_length_type)
		{
			case 1: 
			{
				mac_subheader_without_l = e << 5 | (logic_ch_id); 
				memcpy((void *)ue_mac_subheader_ptr,(void *)mac_subheader_without_l,1); 
				ue_mac_subheader_ptr += 1; 
				mac_subheader_length += 1; 
				break; 
			}
			case 2: 
			{
				mac_subheader_2byte[0] = (e << 5) | (logic_ch_id); 
				mac_subheader_2byte[1] = (f << 7) | (lc_pdu_component[logic_index].final_mac_sdu_size);
				memcpy((void *)ue_mac_subheader_ptr,(void *)mac_subheader_2byte,2); 
				ue_mac_subheader_ptr += 2; 
				mac_subheader_length += 2; 
				break; 
			}
			case 3: 
			{
				mac_subheader_3byte[0] = (e << 5) | (logic_ch_id); 
				mac_subheader_3byte[1] = (f << 7) | (lc_pdu_component[logic_index].final_mac_sdu_size >> 8); 
				mac_subheader_3byte[2] =  (lc_pdu_component[logic_index].final_mac_sdu_size & 0xff); 

				memcpy((void *)ue_mac_subheader_ptr,(void *)mac_subheader_3byte,3); 
				ue_mac_subheader_ptr += 3; 
				mac_subheader_length += 3; 
				break;
			}
			default:
			{
				AssertFatal(0, RLC, 'mac sub header type error!\n'); 
			}
		}
    }
     /**!
		Padding occurs at the end of the MAC PDU, except when single-byte or two-byte padding is required. 
     */
    if((ue_pdu_size_para_ptr->remain_pdu_size > 0) && (ue_pdu_size_para_ptr->remain_pdu_size > 2))
    {
    	
		mac_subheader_without_l = (0 << 5 | 0x1f); //!logic id = b'11111, padding ID 	
        memcpy((void *)ue_mac_subheader_ptr,(void *)mac_subheader_without_l,1); 
		ue_mac_subheader_ptr += 1; 
		mac_subheader_length += 1; 
    }

    //!copy mac subheader to ue_tb_buffer 
    memcpy((void *)mac_pdu_buffer_ptr, subheader_ptr, mac_subheader_length); 
    mac_pdu_buffer_ptr += mac_subheader_length; 

    //!copy rlc sdu to ue_tb_bubfer ,after the mac subheader 
	mac_sdu_tb_size = mac_rlc_serialize_tb(mac_pdu_buffer_ptr, g_rlc_mac_data_req.data);
	mac_pdu_buffer_ptr += mac_sdu_tb_size; 

    //!add padding 
    if (ue_pdu_size_para_ptr->remain_pdu_size > 2)
    {
		memset(mac_pdu_buffer_ptr,0,ue_pdu_size_para_ptr->remain_pdu_size);
    }
	

   
}






void 	rlc_mac_data_send(const protocol_ctxt_t          ctxt,
									const logical_chan_id_t channel_idP,
									const tb_size_t         tb_sizeP,
									logic_channel_pdu_component   *lc_component_ptr,
									mac_pdu_size_para  *ue_mac_pdu_size_ptr,
									struct mac_data_req *data_request)
{
  

  hashtable_rc_t         h_rc;
  rlc_mode_e            rlc_mode        = RLC_MODE_NONE;

  rlc_union_t           *rlc_union_p     = NULL;
  hash_key_t             key             = HASHTABLE_NOT_A_KEY_VALUE;

  srb_flag_t             srb_flag        = (channel_idP <= 2) ? SRB_FLAG_YES : SRB_FLAG_NO;
  tbs_size_t             ret_tb_size         = 0;

  module_id_t    module_idP = ctxt.module_id; 
  rnti_t         rntiP = ctxt.rnti; 
  eNB_index_t    eNB_index = ctxt.eNB_index; 
  eNB_flag_t     enb_flagP = ctxt.enb_flag; 

  frame_t           frameP = ctxt.frame; 
  sub_frame_t       subframeP = ctxt.subframe; 
  
 

  LOG_DEBUG(RLC, PROTOCOL_CTXT_FMT"%s: logic channel:%d,tb_size=%d\n ",
	        PROTOCOL_CTXT_ARGS((&ctxt)),
	        __func__,
	        channel_idP,
	    	tb_sizeP);

 

  key = RLC_COLL_KEY_LCID_VALUE(module_idP, rntiP, enb_flagP, channel_idP, srb_flag);  //!生成key 

  h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p); //!从hash table中，根据key，获得rlc_union_p

  if (h_rc == HASH_TABLE_OK) {
  //	pthread_mutex_lock(&rlc_union_p->rlc_union_mtex); 
    rlc_mode = rlc_union_p->mode;
  } else {
    rlc_mode = RLC_MODE_NONE;
    AssertFatal (0 , RLC,"RLC not configured lcid %u RNTI %x!\n", channel_idP, rntiP);
  }

  switch (rlc_mode) {
  	case RLC_MODE_NONE:
	{
	    ret_tb_size =0;
	    break;
	}    
#ifdef AM_ENABLE 
	  case RLC_MODE_AM:
	  {
	    rlc_am_mui.rrc_mui_num = 0;
	    if (!enb_flagP) rlc_am_set_nb_bytes_requested_by_mac(&rlc_union_p->rlc.am,tb_sizeP);
		data_request = rlc_am_mac_data_request(&ctxt, &rlc_union_p->rlc.am,enb_flagP);
	    break;
	 }	
#endif 

	  case RLC_MODE_UM:
	  {
	    //!将SDU 分段放进PDU中
	    rlc_um_mac_data_request(&ctxt,
	                                           tb_sizeP,
	    									   &rlc_union_p->rlc.um,
	    									   enb_flagP,
	    									   lc_component_ptr,
	    									   ue_mac_pdu_size_ptr,
	    									   data_request);
	
	    break;
      }

	  case RLC_MODE_TM:
	  {
			//！返回一个mac_data_req 
		rlc_tm_mac_data_request(&ctxt, &rlc_union_p->rlc.tm,tb_sizeP,lc_component_ptr,data_request);
		break;
      }
	  default:break;
  }

//  pthread_mutex_unlock(&rlc_union_p->rlc_union_mtex); 

}







void mac_rlc_data_ind	  (
							const module_id_t 		module_idP,
							const rnti_t				rntiP,
							const eNB_index_t 		eNB_index,
							const frame_t 			frameP,
							const eNB_flag_t			enb_flagP,
							const logical_chan_id_t	channel_idP,
							char					   *buffer_pP,
							const tb_size_t			tb_sizeP,
							uint32_t					num_tbP)
{
	//-----------------------------------------------------------------------------
	rlc_mode_e			 rlc_mode	     = RLC_MODE_NONE;
	rlc_union_t			*rlc_union_p	 = NULL;
	hash_key_t			 key			 = HASHTABLE_NOT_A_KEY_VALUE;

	srb_flag_t			 srb_flag		 = (channel_idP <= 2) ? SRB_FLAG_YES : SRB_FLAG_NO;//!SRB0/1/2's ID = 0/1/2; 
	protocol_ctxt_t	  ctxt;
	hashtable_rc_t		 h_rc;


	PROTOCOL_CTXT_SET_BY_MODULE_ID(&ctxt, module_idP, enb_flagP, rntiP, frameP, 0, eNB_index);


	key = RLC_COLL_KEY_LCID_VALUE(module_idP, rntiP, enb_flagP, channel_idP, srb_flag);
	

	h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);

	if (h_rc == HASH_TABLE_OK) {
		rlc_mode = rlc_union_p->mode;
		//pthread_mutex_lock(&rlc_union_p->rlc_union_mtex);
	} else {
	rlc_mode = RLC_MODE_NONE;
	
	}

	//!按照逻辑信道，对逻辑信道的数据搬移到申请出来的buffer中，进行后续处理，在后续处理中，要及时的free掉buffer，防止内存泄露
	struct mac_data_ind data_ind = mac_rlc_deserialize_tb(buffer_pP, tb_sizeP, num_tbP);

	switch (rlc_mode) {
		case RLC_MODE_NONE:
		{
			list_free (&data_ind.data);
		    break;
		}
		//！将数据加载完之后，再根据不同的模式进行处理
#if AM_ENABLE 	
		case RLC_MODE_AM:
		rlc_am_mac_data_indication(&ctxt, &rlc_union_p->rlc.am, data_ind);
		break;
#endif 

		case RLC_MODE_UM:
	    {
			rlc_um_mac_data_indication(&ctxt, &rlc_union_p->rlc.um, data_ind);
			//pthread_mutex_unlock(&rlc_union_p->rlc_union_mtex);
		    break;
		}
		
		case RLC_MODE_TM:
		{
			rlc_tm_mac_data_indication(&ctxt, &rlc_union_p->rlc.tm, data_ind);
			//pthread_mutex_unlock(&rlc_union_p->rlc_union_mtex);
		    break;
		}

    }
	

}


