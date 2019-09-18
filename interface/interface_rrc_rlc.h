/** @file interface_rrc_rlc.h
*
* @brief  d2d protocol rrc-rlc message definition
* @author   bo.liu
* @date     2019/07/29
* COPYRIGHT NOTICE: (c)  smartlogictech.
* All rights reserved.
*/
#ifndef INTERFACE_RRC_RLC_H_
#define INTERFACE_RRC_RLC_H_

#include <typedef.h>
#include <rlc_type.h>

#define MAX_SRB_COUNT    3
#define MAX_DRB_COUNT    32


typedef  enum
{
	RB_TYPE_SRB0,
	RB_TYPE_SRB1,
	RB_TYPE_SRB2,
	RB_TYPE_DRB
}rb_type_e; 

	



typedef enum
{/**1:tm_dl,2:tm_ul, 3:tm ul&dl, 3:um_dl,4:um_ul, 5:um_dl&ul **/
   RLC_MODE_TM, 
   RLC_MODE_TM_DL,      //!0
   RLC_MODE_TM_UL,
   RLC_MODE_UM, 
   RLC_MODE_UM_DL, 
   RLC_MODE_UM_UL, 
   RLC_MODE_AM
}rlc_mode_e; 


typedef struct rlc_ul_um_cfg_s
{
	 /*um mode valid */
	uint16_t  sn_field; /**0:5; 1:10 */
}rlc_ul_um_cfg;



typedef struct rlc_dl_um_cfg_s
{
	 /*um mode valid */
	uint16_t  sn_field; /**0:5; 1:10 */
	uint16_t  t_recordering; /** recordering timer expire time,unit:subsfn, [0~65535]*/
}rlc_dl_um_cfg;

typedef union
{
	rlc_ul_um_cfg   ul_um_cfg;
	rlc_dl_um_cfg   dl_um_cfg; 
}rlc_entity_cfg;

typedef struct rb_info_s
{
	uint8_t rb_id;
	uint8_t logicchannel_type;
	uint8_t logicchannel_id;
	rlc_mode_e rlc_mode;  /**1:tm_dl,2:tm_ul, 3:tm ul&dl, 3:um_dl,4:um_ul, 5:um_dl&ul **/

	rlc_entity_cfg  rlc_mode_cfg;
}rb_info; 


typedef struct rrc_rlc_srb_addmod_req_s
{
	uint32_t requset_id;
	rb_type_e  rb_type; /**1:srb1; 2:drb */
	uint16_t  srb_count;  /**srb count in srb_list */
	rb_info  srb_list[MAX_SRB_COUNT];
	
}rrc_rlc_srb_addmod_req; 


typedef struct rrc_rlc_drb_addmod_req_s
{
	uint32_t requset_id;
	
	rb_type_e rb_type; /**1:srb1; 2:drb */
	uint16_t  drb_count;  /**drb count in srb_list */

	rb_info  drb_list[MAX_DRB_COUNT];
	
}rrc_rlc_drb_addmod_req; 


typedef struct rrc_rlc_drb_release_req_s
{
	uint32_t  requset_id;
	
	rb_type_e  rb_type; /**1:srb1; 2:drb */
	uint16_t   drb_count;  /**drb count in srb_list */
	
	uint8_t drb_release_list[32]; /**drb id list*/
	
}rrc_rlc_drb_release_req; 



/**RRC_RLC_INITIAL_REQ */
typedef struct rrc_rlc_initial_req_s
{

	uint32_t   initial_flag; //!initial,rlc initial ,buffer,hash table establish
	uint32_t   mode; 
}rrc_rlc_initial_req;

/**RLC_RRC_INITIAL_CFM **/
typedef struct rlc_rrc_initial_cfm_s
{
	uint16_t  status; /**1:pass; 0:error*/
	uint16_t  error_code;     /**error code, self definition*/
}rlc_rrc_initial_cfm;

/**RRC_RLC_RELEASE_REQ */
typedef struct rrc_rlc_release_req_s
{
	uint32_t     cell_id;
	uint32_t     ue_index; 
}rrc_rlc_release_req;

typedef  rlc_rrc_initial_cfm   rrc_rlc_release_cfm;


/**RRC_RLC_BUF_STATUS_REQ*/
typedef struct rrc_rlc_buffer_req_s
{
	uint32_t request_id; /**record current request id*/
	rb_type_e  rb_type; 
	uint32_t   rb_id; 
	rnti_t     rnti; 
	uint32_t send_data_size; /**rrc -> rlc data size */
}rrc_rlc_buffer_status_req;

/**RLC_RRC_BUF_STATUS_RPT */
typedef struct rlc_rrc_buffer_rpt_s
{
	uint32_t request_id;
	rb_type_e  rb_type; 
	uint32_t   rb_id; 
	rnti_t     rnti; 
	uint32_t rlc_buffer_valid ; /**1:buffer have enough size for rrc message ,0: don't have enough space*/
    uint32_t  send_data_size; 
	uint32_t rlc_buffer_data_size; /**rlc buffer valid size for rrc data */
}rlc_rrc_buffer_rpt; 


/**msg: RRC_RLC_DATA_IND*/
typedef struct rrc_rlc_data_ind_s
{
	rb_type_e   rb_type;  /** 0:srb0; 1:srb1; 3:drb */
	rb_id_t     rb_id; 
	rnti_t      rnti;
	uint16_t   data_size; /**unit: byte*/
	uint32_t   *data_addr_ptr; /**data address*/
}rrc_rlc_data_ind;


/**RRC_RLC_BCCH_PARA_CFG_REQ*/
typedef struct rrc_rlc_bcch_para_cfg_s
{
	rrc_rlc_srb_addmod_req   srb_cfg_req;  //!srb0 addmod request
}rrc_rlc_bcch_para_cfg;

typedef rlc_rrc_initial_cfm  rlc_rrc_bcch_para_cfg_cfm;


/**RRC_RLC_BCCH_PARA_CFG_REQ*/
typedef struct rrc_rlc_connect_setup_cfg_s
{
    uint32_t  ue_index; 
    uint32_t  ue_rnit; 
    rrc_rlc_srb_addmod_req   srb_cfg_req;  //!srb1 addmod_request
	rrc_rlc_drb_addmod_req   drb_cfg_req;  /**srb addmod request*/
}rrc_rlc_connect_setup_cfg;

typedef rlc_rrc_initial_cfm  rlc_rrc_connect_setup_cfg_cfm;



#endif

