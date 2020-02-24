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
#include <time.h>
#include <test_time.h>
#include <osp_ex.h>



long   g_rlc_tx_max_process_time = 0;
long   g_rlc_tx_max_process_time_sn = 0; 

#ifdef RLC_UT_DEBUG 

void mac_Rlc_data_Req(uint16_t frame, uint16_t subsfn,uint16_t valid_ue_num, rlc_data_req *rlc_data_request_ptr)
{
	MessageDef  *message;  

	uint16_t    ue_index; 

	mac_rlc_data_req *mac_rlc_data_req_ptr = OSP_Alloc_Mem(sizeof(mac_rlc_data_req)); 

 	
 	mac_rlc_data_req_ptr->sfn = frame; 
 	mac_rlc_data_req_ptr->sub_sfn = subsfn; 

 	mac_rlc_data_req_ptr->ue_num = valid_ue_num; 
 	

    memcpy((void *)mac_rlc_data_req_ptr->rlc_data_request,(void *)rlc_data_request_ptr,valid_ue_num * sizeof(rlc_data_req))	; 
 	
    message = itti_alloc_new_message(TASK_D2D_MAC_SCH, MAC_RLC_DATA_REQ,
	                       ( char *)mac_rlc_data_req_ptr, sizeof(mac_rlc_data_req));

	itti_send_msg_to_task(TASK_D2D_RLC_TX,  0, message);
	
}





void mac_Rlc_data_Rpt(uint16_t frame, uint16_t subsfn,uint16_t valid_ue_num, mac_rlc_data_info *rlc_data_rpt)
{
	MessageDef  *message;  

	uint16_t    ue_index; 

	mac_rlc_data_rpt *mac_rlc_data_rpt_ptr = OSP_Alloc_Mem(sizeof(mac_rlc_data_rpt)); 

 	
 	mac_rlc_data_rpt_ptr->sfn = frame; 
 	mac_rlc_data_rpt_ptr->sub_sfn = subsfn; 

 	mac_rlc_data_rpt_ptr->ue_num = valid_ue_num; 
 	

    memcpy((void *)mac_rlc_data_rpt_ptr->sdu_data_rpt,(void *)rlc_data_rpt,valid_ue_num * sizeof(mac_rlc_data_info))	; 
 	
    message = itti_alloc_new_message(TASK_D2D_MAC_SCH, MAC_RLC_DATA_RPT,
	                       ( char *)mac_rlc_data_rpt_ptr, sizeof(mac_rlc_data_rpt));

	itti_send_msg_to_task(TASK_D2D_RLC_RX,  0, message);
	
	
	
}


#endif 


void  rlc_mac_data_ind_message(uint32_t         *ue_pdu_buffer_ptr, 
										uint32_t *ue_tb_size_ptr,
										uint32_t *rnti_array, 
										uint32_t *ue_pdu_buffer_offset,
										uint32_t buffer_id,
										uint32_t  ue_num,
										int32_t  *ue_status)
{
   
	   rlc_mac_data_ind  *rlc_mac_data_send_ptr ; 
       uint32_t  ue_index  = 0; 
       

	   MessageDef	*message; 
	   rlc_mac_data_send_ptr =( rlc_mac_data_ind  *) OSP_Alloc_Mem(sizeof(rlc_mac_data_ind)); 


	   rlc_mac_data_send_ptr->sfn = 0; 
	   rlc_mac_data_send_ptr->sub_sfn = 0; 

	   rlc_mac_data_send_ptr->ue_num = ue_num; 

	   for (ue_index = 0; ue_index < ue_num ; ue_index++)
	   {
	   		
			rlc_mac_data_send_ptr->sdu_pdu_info[ue_index].buffer_id = buffer_id; 
			rlc_mac_data_send_ptr->sdu_pdu_info[ue_index].offset = ue_pdu_buffer_offset[ue_index]; 
			rlc_mac_data_send_ptr->sdu_pdu_info[ue_index].data_buffer_adder_ptr = (uint32_t *)ue_pdu_buffer_ptr[ue_index];
			rlc_mac_data_send_ptr->sdu_pdu_info[ue_index].tb_byte_size = ue_tb_size_ptr[ue_index]; 
			rlc_mac_data_send_ptr->sdu_pdu_info[ue_index].rnti = rnti_array[ue_index]; 
            rlc_mac_data_send_ptr->sdu_pdu_info[ue_index].valid_flag = ue_status[ue_index]; 

           
			
	   }
	   

	
	   message = itti_alloc_new_message(TASK_D2D_RLC_TX, RLC_MAC_DATA_IND,
							  ( char *)rlc_mac_data_send_ptr, sizeof(rlc_mac_data_ind ));
	
	   itti_send_msg_to_task(TASK_D2D_MAC_SCH,0, message);



}


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
    struct rlc_um_tx_sdu_management    *um_tx_sdu_header_ptr = NULL; 




	//DevAssert(sdu_pP != NULL);
	if(sdu_pP == NULL) {
	  LOG_ERROR(RLC_TX, "sdu_pP == NULL\n");
	  return RLC_OP_STATUS_BAD_PARAMETER;
	}
	
	if(sdu_sizeP <= 0) {
	  LOG_ERROR(RLC_TX, "sdu_sizeP %d, file %s, line %d\n", sdu_sizeP, __FILE__ ,__LINE__);
	  return RLC_OP_STATUS_BAD_PARAMETER;
	}

	key = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);
	h_rc = hashtable_get(rlc_coll_p, key, (void **)&rlc_union_p);
    
	if (h_rc == HASH_TABLE_OK) {		
		LOG_INFO(RLC_TX, "func:%s,line:%d: key = %lld: get rlc entity sucess!\n",__func__,__LINE__,key);
		rlc_mode = rlc_union_p->mode;
	} else {
		//AssertFatal (0 , "RLC not configured key %ju\n", key);
		LOG_ERROR(RLC_TX, "not configured key %lld,module_id:%d,  srb_flag:%d,rnti:%d,rb_idP = %d, enb_flag:%d\n", 
					key,ctxt_pP->module_id, srb_flagP, ctxt_pP->rnti, rb_idP, ctxt_pP->enb_flag);
		return RLC_OP_STATUS_OUT_OF_RESSOURCES;
	}



  
	switch (rlc_mode) {
#ifdef AM_ENABLE 
	case RLC_MODE_AM:
	  //!申请新的内存
	  new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_am_data_req_alloc), __func__,__LINE__);

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
	  new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_um_tx_sdu_management), __func__,__LINE__);
      


      
	  if (new_sdu_p != NULL) {
	



		new_sdu_p->pool_id = 1;
		new_sdu_p->size = 100;
     
		

		//！copy 数据到新申请buffer中
		memcpy (((uint8_t *)(new_sdu_p->data) + sizeof(struct  rlc_um_tx_sdu_management)), sdu_pP, sdu_sizeP);

		um_tx_sdu_header_ptr = ((struct rlc_um_tx_sdu_management *)(new_sdu_p->data)); 
		um_tx_sdu_header_ptr->data_size = sdu_sizeP; 

		um_tx_sdu_header_ptr->data_offset = sizeof(struct rlc_um_tx_sdu_management);

        
		

		//free_mem_block(sdu_pP, __func__);
		//！将sdu的地址更新到input_sdu节点中去
	    rlc_um_data_req(ctxt_pP, &rlc_union_p->rlc.um, new_sdu_p);



		//free_mem_block(new_sdu, __func__);
	 //   pthread_mutex_unlock(&(rlc_union_p->rlc_union_mtex));
		return RLC_OP_STATUS_OK;
	  } else {
		//free_mem_block(sdu_pP, __func__);
		return RLC_OP_STATUS_INTERNAL_ERROR;
	  }

	  break;

	case RLC_MODE_TM:
     
	  //！申请Mem时，增加了size: sizeof (struct rlc_tm_data_req_alloc)
	  new_sdu_p = get_free_mem_block (sdu_sizeP + sizeof (struct rlc_tm_data_req_alloc), __func__,__LINE__);

	  if (new_sdu_p != NULL) {
		// PROCESS OF COMPRESSION HERE:
	//	pthread_mutex_lock(&(rlc_union_p->rlc_union_mtex));
		memset ((void *)new_sdu_p->data, 0, sizeof (struct rlc_tm_data_req_alloc));
		memcpy ((uint8_t *)(new_sdu_p->data) + sizeof (struct rlc_tm_data_req_alloc), sdu_pP, sdu_sizeP);
		((struct rlc_tm_data_req *) (new_sdu_p->data))->data_size = sdu_sizeP;
		((struct rlc_tm_data_req *) (new_sdu_p->data))->data_offset = sizeof (struct rlc_tm_data_req_alloc);

		//free_mem_block(sdu_pP, __func__); //!sdu_pP是从上层给过来的SDU,这里将其从memblock 中删除掉
		 //！这里只是将申请出来的mem 地址添加到了input_sdu中，但是数据并没有copy进去，哪里看到数据的搬移呢？
		rlc_tm_data_req(ctxt_pP, &rlc_union_p->rlc.tm, new_sdu_p);
		
	//	pthread_mutex_unlock(&(rlc_union_p->rlc_union_mtex));
		
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
			LOG_ERROR(RLC_TX, "RLC tm entity don't have enough space for the rrc,[%d, %d, %d]\n", 
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
	rlc_buffer_rpt    rlc_buf_sta[D2D_MAX_USER_NUM+1]; 


	int        ret_value = 0; 
	uint32_t   empty_size = 0; 
    uint16_t     sfn; 
	uint16_t     subsfn; 
	uint32_t     ue_num; 
	uint32_t     ue_index; 
	uint32_t     srb_flag = 0; 

	tb_size_t    tb_size; 
	uint32_t   ue_pdu_buffer_array[D2D_MAX_USER_NUM]; 
	uint32_t   ue_pdu_size_array[D2D_MAX_USER_NUM]; 
	uint32_t   ue_rnti_array[D2D_MAX_USER_NUM]; 
	uint32_t   ue_buffer_offset[D2D_MAX_USER_NUM]; 
	uint32_t   buffer_id; 
	uint32_t   buffer_offset = 0; 

	struct timeval    start_time; 
	struct timeval    end_time; 
	long   process_time = 0;
	int32_t   ue_status[D2D_MAX_USER_NUM] = {0};

	
	LOG_ERROR(RLC_TX, "RLC_TX receive message = %d\n",msg_type);
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
		case IP_RLC_DATA_IND:
		{
		    gettimeofday(&start_time,NULL); 
			LOG_ERROR(RLC_TX, "message:%d tx data process start,enb_flag = %d",msg_type,g_rlc_protocol_ctxt.enb_flag);
			rrc_rlc_data_ind_ptr  = (rrc_rlc_data_ind *)message; 
			rb_type = rrc_rlc_data_ind_ptr->rb_type; 
			rb_id = rrc_rlc_data_ind_ptr->rb_id; 
			send_data_size = rrc_rlc_data_ind_ptr->data_size; 
			data_buffer = (uint8_t *)rrc_rlc_data_ind_ptr->data_addr_ptr;
			rnti = rrc_rlc_data_ind_ptr->rnti; 
			g_rlc_protocol_ctxt.rnti = rnti; 

			if (RRC_RLC_DATA_IND == msg_type)
			{
				srb_flag = SRB_FLAG_YES; 
			}
			else 
			{
				srb_flag = SRB_FLAG_NO; 
			}


			rlc_get_tx_data(&g_rlc_protocol_ctxt,
							srb_flag,
							rb_id,
							send_data_size,
							data_buffer);
			gettimeofday(&end_time,NULL); 
			process_time = ( (end_time.tv_sec * 1000000 + end_time.tv_usec) - (start_time.tv_sec * 1000000 + start_time.tv_usec)); 
			if (g_rlc_tx_max_process_time <= process_time )
			{
				g_rlc_tx_max_process_time = process_time; 
				g_rlc_tx_max_process_time_sn = rrc_rlc_data_ind_ptr->data_sn; 
			}
			
            LOG_ERROR(RLC_TX, "message:%d tx data process finished,enb_flag = %d,data SN = %d,max_process_record[%lld,%lld], \
cur process time = [%lld, %lld, %lld] \n",
            				msg_type,g_rlc_protocol_ctxt.enb_flag,rrc_rlc_data_ind_ptr->data_sn,
            				g_rlc_tx_max_process_time,g_rlc_tx_max_process_time_sn,
            				process_time,end_time.tv_usec,start_time.tv_usec);    

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
#ifndef  RLC_UT_DEBUG             
            rlc_Mac_BufferSta_Rpt(sfn,subsfn,ue_num,rlc_buf_sta);
            LOG_ERROR(RLC_TX, "message:%d,[sfn--subsfn]:[%d,%d] send %d ue_num rlc buffer status to mac \n",
            		msg_type,sfn,subsfn);
#endif 
            if (ue_num != 0)
            {
				
#ifdef  RLC_UT_DEBUG 
			
            rlc_data_req   mac_data_req_to_rlc;
            mac_data_req_to_rlc.rnti = 101; 
            mac_data_req_to_rlc.logic_chan_num = 1; 
          //  mac_data_req_to_rlc.tb_size = 1280; //   //!total size ,unit:byte

            //!实际测试中，防止积累的SDU 过多，超出TB size 
			if (g_rlc_ut_tx_buffer_size[0][0] > 4203)
			{
			  g_rlc_ut_tx_buffer_size[0][0] = 4203;
			}
			mac_data_req_to_rlc.tb_size = g_rlc_ut_tx_buffer_size[0][0] ;
            mac_data_req_to_rlc.logicchannel_id[0] =3;  //!DRB = 3

            mac_data_req_to_rlc.mac_pdu_byte_size[0] = g_rlc_ut_tx_buffer_size[0][0] ; //!logic channel's size ,unit:byte

            

            //!第一包手动设置大小，而第二包就需要按照实际大小传输，否则UT case  无法将一包数据全部传输完成
			if (g_rlc_ut_2nd_trans > 0)  
			{
				mac_data_req_to_rlc.tb_size  = g_rlc_ut_tx_buffer_size[0][0]; 
				mac_data_req_to_rlc.mac_pdu_byte_size[0] = g_rlc_ut_tx_buffer_size[0][0];
			}
		    //! send MAC_RLC_DATA_REQ message to RLC_TX 
		    if (1 != g_rlc_no_data_transfer)
		    {
		    	
				mac_Rlc_data_Req(sfn, subsfn, 1, &mac_data_req_to_rlc); 
				g_rlc_ut_2nd_trans++;
				
            	LOG_ERROR(RLC_TX, "message:%d,[sfn--subsfn]:[%d,%d] send rlc buffer status to mac,the data req message flag:%d ,\
tb_size = %d, logic tb_size = %d \n",
            		msg_type,sfn,subsfn,!g_rlc_no_data_transfer, mac_data_req_to_rlc.tb_size,mac_data_req_to_rlc.mac_pdu_byte_size[0]);
				
			}
#endif 

            }

			break; 
		}
		case MAC_RLC_DATA_REQ: 
		{
			mac_rlc_data_req_ptr = (mac_rlc_data_req *)message; 
			sfn = mac_rlc_data_req_ptr->sfn; 
			subsfn = mac_rlc_data_req_ptr->sub_sfn; 
            ue_num = mac_rlc_data_req_ptr->ue_num; 
            buffer_offset = 0; 
            
			for (ue_index = 0; ue_index < ue_num; ue_index++)
			{
				rlc_data_req_ptr = (rlc_data_req * )&mac_rlc_data_req_ptr->rlc_data_request[ue_index];

				
				ue_status[ue_index] = rlc_mac_ue_data_process(sfn,
				 						subsfn,
				 						rlc_data_req_ptr,
				 						&g_rlc_protocol_ctxt,
				 						&g_rlc_pdu_size_para[ue_index],
				 						&g_rlc_pdu_buffer[ue_index * MAX_DLSCH_PAYLOAD_BYTES],
				 						&g_rlc_mac_subheader[ue_index *((MAX_LOGICCHAN_NUM  + 1)* 3)]); 
                buffer_id = (subsfn & 0x1); 
#ifdef  FPGA_PLATFORM
				ue_pdu_buffer_array[ue_index] = (uint32_t) OspGetApeTDateAddr(buffer_id);
				ue_buffer_offset[ue_index] =  buffer_offset; 
				ue_pdu_size_array[ue_index] = (uint32_t )rlc_data_req_ptr->tb_size;
				memcpy((void *) ue_pdu_buffer_array[ue_index] , (void *)&g_rlc_pdu_buffer[ue_index * MAX_DLSCH_PAYLOAD_BYTES],ue_pdu_size_array[ue_index] * sizeof(char)); 
#else 
				ue_pdu_buffer_array[ue_index] = (long)&g_rlc_pdu_buffer[ue_index * MAX_DLSCH_PAYLOAD_BYTES]; 
#endif 
				ue_rnti_array[ue_index] =  	rlc_data_req_ptr->rnti; 
				buffer_offset = buffer_offset + ue_pdu_size_array[ue_index]; //update buffer_offset
				
				
			}
			
#ifndef RLC_UT_DEBUG 			
           rlc_mac_data_ind_message(ue_pdu_buffer_array,ue_pdu_size_array,ue_rnti_array,ue_buffer_offset,buffer_id,ue_num,ue_status); 
#else 
		    mac_rlc_data_info    mac_rlc_data_rpt_temp; 

		    mac_rlc_data_rpt_temp.valid_flag = 1; 
		    mac_rlc_data_rpt_temp.rnti = 101; 
		    mac_rlc_data_rpt_temp.logic_chan_num = 1; 
		    mac_rlc_data_rpt_temp.logicchannel_id[0] = 3; 
		    mac_rlc_data_rpt_temp.mac_pdu_size[0] = g_debug_rlc_lc_pdu_component[0].final_rlc_pdu_size; 
		    mac_rlc_data_rpt_temp.mac_pdu_buffer_ptr[0] = (uint32_t *)((uint8_t *)(&g_rlc_pdu_buffer[0 * MAX_DLSCH_PAYLOAD_BYTES]) 
		    											+ g_rlc_debug_ue_mac_header_size);
		    
			mac_Rlc_data_Rpt(sfn,subsfn,1, &mac_rlc_data_rpt_temp);

#endif 		
			break; 
		}

        
		default: break;
	}

} 


void *rlc_tx_task(MessageDef *recv_msg)
{


	rlc_tx_process(MSG_HEAD_TO_COMM(recv_msg), recv_msg->ittiMsgHeader.MsgType);
	itti_free_message(recv_msg);

}
 


 
 
 
/**************************function******************************/
