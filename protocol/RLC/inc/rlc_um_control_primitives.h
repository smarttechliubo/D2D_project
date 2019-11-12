/******************************************************************
 * @file  rlc_um_control_primitives.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

#ifndef     RLC_UM_CONTROL_PRIMITIVES_H
#define     RLC_UM_CONTROL_PRIMITIVES_H
 

#include <mem_block.h>
#include <typedef.h>
#include <rlc_type.h>
#include <rlc_um_entity.h>

//-----------------------------------------------------------------------------

typedef volatile struct {
  uint32_t             is_uplink_downlink;
  uint32_t             timer_reordering;
  uint32_t             sn_field_length; // 5 or 10
  uint32_t             is_mXch; // boolean, true if configured for MTCH or MCCH
} rlc_um_info_t;


/*! \fn void config_req_rlc_um (
 * const protocol_ctxt_t* const ctxt_pP,
 * const srb_flag_t srb_flagP,
 * const rlc_um_info_t * const config_umP,
 * const rb_id_t rb_idP)
* \brief    Allocate memory for RLC UM instance, reset protocol variables, and set protocol parameters. After this 
configuration the RLC UM protocol instance will be in RLC_DATA_TRANSFER_READY_STATE state.
* \param[in]  ctxt_pP                   Running context.
* \param[in]  srb_flagP                 Flag to indicate signalling radio bearer (1) or data radio bearer (0).
* \param[in]  config_umP                Configuration parameters for RLC UM instance.
* \param[in]  rb_idP                    Radio bearer identifier.
* \param[in]  chan_idP                  Transport channel identifier.
*/
void config_req_rlc_um (
                                      const protocol_ctxt_t* const ctxt_pP,
                                      const srb_flag_t srb_flagP,
                                      const rlc_um_info_t * const config_umP,
                                      const rb_id_t rb_idP,
                                      const logical_chan_id_t chan_idP); 






/*! \fn void rlc_um_reset_state_variables (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t * const rlc_pP)
* \brief    Reset protocol variables and state variables to initial values.
* \param[in]  ctxtP                     Running context.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
*/
void rlc_um_reset_state_variables (
                                      const protocol_ctxt_t* const ctxt_pP,
                                      rlc_um_entity_t * const rlc_pP);

/*! \fn void rlc_um_cleanup(rlc_um_entity_t * const rlc_pP)
* \brief    Free all allocated memory (lists and buffers) previously allocated by this RLC UM instance.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
*/
void rlc_um_cleanup(
                                      rlc_um_entity_t * const rlc_pP);



/*! \fn void rlc_um_set_debug_infos(
*     const protocol_ctxt_t* const ctxt_pP,
*     rlc_um_entity_t * const rlc_pP,
*     const srb_flag_t  srb_flagP,
*     const rb_id_t     rb_idP)
* \brief    Set debug informations for a RLC UM protocol instance, these informations are only for trace purpose.
* \param[in]  ctxtP                     Running context.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
* \param[in]  srb_flagP                 Flag to indicate signalling radio bearer (1) or data radio bearer (0).
* \param[in]  rb_idP                    Radio bearer identifier.
* \param[in]  chan_idP                  Transport channel identifier.
*/
void rlc_um_set_debug_infos(
                                      const protocol_ctxt_t* const ctxt_pP,
                                      rlc_um_entity_t * const rlc_pP,
                                      const srb_flag_t srb_flagP,
                                      const rb_id_t rb_idP,
                                      const logical_chan_id_t chan_idP); 
/** @} */


 
 
#endif
