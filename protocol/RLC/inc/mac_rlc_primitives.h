/******************************************************************
 * @file  mac_rlc_primitives.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

#ifndef     MAC_RLC_PRIMITIVES_H
#define     MAC_RLC_PRIMITIVES_H
 
#include <typedef.h>
#include <list.h>
#include <rlc_type.h>
//#    include "LAYER2/MAC/defs.h"

#define MAC_DCCH                              0xCC
#define MAC_DTCH                              0xDC
//----------------------------------------------------------
// primitives
//----------------------------------------------------------
#define MAC_DATA_REQ                          0x01
#define MAC_DATA_IND                          0x02
#define MAC_STATUS_IND                        0x04
#define MAC_STATUS_RESP                       0x08
//----------------------------------------------------------
// primitives definition
//----------------------------------------------------------
#define UE_ID_TYPE_U_RNTI 0xFF
// should not be used ?
#define UE_ID_TYPE_C_RNTI 0xCC

#define MAC_TX_STATUS_SUCCESSFUL              0x0F
#define MAC_TX_STATUS_UNSUCCESSFUL            0xF0

#define MAC_HEADER_MAX_SIZE                   6


// Parameters:
// a)   data:
// -    it contains the RLC layer messages (RLC-PDU) to be transmitted, or the RLC layer messages
//    that have been received by the MAC sub-layer.
//
// b)   no_tb :
// -    indicates the number of transport blocks transmitted by the peer entity within the
//    transmission time interval, based on the TFI value.
//
// c)   buffer_occupancy :
// -    the parameter Buffer Occupancy (BO) indicates for each logical channel the amount of data
//    in number of bytes that is available for transmission and retransmission in RLC layer.
//    When MAC is connected to an AM RLC entity, control PDUs to be transmitted and RLC PDUs
//    outside the RLC Tx window shall also be included in the BO. RLC PDUs that have been
//    transmitted but not negatively acknowledged by the peer entity shall not be included in the BO.
//
// d)   rx_timing_deviation, TDD only:
// -    it contains the RX Timing Deviation as measured by the physical layer for the physical
//    resources carrying the data of the Message Unit. This parameter is optional and only for
//    Indication. It is needed for the transfer of the RX Timing Deviation measurement of RACH
//    transmissions carrying CCCH data to RRC.
//
// e)   no_pdu:
// -    specifies the number of PDUs that the RLC is permitted to transfer to MAC within a
//    transmission time interval.
//
// f)   pdu_size:
// -    specifies the size of PDU that can be transferred to MAC within a transmission time interval.
//
// g)   UE-ID Type Indicator:
// -    indicates the UE-ID type to be included in MAC for a DCCH when it is mapped onto a common
//    transport channel (i.e. FACH, RACH, DSCH in FDD or CPCH). On the UE side UE-ID Type Indicator
//    shall always be set to C-RNTI.
//
// h)   tx_status:
// -    when set to value "transmission unsuccessful" this parameter indicates to RLC that transmission
//    of an RLC PDU failed in the previous Transmission Time Interval, when set to value
//    "transmission successful" this parameter indicates to RLC that the requested RLC PDU(s)
//     has been submitted for transmission by the physical layer.
//
// i)   rlc_info
// -    indicates to MAC the configuration parameters that are critical to TFC selection depending
//    on its mode and the amount of data that could be transmitted at the next TTI. This primitive
//    is meant to insure that MAC can perform TFC selection.
struct rlc_entity_info {
  unsigned char              rlc_protocol_state;
};

struct mac_tx_tb_management {
  // BE CAREFULL TO KEEP THE SAME MAPPING FOR THE 6 FIELDS BELLOW AS FOR  struct mac_tb_req
  unsigned char             *data_ptr;
  unsigned char              first_bit;    // 0 if data starts on byte boundary(b7), 1 if b6, 2 if b5, etc.
  // Previously designed for interlayers optimizations, (avoid aligning on byte boundary)
  // but not used by L1 !, so extra cost for alignement in MAC.
  // L1H does not care of the field first bit any more, so in order to byte
  tb_size_t                  tb_size_in_bits;      
  // align the tb we have to know its size

  // for reporting tx status to upper layers
  void           *rlc;
  void            (*rlc_callback) (void *rlcP, unsigned short rlc_tb_type);
  unsigned short             rlc_tb_type;
  // index of the logical channel interface from which the tb was submitted
  unsigned short             log_ch_interface_src; 
	// worst case : tctf(5bits)+UE-Id type(2bits)+UE-Id(16 or 32bits)+C/T(4bits)=43bits max

  unsigned char              mac_header[MAC_HEADER_MAX_SIZE];      
};

struct mac_rx_tb_management {
  unsigned char             *data_ptr;
  tb_size_t                  tb_size;      // in bits
  boolean_t                  valid_checksum;
  unsigned char              first_bit;    // 0 if data starts on byte boundary(b7), 1 if b6, 2 if b5, etc
};

struct mac_tb_req {
  // BE CAREFULL TO KEEP THE SAME MAPPING FOR THE 6 FIELDS BELLOW AS FOR  struct mac_tx_tb_management
  //!用于不同模式下的data 首地址，在TM模式下，直接就是数据，而在UM模式下，还包含了header等信息，总的
  // 总的来说，就是PDU的首地址，不管PDU是TM PDU,UM PDU,AM PDU 
  unsigned char             *data_ptr;  //!用于不同模式下的data 首地址，
  
  unsigned char              first_bit;
  tb_size_t                  tb_size;
  // align the tb we have to know its size

  // for reporting tx status to upper layers
  void           *rlc;
  void            (*rlc_callback) (void *rlcP, unsigned short rlc_tb_type);
  unsigned short             rlc_tb_type;
  
  // worst case : tctf(5bits)+UE-Id type(2bits)+UE-Id(16 or 32bits)+C/T(4bits)=43bits max
  unsigned char              mac_header[MAC_HEADER_MAX_SIZE];      
};

// MAC-STATUS-Ind/Resp:
/*!\brief
MAC-STATUS-Ind primitive indicates to RLC for each logical channel the rate at which it may transfer data to MAC. 
Parameters
are the number of PDUs that can be transferred in each transmission time interval and the PDU size; it is possible 
that MAC
would use this primitive to indicate that it expects the current buffer occupancy of the addressed logical channel in 
order
to provide for optimised TFC selection on transport channels with long transmission time interval. At the UE, MAC-
STATUS-Ind
primitive is also used to indicate from MAC to RLC that MAC has requested data transmission by PHY (i.e. PHY-DATA-REQ 
has
been submitted, see Fig. 11.2.2.1), or that transmission of an RLC PDU on RACH or CPCH has failed due to exceeded 
preamble
ramping cycle counter.*/
struct mac_status_ind {
  unsigned short             no_pdu;       /*!< \brief PDUs number*/
  unsigned short             tx_status;    /*!< \brief status is successful or unsuccessful*/
};

/*!\brief
MAC-STATUS-Resp primitive enables RLC to acknowledge a MAC-STATUS-Ind. It is possible
that RLC would use this primitive to indicate that it has nothing to send or that it is
in a suspended state or to indicate the current buffer occupancy to MAC.
*/
struct mac_status_resp {
	 /*!< \brief the parameter Buffer Occupancy (BO) indicates for
	each logical channel the amount of data in number of bytes that is available for transmission and retransmission in
	RLC layer. */
  unsigned int          buffer_occupancy_in_bytes;
  unsigned short             buffer_occupancy_in_pdus;    /*!< xxx*/
  uint32_t             head_sdu_creation_time;
  uint32_t             head_sdu_remaining_size_to_send;
  unsigned char     head_sdu_is_segmented;
  struct rlc_entity_info rlc_info;             /*!< xxx*/
};

struct mac_tb_ind {
  unsigned char             *data_ptr;
  unsigned short             size;
  unsigned char              error_indication;
  unsigned char              first_bit;    // 0 if data starts on byte boundary(b7), 1 if b6, 2 if b5, etc
};


//! mac sub header 
typedef  struct  mac_subheader_s
{
	uint8_t  e;
	uint8_t  f; 
	uint16_t  lcid; 
	uint16_t length; 

}mac_datapdu_subheader; 




//---------------------
/*! \brief MAC-DATA-Req primitive is used to request that an upper layer PDU be sent using the
    procedures for the information transfer service;
*/
struct mac_data_req {
  list_t                   data;   /*!< contains the RLC layer messages (RLC-PDU) to be transmitted*/
  unsigned int             buffer_occupancy_in_bytes;  /*!< the parameter Buffer Occupancy (BO) indicates for each 
logical channel the amount of data
                                                   in number of bytes that is available for transmission and 
retransmission in RLC layer.
                                                   When MAC is connected to an AM RLC entity, control PDUs to be 
transmitted and RLC PDUs
                                                   outside the RLC Tx window shall also be included in the BO. RLC 
PDUs that have been
                                                   transmitted but not negatively acknowledged by the peer entity 
shall not be included in the BO.*/
  unsigned short             buffer_occupancy_in_pdus;   /*!< xxx*/
  struct rlc_entity_info rlc_info;            /*!< indicates to MAC the configuration parameters that are critical to 
TFC selection depending
               on its mode and the amount of data that could be transmitted at the next TTI. This primitive
               is meant to insure that MAC can perform TFC selection.*/
  unsigned char              ue_id_type_indicator;       /*!< indicates the UE-ID type to be included in MAC for a 
DCCH when it is mapped onto a common
                     transport channel (i.e. FACH, RACH, DSCH in FDD or CPCH). On the UE side UE-ID Type Indicator
                     shall always be set to C-RNTI.*/
};
//---------------------
/*! \brief  MAC-DATA-Ind primitive indicates the arrival of upper layer PDUs received within one
    transmission time interval by means of the information transfer service.*/
struct mac_data_ind {
  list_t                     data;   /*!  the RLC layer messages that have been received by the MAC sub-layer*/
  unsigned short             no_tb;
  unsigned short             tb_size;      // in bits
  //unsigned char              error_indication;
};


//---------------------

//---------------------
struct mac_primitive {
  unsigned char              primitive_type;
  union {
    struct mac_data_req data_req;
    struct mac_status_resp status_resp;
    struct mac_data_ind data_ind;
    //struct mac_status_ind  status_ind;
  } primitive;
};





/***********************************************function declare*******************************************/
extern struct mac_data_req rlc_tm_mac_data_request ( const protocol_ctxt_t* const  ctxt_pP,
													  void * const rlc_pP,
													  tbs_size_t   tbs_size);
 
 
#endif
