/** @file interface_rrc_mac.h
 *
 *  @brief
 *  @author bo.liu
 *  @date   2019/07/30
 *  COPYRIGHT NOTICE: (c) smartlogictech.
 *  All rights reserved.
 */

#ifndef INC_INTERFACE_RRC_MAC_H_
#define INC_INTERFACE_RRC_MAC_H_


#include <typedef.h>
#include <d2d_system_global.h>



/**MAC_RRC_INITIAL_CFM **/
typedef struct mac_rrc_initial_cfm_s
{
	uint16_t  status; /**1:pass; 0:error*/
	uint16_t  error_code;     /**error code, self definition*/
}mac_rrc_initial_cfm;


typedef mac_rrc_initial_cfm  mac_rrc_release_cfm;

/**MAC_RRC_CONNECTION_CFM***/
typedef struct mac_rrc_connection_cfm_s
{
	uint16_t ue_index;
	rnti_t rnti;
	uint16_t  status; /**1:pass; 0:error*/
	uint16_t  error_code;     /**error code, self definition*/
}mac_rrc_connection_cfm;

/*****MIB & SIB1 message*******/

/**MAC_RRC_BCCH_SIB1_RPT***/
typedef struct mac_rrc_bcch_sib1_rpt_s
{
	uint16_t  sfn;
	uint16_t  subsfn;

	uint32_t  data_size;
	uint32_t  *data_ptr;
}mac_rrc_bcch_sib1_rpt;

  /**MAC_RRC_CCCH_RPT*/
typedef mac_rrc_bcch_sib1_rpt  mac_rrc_ccch_rpt;

/**MAC_RRC_OUTSYNC_RPT */
typedef struct mac_rrc_outsync_rpt_s
{
	uint16_t  sfn;
	uint16_t  subsfn;

    uint32_t  outsync_flag ; /**1:out_sync; 0:in_sync**/
}mac_rrc_outsync_rpt;


typedef enum
{
	CCCH,
	DTCH
}channel_type_e;

typedef struct
{
	uint16_t rb_num;// value: 1..2, default: 2
	uint16_t rb_start_index; // value: 2..3, default: 2
}pdcch_config_s;

typedef struct
{
	uint16_t cellId; //value: 0..503, default: 0
	uint16_t bandwith; // value: 0..4,  0: 1.5M, 1: 3M, 2: 6M, 3: 12M, 4:20M, default: 1
	uint16_t subframe_config;// value: 0..1, 0: DDUUDDUU, 1: not define
	uint16_t mode; //value: 0..1, 0：Source，1：Destination
	pdcch_config_s pdcch_config;
}rrc_mac_initial_req; //RRC_MAC_INITIAL_REQ

typedef struct
{
	uint16_t cellId; //value: 0..503, default: 0
	uint16_t ue_index;// value: 0..16
	uint16_t releaseCause;//value: 0..1, 0: out sync, 1: other
}rrc_mac_release_req; //RRC_MAC_RELEASE_REQ

typedef struct
{
	frame_t systemFrameNumber;// value: 0
	pdcch_config_s pdcch_config;
}bcch_mib_info_s;

typedef struct
{
	uint16_t size;
	uint8_t *sib_pdu; //tdd-Config
}bcch_si_info_s;

typedef struct
{
	bool flag;// value: 0..2, 0: invalid, 1: mib, 2: sib
	bcch_mib_info_s mib;
	bcch_si_info_s sib;
}rrc_mac_bcch_para_config_req;//RRC_MAC_BCCH_PARA_CFG_REQ

typedef struct
{
	channel_type_e chan_type;
	uint16_t priority;// value:0..16
	uint16_t logical_channel_id;// value:0..16
}logical_channel_config_s;

typedef struct
{
	uint16_t ue_index;// value: 0..16
	uint16_t maxHARQ_Tx;// value: 0..8, default: 4
	uint16_t max_out_sync;// value: 0..8, default: 4
	uint16_t padding;
	logical_channel_config_s logical_channel_config[MAX_LOGICCHAN_NUM];
}rrc_mac_connnection_setup; //RRC_MAC_CONNECT_SETUP_CFG_REQ;

#endif /* INC_INTERFACE_RRC_MAC_H_ */
