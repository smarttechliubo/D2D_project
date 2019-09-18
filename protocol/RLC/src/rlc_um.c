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
 
 
 
/**************************function******************************/
