/******************************************************************
 * @file  rlc_tm.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc.h>
#include <rlc_tm_entity.h>

void rlc_tm_init (
  const protocol_ctxt_t* const  ctxt_pP,
  rlc_tm_entity_t * const rlcP,
  const srb_flag_t  srb_flagP,
  const rb_id_t rb_idP,
  const logical_chan_id_t chan_id
)
{
  //int saved_allocation = rlcP->allocation;
  memset (rlcP, 0, sizeof (struct rlc_tm_entity));
//  rlcP->allocation = saved_allocation;
 
  // TX SIDE
  list_init (&rlcP->pdus_to_mac_layer, NULL);

  rlcP->protocol_state    = RLC_NULL_STATE;
  rlcP->nb_sdu            = 0;
  rlcP->next_sdu_index    = 0;
  rlcP->current_sdu_index = 0;

  rlcP->output_sdu_size_to_write = 0;
  rlcP->buffer_occupancy  = 0;

  if (srb_flagP)
  {
  	rlcP->is_data_plane = 1;
  }
  else 
  {
	rlcP->is_data_plane = 0; 
  }
  rlcP->rb_id = rb_idP;
  rlcP->channel_id = chan_id; 

  
  // SPARE : not 3GPP
  rlcP->size_input_sdus_buffer = 16; //!TM 最大16个buffer
  if ((rlcP->input_sdus_alloc == NULL) && (rlcP->size_input_sdus_buffer > 0)) {
    rlcP->input_sdus_alloc = get_free_mem_block (rlcP->size_input_sdus_buffer * sizeof (void *), __func__);
    if(rlcP->input_sdus_alloc == NULL) return;
	
    rlcP->allocation = TRUE; 
    rlcP->sdu_allocated_buffer_size = rlcP->size_input_sdus_buffer * sizeof (void *); 
    rlcP->input_sdus = (mem_block_t **) (rlcP->input_sdus_alloc->data); //！input buffer 地址
    memset (rlcP->input_sdus, 0, rlcP->size_input_sdus_buffer * sizeof (void *)); //！清0
  }
}


void config_req_rlc_tm (
							  const protocol_ctxt_t* const  ctxt_pP,
							  const srb_flag_t  srb_flagP,
							  const rlc_tm_info_t * const config_tmP,
							  const rb_id_t rb_idP,
							  const logical_chan_id_t chan_idP
							)
{
  rlc_union_t     *rlc_union_p  = NULL;
  rlc_tm_entity_t *rlc_p        = NULL;
  hash_key_t      key;  
  hashtable_rc_t   h_rc;

  key = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);
  h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);

  
  
  if (h_rc == HASH_TABLE_OK) {
  	pthread_mutex_lock(&(rlc_union_p->rlc_union_mtex)); 
    rlc_p = &rlc_union_p->rlc.tm;
    LOG_DEBUG(RLC, PROTOCOL_RLC_TM_CTXT_FMT" CONFIG_REQ (is_uplink_downlink=%d) RB %u\n",
          PROTOCOL_RLC_TM_CTXT_ARGS(ctxt_pP, rlc_p),
          config_tmP->is_uplink_downlink,
          rb_idP);

    rlc_tm_init(ctxt_pP, rlc_p,srb_flagP,rb_idP,chan_idP);
    rlc_p->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
    rlc_tm_configure(ctxt_pP, rlc_p, config_tmP->is_uplink_downlink);

    pthread_mutex_unlock(&(rlc_union_p->rlc_union_mtex)); 
  } else {
    LOG_ERROR(RLC, PROTOCOL_RLC_TM_CTXT_FMT" CONFIG_REQ RB %u RLC NOT FOUND\n",
          PROTOCOL_RLC_TM_CTXT_ARGS(ctxt_pP, rlc_p),
          rb_idP);
  }
}



void rlc_tm_cleanup (rlc_tm_entity_t * const rlcP)
{
  int             index;
  // TX SIDE
  list_free (&rlcP->pdus_to_mac_layer);

  if (rlcP->input_sdus_alloc) { //将申请的SDU buffer都释放掉
  //！一共16个buffer 
    for (index = 0; index < rlcP->size_input_sdus_buffer; index++) {
      if (rlcP->input_sdus[index]) {
        free_mem_block (rlcP->input_sdus[index], __func__);
      }
    }

    free_mem_block (rlcP->input_sdus_alloc, __func__);
    rlcP->input_sdus_alloc = NULL;
  }

  // RX SIDE
  if ((rlcP->output_sdu_in_construction)) {
    free_mem_block (rlcP->output_sdu_in_construction, __func__);
    rlcP->output_sdu_in_construction = NULL;
  }

  memset(rlcP, 0, sizeof(rlc_tm_entity_t));
}
 


void rlc_tm_configure(
  const protocol_ctxt_t* const  ctxt_pP,
  rlc_tm_entity_t * const rlcP,
  const boolean_t is_uplink_downlinkP)
{
  rlcP->is_uplink_downlink = is_uplink_downlinkP;  //！确认是TX 还是RX 的TM 
  rlcP->output_sdu_size_to_write = 0;
  rlcP->buffer_occupancy = 0;
  rlcP->nb_sdu = 0;
  rlcP->next_sdu_index = 0;
  rlcP->current_sdu_index = 0;
} 


void  rlc_tm_no_segment (const protocol_ctxt_t* const  ctxt_pP,
								rlc_tm_entity_t *const rlc_pP)
{
	mem_block_t 					*pdu_p				 = NULL;
	struct rlc_tm_tx_sdu_management *sdu_mngt_p 		 = NULL;
	struct rlc_tm_tx_pdu_management *pdu_mngt_p 		 = NULL;
	int 							 nb_pdu_to_transmit  = 1;
	uint32_t                         tm_sdu_size; 

	// only one SDU per TTI
	while ((rlc_pP->input_sdus[rlc_pP->current_sdu_index]) && (nb_pdu_to_transmit > 0)) {
		//!指向了memory的首地址
		sdu_mngt_p = ((struct rlc_tm_tx_sdu_management *) (rlc_pP->input_sdus[rlc_pP->current_sdu_index]->data));
		//!<从pool中free mem block ，并将其赋给pdu_p,如果释放失败，则return
		//！从pool中申请一块MAC PDU 的大小，PDU 之前加上一个Header rlc_tm_tx_data_pdu_struct + GUARD_CRC_LIH_SIZE;
		
        //!PDU size for TM SDU must >= sdu size 
        tm_sdu_size = sdu_mngt_p->sdu_size; 
		AssertFatal(rlc_pP->rlc_pdu_size >= tm_sdu_size, RLC,
				"RLC TM SDU: rnti:%d,frame:%d,subsfn:%d,PDU size:%d < SDU size:%d !\n",
				ctxt_pP->rnti,
				ctxt_pP->frame,
				ctxt_pP->subframe,
				rlc_pP->rlc_pdu_size,
				tm_sdu_size); 

		//！每次处理一个SDU 都要申请一个PDU memory		
		if (!(pdu_p = get_free_mem_block (((rlc_pP->rlc_pdu_size + 7) >> 3) + \
						sizeof (struct rlc_tm_tx_data_pdu_struct) + GUARD_CRC_LIH_SIZE, __func__))) {
			LOG_DEBUG(RLC, PROTOCOL_RLC_TM_CTXT_FMT"[SEGMENT] ERROR COULD NOT GET NEW PDU, EXIT\n",
				  PROTOCOL_RLC_TM_CTXT_ARGS(ctxt_pP, rlc_pP));
			return;
		}

		// SHOULD BE OPTIMIZED...SOON
		pdu_mngt_p = (struct rlc_tm_tx_pdu_management *) (pdu_p->data);
		memset (pdu_p->data, 0, sizeof (struct rlc_tm_tx_pdu_management));
		//!指向buffer中的data地址，加上一个偏移
		pdu_mngt_p->first_byte = (uint8_t*)&pdu_p->data[sizeof (struct rlc_tm_tx_data_pdu_struct)];

		//从sdu 的首地址 向pdu的首地址copy 数据，byte对齐的
		memcpy (pdu_mngt_p->first_byte, sdu_mngt_p->first_byte, ((rlc_pP->rlc_pdu_size + 7) >> 3));
		((struct mac_tb_req *) (pdu_p->data))->rlc = NULL; //!tx 发送时，等于NULL
		((struct mac_tb_req *) (pdu_p->data))->data_ptr = pdu_mngt_p->first_byte; //!数据首地址,TM模式下没有header，直接就是数据
		((struct mac_tb_req *) (pdu_p->data))->first_bit = 0;
		((struct mac_tb_req *) (pdu_p->data))->tb_size = rlc_pP->rlc_pdu_size ;

		//!将当前的pdu_p的指针加入到pdus_to_mac_layer 这个链表中去
		list_add_tail_eurecom (pdu_p, &rlc_pP->pdus_to_mac_layer);

		rlc_pP->buffer_occupancy -= (rlc_pP->rlc_pdu_size);	//!<RLC 层维护的buffer大小需要减去已经给MAC的大小
		
		free_mem_block (rlc_pP->input_sdus[rlc_pP->current_sdu_index], __func__);
		rlc_pP->input_sdus[rlc_pP->current_sdu_index] = NULL;

		rlc_pP->current_sdu_index = (rlc_pP->current_sdu_index + 1) % rlc_pP->size_input_sdus_buffer;
		rlc_pP->nb_sdu -= 1; 
	}
}


void  rlc_tm_data_req (
							  const protocol_ctxt_t* const	ctxt_pP,
							  void *const rlc_pP,
							  mem_block_t *const sdu_pP)
{
	rlc_tm_entity_t *rlc_p = (rlc_tm_entity_t *) rlc_pP;


	LOG_DEBUG(RLC, PROTOCOL_RLC_TM_CTXT_FMT" RLC_TM_DATA_REQ size %d Bytes, BO %ld , NB SDU %d current_sdu_index=%d \
			next_sdu_index=%d\n",
		 PROTOCOL_RLC_TM_CTXT_ARGS(ctxt_pP, rlc_p),
		 ((struct rlc_um_data_req *) (sdu_pP->data))->data_size,
		 rlc_p->buffer_occupancy,
		 rlc_p->nb_sdu,
		 rlc_p->current_sdu_index,
		 rlc_p->next_sdu_index);


	// not in 3GPP specification but the buffer may be full if not correctly configured
	if (rlc_p->input_sdus[rlc_p->next_sdu_index] == NULL) { //！下一个memory没有被占用
	((struct rlc_tm_tx_sdu_management *) (sdu_pP->data))->sdu_size = ((struct rlc_tm_data_req *) (sdu_pP->data))->data_size;
	//!这里有问题，tm的buffer_occupancy 增加时，sdu_size不应该>>3吧？ 
	rlc_p->buffer_occupancy += ((struct rlc_tm_tx_sdu_management *) (sdu_pP->data))->sdu_size;
	rlc_p->nb_sdu += 1;
	((struct rlc_tm_tx_sdu_management *) (sdu_pP->data))->first_byte = \
									 (uint8_t*)&sdu_pP->data[sizeof (struct rlc_tm_data_req_alloc)];
	rlc_p->input_sdus[rlc_p->next_sdu_index] = sdu_pP;
	rlc_p->next_sdu_index = (rlc_p->next_sdu_index + 1) % rlc_p->size_input_sdus_buffer;

	rlc_Set_Buffer_Status(ctxt_pP->rnti, RLC_MODE_TM,rlc_p->channel_id,
						((struct rlc_tm_tx_sdu_management *) (sdu_pP->data))->sdu_size);
						
	} else {
	free_mem_block (sdu_pP, __func__); //!否则找不到合适的Input buffer,则将其从Memblock中删除掉
	}
}



struct mac_data_req rlc_tm_mac_data_request ( const protocol_ctxt_t* const  ctxt_pP,
													  void * const rlc_pP,
													  tbs_size_t   tbs_size)
{
  rlc_tm_entity_t*    rlc_p = (rlc_tm_entity_t*) rlc_pP;
  struct mac_data_req data_req;

  rlc_p->rlc_pdu_size = tbs_size; //!update pdu size; 
  //！将SDU中的data copy到PDU中，tm模式下没有分段，直接copy 
  rlc_tm_no_segment (ctxt_pP, rlc_p);
  
  list_init (&data_req.data, NULL);  //！将mac_data_req.data这个链表初始化
   //！将pdus_to_mac_layer的list添加到data_req.data 这个list之后
  list_add_list (&rlc_p->pdus_to_mac_layer, &data_req.data);
  
  data_req.buffer_occupancy_in_bytes = rlc_p->buffer_occupancy;
  //！计算PDU 个数
 // data_req.buffer_occupancy_in_pdus = data_req.buffer_occupancy_in_bytes / rlc_p->rlc_pdu_size;
//  data_req.rlc_info.rlc_protocol_state = rlc_p->protocol_state; //更新状态

   //nb_elements 等同于tb 个数
  if (data_req.data.nb_elements > 0) {
    LOG_DEBUG(RLC, PROTOCOL_RLC_TM_CTXT_FMT" MAC_DATA_REQUEST %d TBs\n",
          PROTOCOL_RLC_TM_CTXT_ARGS(ctxt_pP, rlc_p),
          data_req.data.nb_elements);
  }

  

  return data_req;
} 
 
 
/**************************function******************************/
