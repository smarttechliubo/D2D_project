/******************************************************************
 * @file  rlc_um_receive.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月10日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月10日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc.h>



void rlc_um_init_timer_reordering(
  const protocol_ctxt_t* const ctxt_pP,
  rlc_um_entity_t * const rlc_pP,
  const uint32_t  ms_durationP)
{
  rlc_pP->t_reordering.running         = 0;
  rlc_pP->t_reordering.ms_time_out     = 0;
  rlc_pP->t_reordering.ms_start        = 0;
  rlc_pP->t_reordering.ms_duration     = ms_durationP;
  rlc_pP->t_reordering.timed_out       = 0;
}

 
 
 
 
/**************************function******************************/
