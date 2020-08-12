/******************************************************************
 * @file  rrc_global_def.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月13日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月13日        bo.liu       create file

*****************************************************************/

#ifndef      RRC_GLOBAL_DEF_H
#define      RRC_GLOBAL_DEF_H

#include <d2d_system_global.h>

#include <MasterInformationBlock.h>
#include <SystemInformationBlockType1.h>
#include <RRCConnectionSetup.h>
#include <RRCConnectionRequest.h>
#include <RRCConnectionRelease.h>
#include <CCCH-Message.h>
#include <interface_rrc_phy.h>
#include <interface_rrc_mac.h>
#include <interface_rrc_rlc.h>
#include <log.h>
#include <dictionary.h>






#define  D2D_MODE_TYPE_SOURCE     0
#define  D2D_MODE_TYPE_DESTINATION  1


#define  RRC_MESSAGE_EN_DECODE_OK    0
#define  RRC_MESSAGE_EN_DECODE_ERROR   -1

#define  ENCODE_MAX_SIZE  1024 


typedef  enum
{
    RRC_STATUS_NULL,     //!0
	RRC_STATUS_INITIAL,   //!1
	RRC_STATUS_INITIAL_CFM,  //!2
	RRC_STATUS_CELL_SEARCH,   //!3
	RRC_STATUS_IDLE,           //!4
	RRC_STATUS_CONNECT_REQUEST,   //!5 
	RRC_STATUS_CONNECTED,       //! 6
	RRC_STATUS_CONNECTE_COMPLETE, //!7
    RRC_STATUS_CONNECTE_OUT_SYNC,   //!8
    RRC_STATUS_RECONFIG,        //!9
    RRC_STATUS_REESTABLISH ,     //!10
    RRC_STATUS_RELEASE   //!11
}rrc_status_e;



typedef struct rrc_mib_info_s
{

	uint16_t  mib_sfn; 

	uint8_t   pdcch_rb_start; 
	uint8_t   pdcch_rb_num;

} rrc_mib_info; 


 
typedef struct rrc_init_var_s
{
	uint16_t  source_type;       //! 1: source; 0:destination
    uint16_t  cell_id;  
	
	bandwidth_e       band_width;     //!0: 1.5m,1: 3m, 2: 6m, 3: 12m, 4: 20m
	subframeconfig_e  subframe_config; //!0:DDUUDDUU  1: NOT define

	uint16_t  band_info; 
	uint16_t  padding; 
	
	uint16_t  dl_freq; 
	uint16_t  ul_freq; 

	rrc_mib_info  mib_info; 

	uint8_t  rlc_initial_cfm; //!1:initial cfm pass ,0: error 
	uint8_t  mac_initial_cfm; 
    uint8_t  phy_initial_cfm; 
    uint8_t   pad2; 
}rrc_init_var; 






/**************************************extern var *******************************/
extern  rrc_init_var  g_rrc_init_para;
extern  rrc_init_var  g_dst_rrc_init_para;

extern rrc_status_e  g_rrc_status;
extern ue_info_dict  *g_rrc_ue_info_dict;
extern uint8_t   g_rrc_messge_encode[ENCODE_MAX_SIZE];
extern uint16_t  g_rrc_mac_report_rnti; 

extern uint16_t  g_rrc_release_cnt; 
extern uint16_t  g_rrc_release_ue_idx ; 
extern uint16_t  g_rrc_release_ue_rnti ; 
extern uint16_t  g_rrc_reestablish_max_cnt;
extern uint16_t  g_rrc_reestablish_cnt ;
extern RRCConnectionSetup_t   g_connect_setup_record; 

/*************************************extern function****************************/
extern int rrc_GetModeType( ); 
extern rrc_status_e  rrc_GetCurrentStatus( );
extern int rrc_module_Initial();
extern void  *rrc_Sche_Task();
extern int EncodeD2dMib(uint8_t *encode_buffer, uint32_t max_buffersize, uint32_t *encode_size );
extern int EncodeD2dSib1(uint8_t *encode_buffer, uint32_t max_buffersize, uint32_t *encode_size );
extern int EncodeD2dRrcConnectionSetup(uint8_t  *encode_buffer, uint32_t buffersize); 
extern int EncodeD2dRrcConnectRequest(uint8_t  *encode_buffer, uint32_t buffersize);
extern int EncodeD2dRrcConnectRelease(uint8_t  *encode_buffer, uint32_t buffersize);
extern int DecodeD2dMib(MasterInformationBlock_t       **bch_msg,uint8_t *buf,uint32_t size );
extern int DecodeD2dSib1(SystemInformationBlockType1_t       **decode_msg,uint8_t *buf,uint32_t size );
extern int DecodeD2dRrcConnectSetup(RRCConnectionSetup_t       *decode_msg,uint8_t *buf,uint32_t size );
extern int DecodeD2dRrcConnectRequest(RRCConnectionRequest_t           *decode_msg,uint8_t *buf,uint32_t size );
extern int DecodeD2dRrcConnectRelease(RRCConnectionRelease_t           *decode_msg,uint8_t *buf,uint32_t size );
extern int EncodeD2dCcch(uint8_t  *encode_buffer, uint32_t max_buffersize, 
                   uint32_t *encode_size,  CCCH_MessageType_PR ccch_messsage_type);
extern int DecodeD2dCcch(CCCH_Message_t     **decode_msg,uint8_t *buf,uint32_t size ); 

extern rb_info rrc_Rlc_Rbinfo_Generate(rb_type_e rb_type, uint8_t rb_id, 
                                    uint8_t logicch_type,uint8_t logicch_id,
                                    rlc_mode_e rlc_mode,
                                    uint16_t um_sm_field,
                                    uint16_t um_t_recording);
extern rrc_rlc_srb_addmod_req  rrc_Rlc_Srb_Config(rb_type_e rb_type,uint16_t srb_count,rb_info *rb_info_ptr);
extern rrc_rlc_drb_addmod_req  rrc_Rlc_Drb_Config(rb_type_e rb_type,uint16_t drb_count,rb_info *rb_info_ptr);

extern void rrc_Mac_Release_Config(uint16_t cell_id,uint16_t ue_idx, uint16_t release_cause);
extern void rrc_Rlc_Release_Req(uint32_t cell_id, uint32_t ue_index);
extern void rrc_SetStatus(rrc_status_e          rrc_next_status);
extern void rrc_Mac_BcchPara_Config(uint16_t mib_or_sib1,void *bcch_info);
extern void  rrc_Phy_InitialConfig(rrc_init_var init_var); 
extern void rrc_Mac_InitialConfig(uint16_t mode_type,rrc_init_var init_var);
extern void rrc_Rlc_InitialConfig(uint32_t        mode );
extern void rrc_Phy_BcchPara_Config(pusch_config_basic_s push_basic_config, ul_ref_signal_pusch_s ul_refsig_config);
extern void rrc_Rlc_BcchPara_Config(rrc_rlc_srb_addmod_req *srb_info);
extern void rrc_Rlc_DataBuf_Sta_Req(rb_type_e         rb_type,uint32_t rb_id,rnti_t rnti, uint32_t data_size);
extern void rrc_Rlc_ConnectSetup_Config(uint32_t ue_rnti, uint32_t ue_index ,
										  rrc_rlc_srb_addmod_req * srb_info,
										  rrc_rlc_drb_addmod_req *drb_info);



extern void rrc_Phy_ConnectSetup_Config(uint16_t              beta_off_ack_ind);
extern void rrc_Mac_ConnectSetup_Config(uint16_t mode, rnti_t ue_rnti , 
										  uint16_t ue_index, uint16_t max_harq_tx,
										  uint16_t max_out_sync,uint16_t logicch_num, 
										  logical_channel_config_s *LogicChannelConfigPtr);

extern void rrc_Rlc_Data_Send(rb_type_e rb_type, rb_id_t rb_id, rnti_t rnti, uint32_t *data_buffer, uint32_t data_size);
extern int rrc_SetModeType(int mode_type);
extern void rrc_Phy_CellSearch(uint16_t ul_freq, uint16_t dl_freq);
extern void rrc_Phy_Release_config(uint16_t              cell_id);

#endif
