/******************************************************************
 * @file  rlc_tx_task.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月11日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月11日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc.h>
#include <log.h>
#include <d2d_message_type.h>
#include <interface_rrc_rlc.h>
#include <interface_mac_rlc.h>
#include <intertask_interface.h>



rlc_op_status_t rlc_get_tx_data(const protocol_ctxt_t *const ctxt_pP,
								const srb_flag_t   srb_flagP,
								const rb_id_t	   rb_idP,
								sdu_size_t	 sdu_sizeP,
								uint8_t  *sdu_pP) 
{
	//-----------------------------------------------------------------------------
	mem_block_t 		  *new_sdu_p	= NULL;

	rlc_union_t 		  *rlc_union_p = NULL;
	hash_key_t			   key		   = HASHTABLE_NOT_A_KEY_VALUE;
	hashtable_rc_t		   h_rc;
    rlc_mode_e			   rlc_mode;




	//DevAssert(sdu_pP != NULL);
	if(sdu_pP == NULL) {
	  LOG_ERROR(RLC, "sdu_pP == NULL\n");
	  return RLC_OP_STATUS_BAD_PARAMETER;
	}
	
	if(sdu_sizeP <= 0) {
	  LOG_ERROR(RLC, "sdu_sizeP %d, file %s, line %d\n", sdu_sizeP, __FILE__ ,__LINE__);
	  return RLC_OP_STATUS_BAD_PARAMETER;
	}

	key = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);
	h_rc = hashtable_get(rlc_coll_p, key, (void **)&rlc_union_p);

	if (h_rc == HASH_TABLE_OK) {
		pthread_mutex_lock(&(rlc_union_p->rlc_union_mtex));
		rlc_mode = rlc_union_p->mode;
	} else {
		//AssertFatal (0 , "RLC not configured key %ju\n", key);
		LOG_ERROR(RLC, "not configured key %lu\n", key);
		return RLC_OP_STATUS_OUT_OF_RESSOURCES;
	}

#if defined(TRACE_RLC_PAYLOAD)
	LOG_DEBUG(RLC, PROTOCOL_CTXT_FMT"[RB %u] Display of rlc_data_req:\n",
				PROTOCOL_CTXT_ARGS(ctxt_pP),
				rb_idP);
	rlc_util_print_hex_octets(RLC, (unsigned char *)sdu_pP, sdu_sizeP);
#endif


	switch (rlc_mode) {
#ifdef AM_ENABLE 
	case RLC_MODE_AM:
	  //!申请新的内存
	  new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_am_data_req_alloc), __func__);

	  if (new_sdu_p != NULL) {
		// PROCESS OF COMPRESSION HERE:
		memset (new_sdu_p->data, 0, sizeof (struct rlc_am_data_req_alloc));
		//!数据先放在offset处，offset = sizeof(struct rlc_am_data_req_alloc));
	   //!从PDCH的buffer中，向新申请的内存中copy数据
		memcpy (&new_sdu_p->data[sizeof (struct rlc_am_data_req_alloc)], sdu_pP, sdu_sizeP);
		//!再填写数据头的内容，这里的数据头是rlc_am_data_req，
		((struct rlc_am_data_req *) (new_sdu_p->data))->data_size = sdu_sizeP;
		((struct rlc_am_data_req *) (new_sdu_p->data))->conf = confirmP;
		((struct rlc_am_data_req *) (new_sdu_p->data))->mui  = muiP;
		((struct rlc_am_data_req *) (new_sdu_p->data))->data_offset = sizeof (struct rlc_am_data_req_alloc);
		//free_mem_block(sdu_pP, __func__); //!将SDU buffer 清0 
		//！AM SDU 处理，将new_sdu_p的地址写到rlc_p的链表中的节点中
		rlc_am_data_req(ctxt_pP, &rlc_union_p->rlc.am, new_sdu_p);
		
		
		return RLC_OP_STATUS_OK;
	  } else {
		
		//free_mem_block(sdu_pP, __func__);
		return RLC_OP_STATUS_INTERNAL_ERROR;
	  }

	  break;
#endif 
	case RLC_MODE_UM:

	  /* TODO: this is a hack, needs better solution. Let's not use too
	   * much memory and store at maximum 5 millions bytes.
	   */
	  /* look for HACK_RLC_UM_LIMIT for others places related to the hack. Please do not remove this comment. */
#if 0
		if (rlc_um_get_buffer_occupancy(&rlc_union_p->rlc.um) > 5000000) {
		//free_mem_block(sdu_pP, __func__);
		return RLC_OP_STATUS_OUT_OF_RESSOURCES;
	  }
#endif

	  //！这里申请出来的data 的首地址指向的是包含了rlc_um_data_req_alloc这个结构体的
	  new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_um_data_req_alloc), __func__);

	  if (new_sdu_p != NULL) {
		// PROCESS OF COMPRESSION HERE:
		memset (new_sdu_p->data, 0, sizeof (struct rlc_um_data_req_alloc));
		//！copy 数据到新申请buffer中
		memcpy (&new_sdu_p->data[sizeof (struct rlc_um_data_req_alloc)], sdu_pP, sdu_sizeP);
		//！这里的头部是rlc_um_data_req_alloc ,是一个union 
		((struct rlc_um_data_req *) (new_sdu_p->data))->data_size = sdu_sizeP;
		((struct rlc_um_data_req *) (new_sdu_p->data))->data_offset = sizeof (struct rlc_um_data_req_alloc);

		//free_mem_block(sdu_pP, __func__);
		//！将sdu的地址更新到input_sdu节点中去
	    rlc_um_data_req(ctxt_pP, &rlc_union_p->rlc.um, new_sdu_p);

		//free_mem_block(new_sdu, __func__);
	
		return RLC_OP_STATUS_OK;
	  } else {
		//free_mem_block(sdu_pP, __func__);
		return RLC_OP_STATUS_INTERNAL_ERROR;
	  }

	  break;

	case RLC_MODE_TM:

	  //！申请Mem时，增加了size: sizeof (struct rlc_tm_data_req_alloc)
	  new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_tm_data_req_alloc), __func__);

	  if (new_sdu_p != NULL) {
		// PROCESS OF COMPRESSION HERE:
		memset (new_sdu_p->data, 0, sizeof (struct rlc_tm_data_req_alloc));
		memcpy (&new_sdu_p->data[sizeof (struct rlc_tm_data_req_alloc)], sdu_pP, sdu_sizeP);
		((struct rlc_tm_data_req *) (new_sdu_p->data))->data_size = sdu_sizeP;
		((struct rlc_tm_data_req *) (new_sdu_p->data))->data_offset = sizeof (struct rlc_tm_data_req_alloc);

		//free_mem_block(sdu_pP, __func__); //!sdu_pP是从上层给过来的SDU,这里将其从memblock 中删除掉
		 //！这里只是将申请出来的mem 地址添加到了input_sdu中，但是数据并没有copy进去，哪里看到数据的搬移呢？
		rlc_tm_data_req(ctxt_pP, &rlc_union_p->rlc.tm, new_sdu_p);
		
		
		
		return RLC_OP_STATUS_OK;
	  } else {

		//free_mem_block(sdu_pP, __func__);
		return RLC_OP_STATUS_INTERNAL_ERROR;
	  }

	  break;

	default:
	 // free_mem_block(sdu_pP, __func__);
	
	  return RLC_OP_STATUS_INTERNAL_ERROR;
  }
	pthread_mutex_unlock(&(rlc_union_p->rlc_union_mtex));	
}





int  rlc_rrc_tx_status_ind(const protocol_ctxt_t* const ctxt_pP,
								   boolean_t  srb_flagP,
								   rb_id_t rb_idP,
								   uint32_t send_data_size,
								   uint32_t *empty_size)

{
	hashtable_rc_t		   h_lcid_rc;
	hashtable_rc_t		   h_rc;
	uint32_t               sdu_buffer_size; 
	uint32_t               buffer_occupancy;    

	rlc_union_t 		   *rlc_union_p = NULL;
	hash_key_t			   key		   = HASHTABLE_NOT_A_KEY_VALUE;
	hash_key_t			   key_lcid    = HASHTABLE_NOT_A_KEY_VALUE; 
	
		

	key = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);

	h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);

	if (HASH_TABLE_OK == h_rc)
	{
		AssertFatal(rlc_union_p->mode == RLC_MODE_TM,RLC,"%s occur error,rlc union's rlc mode is wrong\n",
					__func__);
	
		//!return the buffer status and valid size to rrc
		buffer_occupancy = rlc_union_p->rlc.tm.buffer_occupancy;
		sdu_buffer_size = rlc_union_p->rlc.tm.sdu_allocated_buffer_size; 
		
		if (send_data_size + buffer_occupancy > sdu_buffer_size )
		{
			LOG_ERROR(RLC, "RLC tm entity don't have enough space for the rrc,[%d, %d, %d]\n", 
			sdu_buffer_size,buffer_occupancy,send_data_size); 
			return 0; 
		}
		else 
		{
			*empty_size = sdu_buffer_size - buffer_occupancy; 
			return 1; 
		}
	}
	else 
	{
		AssertFatal(1 == 0, RLC , "%s occur error,there is no rlc_union founded\n",__func__); 
	}

}


void rlc_tx_process(void *message, MessagesIds      msg_type)
{
	rrc_rlc_buffer_status_req    *rlc_buffer_req_ptr  = NULL; 
	rrc_rlc_data_ind             *rrc_rlc_data_ind_ptr = NULL;  
	mac_rlc_buf_status_req       *mac_data_buffer_req_ptr = NULL; 
	mac_rlc_data_req             *mac_rlc_data_req_ptr = NULL;  
	rlc_data_req                 *rlc_data_req_ptr= NULL; 
	rb_id_t   rb_id; 
	rb_type_e  rb_type; 

	rlc_union_t 	*rlc_union_p; 
	uint32_t    send_data_size ; 
	uint8_t     *data_buffer; 

	rnti_t     rnti; 
	rlc_buffer_rpt    rlc_buf_sta[D2D_MAX_USER_NUM]; 
	uint16_t     sfn; 
	uint16_t     subsfn; 
	uint32_t     ue_num; 
	uint32_t     ue_index; 

	int        ret_value = 0; 
	uint32_t   empty_size = 0; 
	
	switch (msg_type)
	{
		case RRC_RLC_BUF_STATUS_REQ: 
		{
			rlc_buffer_req_ptr = (rrc_rlc_buffer_status_req *)message; 
			rb_id = rlc_buffer_req_ptr->rb_id; 
			rb_type = rlc_buffer_req_ptr->rb_type; 
			rnti = rlc_buffer_req_ptr->rnti; 

			g_rlc_protocol_ctxt.rnti = rnti;  
			//!find the tx buffer accordding to rb_type and rb_id
			//!ret_value = 0: not enough space ,1: ok 
			ret_value = rlc_rrc_tx_status_ind(&g_rlc_protocol_ctxt,
											  SRB_FLAG_YES,
											  rb_id,
											  rlc_buffer_req_ptr->send_data_size,
											  &empty_size); 
			
			
			//！response
			rlc_Rrc_BufStatus_Rpt(rlc_buffer_req_ptr->send_data_size, rb_id, rb_type,rnti, empty_size,  ret_value);

			break; 
		}
		case RRC_RLC_DATA_IND: 
		{
			//!receive RRC data
			rrc_rlc_data_ind_ptr  = (rrc_rlc_data_ind *)message; 
			rb_type = rrc_rlc_data_ind_ptr->rb_type; 
			rb_id = rrc_rlc_data_ind_ptr->rb_id; 
			send_data_size = rrc_rlc_data_ind_ptr->data_size; 
			data_buffer = rrc_rlc_data_ind_ptr->data_addr_ptr;
			rnti = rrc_rlc_data_ind_ptr->rnti; 
			g_rlc_protocol_ctxt.rnti = rnti; 
	
			rlc_get_tx_data(&g_rlc_protocol_ctxt,
							SRB_FLAG_YES,
							rb_id,
							send_data_size,
							data_buffer);

			break; 
		}
		//!TODO  IP DATA transfer message
		case MAC_RLC_BUF_STATUS_REQ:
		{
			mac_data_buffer_req_ptr = (mac_rlc_buf_status_req *)message; 

			sfn = mac_data_buffer_req_ptr->sfn; 
			subsfn = mac_data_buffer_req_ptr->sub_sfn; 
			
			//!srb0/srb1/drb's buffer status report to mac 
           	ue_num =  rlc_Get_Buffer_Status(rlc_buf_sta); 
            
            rlc_Mac_BufferSta_Rpt(sfn,subsfn,ue_num,rlc_buf_sta);

			break; 
		}
		case MAC_RLC_DATA_REQ: 
		{
			mac_rlc_data_req_ptr = (mac_rlc_data_req *)message; 
			sfn = mac_data_buffer_req_ptr->sfn; 
			subsfn = mac_data_buffer_req_ptr->sub_sfn; 
            ue_num = mac_rlc_data_req_ptr->ue_num; 
            
			for (ue_index = 0; ue_index < ue_num; ue_index++)
			{
				rlc_data_req_ptr = (rlc_data_req * )&mac_rlc_data_req_ptr->rlc_data_request[ue_index];
				
				rlc_mac_ue_data_process(sfn,
				 						subsfn,
				 						rlc_data_req_ptr,
				 						&g_rlc_protocol_ctxt,
				 						&g_rlc_pdu_buffer[ue_index * MAX_DLSCH_PAYLOAD_BYTES]); 
			}
			

			break; 
		}

		default: break;
	}

} 


void *rlc_tx_task( )
{

	MessageDef *recv_msg;

	while(1)
	{
		//!TODO change RLC TASK ID 
        if (0 == itti_receive_msg(TASK_D2D_RLC_TX, &recv_msg))
        {
         	
			rlc_tx_process(recv_msg->message_ptr, recv_msg->ittiMsgHeader.messageId);
			itti_free_message(recv_msg);
        }


	}
}
 


 
 
 
/**************************function******************************/
