/******************************************************************
 * @file  rlc_type.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月4日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月4日        bo.liu       create file

*****************************************************************/

#ifndef     RLC_TYPE_H
#define     RLC_TYPE_H
 
 
typedef int16_t            rlc_sn_t;
typedef uint16_t           rlc_usn_t;
typedef int32_t            rlc_buffer_occupancy_t;
typedef signed int         rlc_op_status_t;


typedef int32_t               sdu_size_t;
typedef int32_t               sframe_t;
typedef uint16_t              module_id_t;
typedef uint8_t               slice_id_t;
typedef uint8_t               eNB_index_t;
typedef uint16_t              ue_id_t;
typedef int16_t               smodule_id_t;
typedef uint16_t              rb_id_t;
typedef uint16_t              srb_id_t;
typedef unsigned int          mui_t;
typedef unsigned int       logical_chan_id_t;
typedef sdu_size_t         tb_size_t;

typedef unsigned int       rlc_tx_status_t;


#ifndef _BOOLEAN_T_DEFINED_
  #define _BOOLEAN_T_DEFINED_

  typedef signed char        boolean_t;

  #ifndef TRUE
    #define TRUE               (boolean_t)0x01
  #endif

  #ifndef FALSE
    #define FALSE              (boolean_t)0x00
  #endif

  #define BOOL_NOT(b) (b^TRUE)

#endif /* _BOOLEAN_T_DEFINED_ */


typedef boolean_t             MBMS_flag_t;
#define  MBMS_FLAG_NO         FALSE
#define  MBMS_FLAG_YES        TRUE

typedef boolean_t             eNB_flag_t;
#define  ENB_FLAG_NO          FALSE
#define  ENB_FLAG_YES         TRUE

typedef boolean_t             srb_flag_t;
#define  SRB_FLAG_NO          FALSE
#define  SRB_FLAG_YES         TRUE





typedef struct protocol_ctxt_s {
  module_id_t module_id;     /*!< \brief  Virtualized module identifier      */
  eNB_flag_t  enb_flag;      /*!< \brief  Flag to indicate eNB (1) or UE (0) */
  instance_t  instance;      /*!< \brief  ITTI or OTG module identifier      */
  rnti_t      rnti;
  frame_t     frame;         /*!< \brief  LTE frame number.*/
  sub_frame_t subframe;      /*!< \brief  LTE sub frame number.*/
  eNB_index_t eNB_index;     /*!< \brief  valid for UE indicating the index of connected eNB(s)      */
  boolean_t   configured;  /*!< \brief  flag indicating whether the instance is configured or not  */
} protocol_ctxt_t;


typedef enum config_action_e {
  CONFIG_ACTION_NULL              = 0,
  CONFIG_ACTION_ADD               = 1,
  CONFIG_ACTION_REMOVE            = 2,
  CONFIG_ACTION_MODIFY            = 3,
  CONFIG_ACTION_SET_SECURITY_MODE = 4,
  CONFIG_ACTION_MBMS_ADD          = 10,
  CONFIG_ACTION_MBMS_MODIFY       = 11
} config_action_t;

 
 
 
#endif
