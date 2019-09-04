/******************************************************************
 * @file  rlc_tm_entity.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

#ifndef     RLC_TM_ENTITY_H
#define     RLC_TM_ENTITY_H
  
//-----------------------------------------------------------------------------
#include "typedef.h"

#include "rlc_tm_structs.h"
#include "rlc_def.h"
//-----------------------------------------------------------------------------
/*! \struct  rlc_tm_entity_t
* \brief Structure containing a RLC TM instance protocol variables, allocation variables, buffers and other 
miscellaneous variables.
*/
typedef struct rlc_tm_entity {
  boolean_t            allocation;         /*!< \brief Boolean for rlc_tm_entity_t struct allocation. */
  /*!< \brief Protocol state, can be RLC_NULL_STATE,RLC_DATA_TRANSFER_READY_STATE, RLC_LOCAL_SUSPEND_STATE. */
  rlc_protocol_state_t protocol_state;
  boolean_t            is_uplink_downlink; /*!< \brief Is this instance is a transmitter, a receiver or both? */
  /*!< \brief To know if the RLC belongs to a data radio bearer or a signalling radio bearer, for statistics and trace purpose. */
  boolean_t            is_data_plane;
  // for stats and trace purpose :
  logical_chan_id_t    channel_id;         /*!< \brief Transport channel identifier. */
  rb_id_t              rb_id;              /*!< \brief Radio bearer identifier, for statistics and trace purpose. */
  //boolean_t            is_enb;             /*!< \brief To know if the RLC belongs to a eNB or UE. */
  //-----------------------------
  // tranmission
  //-----------------------------
  // sdu communication;
  /*!< \brief Input SDU buffer (for SDUs coming from upper layers). Should be accessed as an array. */
  mem_block_t     **input_sdus;
  /*!< \brief Allocated memory for the input SDU buffer (for SDUs coming from upper layers). */
  mem_block_t      *input_sdus_alloc;
  uint16_t             size_input_sdus_buffer;  /*!< \brief Size of the input SDU buffer. */
  uint16_t             nb_sdu;                  /*!< \brief Total number of SDUs in input_sdus[] */
  uint16_t             next_sdu_index;          /*!< \brief Next SDU index for a new incomin SDU in input_sdus[]. */
  uint16_t             current_sdu_index;       /*!< \brief Current SDU index in input_sdus array to be segmented. */
  list_t            pdus_to_mac_layer;       /*!< \brief PDUs buffered for transmission to MAC layer. */
  sdu_size_t        rlc_pdu_size;
  rlc_buffer_occupancy_t buffer_occupancy;        /*!< \brief Number of bytes contained in input_sdus buffer.*/
  //-----------------------------
  // receiver
  //-----------------------------
  unsigned int      output_sdu_size_to_write;     /*!< \brief Size of the reassemblied SDU. */
  /*!< \brief Memory area where a complete SDU is reassemblied before being send to upper layers. */
  mem_block_t      *output_sdu_in_construction;
} rlc_tm_entity_t;

 
 
 
#endif
