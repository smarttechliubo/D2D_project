/******************************************************************
 * @file  rlc_um.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

#ifndef     RLC_UM_H
#define     RLC_UM_H
 
#include <errno.h>
#include "typedef.h"
#include "rlc_def.h"
#include "rlc_def_lte.h"
#include "rlc_um_constants.h"
#include "rlc_um_structs.h"
#include "rlc_um_entity.h"
#include "mem_block.h"
#include "rlc_um_control_primitives.h"
#include "rlc.h"


#define PROTOCOL_RLC_UM_CTXT_FMT PROTOCOL_CTXT_FMT"[%s %02u]"

#define PROTOCOL_RLC_UM_CTXT_ARGS(CTXT_Pp, rLC_Pp) PROTOCOL_CTXT_ARGS(CTXT_Pp),\
          (rLC_Pp->is_data_plane) ? "DRB UM" : "SRB UM",\
          rLC_Pp->rb_id

          
#define PROTOCOL_RLC_UM_MSC_FMT "[RNTI %"PRIx16" %s %02u]"
#define PROTOCOL_RLC_UM_MSC_ARGS(CTXT_Pp, rLC_Pp) \
        CTXT_Pp->rnti,\
          (rLC_Pp->is_data_plane) ? "DRB UM" : "SRB UM",\
          rLC_Pp->rb_id

#if defined(TRACE_RLC_MUTEX)
#define RLC_UM_MUTEX_LOCK(mUTEX, cTXT, rLC) \
	do {\
      int pmtl_rc = pthread_mutex_trylock(mUTEX);\
	  if (pmtl_rc != 0){\
        if (pmtl_rc == EBUSY) {\
          MSC_LOG_EVENT((cTXT->enb_flag == ENB_FLAG_YES) ? MSC_RLC_ENB:MSC_RLC_UE,\
                       "0 "PROTOCOL_RLC_UM_MSC_FMT" Warning try lock %s busy",\
                       PROTOCOL_RLC_UM_MSC_ARGS(cTXT,rLC),\
                       #mUTEX);\
          pthread_mutex_lock(mUTEX);\
        } else {\
            MSC_LOG_EVENT((cTXT->enb_flag == ENB_FLAG_YES) ? MSC_RLC_ENB:MSC_RLC_UE,\
                       "0 "PROTOCOL_RLC_UM_MSC_FMT" Error try lock %s %d",\
                       PROTOCOL_RLC_UM_MSC_ARGS(cTXT,rLC),\
                       #mUTEX, pmtl_rc);\
        }\
      }\
	} while (0);
#else
#define RLC_UM_MUTEX_LOCK(mUTEX, cTXT, rLC) pthread_mutex_lock(mUTEX)
#endif

#define RLC_UM_MUTEX_UNLOCK(mUTEX) pthread_mutex_unlock(mUTEX)

/*! \fn void     rlc_um_stat_req     (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t * const rlc_pP,
                        unsigned int* stat_tx_pdcp_sdu,
                        unsigned int* stat_tx_pdcp_bytes,
                        unsigned int* stat_tx_pdcp_sdu_discarded,
                        unsigned int* stat_tx_pdcp_bytes_discarded,
                        unsigned int* stat_tx_data_pdu,
                        unsigned int* stat_tx_data_bytes,
                        unsigned int* stat_rx_pdcp_sdu,
                        unsigned int* stat_rx_pdcp_bytes,
                        unsigned int* stat_rx_data_pdus_duplicate,
                        unsigned int* stat_rx_data_bytes_duplicate,
                        unsigned int* stat_rx_data_pdu,
                        unsigned int* stat_rx_data_bytes,
                        unsigned int* stat_rx_data_pdu_dropped,
                        unsigned int* stat_rx_data_bytes_dropped,
                        unsigned int* stat_rx_data_pdu_out_of_window,
                        unsigned int* stat_rx_data_bytes_out_of_window,
                        unsigned int* stat_timer_reordering_timed_out)
* \brief    Request TX and RX statistics of a RLC AM protocol instance.
* \param[in]  ctxt_pP                              Running context.
* \param[in]  rlc_pP                               RLC UM protocol instance pointer.
* \param[out] stat_tx_pdcp_sdu                     Number of SDUs coming from upper layers.
* \param[out] stat_tx_pdcp_bytes                   Number of bytes coming from upper layers.
* \param[out] stat_tx_pdcp_sdu_discarded           Number of discarded SDUs coming from upper layers.
* \param[out] stat_tx_pdcp_bytes_discarded         Number of discarded bytes coming from upper layers.
* \param[out] stat_tx_data_pdu                     Number of transmitted data PDUs to lower layers.
* \param[out] stat_tx_data_bytes                   Number of transmitted data bytes to lower layers.
* \param[out] stat_rx_pdcp_sdu                     Number of SDUs delivered to upper layers.
* \param[out] stat_rx_pdcp_bytes                   Number of bytes delivered to upper layers.
* \param[out] stat_rx_data_pdus_duplicate          Number of duplicate PDUs received.
* \param[out] stat_rx_data_bytes_duplicate         Number of duplicate bytes received.
* \param[out] stat_rx_data_pdu                     Number of received PDUs from lower layers.
* \param[out] stat_rx_data_bytes                   Number of received bytes from lower layers.
* \param[out] stat_rx_data_pdu_dropped             Number of received PDUs from lower layers, then dropped.
* \param[out] stat_rx_data_bytes_dropped           Number of received bytes from lower layers, then dropped.
* \param[out] stat_rx_data_pdu_out_of_window       Number of data PDUs received out of the receive window.
* \param[out] stat_rx_data_bytes_out_of_window     Number of data bytes received out of the receive window.
* \param[out] stat_timer_reordering_timed_out      Number of times the timer "reordering" has timed-out.
*/
void rlc_um_stat_req     (rlc_um_entity_t *rlc_pP,
                          unsigned int* stat_tx_pdcp_sdu,
                          unsigned int* stat_tx_pdcp_bytes,
                          unsigned int* stat_tx_pdcp_sdu_discarded,
                          unsigned int* stat_tx_pdcp_bytes_discarded,
                          unsigned int* stat_tx_data_pdu,
                          unsigned int* stat_tx_data_bytes,
                          unsigned int* stat_rx_pdcp_sdu,
                          unsigned int* stat_rx_pdcp_bytes,
                          unsigned int* stat_rx_data_pdus_duplicate,
                          unsigned int* stat_rx_data_bytes_duplicate,
                          unsigned int* stat_rx_data_pdu,
                          unsigned int* stat_rx_data_bytes,
                          unsigned int* stat_rx_data_pdu_dropped,
                          unsigned int* stat_rx_data_bytes_dropped,
                          unsigned int* stat_rx_data_pdu_out_of_window,
                          unsigned int* stat_rx_data_bytes_out_of_window,
                          unsigned int* stat_timer_reordering_timed_out);

/*! \fn void     rlc_um_get_pdus (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t * const rlc_pP)
* \brief    Request the segmentation of SDUs based on status previously sent by MAC.
* \param[in]  ctxt_pP                   Running context.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
*/
extern int32_t rlc_um_get_pdus (const protocol_ctxt_t *const ctxt_pP, void *argP,
								logic_channel_pdu_component  *lc_pdu_component_ptr,
								mac_pdu_size_para  *ue_mac_pdu_size_ptr) ;
/*! \fn void rlc_um_rx (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t * const rlc_pP, struct mac_data_ind 
data_indication)
* \brief    Process the received PDUs from lower layer.
* \param[in]  ctxt_pP                   Running context.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
* \param[in]  data_indication           PDUs from MAC.
*/
void
rlc_um_rx (const protocol_ctxt_t* const ctxt_pP, void *argP, struct mac_data_ind data_indP);
/*! \fn struct mac_status_resp rlc_um_mac_status_indication (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t * 
const rlc_pP, uint16_t tbs_sizeP, struct mac_status_ind tx_statusP)
* \brief    Request the maximum number of bytes that can be served by RLC instance to MAC and fix the amount of bytes 
requested by MAC for next RLC transmission.
* \param[in]  ctxt_pP                   Running context.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
* \param[in]  tbs_sizeP                 Number of bytes requested by MAC for next transmission.
* \param[in]  tx_statusP                Transmission status given by MAC on previous MAC transmission of the PDU.
* \return     The maximum number of bytes that can be served by RLC instance to MAC.
*/
struct mac_status_resp
rlc_um_mac_status_indication (const protocol_ctxt_t* const ctxt_pP, void *rlc_pP, uint16_t tbs_sizeP, struct 
mac_status_ind tx_statusP,const eNB_flag_t enb_flagP);

/*! \fn void     rlc_um_set_nb_bytes_requested_by_mac (rlc_um_entity_t * const rlc_pP, const tb_size_t		tb_sizeP)
* \brief    Set available TBS size for MAC Tx.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
* \param[in]  tb_sizeP                 remaining TBS in bytes.
*/
void
rlc_um_set_nb_bytes_requested_by_mac (
  void *                                rlc_pP,
  const tb_size_t               tb_sizeP
);


extern int32_t	rlc_um_mac_data_request(const protocol_ctxt_t *const ctxt_pP, 
                                                    const tb_size_t   tb_sizeP,
													void *rlc_pP,
													const eNB_flag_t  enb_flagP,
													logic_channel_pdu_component  *lc_pdu_component_ptr,
													mac_pdu_size_para  *ue_mac_pdu_size_ptr,
													struct mac_data_req *data_req) ;

extern int32_t  rlc_um_get_pdus (const protocol_ctxt_t *const ctxt_pP, void *argP,
							logic_channel_pdu_component  *lc_pdu_component_ptr,
							mac_pdu_size_para  *ue_mac_pdu_size_ptr) ;

/*! \fn void     rlc_um_mac_data_indication (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t * const rlc_pP,
struct mac_data_ind data_indP)
* \brief    Receive PDUs from lower layer MAC.
* \param[in]  ctxt_pP                   Running context.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
* \param[in]  data_indP                 PDUs from MAC.
*/

/*! \fn uint32_t rlc_um_get_buffer_occupancy (rlc_um_entity_t *rlc_pP)
* \brief    Gets Tx Buffer Occupancy.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.)
*/
uint32_t rlc_um_get_buffer_occupancy (rlc_um_entity_t *rlc_pP);

/*! \fn void     rlc_um_data_req (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t * const rlc_pP, mem_block_t *
sduP)
* \brief    Interface with higher layers, buffer higher layer SDUS for transmission.
* \param[in]  ctxt_pP                   Running context.
* \param[in]  rlc_pP                    RLC UM protocol instance pointer.
* \param[in]  sduP                      SDU. (A struct rlc_um_data_req is mapped on sduP->data.)
*/
void rlc_um_mac_data_indication (const protocol_ctxt_t* const ctxt_pP, void *rlc_pP, struct mac_data_ind data_indP);
void rlc_um_data_req (const protocol_ctxt_t *const ctxt_pP, void *rlc_pP, mem_block_t *sdu_pP) ;
/** @} */

extern  int32_t  rlc_um_segment_10(const protocol_ctxt_t* const ctxt_pP, 
							    rlc_um_entity_t *rlc_pP,
							    logic_channel_pdu_component  *lc_pdu_component_ptr,
							    mac_pdu_size_para  *ue_mac_pdu_size_ptr);
 
 
#endif
