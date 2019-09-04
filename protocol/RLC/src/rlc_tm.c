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
#include <rlc_tm_entity.h>


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
 
 
 
 
 
/**************************function******************************/
