/******************************************************************
 * @file  rlc_tm_structs.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月4日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月4日        bo.liu       create file

*****************************************************************/

#ifndef     RLC_TM_STRUCTS_H
#define     RLC_TM_STRUCTS_H
 

#include  "typedef.h"
#include  "list.h"
#include  "mac_rlc_primitives.h"
#include <rlc_tm.h>
//-----------------------

struct rlc_tm_data_req {
  uint16_t             data_size;    // in bits
  //// beginning of payload data may not start just after the header (PDCP header compression, etc)
  uint16_t             data_offset;  
};


struct rlc_tm_tx_sdu_management {
  uint8_t             *first_byte;
  int32_t             sdu_creation_time;
  uint16_t             sdu_size;
};
//-----------------------
struct rlc_tm_tx_pdu_management {
  uint8_t             *first_byte;
  uint8_t              dummy[MAC_HEADER_MAX_SIZE];
};
//-----------------------
struct rlc_tm_rx_pdu_management {
  uint8_t             *first_byte;
};
//-----------------------
struct rlc_tm_tx_data_pdu_struct {
  union {
    struct rlc_tm_tx_pdu_management tx_pdu_mngmnt;
    struct mac_tb_req tb_req;
    struct mac_tx_tb_management tb_mngt;
#        ifdef BYPASS_L1
    struct rlc_tm_rx_pdu_management dummy1;
    struct mac_tb_ind dummy2;
    struct mac_rx_tb_management dummy3;
    struct rlc_indication dummy4;
#        endif
  } dummy;
  uint8_t              data[1];
};
//-----------------------
struct rlc_tm_data_req_alloc {  // alloc enought bytes for sdu mngt also
  union {
    struct rlc_tm_data_req dummy1;
    struct rlc_tm_tx_sdu_management dummy2;
  } dummy;
};

 
 
 
 
#endif
