/**********************************************************
* @file rrc_sim.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/07
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RRC_SIM_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RRC_SIM_H_

#include "typedef.h"
#include "mac_defs.h"
#include "msg_handler.h"

typedef enum
{
	ERRC_NONE,
	ERRC_INITAIL,
	ERRC_INITAIL_CFM,
	ERRC_BCCH_SEND,
	ERRC_BCCH_CFM,
	ERRC_SETUPING_UE,
	ERRC_DONE
}rrc_status_e;

typedef enum
{
	ERRC_UE_INVALID,
	ERRC_UE_SETUP,
	ERRC_UE_SETUP_COMPLETE,
	ERRC_UE_IDLE,
	ERRC_UE_CONNECT
}rrc_ue_status_e;

typedef struct
{
	uint16_t flag; // 0:setup req, 1:setup, 2:setup complete, 3: src user done, 4: dst user done
	uint16_t cause;//0:faiul, 1:success
	uint16_t ueId;
	rnti_t   rnti;
}ccch_info;

typedef struct
{
	uint16_t ueId;
	uint16_t rnti;
	uint16_t mode; //0:source, 1:destination
	uint16_t setup_timer; // setup waiting timer
	rrc_ue_status_e status;
}rrc_ue_info;

typedef struct
{
	frame_t      frame;
	sub_frame_t  subframe;
	rrc_status_e status;
	uint16_t     mode;//0:source, 1:destination
	uint16_t     cellId;
	uint16_t     bandwith;

	uint16_t     num_ue;
	rrc_ue_info ue[D2D_MAX_USER_NUM];
}rrc_info;

void init_rrc_src_sim();
void init_rrc_dst_sim();
void rrcSrsMsgHandler(msgDef* msg, const msgId msg_id);
void rrcDstMsgHandler(msgDef* msg, const msgId msg_id);

void rrcSrcStatusHandler();
void rrcSrcUserStatusHandler();
void rrcDstStatusHandler();
void rrcDstUserStatusHandler();

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_RRC_SIM_H_ */
