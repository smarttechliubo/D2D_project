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
  const rb_id_t rb_idP
)
{
  int saved_allocation = rlcP->allocation;
  memset (rlcP, 0, sizeof (struct rlc_tm_entity));
  rlcP->allocation = saved_allocation;
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
  // SPARE : not 3GPP
  rlcP->size_input_sdus_buffer = 16; //!TM 最大16个buffer

  if ((rlcP->input_sdus_alloc == NULL) && (rlcP->size_input_sdus_buffer > 0)) {
    rlcP->input_sdus_alloc = get_free_mem_block (rlcP->size_input_sdus_buffer * sizeof (void *), __func__);
    if(rlcP->input_sdus_alloc == NULL) return;
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
    rlc_p = &rlc_union_p->rlc.tm;
    LOG_DEBUG(RLC, PROTOCOL_RLC_TM_CTXT_FMT" CONFIG_REQ (is_uplink_downlink=%d) RB %u\n",
          PROTOCOL_RLC_TM_CTXT_ARGS(ctxt_pP, rlc_p),
          config_tmP->is_uplink_downlink,
          rb_idP);

    rlc_tm_init(ctxt_pP, rlc_p,srb_flagP,rb_idP);
    rlc_p->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
    rlc_tm_configure(ctxt_pP, rlc_p, config_tmP->is_uplink_downlink);
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
 
 
 
/**************************function******************************/
