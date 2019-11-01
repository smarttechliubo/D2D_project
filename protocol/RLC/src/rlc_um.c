/******************************************************************
 * @file  rlc_um.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <stdio.h>
#include <rlc.h>
#include <pthread.h>


void rlc_um_cleanup ( rlc_um_entity_t * const rlc_pP)
{
  int             index;
  // TX SIDE
  list_free (&rlc_pP->pdus_to_mac_layer);
  pthread_mutex_destroy(&rlc_pP->lock_input_sdus);
  list_free (&rlc_pP->input_sdus);

  // RX SIDE
  list_free (&rlc_pP->pdus_from_mac_layer);

  if ((rlc_pP->output_sdu_in_construction)) {
    free_mem_block (rlc_pP->output_sdu_in_construction, __func__);
    rlc_pP->output_sdu_in_construction = NULL;
  }

  if (rlc_pP->dar_buffer) {
    for (index = 0; index < 1024; index++) {
      if (rlc_pP->dar_buffer[index]) {
        free_mem_block (rlc_pP->dar_buffer[index], __func__);
      }
    }

    free (rlc_pP->dar_buffer);
    rlc_pP->dar_buffer = NULL;
  }

  pthread_mutex_destroy(&rlc_pP->lock_dar_buffer);
  memset(rlc_pP, 0, sizeof(rlc_um_entity_t));
}



void  rlc_um_init (const protocol_ctxt_t* const ctxt_pP,
  				rlc_um_entity_t * const rlc_pP,
  				 const srb_flag_t  srb_flagP,
  				const rb_id_t rb_idP)
{

  //AssertFatal(rlc_pP, "Bad RLC UM pointer (NULL)");
  if(rlc_pP == NULL) {
    LOG_ERROR(RLC, "Bad RLC UM pointer (NULL)\n");
    return;
  }



  if (rlc_pP->initialized) {
    LOG_DEBUG(RLC,PROTOCOL_RLC_UM_CTXT_FMT" [INIT] ALREADY DONE, DOING NOTHING\n",
          PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP));
  } else {
    memset (rlc_pP, 0, sizeof (rlc_um_entity_t));  //实体全部清0 
    // TX SIDE
    list_init (&rlc_pP->pdus_to_mac_layer, NULL); //!<pdus_to_mac_layer 头尾指针都清0 
    pthread_mutex_init(&rlc_pP->lock_input_sdus, NULL); //!< 互斥锁初始化后，处于未锁住态 
    list_init (&rlc_pP->input_sdus, NULL); //!<input_sdus 上层传输进来的Buffer

    rlc_pP->protocol_state = RLC_NULL_STATE;

    //rlc_pP->vt_us = 0;

	if (srb_flagP)
	{
		rlc_pP->is_data_plane = 0;
	}
	else
	{
		rlc_pP->is_data_plane = 1;
	}

	rlc_pP->rb_id = rb_idP;

    // RX SIDE
    list_init (&rlc_pP->pdus_from_mac_layer, NULL); //！pdus_from_mac_layer链表清0
    //rlc_pP->vr_ur = 0;
    //rlc_pP->vr_ux = 0;
    //rlc_pP->vr_uh = 0;
    //rlc_pP->output_sdu_size_to_write = 0;
    //rlc_pP->output_sdu_in_construction = NULL;

    rlc_pP->rx_sn_length          = 10; //！上层发送下来的SDU 组包PDU时的SN号的长度，初始化为10，可以为5
    rlc_pP->rx_header_min_length_in_bytes = 2; //！SN为10的话，则PDU header 长度为2个byte 
    rlc_pP->tx_sn_length          = 10;    //!< tx 和rx 的HEADER一致 
    rlc_pP->tx_header_min_length_in_bytes = 2;

    pthread_mutex_init(&rlc_pP->lock_dar_buffer, NULL); //！锁经过初始化后，为未锁定状态

    if (rlc_pP->dar_buffer == NULL) {
    	//dar_buffer 是指向结构体指针的指针，这里申请1024个int来存放指针
      rlc_pP->dar_buffer = calloc (1, 1024 * sizeof (void *)); 
    }

    rlc_pP->first_pdu = 1;
    rlc_pP->initialized = TRUE;

    LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" [INIT DONE!] STATE VARIABLES, BUFFERS, LISTS\n",
            PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP));
  }
}



int    rlc_um_fsm_notify_event (const protocol_ctxt_t* const ctxt_pP,
									    rlc_um_entity_t *rlc_pP, uint8_t eventP)
{

  switch (rlc_pP->protocol_state) {
	//-------------------------------
	// RLC_NULL_STATE
	//-------------------------------
  case RLC_NULL_STATE:
	switch (eventP) {
	case RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT:
	  LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM RLC_NULL_STATE -> RLC_DATA_TRANSFER_READY_STATE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  rlc_pP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
	  return 1;
	  break;

	default:
	  LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM WARNING PROTOCOL ERROR - EVENT %02X hex NOT EXPECTED FROM NULL_STATE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			eventP);
	  //AssertFatal(1==0,"RLC-UM FSM WARNING PROTOCOL ERROR - EVENT NOT EXPECTED FROM NULL_STATE");
	  return 0;
	}

	break;

	//-------------------------------
	// RLC_DATA_TRANSFER_READY_STATE
	//-------------------------------
  case RLC_DATA_TRANSFER_READY_STATE:
	switch (eventP) {
	case RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
	  LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM RLC_DATA_TRANSFER_READY_STATE -> RLC_NULL_STATE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  rlc_pP->protocol_state = RLC_NULL_STATE;
	  return 1;
	  break;

	case RLC_UM_RECEIVE_CRLC_SUSPEND_REQ_EVENT:
	case RLC_UM_TRANSMIT_CRLC_SUSPEND_CNF_EVENT:
	  LOG_INFO(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM RLC_DATA_TRANSFER_READY_STATE -> RLC_LOCAL_SUSPEND_STATE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  rlc_pP->protocol_state = RLC_LOCAL_SUSPEND_STATE;
	  return 1;
	  break;

	default:
	  LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM WARNING PROTOCOL ERROR - EVENT %02X hex NOT EXPECTED FROM \
				DATA_TRANSFER_READY_STATE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			eventP);
	  return 0;
	}

	break;

	//-------------------------------
	// RLC_LOCAL_SUSPEND_STATE
	//-------------------------------
  case RLC_LOCAL_SUSPEND_STATE:
	switch (eventP) {
	case RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
	  LOG_INFO(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM RLC_LOCAL_SUSPEND_STATE -> RLC_NULL_STATE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  rlc_pP->protocol_state = RLC_NULL_STATE;
	  return 1;
	  break;

	case RLC_UM_RECEIVE_CRLC_RESUME_REQ_EVENT:
	  LOG_INFO(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM RLC_LOCAL_SUSPEND_STATE -> RLC_DATA_TRANSFER_READY_STATE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  rlc_pP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
	  return 1;
	  break;

	default:
	  LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM	WARNING PROTOCOL ERROR - EVENT %02X hex NOT EXPECTED FROM \
				RLC_LOCAL_SUSPEND_STATE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			eventP);
	  return 0;
	}

	break;

  default:
	LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" FSM ERROR UNKNOWN STATE %d\n",
		  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
		  rlc_pP->protocol_state);
	return 0;
  }
}

void  rlc_um_reset_state_variables (
  const protocol_ctxt_t* const ctxt_pP,
  rlc_um_entity_t * const rlc_pP
)
{
  rlc_pP->buffer_occupancy = 0;
  rlc_pP->rlc_header_size = 0; 

  // TX SIDE
  rlc_pP->vt_us = 0;
  // RX SIDE
  rlc_pP->vr_ur = 0;
  rlc_pP->vr_ux = 0;
  rlc_pP->vr_uh = 0;
}


void rlc_um_configure(
							const protocol_ctxt_t* const ctxt_pP,
							rlc_um_entity_t * const rlc_pP,
							const uint32_t		   timer_reorderingP,
							const uint32_t		   rx_sn_field_lengthP,
							const uint32_t		   tx_sn_field_lengthP,
							const uint32_t		   is_mXchP)
{
	
	if (rx_sn_field_lengthP == 10) {
	  rlc_pP->rx_sn_length					= 10;
	  rlc_pP->rx_sn_modulo					= RLC_UM_SN_10_BITS_MODULO;
	   //！根据协议，sn = 10bit时，接收窗长512
	  rlc_pP->rx_um_window_size 			= RLC_UM_WINDOW_SIZE_SN_10_BITS; 
	  rlc_pP->rx_header_min_length_in_bytes = 2;
	} else if (rx_sn_field_lengthP == 5) {
	  rlc_pP->rx_sn_length					= 5;
	  rlc_pP->rx_sn_modulo					= RLC_UM_SN_5_BITS_MODULO;
	  //！根据协议，sn = 5bit时，接收窗长16 
	  rlc_pP->rx_um_window_size 			= RLC_UM_WINDOW_SIZE_SN_5_BITS; 
	  rlc_pP->rx_header_min_length_in_bytes = 1;
	} else if (rx_sn_field_lengthP != 0) {
	  LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" [CONFIGURE] RB %u INVALID RX SN LENGTH %d BITS NOT IMPLEMENTED YET,\
			  RLC NOT CONFIGURED\n", \
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),\
			rlc_pP->rb_id,\
			rx_sn_field_lengthP);
	  return;
	}

	if (tx_sn_field_lengthP == 10) {	  //！发送的设置，这里的发送窗跟接收窗一样，但是协议中没有设置发送窗
	  rlc_pP->tx_sn_length					= 10;
	  rlc_pP->tx_sn_modulo					= RLC_UM_SN_10_BITS_MODULO;
	  rlc_pP->tx_um_window_size 			= RLC_UM_WINDOW_SIZE_SN_10_BITS;
	  rlc_pP->tx_header_min_length_in_bytes = 2;
	} else if (tx_sn_field_lengthP == 5) {
	  rlc_pP->tx_sn_length					= 5;
	  rlc_pP->tx_sn_modulo					= RLC_UM_SN_5_BITS_MODULO;
	  rlc_pP->tx_um_window_size 			= RLC_UM_WINDOW_SIZE_SN_5_BITS;
	  rlc_pP->tx_header_min_length_in_bytes = 1;
	} else if (tx_sn_field_lengthP != 0) {
	  LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" [CONFIGURE] RB %u INVALID RX SN LENGTH %d BITS NOT IMPLEMENTED YET, \
			  RLC NOT CONFIGURED\n",\
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),\
			rlc_pP->rb_id,\
			tx_sn_field_lengthP);
	  return;
	}

	if (is_mXchP > 0) {
	  rlc_pP->tx_um_window_size = 0;
	  rlc_pP->rx_um_window_size = 0;
	}

	rlc_pP->is_mxch = is_mXchP;

	rlc_pP->last_reassemblied_sn  = rlc_pP->rx_sn_modulo - 1;
	rlc_pP->last_reassemblied_missing_sn  = rlc_pP->rx_sn_modulo - 1;
	rlc_pP->reassembly_missing_sn_detected = 0;  //！用在接收多个PDU 才能组包一个SDU时，检测是否丢包
	//! timers initial 
	rlc_um_init_timer_reordering(ctxt_pP,rlc_pP, timer_reorderingP);

	rlc_pP->first_pdu = 1;

	rlc_um_reset_state_variables (ctxt_pP,rlc_pP);
}



void config_req_rlc_um (
  const protocol_ctxt_t* const ctxt_pP,
  const srb_flag_t      srb_flagP,
  const rlc_um_info_t  * const config_um_pP,
  const rb_id_t           rb_idP,
  const logical_chan_id_t chan_idP)
{
  rlc_union_t     *rlc_union_p  = NULL;
  rlc_um_entity_t *rlc_p        = NULL;
  hash_key_t       key; 
  hashtable_rc_t   h_rc;


  key  = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);
  h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);

  if (h_rc == HASH_TABLE_OK) {
    pthread_mutex_lock(&(rlc_union_p->rlc_union_mtex));
    rlc_p = &rlc_union_p->rlc.um;

    rlc_um_init(ctxt_pP, rlc_p,srb_flagP,rb_idP); //！初始化

 
    if (rlc_um_fsm_notify_event (ctxt_pP, rlc_p, RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT)) {
      
      rlc_um_configure(
        ctxt_pP,
        rlc_p,
        config_um_pP->timer_reordering,
        config_um_pP->sn_field_length,
        config_um_pP->sn_field_length,
        config_um_pP->is_mXch);

       LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" CONFIG_REQ timer_reordering=%d \
       rx_sn_field_length=%d,tx_sn_field_length=%d,\
       is_mXch=%d RB %u\n",
          PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_p),
          rlc_p->t_reordering.ms_duration,
          rlc_p->rx_sn_length,
          rlc_p->tx_sn_length,
          rlc_p->is_mxch,
          rlc_p->rb_id);
    }
    pthread_mutex_unlock(&(rlc_union_p->rlc_union_mtex));
  } else {
    LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT"  CONFIG_REQ RB %u  RLC UM NOT FOUND\n",
          PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_p),
          rb_idP);
  }
} 





void rlc_um_data_req (const protocol_ctxt_t *const ctxt_pP, void *rlc_pP, mem_block_t *sdu_pP) 
{
	rlc_um_entity_t *rlc_p = (rlc_um_entity_t *) rlc_pP;
	uint16_t			 data_offset;
	uint16_t			 data_size;
	size_t				 message_string_size = 0;
	int 				 octet_index, index;
	char				 message_string[7000];
	LOG_INFO(RLC, PROTOCOL_RLC_UM_CTXT_FMT" RLC_UM_DATA_REQ size %d Bytes, BO %d , NB SDU %d\n",
		  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_p),
		  ((struct rlc_um_data_req *) (sdu_pP->data))->data_size,
		  rlc_p->buffer_occupancy,
		  rlc_p->input_sdus.nb_elements);
	// IMPORTANT : do not change order of affectations
	((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_size = ((struct rlc_um_data_req *) (sdu_pP->data))->data_size;
	//rlc_p->nb_sdu += 1;
	//! first byte 地址偏移掉header 
	((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->first_byte = (uint8_t *)&sdu_pP->data[sizeof (struct rlc_um_data_req_alloc)];
	((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_remaining_size = \
													((struct rlc_um_tx_sdu_management *)(sdu_pP->data))->sdu_size;
	((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_segmented_size = 0;  //!已经分配给PDU的size 
	((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_creation_time = ctxt_pP->frame; 
	//rlc_p->next_sdu_index = (rlc_p->next_sdu_index + 1) % rlc_p->size_input_sdus_buffer;
	rlc_p->stat_tx_pdcp_sdu   += 1;
	rlc_p->stat_tx_pdcp_bytes += ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_size;
	

	RLC_UM_MUTEX_LOCK(&rlc_p->lock_input_sdus, ctxt_pP, rlc_p);
	//！SDU中的Buffer size 增加
	rlc_p->buffer_occupancy += ((struct rlc_um_tx_sdu_management *) (sdu_pP->data))->sdu_size;
	
	
   
	rlc_Set_Buffer_Status(ctxt_pP->rnti, RLC_MODE_UM,rlc_p->channel_id,rlc_p->input_sdus.nb_elements,
						((struct rlc_tm_tx_sdu_management *) (sdu_pP->data))->sdu_size);
	//！将新的sdu 加入到rlc->input_sdu中 ，更新节点中的地址
	list_add_tail_eurecom(sdu_pP, &rlc_p->input_sdus);

	LOG_INFO(RLC, "rlc_p->stat_tx_pdcp_sdu: %d  , rlc_p->stat_tx_pdcp_bytes: %lld ,rlc_p->buffer_occupancy = %d, UM tx sdu List element count = %d\n", 
				rlc_p->stat_tx_pdcp_sdu , rlc_p->stat_tx_pdcp_bytes,rlc_p->buffer_occupancy,
				rlc_p->input_sdus.nb_elements);
	RLC_UM_MUTEX_UNLOCK(&rlc_p->lock_input_sdus);

	
}




void    rlc_um_set_nb_bytes_requested_by_mac( void         *rlc_pP,
														  const tb_size_t   tb_sizeP) 
{
  ((rlc_um_entity_t *) rlc_pP)->nb_bytes_requested_by_mac = tb_sizeP;
} 





void	rlc_um_mac_data_request(const protocol_ctxt_t *const ctxt_pP, 
                                                    const tb_size_t   tb_sizeP,
													void *rlc_pP,
													const eNB_flag_t  enb_flagP,
													logic_channel_pdu_component  *lc_pdu_component_ptr,
													mac_pdu_size_para  *ue_mac_pdu_size_ptr,
													struct mac_data_req *data_req) 
{
	
	int16_t 			  tb_size_in_bytes;
	mem_block_t 	   *tb_p;

	rlc_um_pdu_info_t	pdu_info;
	int 				octet_index, index;
	rlc_um_entity_t *l_rlc_p = (rlc_um_entity_t *) rlc_pP;

	l_rlc_p->nb_bytes_requested_by_mac = tb_sizeP;
	//！把填好的PDU 添加到rlc_p->pdus_to_mac_layer 链表中，
	rlc_um_get_pdus(ctxt_pP, l_rlc_p,lc_pdu_component_ptr,ue_mac_pdu_size_ptr);

	//！将pdus_to_mac_layer 链表添加到data 这个链表中
	list_add_list(&l_rlc_p->pdus_to_mac_layer, &data_req->data);

	
    data_req->buffer_occupancy_in_bytes +=  l_rlc_p->buffer_occupancy;


#if 0
	if (data_req.data.nb_elements > 0) 
	{ //!data 是一个链表，链表中的成员个数> 0 
	  tb_p = data_req.data.head; //!得到链表的头节点

	  while (tb_p != NULL) { //！从头节点开始依次处理

		//！更新RLC 实体的发送参数
		tb_size_in_bytes   = ((struct mac_tb_req *) (tb_p->data))->tb_size;
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" MAC_DATA_REQUEST  TB SIZE %u\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,l_rlc_p),
			  ((struct mac_tb_req *) (tb_p->data))->tb_size);
		l_rlc_p->stat_tx_data_pdu	+= 1;
		l_rlc_p->stat_tx_data_bytes += tb_size_in_bytes;

		//AssertFatal( tb_size_in_bytes > 0 , "RLC UM PDU LENGTH %d", tb_size_in_bytes);
		if(tb_size_in_bytes <= 0) {
		  LOG_ERROR(RLC, "RLC UM PDU LENGTH %d\n", tb_size_in_bytes);
		  tb_p = tb_p->next;
		  continue;
	    }

		 tb_p = tb_p->next;
 	  } /* while (tb_p != NULL) */
	} /* if (data_req.data.nb_elements > 0) */
#endif 


}



void  rlc_um_segment_10(const protocol_ctxt_t* const ctxt_pP, 
							    rlc_um_entity_t *rlc_pP,
							    logic_channel_pdu_component  *lc_pdu_component_ptr,
							    mac_pdu_size_para  *ue_mac_pdu_size_ptr)
{
  list_t			  pdus;
  signed int		  pdu_remaining_size;
  signed int		  test_pdu_remaining_size;

  int				  nb_bytes_to_transmit = 0; //！MAC需要的TB size 
  int                 remain_byte_to_allocate = 0;
  rlc_um_pdu_sn_10_t *pdu_p;
  struct mac_tb_req  *pdu_tb_req_p; //!mac tb request 
  mem_block_t		 *pdu_mem_p;
  char				 *data;
  char				 *data_sdu_p;
  rlc_um_e_li_t 	 *e_li_p;
  struct rlc_um_tx_sdu_management *sdu_mngt_p;
  unsigned int		 li_length_in_bytes;
  unsigned int		 test_li_length_in_bytes;
  unsigned int		 test_remaining_size_to_substract;
  unsigned int		 test_remaining_num_li_to_substract;
  unsigned int		 continue_fill_pdu_with_sdu;  //!是否在一个PDU 中还要填其他SDU 
  unsigned int		 num_fill_sdu;
  unsigned int		 test_num_li;
  unsigned int		 fill_num_li;
  mem_block_t		 *sdu_in_buffer = NULL;
  unsigned int		 data_pdu_size;

  unsigned int		 fi_first_byte_pdu_is_first_byte_sdu;
  unsigned int		 fi_last_byte_pdu_is_last_byte_sdu;
  unsigned int		 fi;
  unsigned int		 max_li_overhead;
  logic_channel_pdu_component    *next_lc_pdu_ptr = NULL; 
  tb_size_t                       next_lc_pdu_size ; 

  uint32_t                        rlc_sdu_length ; 


  //!calculate the actually PDU size for transmission 
  remain_byte_to_allocate  = ue_mac_pdu_size_ptr->remain_pdu_size;

 
  nb_bytes_to_transmit = rlc_pP->nb_bytes_requested_by_mac;	

  if (nb_bytes_to_transmit > remain_byte_to_allocate)
  {
		nb_bytes_to_transmit = remain_byte_to_allocate; //!there only have remain_byte_to_allocate for this logic channel 
  }
 
  
  

  if (nb_bytes_to_transmit < 3) {
	LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" NO SEGMENTATION nb_bytes to transmit = %d\n",
		  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
		  nb_bytes_to_transmit);

	return;
  }

  if ((0 == remain_byte_to_allocate ) && (1 == lc_pdu_component_ptr->is_last_sub_header_flag))
  {

	LOG_INFO(RLC, PROTOCOL_RLC_UM_CTXT_FMT" NO SPACE remained for the last logic channel: %d, has occupied by channel %d !\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  lc_pdu_component_ptr->logic_ch_index,
			  lc_pdu_component_ptr->occupy_by_previous_lc_idx); 
	return ; 
  }


  
  //初始化链表
  list_init (&pdus, NULL);	  // param string identifying the list is NULL
  pdu_mem_p = NULL;


  RLC_UM_MUTEX_LOCK(&rlc_pP->lock_input_sdus, ctxt_pP, rlc_pP); //!加锁
  
   //!从rlc_pp的input_sdu链表中，获取每个节点，进行如下处理： 
  while ((list_get_head(&rlc_pP->input_sdus)) && (nb_bytes_to_transmit > 0)) {

	LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" nb_bytes_to_transmit %d BO %d\n",
		  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
		  nb_bytes_to_transmit,
		  rlc_pP->buffer_occupancy);


	// pdu_p management
	//!< UM PDU是由一个或多个data segment 组成的，每个data segment对应一个SDU或者SDU 分段
	//!< 最后一个data segment不需要LI 字段。如果是奇数个LI,则需要增加4bit padding.
	//! 计算LI+E 和可能存在的Padding的header大小
	//! 没有申请过内存时，先header和扩展部分的大小，然后根据sdu大小+header+扩展部分的大小和MAC tb_size进行
	//！ 比较，确定要传给MAC的大小 

	//!calculate the rlc header size if rlc transmit all of the SDU data 
	if (!pdu_mem_p) {  
		  if (rlc_pP->input_sdus.nb_elements <= 1) {
			max_li_overhead = 0;  //!<只有一个sdu
		  } else { //!如果有多个SDU,则需要有扩展部分的E+LI
			max_li_overhead = (((rlc_pP->input_sdus.nb_elements - 1) * 3) / 2) + ((rlc_pP->input_sdus.nb_elements - 1) % 2);
		  }

		 
		  //!RLC total size 
		  rlc_sdu_length =  (rlc_pP->buffer_occupancy + rlc_pP->tx_header_min_length_in_bytes + max_li_overhead); 
	  
	   //!如果mac tb size 大于RLC total size , 则要发送的data_pdu_size就是实际大小,然后加padding 
		if  (nb_bytes_to_transmit >= rlc_sdu_length) 
		{
		data_pdu_size = rlc_sdu_length;

		lc_pdu_component_ptr->final_mac_sdu_size = rlc_sdu_length; 
		if (1 == lc_pdu_component_ptr->is_last_sub_header_flag)
		{
			lc_pdu_component_ptr->padding_byte = nb_bytes_to_transmit - rlc_sdu_length; 
		}
		else 
		{
			lc_pdu_component_ptr->padding_byte = 0; 
		}
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT"SDU doesn't need to split, MAC TB size [%d] >= RLC SDU + header size [%d],the final pdu size[%d]!\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  nb_bytes_to_transmit,
			  rlc_sdu_length,
			  data_pdu_size);

		} 
		else
		{		
		 
		if (1 == lc_pdu_component_ptr->is_last_sub_header_flag)
		{
			data_pdu_size = nb_bytes_to_transmit;
			lc_pdu_component_ptr->final_mac_sdu_size = nb_bytes_to_transmit; 	
		}
		else  //!this logic channel's data will occupy the next logic channel's space
		{
			next_lc_pdu_ptr = lc_pdu_component_ptr + 1;
			//!calculate the next logic channle's occupied space 
			while (next_lc_pdu_ptr->valid_flag != 0)
			{
				next_lc_pdu_size = next_lc_pdu_ptr->remain_mac_pdu_size; 
				if (next_lc_pdu_size != 0)
				{
					if ((nb_bytes_to_transmit + next_lc_pdu_size) < rlc_sdu_length)
					{
						lc_pdu_component_ptr->final_mac_sdu_size = lc_pdu_component_ptr->final_mac_sdu_size + next_lc_pdu_size;
						next_lc_pdu_ptr->occupy_by_previous_lc_flag = 1; 
						next_lc_pdu_ptr->occupy_by_previous_lc_idx = lc_pdu_component_ptr->logic_ch_index;
						next_lc_pdu_ptr->remain_mac_pdu_size = 0;
						
					}
					else 
					{
						//!update the occupied logic channel's info 
						next_lc_pdu_ptr->remain_mac_pdu_size = next_lc_pdu_size - 
							                                     (rlc_sdu_length - lc_pdu_component_ptr->final_mac_sdu_size); 
							                                     
						lc_pdu_component_ptr->final_mac_sdu_size = rlc_sdu_length;

						next_lc_pdu_ptr->occupy_by_previous_lc_flag = 1; 
						next_lc_pdu_ptr->occupy_by_previous_lc_idx = lc_pdu_component_ptr->logic_ch_index;
						break; 
					}
				}
				next_lc_pdu_ptr = next_lc_pdu_ptr + 1;
				 
			}

			data_pdu_size = lc_pdu_component_ptr->final_mac_sdu_size; //!this logic channel's final SDU size 
		}

		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" SDU  need to occupy other lc's space, MAC TB size [%d] < RLC SDU + header size [%d],the final PDU size:[%d]\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  nb_bytes_to_transmit,
			  rlc_sdu_length,
			  data_pdu_size);

	  }

       //!set this logic channel's mac subheader type and mac subheader length 
		lc_pdu_component_ptr->rlc_data_length = data_pdu_size; 

		if (1 == lc_pdu_component_ptr->is_last_sub_header_flag)
		{
			lc_pdu_component_ptr->mac_subheader_length = 1; 
			lc_pdu_component_ptr->mac_subheader_length_type = 1; 
		}
		else
		{	
			if (lc_pdu_component_ptr->rlc_data_length < 128)
			{
				lc_pdu_component_ptr->mac_subheader_length = 2; 
				lc_pdu_component_ptr->mac_subheader_length_type  = 2; 
			}
			else 
			{
				lc_pdu_component_ptr->mac_subheader_length = 3; 
				lc_pdu_component_ptr->mac_subheader_length_type  = 3;

			}

		} 

      
	   
	  //！申请一块内存，大小为data_pdu_size + sizeof(struct mac_tb_req);
	  if (!(pdu_mem_p = get_free_mem_block (data_pdu_size + sizeof(struct mac_tb_req), __func__))) {
			LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" ERROR COULD NOT GET NEW PDU, EXIT\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP));
			RLC_UM_MUTEX_UNLOCK(&rlc_pP->lock_input_sdus); //出错，return之前解锁
		return;
	  }

	  pdu_remaining_size = data_pdu_size - 2; //去掉2个固定的header 
	   //!pdu_p指向的是PDU的开头部分，UM模式下是包含了header的
	  pdu_p 	   = (rlc_um_pdu_sn_10_t*) (&pdu_mem_p->data[sizeof(struct mac_tb_req)]);
	  //!<开头是mac_tb_req,然后才是pdu
	  pdu_tb_req_p = (struct mac_tb_req*) (pdu_mem_p->data);
	  //！先清0
	  memset (pdu_mem_p->data, 0, sizeof (rlc_um_pdu_sn_10_t)+sizeof(struct mac_tb_req));
	  li_length_in_bytes = 1;
	}

	//----------------------------------------
	// compute how many SDUS can fill the PDU
	//----------------------------------------
	continue_fill_pdu_with_sdu = 1;
	num_fill_sdu			   = 0;
	test_num_li 			   = 0; //! number of "LI+E"
	sdu_in_buffer			   = list_get_head(&rlc_pP->input_sdus); //！得到header节点 
	test_pdu_remaining_size    = pdu_remaining_size; 
	test_li_length_in_bytes    = 1;
	test_remaining_size_to_substract   = 0;
	test_remaining_num_li_to_substract = 0;

	 //!calculate how many SDU will fill into the PDU 
	while ((sdu_in_buffer) && (continue_fill_pdu_with_sdu > 0)) {
	  sdu_mngt_p = ((struct rlc_um_tx_sdu_management *) (sdu_in_buffer->data));

	  //! 
	  if (sdu_mngt_p->sdu_remaining_size > test_pdu_remaining_size) {
		// no LI
		continue_fill_pdu_with_sdu = 0;  //！只需要填写一次PDU即可，不需要再填PDU了
		num_fill_sdu += 1;
		test_pdu_remaining_size = 0; //!PDU 全部占满
		test_remaining_size_to_substract = 0;
		test_remaining_num_li_to_substract = 0;
	  } else if (sdu_mngt_p->sdu_remaining_size == test_pdu_remaining_size) {
		// fi will indicate end of PDU is end of SDU, no need for LI
		continue_fill_pdu_with_sdu = 0;
		num_fill_sdu += 1;
		test_pdu_remaining_size = 0;
		test_remaining_size_to_substract = 0;
		test_remaining_num_li_to_substract = 0;
	  } else if ((sdu_mngt_p->sdu_remaining_size + (test_li_length_in_bytes ^ 3)) == test_pdu_remaining_size ) {
		// no LI
		//！SDU 的size 小于PDU的size,但是SDU的size +2个byte就 = PDU size ,
		//！	
		continue_fill_pdu_with_sdu = 0;
		num_fill_sdu += 1;
		test_pdu_remaining_size = 0;
		test_remaining_size_to_substract = 0;
		test_remaining_num_li_to_substract = 0;
		//pdu_remaining_size = pdu_remaining_size - (test_li_length_in_bytes ^ 3);
	  } else if ((sdu_mngt_p->sdu_remaining_size + (test_li_length_in_bytes ^ 3)) < test_pdu_remaining_size ) {
		//！SDU 的size 小于PDU的size,但是SDU的size +2个byte		 <	PDU size ,
		//！ PDU 可以容纳2个SDU,因此这里有一个LI ,
		test_num_li += 1; //!说明当前的PDU 还可以容纳其他SDU，因此test_num_li + 1
		num_fill_sdu += 1;
		//!<PDU 内剩下的size 
		test_pdu_remaining_size = test_pdu_remaining_size - (sdu_mngt_p->sdu_remaining_size + (test_li_length_in_bytes ^ 3));
		//!test_li_length_in_bytes =1时，1^3 = 2，当test_li_length_in_bytes =2时，2^3 = 1
		test_remaining_size_to_substract = test_li_length_in_bytes ^ 3; 
		test_remaining_num_li_to_substract = 1;
		test_li_length_in_bytes = test_li_length_in_bytes ^ 3;
	  } else {
#if TRACE_RLC_UM_SEGMENT
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" sdu_mngt_p->sdu_remaining_size=%d test_pdu_remaining_size=%d test_li_length_in_bytes=%d\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  sdu_mngt_p->sdu_remaining_size,
			  test_pdu_remaining_size,
			  test_li_length_in_bytes ^ 3);
#endif
		// reduce the size of the PDU
		continue_fill_pdu_with_sdu = 0;
		num_fill_sdu += 1;
		test_pdu_remaining_size = 0;
		test_remaining_size_to_substract = 0;
		test_remaining_num_li_to_substract = 0;
		pdu_remaining_size = pdu_remaining_size - 1;//!??
	  }

	  sdu_in_buffer = sdu_in_buffer->next;	
	}
	
#if 0
	if (test_remaining_num_li_to_substract > 0) {  //!有多个SDU填到一个PDU 
	  // there is a LI that is not necessary
	  test_num_li = test_num_li - 1;  //!<最后一个data segment不需要LI  
	  //!<pdu_remaining_size 再次更新
	  pdu_remaining_size = pdu_remaining_size - test_remaining_size_to_substract;
	}
#endif 

	//----------------------------------------
	// Do the real filling of the pdu_p
	//----------------------------------------

	LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" data shift %d Bytes num_li %d\n",
		  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
		  ((test_num_li*3) +1) >> 1,
		  test_num_li);

	//! data指向的是data field 
	data = ((char*)(&pdu_p->data[((test_num_li*3) +1) >> 1]));	//！偏移掉E+LI 扩展部分
	e_li_p = (rlc_um_e_li_t*)(pdu_p->data); //！指向pdu_p->data头部段,开始填写E+LI的地址
	continue_fill_pdu_with_sdu			= 1;
	li_length_in_bytes					= 1;
	fill_num_li 						= 0;
	fi_first_byte_pdu_is_first_byte_sdu = 0;
	fi_last_byte_pdu_is_last_byte_sdu	= 0;

	sdu_in_buffer = list_get_head(&rlc_pP->input_sdus);

	if (((struct rlc_um_tx_sdu_management *) (sdu_in_buffer->data))->sdu_remaining_size ==
	  ((struct rlc_um_tx_sdu_management *) (sdu_in_buffer->data))->sdu_size) {
	  fi_first_byte_pdu_is_first_byte_sdu = 1; //！PDU 的first byte 是SDU的first byte  
	}

	while ((sdu_in_buffer) && (continue_fill_pdu_with_sdu > 0)) {
	  sdu_mngt_p = ((struct rlc_um_tx_sdu_management *) (sdu_in_buffer->data));

	  if (sdu_mngt_p->sdu_segmented_size == 0) {  //！sdu_segmented_size 表示已经填给PDU的size 

		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" GET NEW SDU %p AVAILABLE SIZE %d Bytes\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  sdu_mngt_p,
			  sdu_mngt_p->sdu_remaining_size);	//!sdu_remaining_size 表示的是SDU 中还没有填进去的size 

	  } else {
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" GET AGAIN SDU %p REMAINING AVAILABLE SIZE %d Bytes / %d Bytes \n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  sdu_mngt_p,
			  sdu_mngt_p->sdu_remaining_size,
			  sdu_mngt_p->sdu_size);

	  }

	  data_sdu_p = (char *) &(sdu_in_buffer->data[sizeof (struct rlc_um_tx_sdu_management) + sdu_mngt_p->sdu_segmented_size]);

	  //! SDU size > remained PDU size 
	  if (sdu_mngt_p->sdu_remaining_size > pdu_remaining_size) {
#if TRACE_RLC_UM_SEGMENT
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" Filling all remaining PDU with %d bytes\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  pdu_remaining_size);
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" pdu_mem_p %p pdu_mem_p->data %p pdu_p %p pdu_p->data %p data %p data_sdu_p %p pdu_remaining_size %d\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  pdu_mem_p,
			  pdu_mem_p->data,
			  pdu_p,
			  pdu_p->data,
			  data,
			  data_sdu_p,
			  pdu_remaining_size);
#endif

		memcpy(data, data_sdu_p, pdu_remaining_size); //!向PDU中的data field copy数据
		 //！更新SDU 中已经分配的，残留的 
		sdu_mngt_p->sdu_remaining_size = sdu_mngt_p->sdu_remaining_size - pdu_remaining_size;
		sdu_mngt_p->sdu_segmented_size = sdu_mngt_p->sdu_segmented_size + pdu_remaining_size;
		fi_last_byte_pdu_is_last_byte_sdu = 0; //!< last byte in PDU is not the last byte of SDU 
		// no LI
		rlc_pP->buffer_occupancy -= pdu_remaining_size; //!分配出去一个PDU的size 
		continue_fill_pdu_with_sdu = 0;  
		pdu_remaining_size = 0;
	  } 
	  else if (sdu_mngt_p->sdu_remaining_size == pdu_remaining_size) {
#if TRACE_RLC_UM_SEGMENT
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" Exactly Filling remaining PDU with %d remaining bytes of SDU\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  pdu_remaining_size);
#endif
		//!< SDU size = PDU size ,直接copy 
		memcpy(data, data_sdu_p, pdu_remaining_size);

		// free SDU
		rlc_pP->buffer_occupancy -= sdu_mngt_p->sdu_remaining_size;

		//！处理完一个sdu后，free head,以便于处理下一个header
		sdu_in_buffer = list_remove_head(&rlc_pP->input_sdus);
		free_mem_block (sdu_in_buffer, __func__);
		sdu_in_buffer = list_get_head(&rlc_pP->input_sdus);
		sdu_mngt_p	  = NULL;


		fi_last_byte_pdu_is_last_byte_sdu = 1;
		// fi will indicate end of PDU is end of SDU, no need for LI
		continue_fill_pdu_with_sdu = 0;
		pdu_remaining_size = 0;
	  } 
	  else if ((sdu_mngt_p->sdu_remaining_size + (li_length_in_bytes ^ 3)) < pdu_remaining_size ) {
#if TRACE_RLC_UM_SEGMENT
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" Filling  PDU with %d all remaining bytes of SDU\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  sdu_mngt_p->sdu_remaining_size);
#endif
		//!< SDU + 一个LI < PDU
		memcpy(data, data_sdu_p, sdu_mngt_p->sdu_remaining_size); //将SDU 的所有byte都分配给PDU 
		data = &data[sdu_mngt_p->sdu_remaining_size];
		li_length_in_bytes = li_length_in_bytes ^ 3;  //! =2
		fill_num_li += 1; //! =1 

        //!当E+LI = 奇数个时，是计算rlc header的周期，每2个rlc header = 3byte.
		if (li_length_in_bytes	== 2) {  //!奇数个LI时
		 //!如果是奇数个E+LI,并且等于最后一个segment,则不需要再填写header了。 
		  if (fill_num_li == test_num_li) {
			//!e_li_p->e1  = 0;
			e_li_p->b1 = 0;  
		  } else {
		   //! 否则开始3个byte的header的配置，将byte1 的最高为e = 1
			//!e_li_p->e1  = 1;
			e_li_p->b1 =  0x80;
		  }

		  //e_li_p->li1 = sdu_mngt_p->sdu_remaining_size;
		  //!右移4bit,剩下7bit和E 组成一个byte
		  e_li_p->b1 = e_li_p->b1 | (sdu_mngt_p->sdu_remaining_size >> 4);	
		  e_li_p->b2 = sdu_mngt_p->sdu_remaining_size << 4; //! 左移4bit,作为b2的高4bit
#if TRACE_RLC_UM_SEGMENT
		  LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" set e_li_p->b1=%02X set e_li_p->b2=%02X fill_num_li=%d test_num_li=%d\n",
				PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
				e_li_p->b1,
				e_li_p->b2,
				fill_num_li,
				test_num_li);
#endif
		} else { //!偶数个LI+E
		  //！偶数个LI,占3个byte的整数倍
		  if (fill_num_li != test_num_li) {
			e_li_p->b2	= e_li_p->b2 | 0x08; //！第二个E = 1,表示后面是E+L1,在byte2的bit4 
		  }

		  //e_li_p->li2 = sdu_mngt_p->sdu_remaining_size;
		  e_li_p->b2 = e_li_p->b2 | (sdu_mngt_p->sdu_remaining_size >> 8);
		  e_li_p->b3 = sdu_mngt_p->sdu_remaining_size & 0xFF; //!B3是LI 的低8bit 
#if TRACE_RLC_UM_SEGMENT
		  LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" set e_li_p->b2=%02X set e_li_p->b3=%02X fill_num_li=%d test_num_li=%d\n",
				PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
				e_li_p->b2,
				e_li_p->b3,
				fill_num_li,
				test_num_li);
#endif
		  e_li_p++;
		}
		//MAC PDU 还残留的size 
		pdu_remaining_size = pdu_remaining_size - (sdu_mngt_p->sdu_remaining_size + li_length_in_bytes);

		// free SDU
		rlc_pP->buffer_occupancy -= sdu_mngt_p->sdu_remaining_size;
		sdu_in_buffer = list_remove_head(&rlc_pP->input_sdus);
		free_mem_block (sdu_in_buffer, __func__);
		sdu_in_buffer = list_get_head(&rlc_pP->input_sdus);  //!get the next sdu 
		sdu_mngt_p	  = NULL;

	  } 
	  else {  //!sdu + header > remain_pdu_size,
#if TRACE_RLC_UM_SEGMENT
		LOG_DEBUG(RLC, PROTOCOL_RLC_UM_CTXT_FMT" Filling  PDU with %d all remaining bytes of SDU and reduce TB size by %d bytes\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
			  sdu_mngt_p->sdu_remaining_size,
			  pdu_remaining_size - sdu_mngt_p->sdu_remaining_size);
#endif
//#if !EXMIMO
//		  assert(1!=1);
//#endif
		memcpy(data, data_sdu_p, sdu_mngt_p->sdu_remaining_size); //!把SDU 的全部都给PDU 
		// reduce the size of the PDU
		continue_fill_pdu_with_sdu = 0;//!最多还剩下一个byte的空间， 这里不再继续填充SDU了，所以这里存在一个空洞。
		fi_last_byte_pdu_is_last_byte_sdu = 1; //PDU 的当前byte 中的最后一个byte 是SDU的最后一个。
		//！PDU中还能容纳SDU 
		pdu_remaining_size = pdu_remaining_size - sdu_mngt_p->sdu_remaining_size; 
		// free SDU
		rlc_pP->buffer_occupancy -= sdu_mngt_p->sdu_remaining_size;
		sdu_in_buffer = list_remove_head(&rlc_pP->input_sdus);
		free_mem_block (sdu_in_buffer, __func__);
		sdu_in_buffer = list_get_head(&rlc_pP->input_sdus); //!指向下一个head 
		sdu_mngt_p	  = NULL;

	  }
	}


	 //！开始填写固定的2Byte header信息，include [FI,E,SN] field  
	// set framing info
	if (fi_first_byte_pdu_is_first_byte_sdu) {
	  fi = 0;  //!< FI[1] = 0
	} else {
	  fi = 2;  //!< FI[1] = 1
	}

	if (!fi_last_byte_pdu_is_last_byte_sdu) {
	  fi = fi + 1;	//!FI[0]
	}
	 //!SN = 10时，FI 占bit[3-4]
	pdu_p->b1 =  (fi << 3); //pdu_p->b1 |

	// set fist e bit
	if (fill_num_li > 0) {
	  pdu_p->b1 = pdu_p->b1 | 0x04;  //! E 占bit[2] 
	}
	//!SN 保留高2bit 在b1中， 
	pdu_p->b1 = pdu_p->b1 | ((rlc_pP->vt_us >> 8) & 0x03);
	pdu_p->b2 = rlc_pP->vt_us & 0xFF;  //！SN保留低8bit在b2中 
	rlc_pP->vt_us = rlc_pP->vt_us+1;  //vt_us ++

	pdu_tb_req_p->data_ptr		  = (unsigned char*)pdu_p;
	//!this logic channel's final tb size , the pdu_remaining_size = 0 or 1,no other value. 
	pdu_tb_req_p->tb_size = data_pdu_size - pdu_remaining_size; 
	
	list_add_tail_eurecom (pdu_mem_p, &rlc_pP->pdus_to_mac_layer); //!将这块memory 添加到pdus_to_mac_layer中去 

	rlc_pP->stat_tx_data_pdu	+= 1;
	rlc_pP->stat_tx_data_bytes += pdu_tb_req_p->tb_size;
	
#if TRACE_RLC_PAYLOAD
	rlc_util_print_hex_octets(RLC, pdu_mem_p->data, data_pdu_size); 
#endif
	//AssertFatal( pdu_tb_req_p->tb_size > 0 , "SEGMENT10: FINAL RLC UM PDU LENGTH %d", pdu_tb_req_p->tb_size);
	if(pdu_tb_req_p->tb_size <= 0) {
	  LOG_ERROR(RLC, "SEGMENT10: FINAL RLC UM PDU LENGTH %d\n", pdu_tb_req_p->tb_size);
	  break;
	}
	pdu_p = NULL;
	pdu_mem_p = NULL;

	//nb_bytes_to_transmit = nb_bytes_to_transmit - data_pdu_size;
	nb_bytes_to_transmit = 0; // 1 PDU only
  }

  RLC_UM_MUTEX_UNLOCK(&rlc_pP->lock_input_sdus); //!退出函数之前，解锁
}



void rlc_um_get_pdus (const protocol_ctxt_t *const ctxt_pP, void *argP,
							logic_channel_pdu_component  *lc_pdu_component_ptr,
							mac_pdu_size_para  *ue_mac_pdu_size_ptr) 
{
  rlc_um_entity_t *rlc_p = (rlc_um_entity_t *) argP;

  switch (rlc_p->protocol_state) {
	case RLC_NULL_STATE:
	  // from 3GPP TS 25.322 V9.2.0 p43
	  // In the NULL state the RLC entity does not exist and therefore it is
	  // not possible to transfer any data through it.
	  // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
	  // establishment, the RLC entity:
	  //   - is created; and
	  //   - enters the DATA_TRANSFER_READY state.
	  break;

	case RLC_DATA_TRANSFER_READY_STATE:

	  // from 3GPP TS 25.322 V9.2.0 p43-44
	  // In the DATA_TRANSFER_READY state, unacknowledged mode data can be
	  // exchanged between the entities according to subclause 11.2.
	  // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
	  // release, the RLC entity:
	  // -enters the NULL state; and
	  // -is considered as being terminated.
	  // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
	  // modification, the RLC entity:
	  // - stays in the DATA_TRANSFER_READY state;
	  // - modifies only the protocol parameters and timers as indicated by
	  // upper layers.
	  // Upon reception of a CRLC-SUSPEND-Req from upper layers, the RLC
	  // entity:
	  // - enters the LOCAL_SUSPEND state.

	  // SEND DATA TO MAC
	  if (rlc_p->tx_sn_length == 10) {
		//!按照PDU 的header，size,从SDU中分割数据给PDU
		rlc_um_segment_10 (ctxt_pP, rlc_p,lc_pdu_component_ptr,  ue_mac_pdu_size_ptr);
	  }

	  else{
		LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" wrong tx sn filed %d\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_p),
			rlc_p->tx_sn_length );
	  }

	  break;

	case RLC_LOCAL_SUSPEND_STATE:
	  // from 3GPP TS 25.322 V9.2.0 p44
	  // In the LOCAL_SUSPEND state, the RLC entity is suspended, i.e. it does
	  // not send UMD PDUs with "Sequence Number" greater than or equal to a
	  // certain specified value (see subclause 9.7.5).
	  // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
	  // release, the RLC entity:
	  // - enters the NULL state; and
	  // - is considered as being terminated.
	  // Upon reception of a CRLC-RESUME-Req from upper layers, the RLC entity:
	  // - enters the DATA_TRANSFER_READY state; and
	  // - resumes the data transmission.
	  // Upon reception of a CRLC-CONFIG-Req from upper layer indicating
	  // modification, the RLC entity:
	  // - stays in the LOCAL_SUSPEND state;
	  // - modifies only the protocol parameters and timers as indicated by
	  //   upper layers.
	  // TO DO TAKE CARE OF SN : THE IMPLEMENTATION OF THIS FUNCTIONNALITY IS NOT CRITICAL
	  break;

	default:
	  LOG_ERROR(RLC, PROTOCOL_RLC_UM_CTXT_FMT" MAC_DATA_REQ UNKNOWN PROTOCOL STATE %02X hex\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_p),
			rlc_p->protocol_state);
  }
}



 
 
/**************************function******************************/
