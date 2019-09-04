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
#include <rlc_um_entity.h>
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
 
 
 
 
/**************************function******************************/
