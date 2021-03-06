/******************************************************************
 * @file  rlc.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

#ifndef     RLC_H
#define     RLC_H
 
#include <inttypes.h>
#include <stdint.h>
#include "typedef.h"
#include <rlc_type.h>
#include <hashtable.h>
#include <log.h>
#include <interface_rrc_rlc.h>
#include <interface_mac_rlc.h>
#include <rlc_tm_init.h>
#include <rlc_um_control_primitives.h>
#include <rlc_um_entity.h>
#include <rlc_um.h>
#include <rlc_tm_entity.h>
#include <pthread.h>
#include <dictionary.h>
#include <mac_vars.h>







//#define  AM_ENABLE   1

#define MAX_NUM_DLSCH_SEGMENTS 16
#define MAX_NUM_ULSCH_SEGMENTS MAX_NUM_DLSCH_SEGMENTS
#define MAX_DLSCH_PAYLOAD_BYTES (MAX_NUM_DLSCH_SEGMENTS*768)
#define MAX_ULSCH_PAYLOAD_BYTES (MAX_NUM_ULSCH_SEGMENTS*768)


//-----------------------------------------------------------------------------
#define  RLC_OP_STATUS_OK                1
#define  RLC_OP_STATUS_BAD_PARAMETER     22
#define  RLC_OP_STATUS_INTERNAL_ERROR    2
#define  RLC_OP_STATUS_OUT_OF_RESSOURCES 3

#define  RLC_MUI_UNDEFINED     (mui_t)0

#define  RLC_RB_UNALLOCATED    (rb_id_t)0
#define  RLC_LC_UNALLOCATED    (logical_chan_id_t)0
//! 采用字符串串接的方式组成printf的格式符 
#define PROTOCOL_CTXT_FMT        "[FRAME %05u][SUBFRAME %05u][%s][MOD %02u][RNTI %d]"   //!# define PRIx64  __PRI64_PREFIX "x"
#define PROTOCOL_CTXT_ARGS(CTXT_Pp) \
  (CTXT_Pp)->frame, \
  (CTXT_Pp)->subframe,   \
  ((CTXT_Pp)->enb_flag == ENB_FLAG_YES) ? "eNB":" UE", \
  (CTXT_Pp)->module_id, \
  (CTXT_Pp)->rnti 


#define PROTOCOL_CTXT_SET_BY_MODULE_ID(Ctxt_Pp, mODULE_iD, eNB_fLAG, rNTI, fRAME, sUBfRAME, eNB_iNDEX) \
  (Ctxt_Pp)->module_id = mODULE_iD; \
  (Ctxt_Pp)->enb_flag  = eNB_fLAG; \
  (Ctxt_Pp)->rnti      = rNTI; \
  (Ctxt_Pp)->frame     = fRAME; \
  (Ctxt_Pp)->subframe  = sUBfRAME; \
  (Ctxt_Pp)->eNB_index  = eNB_iNDEX; \
  
#define  SUBSFN_MS_TIME   800 
#define  FRAME_SUBSFN_TIME  4 
#define PROTOCOL_CTXT_TIME_MILLI_SECONDS(CtXt_h) ((Get_Frame() *FRAME_SUBSFN_TIME+ Get_Subsfn()) * SUBSFN_MS_TIME)


typedef enum rlc_confirm_e {
  RLC_SDU_CONFIRM_NO    = 0,
  RLC_SDU_CONFIRM_YES   = 1,
} rlc_confirm_t;


/*! \struct  rlc_info_t
* \brief Structure containing RLC protocol configuration parameters.
*/
typedef volatile struct {
  rlc_mode_e             rlc_mode;
  union {
   //! rlc_am_info_t              rlc_am_info; /*!< \sa rlc_am.h. */
    rlc_tm_info_t              rlc_tm_info; /*!< \sa rlc_tm.h. */
    rlc_um_info_t              rlc_um_info; /*!< \sa rlc_um.h. */
  } rlc;
} rlc_info_t;

/*! \struct  mac_rlc_status_resp_t
* \brief Primitive exchanged between RLC and MAC informing about the buffer occupancy of the RLC protocol instance.
*/
typedef  struct {
  rlc_buffer_occupancy_t       bytes_in_buffer; /*!< \brief Bytes buffered in RLC protocol instance. */
  rlc_buffer_occupancy_t       pdus_in_buffer;  /*!< \brief Number of PDUs buffered in RLC protocol instance (OBSOLETE
). */
  frame_t                      head_sdu_creation_time;           /*!< \brief Head SDU creation time. */
  sdu_size_t                   head_sdu_remaining_size_to_send;  /*!< \brief remaining size of sdu: could be the 
total size or the remaining size of already segmented sdu */
  boolean_t                    head_sdu_is_segmented;     /*!< \brief 0 if head SDU has not been segmented, 1 if 
already segmented */
} mac_rlc_status_resp_t;

#if 0
/*! \struct  mac_rlc_max_rx_header_size_t
* \brief Usefull only for debug scenario where we connect 2 RLC protocol instances without the help of the MAC .
*/
typedef struct {
  union {
  //!  struct rlc_am_rx_pdu_management dummy1;
  //!  struct rlc_tm_rx_pdu_management dummy2;
    //struct rlc_um_rx_pdu_management dummy3;
    struct mac_tb_ind dummy4;
    struct mac_rx_tb_management dummy5;
  } dummy;
} mac_rlc_max_rx_header_size_t;   //!< MAC->RLC的数据头定义

#endif

//-----------------------------------------------------------------------------
//   PRIVATE INTERNALS OF RLC
//-----------------------------------------------------------------------------
#if 0
#define  RLC_MAX_MBMS_LC (LTE_maxSessionPerPMCH * LTE_maxServiceCount)
#define  RLC_MAX_LC  ((max_val_LTE_DRB_Identity+1)* MAX_MOBILES_PER_ENB)
#endif

void (*rlc_rrc_data_ind)(
                const protocol_ctxt_t* const ctxtP,
                const rb_id_t     rb_idP,
                const sdu_size_t  sdu_sizeP,
                const uint8_t   * const sduP);

void (*rlc_rrc_data_conf)(
                const protocol_ctxt_t* const ctxtP,
                const rb_id_t         rb_idP,
                const mui_t           muiP,
                const rlc_tx_status_t statusP);

typedef void (rrc_data_ind_cb_t)(
  const protocol_ctxt_t* const ctxtP,
  const rb_id_t     rb_idP,
  const sdu_size_t  sdu_sizeP,
  const uint8_t   * const sduP);

typedef void (rrc_data_conf_cb_t)(
  const protocol_ctxt_t* const ctxtP,
  const rb_id_t         rb_idP,
  const mui_t           muiP,
  const rlc_tx_status_t statusP);


/*! \struct  rlc_t
* \brief Structure to be instanciated to allocate memory for RLC protocol instances.
*/
typedef struct rlc_union_s {
  rlc_mode_e           mode;
  pthread_mutex_t      rlc_union_mtex; 
  union {
    //rlc_am_entity_t  am;
    rlc_um_entity_t  um;
    rlc_tm_entity_t  tm;
  } rlc;
} rlc_union_t;


typedef struct rlc_am_e_li {
  uint8_t  b1; /*!< \brief 1st byte. */
  uint8_t  b2; /*!< \brief 2nd byte. */
  uint8_t  b3; /*!< \brief 3rd byte. */
} rlc_am_e_li_t;



typedef struct ue_srb_info_s {
	uint16_t    srb_rb_id; 
	uint16_t    srb_logic_ch_id; 
	rlc_mode_e  rlc_mode;
 
}ue_srb_info; 

typedef struct ue_drb_info_s {

	uint16_t	drb_index; 
	uint16_t    drb_rb_id; 
	uint16_t    drb_logic_ch_id; 
	rlc_mode_e  rlc_mode;
 

}ue_drb_info; 



typedef struct rlc_ue_info_s{
	uint16_t ue_rnti; 
	uint16_t srb_setup_flag; 
	uint16_t drb_setup_flag;
	uint16_t srb_count; 
	uint16_t drb_count; 
	uint16_t pad; 
	
	ue_srb_info  srb_info[MAX_SRB_COUNT];
	ue_drb_info  drb_info[MAX_DRB_COUNT];

}rlc_ue_info; 




#define RLC_COLL_KEY_VALUE(eNB_iD, rNTI, iS_eNB, rB_iD, iS_sRB) \
   ((hash_key_t)eNB_iD             | \
    (((hash_key_t)(rNTI))   << 8)  | \
    (((hash_key_t)(iS_eNB)) << 24) | \
    (((hash_key_t)(rB_iD))  << 25) | \
    (((hash_key_t)(iS_sRB)) << 33) | \
    (((hash_key_t)(0x05))   << 34))

// index to the same RLC entity as RLC_COLL_KEY_VALUE(), but using LC_id instead
// the hidden last key indicates if this is a hash-key with RB_id (0x05) or LC_id (0x0a)
#define RLC_COLL_KEY_LCID_VALUE(eNB_iD, rNTI, iS_eNB, lC_iD, iS_sRB) \
   ((hash_key_t)eNB_iD             | \
    (((hash_key_t)(rNTI))   << 8)  | \
    (((hash_key_t)(iS_eNB)) << 24) | \
    (((hash_key_t)(lC_iD))  << 25) | \
    (((hash_key_t)(iS_sRB)) << 33) | \
    (((hash_key_t)(0x0a))   << 34))

#define RLC_COLL_KEY_SOURCE_DEST_VALUE(eNB_iD, rNTI, iS_eNB, lC_iD, sOURCE_iD, dEST_iD, iS_sRB) \
   ((hash_key_t)eNB_iD             | \
    (((hash_key_t)(rNTI))   << 8)  | \
    (((hash_key_t)(iS_eNB)) << 24) | \
    (((hash_key_t)(lC_iD))  << 25) | \
    (((hash_key_t)(dEST_iD)) << 33) | \
    (((hash_key_t)(0x05))   << 57))

#define RLC_COLL_KEY_LCID_SOURCE_DEST_VALUE(eNB_iD, rNTI, iS_eNB, lC_iD, sOURCE_iD, dEST_iD, iS_sRB) \
   ((hash_key_t)eNB_iD             | \
    (((hash_key_t)(rNTI))   << 8)  | \
    (((hash_key_t)(iS_eNB)) << 24) | \
    (((hash_key_t)(lC_iD))  << 25) | \
    (((hash_key_t)(dEST_iD)) << 33) | \
    (((hash_key_t)(0x0a))   << 57))


// service id max val is maxServiceCount = 16 (asn1_constants.h)

#define RLC_COLL_KEY_MBMS_VALUE(eNB_iD, rNTI, iS_eNB, sERVICE_ID, sESSION_ID) \
   ((hash_key_t)eNB_iD             | \
    (((hash_key_t)(rNTI))       << 8)  | \
    (((hash_key_t)(iS_eNB))     << 24) | \
    (((hash_key_t)(sERVICE_ID)) << 32) | \
    (((hash_key_t)(sESSION_ID)) << 37) | \
    (((hash_key_t)(0x0000000000000001))  << 63))


hash_table_t  *rlc_coll_p;

typedef struct buffer_status_s
{
    uint32_t  valid_flag;
	rnti_t    rnti;
	uint32_t  latest_logic_ch_num; //!index for the logic chan in the following arrays
	uint32_t  data_size[MAX_LOGICCHAN_NUM]; //!include the rlc header size.

	uint16_t logicchannel_id[MAX_LOGICCHAN_NUM];
	uint32_t rlc_header_size[MAX_LOGICCHAN_NUM];

}rlc_buffer_status;

/************************************global variable declare ********************/
extern ue_info_dict  *g_rrc_ue_info_dict;
extern rlc_ue_info      g_rlc_ue_info[D2D_MAX_USER_NUM];
extern protocol_ctxt_t    g_rlc_protocol_ctxt;
extern const uint32_t t_Reordering_tab[31] ;
extern char *g_rlc_mode_str[];

extern rlc_buffer_status   g_rlc_buffer_status[D2D_MAX_USER_NUM+1];

extern pthread_mutex_t    g_rlc_buffer_mutex; 

extern uint8_t            g_rlc_pdu_buffer[D2D_MAX_USER_NUM * MAX_DLSCH_PAYLOAD_BYTES];

extern mac_pdu_size_para  g_rlc_pdu_size_para[D2D_MAX_USER_NUM];
extern uint8_t            g_rlc_mac_subheader[D2D_MAX_USER_NUM * ((MAX_LOGICCHAN_NUM  + 1)* 3)]; 
extern mac_pdu_size_para  g_rlc_pdu_size_para[D2D_MAX_USER_NUM]; 

extern struct mac_data_req g_rlc_mac_data_req;
extern uint8_t  g_mac_rlc_pdu_buffer[D2D_MAX_USER_NUM *  MAX_DLSCH_PAYLOAD_BYTES];


#ifdef RLC_UT_DEBUG 
extern  uint32_t   g_rlc_no_data_transfer; 
extern  uint32_t g_rlc_debug_ue_mac_header_size;
extern  uint32_t   g_rlc_ut_tx_buffer_size[D2D_MAX_USER_NUM][MAX_LOGICCHAN_NUM];
extern logic_channel_pdu_component   g_debug_rlc_lc_pdu_component[MAX_LOGICCHAN_NUM];
extern uint32_t  g_rlc_ut_2nd_trans;
#endif 




/************************************function declaration*************************/
extern void *rlc_rrc_config_task( );
extern rlc_union_t 	*rlc_Get_HashNode(const protocol_ctxt_t* const ctxt_pP,
									   boolean_t  srb_flagP,
									   rb_id_t rb_idP);

extern void mac_rlc_data_ind	  (
							const module_id_t 		module_idP,
							const rnti_t				rntiP,
							const eNB_index_t 		eNB_index,
							const frame_t 			frameP,
							const eNB_flag_t			enb_flagP,
							const logical_chan_id_t	channel_idP,
							char					   *buffer_pP,
							const tb_size_t			tb_sizeP,
							uint32_t					num_tbP);
extern void   rlc_Set_Buffer_Status(rnti_t rnti,
									   rlc_mode_e rlc_mode,
									   uint32_t input_sdu_num,
									   logical_chan_id_t logical_chan_id_t,
									   uint16_t data_size);
extern int   rlc_Get_Buffer_Status(rlc_buffer_rpt *buffer_status);
extern void  rlc_ue_data_status_update(rnti_t rnti, 
										rlc_mode_e rlc_mode, 
										uint32_t fill_sdu_num,
										uint32_t remaind_sdu_num,
										logical_chan_id_t logical_chan_id, 
									    uint16_t send_data_size); 

									    
extern void  rlc_um_mac_data_indication (const protocol_ctxt_t *const ctxt_pP, void *rlc_pP, struct mac_data_ind data_indP); 

extern void rlc_um_init_timer_reordering(const protocol_ctxt_t* const ctxt_pP,
												  rlc_um_entity_t * const rlc_pP,
												  const uint32_t  ms_durationP);


extern int32_t 	rlc_mac_logicchan_data_send(const protocol_ctxt_t          ctxt,
									const logical_chan_id_t channel_idP,
									const tb_size_t         logicch_tb_sizeP,
									logic_channel_pdu_component   *lc_component_ptr,
									mac_pdu_size_para  *ue_mac_pdu_size_ptr,
									struct mac_data_req *data_request);

extern void  rlc_tm_mac_data_indication (const protocol_ctxt_t* const  ctxt_pP,
											void * const		rlc_pP,
											struct mac_data_ind data_indP);

extern void rlc_tm_cleanup (rlc_tm_entity_t * const rlcP);
extern void rlc_tm_configure(
								  const protocol_ctxt_t* const  ctxt_pP,
								  rlc_tm_entity_t * const rlcP,
								  const boolean_t is_uplink_downlinkP);
extern void  rlc_tm_data_req (
							  const protocol_ctxt_t* const	ctxt_pP,
							  void *const rlc_pP,
							  mem_block_t *const sdu_pP);

extern void rlc_Rrc_BufStatus_Rpt(uint32_t send_data_size, rb_id_t rb_id, rb_type_e rb_type, rnti_t rnti, 
									uint32_t  rlc_buffer_empty_size, uint32_t rlc_buffer_valid);
extern void rlc_Mac_BufferSta_Rpt(uint16_t   sfn, uint16_t  subsfn, uint32_t valid_ue_num,rlc_buffer_rpt *buffer_status);
extern int32_t  rlc_mac_ue_data_process(frame_t frameP, 
									sub_frame_t  subframeP,
								    rlc_data_req *rlc_data_ptr, 
								    protocol_ctxt_t *protocol_ctxt_ptr,
								    mac_pdu_size_para  *ue_pdu_size_para_ptr,
								    uint8_t *ue_pdu_buffer,
								    uint8_t *ue_mac_subheader_ptr
								    );
extern void config_req_rlc_tm(const protocol_ctxt_t* const  ctxt_pP,
							  const srb_flag_t  srb_flagP,
							  const rlc_tm_info_t * const config_tmP,
							  const rb_id_t rb_idP,
							  const logical_chan_id_t chan_idP
							);

extern void rlc_Rrc_Configure_Cfm(uint32_t        message_id);
#endif
